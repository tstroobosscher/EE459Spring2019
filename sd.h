/*
 *	USC EE459 Spring 2019 Team 17 - SD Header
 */

#ifndef SD_H
#define SD_H

#define SECTOR_SIZE 512

#define CMD_RESP_BYTES 8

#define MAX_CMD_TRIALS 8

#define R1_READY_STATE 0
#define R1_IDLE_STATE (1 << 0)
#define R1_ERASE_RESET (1 << 1)
#define R1_ILLEGAL_COMMAND (1 << 2)
#define R1_CRC_ERROR (1 << 3)
#define R1_ERASE_SEQ_ERROR (1 << 4)
#define R1_ADDRESS_ERR (1 << 5)
#define R1_PARAMETER_ERR (1 << 6)
#define R1_RESPONSE_ERR 0xFF

#define SDHC_OCR_MASK 0xC0
#define RET_MSB_MASK 0x80
#define CMD8_RESPONSE_PATTERN 0xAA
#define SDHC_HIGH_SPEED_FLAG 0x40

/*
 *	Command 				ID 		Argument
 *Response Go Idle State 			CMD0 	0
 *R1 Send If Cond 			CMD8 	…
 *R7 Stop Transmission 		CMD12 	0
 *R1 Set Block Length 		CMD16 	Length 			R1 Read
 *Single Block 		CMD17 	Block 			R1 Read Multiple
 *Blocks 	CMD18 	Start Block 	R1 Application Command 	CMD55 	0
 *R1 Read OCR 				CMD58 	…
 *R3
 */

/*
 *	Voltage check command
 *	6 byte:
 *	bit    |48|46| 45-40 | 39-20| 19-16   | 15-8
 *	width  |1 |1 |  6    | 20   | 4       |
 *	value  |0 |1 |b001000| 0x0  | voltage |
 *
 *	0x00 0x00 0x01 0xAA
 */

#define NOARG 0x00

#define CMD0_CRC 0x95
#define CMD8_CRC 0x87
#define NOCRC 0xFF

#define CMD0 0x40
#define CMD1 0x40 | 0
#define CMD8 0x40 | 8
#define CMD16 0x40 | 16
#define CMD17 0x40 | 17
#define CMD55 0X40 | 55
#define CMD58 0X40 | 58

#define ACMD41 0x40 | 41

static const enum {
  SD_TYPE_1,
  SD_TYPE_2,
  SD_TYPE_SDHC,
} sd_type;

static const enum {
  SD_ENABLED,
  SD_DISABLED,
} sd_status;

struct sd_ctx {
  uint8_t sd_type;
  uint8_t sd_status;
  uint16_t sd_sector_size;
};

int8_t initialize_sd();

int16_t sd_get_sector(struct sd_ctx *sd, uint32_t addr, uint8_t *buf,
                      uint16_t size);

#endif
