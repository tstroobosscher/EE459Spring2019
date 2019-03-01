/*
 *	USC EE459 Spring 2019 Team 17 - OBD2 Test code
 */

/*
 *	February 26, 2019
 *		So i need to write the routine that is going to grab the data bytes 
 *		from the OBD2 response efficiently. All responses are preambled with
 *		0x40 or'ed with the command number (we saw something extremely 
 *		similar with the SD data-link layer code). All bytes before that
 *		preamble are ignored, and each command has an expected number og bytes
 *		to listen for
 *		
 *		Also i think its important to keep layering in mind. we probably 
 *		shouldnt be doing string comparison or byte filtering at all at the
 *		lowest level. we just need to copy the bytes into a buffer of an
 *		appropriate size, and let the upper layers deal with the filtering
 */

/*
 *	Wow, so declaring variables after string buffers can cause issues when
 *	the strings are expected to be NULL terminated, switch the order of i 
 *	and buf and the program will run incorrectly since printf is looking
 *	for a NULL char (when using printf())
 */

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#define NO_PARITY 0

#define ARRAY_SIZE(x) (sizeof((x))/sizeof((x)[0]))

#define BUF_SIZE 64
#define MAX_TRIALS 64
#define HEX_BASE 16
#define PREAM_CONST 0x40
#define OBD_CMD_LEN 4 + 1 /* +1 for carriage return */

static const enum obd_unit{
	UNITS_NONE,
	UNITS_PERCENT,
	UNITS_CELCIUS,
	UNITS_KPA,
	UNITS_RPM,
	UNITS_KMPH,
	UNITS_DEG_ADV,
	UNITS_GRAMSSEC,
	UNITS_VOLTS,
	UNITS_SEC,
	UNITS_KM,
	UNITS_RATIO,
	UNITS_COUNT,
	UNITS_PA,
	UNITS_MILLIAMP,
	UNITS_VOLTS_PERC,
} obd_unit;

const char *NO_DATA = "NO DATA";
const char *OK = "OK";
const char *NO_CONNECT = "UNABLE TO CONNECT";

const char *ELM_RESET = "ATZ\r";
const char *ECHO_OFF = "ATE0\r";
const char *SEARCH_BUS = "ATSP0\r";
const char *GET_DEVS = "0100\r";


static const enum obd_pid{
	/* 00 */	PIDS_SUPPORTED_01_02,	/* [01 - 20] Bit encoded [A7..D0] == [PID $01..PID $20] See below */
	/* 01 */	MONITOR_STATUS_SINCE_DTCS_CLEARED,	/* since DTCs cleared. (Includes malfunction indicator lamp (MIL) status and number of DTCs.) Bit encoded. See below */
	/* 02 */	FREEZE_DTC,				/* Error codes, See below */
	/* 03 */	FUEL_SYSTEM_STATUS,		/* Bit encoded. See below */
	/* 04 */	CALCULATED_ENGINE_LOAD,	/* A/2.55 [%] [0-100] */
	/* 05 */	ENGINE_COOLANT_TEMP_1,	/* A - 40 [°C]	[-40-215] */
	/* 06 */	SHORT_TERM_FUEL_TRIM_1,	/* A/128 - 100 [%] [-100 (Reduce Fuel: Too Rich-99.2 (Add Fuel: Too Lean)] */
	/* 07 */	LONG_TERM_FUEL_TRIM_1,	/* A/128 - 100 [%] [-100 (Reduce Fuel: Too Rich-99.2 (Add Fuel: Too Lean)] */
	/* 08 */	SHORT_TERM_FUEL_TRIM_2,	/* A/128 - 100 [%] [-100 (Reduce Fuel: Too Rich-99.2 (Add Fuel: Too Lean)] */
	/* 09 */	LONG_TERM_FUEL_TRIM_2,	/* A/128 - 100 [%] [-100 (Reduce Fuel: Too Rich-99.2 (Add Fuel: Too Lean)] */
	/* 0A */	FUEL_PRESSURE,			/* 3A [kPa]	[0-765] */
	/* 0B */	INTAKE_MANIFOLD_ABS_PRESSURE_1,	/* A [kPa] [0-255] */
	/* 0C */	ENGINE_RPM,				/* (256A + B)/4 [rpm] [0-16,383.75] */
	/* 0D */	VEHICLE_SPEED, 			/* A [km/h] [0-255] */
	/* 0E */	TIMING_ADVANCE,			/* A/2 - 64 [° before TDC] [-64-63.5] */
	/* 0F */	INTAKE_AIR_TEMP_1,		/* A - 40 [°C] [-40-215] */
	/* 10 */	MAF_RATE,				/* 0	655.35	grams/sec */
	/* 11 */	THROTTLE_POSITION_1,		/* 0	100	%	{\displaystyle {\tfrac {100}{255}}A} {\displaystyle {\tfrac {100}{255}}A} */
	/* 12 */	COMMANDED_SECONDARY_AIR_STAT, /* Bit encoded. See below */
	/* 13 */	TOTAL_OXYGEN_SENSORS_2_BANKS, 	/* [A0..A3] == Bank 1, Sensors 1-4. [A4..A7] == Bank 2... */
	/* 14 */	OXYGEN_SENSOR_1_2_BANK,		/* A: Voltage, B: Short term fuel trim	CALC1:[A/200][0-1.275][volts] (if B=0xFF) CALC2:[B/1.28 - 100][-100-99.2] [%] */
	/* 15 */	OXYGEN_SENSOR_2_2_BANK,		/* A: Voltage, B: Short term fuel trim	CALC1:[A/200][0-1.275][volts] (if B=0xFF) CALC2:[B/1.28 - 100][-100-99.2] [%] */
	/* 16 */	OXYGEN_SENSOR_3_2_BANK,		/* A: Voltage, B: Short term fuel trim	CALC1:[A/200][0-1.275][volts] (if B=0xFF) CALC2:[B/1.28 - 100][-100-99.2] [%] */
	/* 17 */	OXYGEN_SENSOR_4_2_BANK, 	/* A: Voltage, B: Short term fuel trim	CALC1:[A/200][0-1.275][volts] (if B=0xFF) CALC2:[B/1.28 - 100][-100-99.2] [%] */
	/* 18 */	OXYGEN_SENSOR_5_2_BANK,		/* A: Voltage, B: Short term fuel trim	CALC1:[A/200][0-1.275][volts] (if B=0xFF) CALC2:[B/1.28 - 100][-100-99.2] [%] */
	/* 19 */	OXYGEN_SENSOR_6_2_BANK,		/* A: Voltage, B: Short term fuel trim	CALC1:[A/200][0-1.275][volts] (if B=0xFF) CALC2:[B/1.28 - 100][-100-99.2] [%] */
	/* 1A */	OXYGEN_SENSOR_7_2_BANK,		/* A: Voltage, B: Short term fuel trim	CALC1:[A/200][0-1.275][volts] (if B=0xFF) CALC2:[B/1.28 - 100][-100-99.2] [%] */
	/* 1B */	OXYGEN_SENSOR_8_2_BANK, 	/* A: Voltage, B: Short term fuel trim	CALC1:[A/200][0-1.275][volts] (if B=0xFF) CALC2:[B/1.28 - 100][-100-99.2] [%] */
	/* 1C */	OBD_STANDARDS,			/* Bit encoded. See below */
	/* 1D */	TOTAL_OXYGEN_SENSORS_4_BANKS,	/* Similar to PID 13, but [A0..A7] == [B1S1, B1S2, B2S1, B2S2, B3S1, B3S2, B4S1, B4S2] */
	/* 1E */	AUXILIARY_INPUT_STATUS,	/* A0 == Power Take Off (PTO) status (1 == active) [A1..A7] not used */
	/* 1F */	RUN_TIME_ENGINE_START,	/*	256A + B [seconds] [0-65,535] */
} obd_pid;

/*
 *	The flags for the supported sensors are set in the obd init
 *
 *	each structure has a function pointer of a common format,
 *	during the init function, the context member that points to a
 *	list of function pointers is indexed and each handler is executed.
 *	
 *	each function pointer returns a pointer to a dynamically allocated
 *	string which has the raw values
 */

struct obd_ctx {
	uint32_t supported_devices;

	// linked list for supported devices
	void *linked_list;
};

int obd_set_supported_ops(void *dat, int bytes, struct obd_ctx *ctx);

struct obd_cmd {
	int obd_pid;
	int resp_bytes;
	int obd_unit;
	const char *obd_cmd;
	int (*handle_data)(void *dat, int bytes, struct obd_ctx *ctx);
} obd_cmds[] = {
	{ PIDS_SUPPORTED_01_02, 				4,	UNITS_NONE,		"0100", &obd_set_supported_ops},
	{ MONITOR_STATUS_SINCE_DTCS_CLEARED,	4,	UNITS_NONE,		"0101", NULL, },
	{ FREEZE_DTC,							2,	UNITS_NONE,		"0102", NULL, },
	{ FUEL_SYSTEM_STATUS,					2,	UNITS_NONE,		"0103", NULL, },
	{ CALCULATED_ENGINE_LOAD,				1,	UNITS_PERCENT,	"0104", NULL, },
	{ ENGINE_COOLANT_TEMP_1,				1,	UNITS_CELCIUS,	"0105", NULL, },
	{ SHORT_TERM_FUEL_TRIM_1,				1,	UNITS_PERCENT,	"0106", NULL, },
	{ LONG_TERM_FUEL_TRIM_1,				1,	UNITS_PERCENT,	"0107", NULL, },
	{ SHORT_TERM_FUEL_TRIM_2,				1,	UNITS_PERCENT,	"0108", NULL, },
	{ LONG_TERM_FUEL_TRIM_2,				1,	UNITS_PERCENT,	"0109", NULL, },
	{ FUEL_PRESSURE,						1,	UNITS_KPA,		"010A", NULL, },
	{ INTAKE_MANIFOLD_ABS_PRESSURE_1,		1,	UNITS_KPA,		"010B", NULL, },
	{ ENGINE_RPM,							2,	UNITS_RPM,		"010C", NULL, },
	{ VEHICLE_SPEED, 						1,	UNITS_KMPH,		"010D", NULL, },
	{ TIMING_ADVANCE,						1,	UNITS_DEG_ADV,	"010E", NULL, },
	{ INTAKE_AIR_TEMP_1,					1,	UNITS_CELCIUS,	"010F", NULL, },
	{ MAF_RATE,							2,	UNITS_GRAMSSEC,	"0110", NULL, },
	{ THROTTLE_POSITION_1,				1,	UNITS_PERCENT,	"0111", NULL, },
	{ COMMANDED_SECONDARY_AIR_STAT, 		1,	UNITS_NONE,		"0112", NULL, },
	{ TOTAL_OXYGEN_SENSORS_2_BANKS, 		1,	UNITS_NONE,		"0113", NULL, },
	{ OXYGEN_SENSOR_1_2_BANK,				2,	UNITS_VOLTS_PERC,	"0114", NULL, },
	{ OXYGEN_SENSOR_2_2_BANK,				2,	UNITS_VOLTS_PERC,	"0115", NULL, },
	{ OXYGEN_SENSOR_3_2_BANK,				2,	UNITS_VOLTS_PERC,	"0116", NULL, },
	{ OXYGEN_SENSOR_4_2_BANK, 			2,	UNITS_VOLTS_PERC,	"0117", NULL, },
	{ OXYGEN_SENSOR_5_2_BANK,				2,	UNITS_VOLTS_PERC,	"0118", NULL, },
	{ OXYGEN_SENSOR_6_2_BANK,				2,	UNITS_VOLTS_PERC,	"0119", NULL, },
	{ OXYGEN_SENSOR_7_2_BANK,				2,	UNITS_VOLTS_PERC,	"011A", NULL, },
	{ OXYGEN_SENSOR_8_2_BANK, 			2,	UNITS_VOLTS_PERC,	"011B", NULL, },
	{ OBD_STANDARDS,						1,	UNITS_NONE,			"011C", NULL, },
	{ TOTAL_OXYGEN_SENSORS_4_BANKS,		1,	UNITS_NONE,			"011E", NULL, },
	{ AUXILIARY_INPUT_STATUS,				1,	UNITS_NONE,			"011F", NULL, },
	{ RUN_TIME_ENGINE_START,				2,	UNITS_SEC,			"0120", NULL, },
};

int obd_set_supported_ops(void *dat, int bytes, struct obd_ctx *ctx) {
	return 0;
}

int set_tty_attr(int fd, int speed, int parity)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);

    if (tcgetattr (fd, &tty) != 0) {
            printf("error %d from tcgetattr", errno);
            return -1;
    }

    cfsetospeed (&tty, speed);
    cfsetispeed (&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
                                    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr (fd, TCSANOW, &tty) != 0) {
            printf("error %d from tcsetattr", errno);
            return -1;
    }
    return 0;
}

void set_tty_blocking (int fd, int should_block)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0) {
            printf("error %d from tggetattr", errno);
            return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr (fd, TCSANOW, &tty) != 0)
            printf("error %d setting term attributes", errno);
}

int elm_response_complete(const char *buf) {
	/*
	 *	console will send a '>' once it is ready for the next command
	 */

	if(strstr(buf, ">"))
		return 1;

	return 0;
}

int elm_write(int device, const char *buf, int len) {
	/*
	 *	write n bytes from the buffer
	 */

	int ret =  0;

	while(len > 0) {

		ret = write(device, buf, len);
		if(ret < 0)
			return -1;

		len -= ret;
	}

	/* why was this here ? */
	//write(device, "\r", 1);

	return 0;

}

int elm_read(int device, char *buf, int len) {
	/*
	 *	read n bytes and put into the buffer
	 */

	char *cur = buf;
	int ret = 0;
	int n;

	while(len) {
		n = read(device, cur, 1);

		cur += n;
		ret += n;
		len -= n;

		*cur = '\0';

		if(elm_response_complete(buf))
			break;

	}

	return 0;
}

int elm_command(int device, const char *cmd, int len, char *buf, int size) {
	/*
	 *	send the command and then get the raw bytes until the '>', or tout
	 */

	if(elm_write(device, cmd, len) < 0)
		return -1;

	return (elm_read(device, buf, size));

}

char* obd_resp(const char *cmd, char *buf) {
	/*
	 *	find the beginning of the response data
	 */

	char pream[3];

	if((strlen(cmd) != 5)) {
		printf("obd_resp: strlen error\n");
		return NULL;
	}

	if(!strncpy(pream, cmd, 2)) {
		printf("obd_resp: strncpy error\n");
		return NULL;
	}

	/* NULL terminate substr, strncpy might already do this */
	pream[2] = '\0';

	/* The obd standard responds with the mode arg added to 0x40 */
	snprintf(pream, 3, "%02lX", 0x40 + strtol(pream, 0, HEX_BASE));

	/* find the beginning of the data */
	return strstr(buf, pream);
}

int obd_command(int device, const char *cmd, void *dat, int size) {
	/*
	 *	send cmd of 4 bytes to device, expect size bytes and put into dat
	 *	
	 *	cmds are NULL terminated
	 *	
	 *	cmds are 4 digit hex values passed as ascii
	 *	
	 */

	/* 
	 *	string ops are just easier and more efficient with dynamic allocation.
	 *	but we really shouldnt need dynamic allocation. all the bytes are held
	 *	in a butter right off the bat, and all the operations we are doing 
	 *	are monotonically increasing through the index. hmmm
	 */

	/*
	 *	return a buffer with the actual (not ascii) hex data in order
	 *	by reference
	 *	
	 *	return a buffer of the response size in bytes with the acutal data
	 */

	char buf[BUF_SIZE];
	char *data;
	int n = 0;
	unsigned char *ret = (unsigned char *) dat;

	printf("obd_command: %s\n", cmd);

	if((strlen(cmd) != OBD_CMD_LEN)) {
		printf("obd_command: incorrect command format\n");
		return -1;
	}

	/* send the OBD cmd to the ELM */
	if(elm_command(device, cmd, OBD_CMD_LEN, buf, size) < 0) {
		printf("obd_command: elm device failure\n");
		return -1;
	}

	/* did the OBD CMD fail? */
	if(strstr(buf, NO_DATA) || strstr(buf, NO_CONNECT)) {
		printf("obd_command: OBD not configured\n");
		return -1;
	}
	
	/* get the pointer to the beginning of the response */
	if(!(data = obd_resp(cmd, buf))) {
		printf("obd_command: unable to find responses\n");
		return -1;
	}

	/* format the response string */
	for(int i = 0; i < strlen(data); i++) {

		if(isspace(data[i]))
			continue;
		else if(isxdigit(data[i])) {
			data[n++] = data[i];
		}
	}

	/* NULL terminate the formatted response string */
	data[n] = '\0';

	if(strlen(data)%2)

		/* incorrect response length */
		return -1;

	printf("string: %s\n", data);

	/* there should only be an even number of hex char's now */
	for(int i = 0; i < strlen(data)/2; i++) {

		/* grab each byte string, convert to int */
		char ch[3]  = {data[i*2], data[i*2 + 1], '\0'};

		printf("%s\n", ch);

		ret[i] = (unsigned char) strtol(ch, 0, HEX_BASE);
	}

	return 0;
}

int initialize_elm(int device) {

	char buf[BUF_SIZE];

	if(elm_command(device, ELM_RESET, sizeof(ELM_RESET), buf, sizeof(buf)) < 0)
		return -1;

	printf("ATZ: %s\n", buf);

	if(elm_command(device, ECHO_OFF, sizeof(ECHO_OFF), buf, sizeof(buf)) < 0)
		if(!strstr(buf, OK))
			return -1;

	printf("ATE0: %s\n", buf);

	if(elm_command(device, SEARCH_BUS, sizeof(SEARCH_BUS), buf, 
		sizeof(buf)) < 0)
		if(!strstr(buf, OK))
			return -1;

	printf("ATSP0: %s\n", buf);


	return 0;
}

int initialize_obd(int device) {

	unsigned char dat[BUF_SIZE];

	if(obd_command(device, GET_DEVS, dat, sizeof(dat)) < 0)
		return -1;

	uint32_t res = (dat[2] << 24) | (dat[3] << 16) | (dat[4] << 8) | (dat[5] << 0);

	printf("%08lX", res);

	// get number of devices, malloc cmd array, put each one in

	struct obd_cmd **cmds = malloc(sizeof(struct obd_cmd));

	/* first command that is supported */
	cmds[0] = &obd_cmds[0];

	for(int i = 0; i < 0x20; i++) {
		if((res << 1) & (1 << 32))
			cmds = reallocarray(sizeof(cmds) + sizeof(struct obd_cmd));


	}

	return 0;
}

char buf[BUF_SIZE];

int main(int argc, char * argv[]) {

	const char *pname = argv[0];

	if(argc != 2) {
		printf("%s: incorrect number of args\n", pname);
		return -1;
	}

	const char *port = argv[1];

	int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
	if(fd < 0) {
		printf("%s: error openning tty\n", pname);
		return -1;
	}

	if(set_tty_attr(fd, B38400, NO_PARITY) < 0) {
		printf("%s: error setting tty attributes\n", pname);
		return -1;
	}

	set_tty_blocking(fd, 0);
	if(initialize_elm(fd) < 0)
		printf("initialize_elm: error\n");

	if(initialize_obd(fd) < 0)
		printf("initialize_obd: error\n");

	return 0;
}

