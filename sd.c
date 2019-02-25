/*
 *	USC EE459 Spring 2019 Team 17 - SD Flash
 */

#include <stdint.h>
#include <string.h>
#include <avr/io.h>

#include "sd.h"
#include "spi.h"
#include "utils.h"
#include "time.h"
#include "uart.h"
#include "debug.h"

static __attribute__((always inline)) int8_t sd_wake_up() {
	/* enable sd card */
	if(spi_device_enable(SPI_SD_CARD) < 0)

		/* sanity check */
		return -1;

	/* 80 clocks, init card */
	for(uint8_t i = 0; i < 10; i++)
		spi_write_char(0xFF);

	/* disable sd card */
	if(spi_device_disable(SPI_SD_CARD) < 0)

		/* sanity check */
		return -1;

	return 0;
}

static __attribute__((always inline)) int8_t sd_is_busy() {

	/* true/false logic */
	if(spi_read_char() == 0xFF)
		return 0;
	else
		return 1;
}

static __attribute__((always inline)) 
	uint8_t sd_command(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t bytes, 
	uint8_t *buf) {
	/*
	 *	expecting a response of bytes length
	 *	buf must be at least bytes long
	 *	
	 *	SPI SD does not use crc, just default 0xFF once SPI is init'd
	 *	
	 *	This function does employ a staic delay to help the commands return
	 *	correctly, but it is only used during the init function, so the
	 *	overhead incurred happens once
	 */

	spi_device_enable(SPI_SD_CARD);

	while(sd_is_busy()){}

    spi_write_char(cmd);
    spi_write_char(arg>>24);
    spi_write_char(arg>>16);
    spi_write_char(arg>>8);
    spi_write_char(arg);
    spi_write_char(crc);

    /* tested 2/24/19 - delay less than 7ms causes init to fail */
    DELAY_MS(10);

    /* bytes == 0, return the flagged response */
    if(!bytes){
    	uint8_t ret;
    	uint8_t trials = 0;

    	UART_DBG("CMD ");
    	UART_DBG_HEX(cmd);
    	UART_DBG(" ");

    	DELAY_MS(10);

    	do {

    		ret = spi_read_char();

    		UART_DBG_HEX(ret);
    		UART_DBG(" ");

    		DELAY_MS(10);

    		if(++trials >= MAX_CMD_TRIALS)

    			/* MSB set is an error response */
    			return R1_RESPONSE_ERR;

    	} while((ret & RET_MSB_MASK) == RET_MSB_MASK);

    	UART_DBG("\r\n");
    	DELAY_MS(10);

    	/* MSB is unset */
    	return ret;

    } else
        for(uint8_t i = 0; i < bytes; i++)
        	buf[i] = spi_read_char();
                
    /* cs needs to be toggled between each command */
    spi_device_disable(SPI_SD_CARD);

    /* return is not checked with reference parameters */
    return 0xFF;
}

static __attribute__((always inline)) 
	uint8_t sd_acommand(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t bytes, 
	uint8_t *buf) {

	/* application command, lead with cmd55 */
	if(sd_command(CMD55, bind_args(NOARG, NOARG, NOARG, NOARG), NOCRC, 
		0, NULL) == R1_RESPONSE_ERR) {

			UART_DBG("sd: CMD55 failure\r\n");
			DELAY_MS(10);
			return 0xFF;
	}

	while(sd_is_busy()){}

	return sd_command(cmd, arg, crc, bytes, buf);
}

static __attribute__((always inline)) 
	uint8_t sd_get_resp_byte(uint8_t *buf, uint8_t size) {
	for(uint8_t i = 0; i < size; i++)
		if(buf[i] & 0x80)
			continue;
		else
			return buf[i];
	return 0xFF;
}

static __attribute__((always inline)) 
	int8_t sd_find_resp_byte(uint8_t *buf, uint8_t size) {
	for(uint8_t i = 0; i < size; i++)
		if(buf[i] & 0x80)
			continue;
		else
			return i;
	return -1;
}

static __attribute__((always inline)) 
	void sd_get_bytes(uint16_t bytes, uint8_t *buf) {

	spi_device_enable(SPI_SD_CARD);
	
	UART_DBG("sd: spi received: ");
	//DELAY_MS(10);

	for(uint16_t i = 0; i < bytes; i++) {
		buf[i] = spi_read_char();
		//DELAY_MS(10);
		UART_DBG_HEX(buf[i]);
		UART_DBG(" ");
	}

	UART_DBG("\r\n");
	//DELAY_MS(10);

	spi_device_disable(SPI_SD_CARD);
}

static __attribute__((always inline)) 
	int8_t sd_init_read_sector(uint32_t arg) {
	uint16_t trials = 0;

	spi_device_enable(SPI_SD_CARD);

	/* reading in succession needs waiting */
	while(sd_is_busy()){}

    spi_write_char(CMD17);
    spi_write_char(arg>>24);
    spi_write_char(arg>>16);
    spi_write_char(arg>>8);
    spi_write_char(arg);
    spi_write_char(0xFF);

    /* data read token */
    while(spi_read_char() != 0xFE) {

    	UART_DBG("sd: CMD17 error\r\n");

    	/* can't read loop here because we need the bytes */
    	DELAY_MS(1);

    	/* data access can take up to 2 sec, need timeout */
    	if(++trials > 1000) {

    		spi_device_disable(SPI_SD_CARD);
    		return -1;
    	}
    }

    spi_device_disable(SPI_SD_CARD);
    return 0;
}

/* export */
int8_t initialize_sd(struct sd_ctx *sd) {
	uint8_t buf[CMD_RESP_BYTES];
	uint8_t ret;
	int trials;

	if(sd_wake_up() < 0)

		/* unhandled error */
		goto failure;

	UART_DBG("sd: sd card woken up\r\n");
	DELAY_MS(10);

	/* start the clock, must find the correct response in time */
	trials = 0;  

	/* send CMD0, get IDLE state */
	while(sd_command(CMD0, bind_args(NOARG, NOARG, NOARG, NOARG), 
		CMD0_CRC, 0, NULL) != R1_IDLE_STATE) {

		UART_DBG("sd: CMD0 error\r\n");
		DELAY_MS(10);

		/* too late */
		if(++trials > MAX_CMD_TRIALS)
			goto failure;
	}

	while(sd_is_busy()) {
		/* need timeout */

		UART_DBG("sd: card is busy\r\n");
		DELAY_MS(10);
	}

	UART_DBG("sd: CMD0 succesful\r\n");
	DELAY_MS(10);

	trials = 0;

	/* send CMD8, get resp byte */
	while((ret = sd_command(CMD8, bind_args(NOARG, NOARG, 0x01, 
		CMD8_RESPONSE_PATTERN), CMD8_CRC, 0, NULL)) != R1_IDLE_STATE) {

		UART_DBG("sd: CMD8 error\r\n");
		DELAY_MS(10);

		if(++trials > MAX_CMD_TRIALS)
			goto failure;
	}

	UART_DBG("sd: CMD8 succesful\r\n");
	DELAY_MS(10);

	/* SD type 1 cards will return illegal and idle */
	if(ret == (R1_IDLE_STATE | R1_ILLEGAL_COMMAND)){

		UART_DBG("sd: type 1 sd card\r\n");
		DELAY_MS(10);

		sd->sd_type = SD_TYPE_1;
	}
	
	/* command received correctly */
	else if(ret == R1_IDLE_STATE) {

		UART_DBG("sd: checking voltage\r\n");
		DELAY_MS(10);

		/* get the response byte R7 */
		sd_get_bytes(CMD_RESP_BYTES, buf);

		/* check for confirmation byte */
		if(buf[3] == CMD8_RESPONSE_PATTERN) {

			/* type 2 sd card */
			UART_DBG("sd: type 2 sd card\r\n");
			DELAY_MS(10);
			sd->sd_type = SD_TYPE_2;
		
		} else {

			UART_DBG("sd: CMD8 response error: ");
			UART_DBG_HEX(buf[0]);
			UART_DBG(" ");
			UART_DBG_HEX(buf[1]);
			UART_DBG(" ");
			UART_DBG_HEX(buf[2]);
			UART_DBG(" ");
			UART_DBG_HEX(buf[3]);
			UART_DBG("\r\n");
			DELAY_MS(10);

			/* unhandled error */
			goto failure;
		}
	}

	/* unhandled error */
	else {

		UART_DBG("sd: unhandled CMD8 return: ");
		UART_DBG_HEX(ret);
		UART_DBG("\r\n");
		DELAY_MS(10);

		goto failure;
	}

	trials = 0;

	/* look for ready state, not idle state, keep trying until found */
	while(sd_acommand(ACMD41, sd->sd_type == SD_TYPE_2 ? 
		(bind_args(SDHC_HIGH_SPEED_FLAG, NOARG, NOARG, NOARG)) : 
		(bind_args(NOARG, NOARG, NOARG, NOARG)), 
		NOCRC, 0, NULL) != R1_READY_STATE) {

		UART_DBG("sd: ACMD41 error\r\n");
		DELAY_MS(10);

		while(sd_is_busy()){}

		if(++trials > MAX_CMD_TRIALS)
			goto failure;
	}

	trials = 0;

	while(sd_is_busy()){}

	if(sd->sd_type == SD_TYPE_2) {

		/* check OCR register for SDHC */
		while((ret = sd_command(CMD58, bind_args(NOARG, NOARG, NOARG, NOARG), 
			NOCRC, 0, NULL)) != R1_READY_STATE) {

			UART_DBG("sd: CMD58 error\r\n");
			DELAY_MS(10);

			if(++trials > MAX_CMD_TRIALS)
				goto failure;
		}

		sd_get_bytes(CMD_RESP_BYTES, buf);
		UART_DBG("sd: CMD58 response: ");
		UART_DBG_HEX(buf[0]);
		UART_DBG(" ");
		UART_DBG_HEX(buf[1]);
		UART_DBG(" ");
		UART_DBG_HEX(buf[2]);
		UART_DBG(" ");
		UART_DBG_HEX(buf[3]);
		UART_DBG("\r\n");
		DELAY_MS(10);

		if((buf[0] & SDHC_OCR_MASK) == SDHC_OCR_MASK) {

			/* SDHC */
			sd->sd_type = SD_TYPE_SDHC;
			UART_DBG("sd: SDHC card\r\n");
			DELAY_MS(10);
		}
	}

	sd->sd_status = SD_ENABLED;
	sd->sd_sector_size = SECTOR_SIZE;
	return 0;

failure:
	
	/* just to be sure we aren't locking up the SPI bus */
	spi_device_disable(SPI_SD_CARD);

	sd->sd_status = SD_DISABLED;
	return -1;

}

/* export */
int16_t sd_get_sector(struct sd_ctx * sd, uint32_t addr, uint8_t *buf, 
	uint16_t size) {
	uint8_t trials = 0;

	if(sd_init_read_sector(addr) < 0) {

		UART_DBG("sd: unable to initialize sector\r\n");
		//DELAY_MS(10);

		return -1;
	}

	UART_DBG("sd: sector ready\r\n");
	//DELAY_MS(10);

	sd_get_bytes(size, buf);

	return 0;
}
