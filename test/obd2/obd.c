/*
 *	USC EE459 Spring 2019 Team 17 - OBD2 Test code
 */

/*
 *	February 26, 2019
 *		So i need to write the routine that is going to grab the data
 *bytes from the OBD2 response efficiently. All responses are preambled with
 *		0x40 or'ed with the command number (we saw something extremely
 *		similar with the SD data-link layer code). All bytes before that
 *		preamble are ignored, and each command has an expected number og
 *bytes to listen for
 *
 *		Also i think its important to keep layering in mind. we probably
 *		shouldnt be doing string comparison or byte filtering at all at
 *the lowest level. we just need to copy the bytes into a buffer of an
 *		appropriate size, and let the upper layers deal with the
 *filtering
 */

/*
 *	Wow, so declaring variables after string buffers can cause issues when
 *	the strings are expected to be NULL terminated, switch the order of i
 *	and buf and the program will run incorrectly since printf is looking
 *	for a NULL char (when using printf())
 */

/*
 *  B2300 supported codes
 * 
 *  OXYGEN SENSOR 2 2 BANK
 *  OXYGEN SENSOR 1 2 BANK
 *  TOTAL OXYGEN SENSORS 2 BANKS
 *  THROTTLE POSITION 1
 *  MAF RATE
 *  INTAKE AIR TEMP 1
 *  TIMING ADVANCE
 *  VEHICLE SPEED
 *  ENGINE RPM
 *  INTAKE MANIFOLD ABS PRESSURE 1
 *  LONG TERM FUEL TRIM 1
 *  SHORT TERM FUEL TRIM 1
 *  ENGINE COOLANT TEMP 1
 *  CALCULATED ENGINE LOAD
 *  FUEL SYSTEM STATUS
 *  MONITOR STATUS SINCE DTCS CLEARED
*/

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define NO_PARITY 0

#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

#define BUF_SIZE 64
#define MAX_TRIALS 64
#define HEX_BASE 16
#define PREAM_CONST 0x40
#define OBD_CMD_LEN 4  + 1
#define OBD_PID_REGISTER_SIZE 0x20 

static const enum obd_unit {
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

struct node {
  void *data;

  struct node *next;
};

void push_head(struct node **head, void *data, size_t bytes) {

  struct node *new_node = (struct node *)malloc(sizeof(struct node));

  new_node->data = malloc(bytes);
  new_node->next = *head;

  for (int i = 0; i < bytes; i++) {
    /* dereference the value of the void pointer cast to char *, copy */
    *(char *)(new_node->data + i) = *(char *)(data + i);
  }

  (*head) = new_node;
}

static const enum obd_pid {
  /* 00 */ PIDS_SUPPORTED_01_02, /* [01 - 20] Bit encoded [A7..D0] == [PID
                                    $01..PID $20] See below */
  /* 01 */ MONITOR_STATUS_SINCE_DTCS_CLEARED, /* since DTCs cleared. (Includes
                                                 malfunction indicator lamp
                                                 (MIL) status and number of
                                                 DTCs.) Bit encoded. See below
                                               */
  /* 02 */ FREEZE_DTC,                        /* Error codes, See below */
  /* 03 */ FUEL_SYSTEM_STATUS,                /* Bit encoded. See below */
  /* 04 */ CALCULATED_ENGINE_LOAD,            /* A/2.55 [%] [0-100] */
  /* 05 */ ENGINE_COOLANT_TEMP_1,             /* A - 40 [°C]	[-40-215] */
  /* 06 */ SHORT_TERM_FUEL_TRIM_1, /* A/128 - 100 [%] [-100 (Reduce Fuel: Too
                                      Rich-99.2 (Add Fuel: Too Lean)] */
  /* 07 */ LONG_TERM_FUEL_TRIM_1,  /* A/128 - 100 [%] [-100 (Reduce Fuel: Too
                                      Rich-99.2 (Add Fuel: Too Lean)] */
  /* 08 */ SHORT_TERM_FUEL_TRIM_2, /* A/128 - 100 [%] [-100 (Reduce Fuel: Too
                                      Rich-99.2 (Add Fuel: Too Lean)] */
  /* 09 */ LONG_TERM_FUEL_TRIM_2,  /* A/128 - 100 [%] [-100 (Reduce Fuel: Too
                                      Rich-99.2 (Add Fuel: Too Lean)] */
  /* 0A */ FUEL_PRESSURE,          /* 3A [kPa]	[0-765] */
  /* 0B */ INTAKE_MANIFOLD_ABS_PRESSURE_1, /* A [kPa] [0-255] */
  /* 0C */ ENGINE_RPM,          /* (256A + B)/4 [rpm] [0-16,383.75] */
  /* 0D */ VEHICLE_SPEED,       /* A [km/h] [0-255] */
  /* 0E */ TIMING_ADVANCE,      /* A/2 - 64 [° before TDC] [-64-63.5] */
  /* 0F */ INTAKE_AIR_TEMP_1,   /* A - 40 [°C] [-40-215] */
  /* 10 */ MAF_RATE,            /* 0	655.35	grams/sec */
  /* 11 */ THROTTLE_POSITION_1, /* 0	100	%	{\displaystyle {\tfrac
                                   {100}{255}}A} {\displaystyle {\tfrac
                                   {100}{255}}A} */
  /* 12 */ COMMANDED_SECONDARY_AIR_STAT, /* Bit encoded. See below */
  /* 13 */ TOTAL_OXYGEN_SENSORS_2_BANKS, /* [A0..A3] == Bank 1, Sensors 1-4.
                                            [A4..A7] == Bank 2... */
                                         /* 14 */
  OXYGEN_SENSOR_1_2_BANK,                /* A: Voltage, B: Short term fuel trim
                                            CALC1:[A/200][0-1.275][volts] (if B=0xFF)
                                            CALC2:[B/1.28 - 100][-100-99.2] [%] */
                                         /* 15 */
  OXYGEN_SENSOR_2_2_BANK,                /* A: Voltage, B: Short term fuel trim
                                            CALC1:[A/200][0-1.275][volts] (if B=0xFF)
                                            CALC2:[B/1.28 - 100][-100-99.2] [%] */
                                         /* 16 */
  OXYGEN_SENSOR_3_2_BANK,                /* A: Voltage, B: Short term fuel trim
                                            CALC1:[A/200][0-1.275][volts] (if B=0xFF)
                                            CALC2:[B/1.28 - 100][-100-99.2] [%] */
                                         /* 17 */
  OXYGEN_SENSOR_4_2_BANK,                /* A: Voltage, B: Short term fuel trim
                                            CALC1:[A/200][0-1.275][volts] (if B=0xFF)
                                            CALC2:[B/1.28 - 100][-100-99.2] [%] */
                                         /* 18 */
  OXYGEN_SENSOR_5_2_BANK,                /* A: Voltage, B: Short term fuel trim
                                            CALC1:[A/200][0-1.275][volts] (if B=0xFF)
                                            CALC2:[B/1.28 - 100][-100-99.2] [%] */
                                         /* 19 */
  OXYGEN_SENSOR_6_2_BANK,                /* A: Voltage, B: Short term fuel trim
                                            CALC1:[A/200][0-1.275][volts] (if B=0xFF)
                                            CALC2:[B/1.28 - 100][-100-99.2] [%] */
                                         /* 1A */
  OXYGEN_SENSOR_7_2_BANK,                /* A: Voltage, B: Short term fuel trim
                                            CALC1:[A/200][0-1.275][volts] (if B=0xFF)
                                            CALC2:[B/1.28 - 100][-100-99.2] [%] */
                                         /* 1B */
  OXYGEN_SENSOR_8_2_BANK,                /* A: Voltage, B: Short term fuel trim
                                            CALC1:[A/200][0-1.275][volts] (if B=0xFF)
                                            CALC2:[B/1.28 - 100][-100-99.2] [%] */
  /* 1C */ OBD_STANDARDS,                /* Bit encoded. See below */

  /* 1D */ TOTAL_OXYGEN_SENSORS_4_BANKS, /* Similar to PID 13, but [A0..A7] ==
                                      [B1S1, B1S2, B2S1, B2S2, B3S1, B3S2, B4S1,
                                      B4S2] */
  /* 1E */ AUXILIARY_INPUT_STATUS, /* A0 == Power Take Off (PTO) status (1 ==
                                      active) [A1..A7] not used */
  /* 1F */ RUN_TIME_ENGINE_START,  /*	256A + B [seconds] [0-65,535] */
  /* 20 */  PIDS_SUPPORTED_21_40, /* Bit encoded [A7..D0] == [PID $21..PID $40] See below */
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
  struct node *linked_list;
};

char *obd_set_supported_ops(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_engine_load(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_coolant_temp(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_fuel_trim(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_fuel_pressure(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_manifold_pressure(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_engine_rpm(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_vehicle_speed(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_timing_advance(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_air_temp(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_maf_rate(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_throttle_pos(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_oxygen_sensors(void *dat, int bytes, struct obd_ctx *ctx);
char *obd_get_oxygen_data(void *dat, int bytes, struct obd_ctx *ctx);

struct obd_cmd {
  /* i think pids are limited to 256 */
  unsigned char obd_pid;
  int resp_bytes;
  const char *obd_units;
  const char *obd_cmd;
  const char *cmd_str;
  char *(*handle_data)(void *dat, int bytes, struct obd_ctx *ctx);
} obd_cmds[] = {
    {
        PIDS_SUPPORTED_01_02,
        4,
        /* UNITS_NONE, */
        NULL,
        "0100\r",
        "PIDS SUPPORTED 01 02",
        &obd_set_supported_ops,
    },
    {
        MONITOR_STATUS_SINCE_DTCS_CLEARED,
        4,
        /* UNITS_NONE, */
        NULL,
        "0101\ready",
        "MONITOR STATUS SINCE DTCS CLEARED",
        NULL,
    },
    {
        FREEZE_DTC,
        2,
        /* UNITS_NONE, */
        NULL,
        "0102\r",
        "FREEZE DTC",
        NULL,
    },
    {
        FUEL_SYSTEM_STATUS,
        2,
        /* UNITS_NONE, */
        NULL,
        "0103\r",
        "FUEL SYSTEM STATUS",
        NULL,
    },
    {
        CALCULATED_ENGINE_LOAD,
        1,
        /* UNITS_PERCENT, */
        "%",
        "0104\r",
        "CALCULATED ENGINE LOAD",
        &obd_get_engine_load,
    },
    {
        ENGINE_COOLANT_TEMP_1,
        1,
        /* UNITS_CELCIUS, */
        "° Celsius",
        "0105\r",
        "ENGINE COOLANT TEMP 1",
        &obd_get_coolant_temp,
    },
    {
        SHORT_TERM_FUEL_TRIM_1,
        1,
        /* UNITS_PERCENT, */
        "%",
        "0106\r",
        "SHORT TERM FUEL TRIM 1",
        &obd_get_fuel_trim,
    },
    {
        LONG_TERM_FUEL_TRIM_1,
        1,
        /* UNITS_PERCENT, */
        "%",
        "0107\r",
        "LONG TERM FUEL TRIM 1",
        &obd_get_fuel_trim,
    },
    {
        SHORT_TERM_FUEL_TRIM_2,
        1,
        /* UNITS_PERCENT, */
        "%",
        "0108\r",
        "SHORT TERM FUEL TRIM 2",
        &obd_get_fuel_trim,
    },
    {
        LONG_TERM_FUEL_TRIM_2,
        1,
        /* UNITS_PERCENT, */
        "%",
        "0109\r",
        "LONG TERM FUEL TRIM 2",
        &obd_get_fuel_trim,
    },
    {
        FUEL_PRESSURE,
        1,
        /* UNITS_KPA, */
        "kPa",
        "010A\r",
        "FUEL PRESSURE",
        &obd_get_fuel_pressure,
    },
    {
        INTAKE_MANIFOLD_ABS_PRESSURE_1,
        1,
        /* UNITS_KPA, */
        "kPa",
        "010B\r",
        "INTAKE MANIFOLD ABS PRESSURE 1",
        &obd_get_manifold_pressure,
    },
    {
        ENGINE_RPM,
        2,
        /* UNITS_RPM, */
        "rpm",
        "010C\r",
        "ENGINE RPM",
        &obd_get_engine_rpm,
    },
    {
        VEHICLE_SPEED,
        1,
        /* UNITS_KMPH, */
        "km/h",
        "010D\r",
        "VEHICLE SPEED",
        &obd_get_vehicle_speed,
    },
    {
        TIMING_ADVANCE,
        1,
        /* UNITS_DEG_ADV, */
        "° before TDC",
        "010E\r",
        "TIMING ADVANCE",
        &obd_get_timing_advance,
    },
    {
        INTAKE_AIR_TEMP_1,
        1,
        /* UNITS_CELCIUS, */
        "° Celsius",
        "010F\r",
        "INTAKE AIR TEMP 1",
        &obd_get_air_temp,
    },
    {
        MAF_RATE,
        2,
        /* UNITS_GRAMSSEC, */
        "g/s",
        "0110\r",
        "MAF RATE",
        &obd_get_maf_rate,
    },
    {
        THROTTLE_POSITION_1,
        1,
        /* UNITS_PERCENT, */
        "%",
        "0111\r",
        "THROTTLE POSITION 1",
        &obd_get_throttle_pos,
    },
    {
        COMMANDED_SECONDARY_AIR_STAT,
        1,
        UNITS_NONE,
        "0112\r",
        "COMMANDED SECONDARY AIR STAT",
        NULL,
    },
    {
        TOTAL_OXYGEN_SENSORS_2_BANKS,
        1,
        /* UNITS_NONE, */
        "Sensor(s)",
        "0113\r",
        "TOTAL OXYGEN SENSORS 2 BANKS",
        &obd_get_oxygen_sensors,
    },
    {
        OXYGEN_SENSOR_1_2_BANK,
        2,
        /* UNITS_VOLTS_PERC, */
        "Volts",
        "0114\r",
        "OXYGEN SENSOR 1 2 BANK",
        &obd_get_oxygen_data,
    },
    {
        OXYGEN_SENSOR_2_2_BANK,
        2,
        /* UNITS_VOLTS_PERC, */
        "Volts",
        "0115\r",
        "OXYGEN SENSOR 2 2 BANK",
        &obd_get_oxygen_data,
    },
    {
        OXYGEN_SENSOR_3_2_BANK,
        2,
        /* UNITS_VOLTS_PERC, */
        "Volts",
        "0116\r",
        "OXYGEN SENSOR 3 2 BANK",
        &obd_get_oxygen_data,
    },
    {
        OXYGEN_SENSOR_4_2_BANK,
        2,
        /* UNITS_VOLTS_PERC, */
        "Volts",
        "0117\r",
        "OXYGEN SENSOR 4 2 BANK",
        &obd_get_oxygen_data,
    },
    {
        OXYGEN_SENSOR_5_2_BANK,
        2,
        /* UNITS_VOLTS_PERC, */
        "Volts",
        "0118\r",
        "OXYGEN SENSOR 5 2 BANK",
        &obd_get_oxygen_data,
    },
    {
        OXYGEN_SENSOR_6_2_BANK,
        2,
        /* UNITS_VOLTS_PERC, */
        "Volts",
        "0119\r",
        "OXYGEN SENSOR 6 2 BANK",
        &obd_get_oxygen_data,
    },
    {
        OXYGEN_SENSOR_7_2_BANK,
        2,
        /* UNITS_VOLTS_PERC, */
        "Volts",
        "011A\r",
        " OXYGEN SENSOR 7 2 BANK",
        &obd_get_oxygen_data,
    },
    {
        OXYGEN_SENSOR_8_2_BANK,
        2,
        /* UNITS_VOLTS_PERC, */
        "Volts",
        "011B\r",
        "OXYGEN SENSOR 8 2 BANK",
        &obd_get_oxygen_data,
    },
    {
        OBD_STANDARDS,
        1,
        UNITS_NONE,
        "011C\r",
        "OBD STANDARDS",
        NULL,
    },
    {
        /* lol what car has 4 banks */
        TOTAL_OXYGEN_SENSORS_4_BANKS,
        1,
        UNITS_NONE,
        "011D\r",
        "TOTAL OXYGEN SENSORS 4 BANKS",
        NULL,
    },
    {
        AUXILIARY_INPUT_STATUS,
        1,
        UNITS_NONE,
        "011E\r",
        "AUXILIARY INPUT STATUS",
        NULL,
    },
    {
        RUN_TIME_ENGINE_START,
        2,
        UNITS_SEC,
        "011F\r",
        "RUN TIME ENGINE START",
        NULL,
    },
};

int obd_print_cmd(struct obd_cmd *cmd) { printf("%s\n", cmd->cmd_str); return 0;}

char *obd_get_engine_load(void *dat, int bytes, struct obd_ctx *ctx) {

  unsigned char *res = (unsigned char *) dat;

  float calc = res[2]/2.55;

  unsigned char *ret;

  asprintf(&ret, "%f", calc);

  return ret;

}

char *obd_get_coolant_temp(void *dat, int bytes, struct obd_ctx *ctx) {
  /* can be negative */
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2] - 40;

  unsigned char *ret;

  asprintf(&ret, "%d", calc);

  return ret;
}

char *obd_get_fuel_trim(void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  float calc = res[2]/1.28 - 100;

  unsigned char *ret;

  asprintf(&ret, "%f", calc);

  return ret;
}

char *obd_get_fuel_pressure(void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2] * 3;

  unsigned char *ret;

  asprintf(&ret, "%d", calc);

  return ret;
}

char *obd_get_manifold_pressure(void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2];

  unsigned char *ret;

  asprintf(&ret, "%d", calc);

  return ret;
}

char *obd_get_engine_rpm(void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = (res[2] * 256 + res[3])/4;

  unsigned char *ret;

  asprintf(&ret, "%d", calc);

  return ret;
}

char *obd_get_vehicle_speed(void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2];

  unsigned char *ret;

  asprintf(&ret, "%d", calc);

  return ret;
}

char *obd_get_timing_advance(void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2]/2 - 64;

  unsigned char *ret;

  asprintf(&ret, "%d", calc);

  return ret;
}

char *obd_get_air_temp(void *dat, int bytes, struct obd_ctx *ctx){
  /* can be negative */
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2] - 40;

  unsigned char *ret;

  asprintf(&ret, "%d", calc);

  return ret;
}

char *obd_get_maf_rate(void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = (res[2] * 256 + res[3])/100;

  unsigned char *ret;

  asprintf(&ret, "%d", calc);

  return ret;
}

char *obd_get_throttle_pos(void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  float calc = res[2]/2.55;

  unsigned char *ret;

  asprintf(&ret, "%f", calc);

  return ret;
}

char *obd_get_oxygen_sensors(void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int bank_1 = (res[2] >> 0) & 0x0F;
  int bank_2 = (res[2] >> 4) & 0x0F;

  int calc = bank_1 + bank_2;

  unsigned char *ret;

  asprintf(&ret, "%d", calc);

  return ret;
}

char *obd_get_oxygen_data(void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  /* this one can have multiple responses if permitted, % fuel trim and voltage */
  float calc = res[2]/200;

  unsigned char *ret;

  asprintf(&ret, "%f", calc);

  return ret;
}

char *obd_set_supported_ops(void *dat, int bytes, struct obd_ctx *ctx) {

  uint32_t res = *(uint32_t *)dat;

  ctx->linked_list = NULL;

  /* skip the 0 pid, not in the return data */
  for(int i = 1; i < bytes * 8; i++)

    /* 
     *  the msb in the res corresponds to the lowest order pid, not the 00 pid
     * 
     *  offset for the 0 pid
     *  
     *  The 1 pid corresponds to the 32nd bitmask on the res32, which would be
     *  shifted over 31 times, 0x20 - pid
     */
    if (res & (1 << (0x20 - obd_cmds[i].obd_pid)))

      push_head(&ctx->linked_list, (struct obd_cmd *) &obd_cmds[i], sizeof(struct obd_cmd));

  return 0;
}

int set_tty_attr(int fd, int speed, int parity) {
  struct termios tty;
  memset(&tty, 0, sizeof tty);

  if (tcgetattr(fd, &tty) != 0) {
    printf("error %d from tcgetattr", errno);
    return -1;
  }

  cfsetospeed(&tty, speed);
  cfsetispeed(&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
  // disable IGNBRK for mismatched speed tests; otherwise receive break
  // as \000 chars
  tty.c_iflag &= ~IGNBRK; // disable break processing
  tty.c_lflag = 0;        // no signaling chars, no echo,
                          // no canonical processing
  tty.c_oflag = 0;        // no remapping, no delays
  tty.c_cc[VMIN] = 0;     // read doesn't block
  tty.c_cc[VTIME] = 5;    // 0.5 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

  tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls,
                                     // enable reading
  tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    printf("error %d from tcsetattr", errno);
    return -1;
  }
  return 0;
}

void set_tty_blocking(int fd, int should_block) {
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if (tcgetattr(fd, &tty) != 0) {
    printf("error %d from tggetattr", errno);
    return;
  }

  tty.c_cc[VMIN] = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

  if (tcsetattr(fd, TCSANOW, &tty) != 0)
    printf("error %d setting term attributes", errno);
}

int elm_response_complete(const char *buf) {
  /*
   *	console will send a '>' once it is ready for the next command
   */

  if (strstr(buf, ">"))
    return 1;

  return 0;
}

int elm_write(int device, const char *buf, int len) {
  /*
   *	write n bytes from the buffer
   */

  int ret = 0;

  while (len > 0) {

    ret = write(device, buf, len);
    if (ret < 0)
      return -1;

    len -= ret;
  }

  /* why was this here ? */
  // write(device, "\r", 1);

  return 0;
}

int elm_read(int device, char *buf, int len) {
  /*
   *	read n bytes and put into the buffer
   */

  char *cur = buf;
  int ret = 0;
  int n;

  while (len) {
    n = read(device, cur, 1);

    cur += n;
    ret += n;
    len -= n;

    *cur = '\0';

    if (elm_response_complete(buf))
      break;
  }

  return 0;
}

int elm_command(int device, const char *cmd, int len, char *buf, int size) {
  /*
   *	send the command and then get the raw bytes until the '>', or tout
   */

  if (elm_write(device, cmd, len) < 0)
    return -1;

  return (elm_read(device, buf, size));
}

char *obd_resp(const char *cmd, char *buf) {
  /*
   *	find the beginning of the response data
   */

  char pream[3];

  if ((strlen(cmd) != OBD_CMD_LEN)) {
    printf("obd_resp: strlen error\n");
    return NULL;
  }

  if (!strncpy(pream, cmd, 2)) {
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

int obd_fmt_resp(char *str) {

  /* format the response string */
  int n = 0;
  for (int i = 0; i < strlen(str); i++) {

    if (isspace(str[i]))

      /* skip the spaces */
      continue;

    /* should only be processing hex data */
    else if (isxdigit(str[i])) {
      str[n++] = str[i];
    }
  }

  /* NULL terminate the formatted response string */
  str[n] = '\0';

  /* check for evenness */
  if ((strlen(str) % 2))

    /* incorrect response length */
    return -1;

  return 0;
}

int obd_command(int device, const char *cmd, void *dat, int size) {
  /*
   *	send cmd of 4 bytes to device, expect size bytes and put into dat
   *
   *	cmds are NULL terminated
   *
   *	cmds are 4 digit hex values passed as ascii
   *
   *	string ops are just easier and more efficient with dynamic allocation.
   *	but we really shouldnt need dynamic allocation. all the bytes are held
   *	in a butter right off the bat, and all the operations we are doing
   *	are monotonically increasing through the index. hmmm
   *
   *	return a buffer with the actual (not ascii) hex data in order
   *	by reference
   *
   *	return a buffer of the response size in bytes with the acutal data
   */

  /* BUF_SIZE seems arbitrary, can be a varying number of whitespaces */
  char buf[BUF_SIZE];
  char *data;
  unsigned char *ret = (unsigned char *)dat;

  if ((strlen(cmd) != OBD_CMD_LEN)) {
    printf("obd_command: incorrect command format, cmd len = %d\n", strlen(cmd));
    return -1;
  }

  /* send the OBD cmd to the ELM, put the response in the buffer */
  if (elm_command(device, cmd, OBD_CMD_LEN, buf, size) < 0) {
    printf("obd_command: elm device failure\n");
    return -1;
  }

  /* did the OBD CMD fail? */
  if (strstr(buf, NO_DATA) || strstr(buf, NO_CONNECT)) {
    printf("obd_command: OBD not configured\n");
    return -1;
  }

  /* get the pointer to the beginning of the response */
  if (!(data = obd_resp(cmd, buf))) {
    printf("obd_command: unable to find responses\n");
    return -1;
  }

  /* place all the hex chars into sequential order with no whitespace */
  if(obd_fmt_resp(data) < 0 ) {
    printf("obd_command: unable to format response\n");
    return -1;
  }

   /* there should only be an even number of hex char's now */
  for (int i = 0; i < strlen(data)/2; i++) {

    /* grab each byte string, convert to unsigned char */
    char ch[3] = {data[i * 2], data[i * 2 + 1], '\0'};

    //printf("%s\n", ch);

    /* strlen(data)/2 should also be equal to size NO! size is the max buf*/
    ret[i] = (unsigned char) strtol(ch, 0, HEX_BASE);
  }

  return 0;
}

void dump_list(struct node *ptr, void (*fptr)(void *)) {
  while (ptr) {
    (*fptr)(ptr->data);
    ptr = ptr->next;
  }
}

int initialize_elm(int device) {

  char buf[BUF_SIZE];

  if (elm_command(device, ELM_RESET, sizeof(ELM_RESET), buf, sizeof(buf)) < 0)
    return -1;

  printf("ATZ: %s\n", buf);

  if (elm_command(device, ECHO_OFF, sizeof(ECHO_OFF), buf, sizeof(buf)) < 0)
    if (!strstr(buf, OK))
      return -1;

  printf("ATE0: %s\n", buf);

  if (elm_command(device, SEARCH_BUS, sizeof(SEARCH_BUS), buf, sizeof(buf)) < 0)
    if (!strstr(buf, OK))
      return -1;

  printf("ATSP0: %s\n", buf);

  return 0;
}

int initialize_obd(int device, struct obd_ctx *ctx) {

  unsigned char dat[BUF_SIZE];

  if (obd_command(device, obd_cmds[PIDS_SUPPORTED_01_02].obd_cmd, dat, sizeof(dat)) < 0)
    return -1;

  /*
   *  build the 32 bit response from the char array, 0 and 1 are preamble
   *  This is not going to work on 8 bit architecture, need to assign to larger
   *  buffers first
   */
  uint32_t res = (dat[2] << 24) | (dat[3] << 16) | (dat[4] << 8) | (dat[5] << 0);

  obd_set_supported_ops(&res, sizeof(res), ctx);

  dump_list(ctx->linked_list, obd_print_cmd);

  return 0;
}

char buf[BUF_SIZE];

struct obd_ctx obd;

int main(int argc, char *argv[]) {

  const char *pname = argv[0];

  if (argc != 2) {
    printf("%s: incorrect number of args\n", pname);
    return -1;
  }

  const char *port = argv[1];

  int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0) {
    printf("%s: error openning tty\n", pname);
    return -1;
  }

  if (set_tty_attr(fd, B38400, NO_PARITY) < 0) {
    printf("%s: error setting tty attributes\n", pname);
    return -1;
  }

  set_tty_blocking(fd, 0);

  if (initialize_elm(fd) < 0)
    printf("initialize_elm: error\n");

  if (initialize_obd(fd, &obd) < 0)
    printf("initialize_obd: error\n");

  while(1) {
    struct node *ptr = obd.linked_list;

    while(ptr) {

      /* the compiler needs offset information to dereference void pointers */
      struct obd_cmd *cmd = (struct obd_cmd *) ptr->data;

      if(cmd->handle_data != NULL) {
        if(obd_command(fd, cmd->obd_cmd, buf, BUF_SIZE) < 0) {
          ptr = ptr->next;
          continue;
        }
        char *res = (*(cmd->handle_data))(buf, cmd->resp_bytes, &obd);
        printf("%s = %s %s\n", cmd->cmd_str, res, cmd->obd_units);
        free(res);
      }
      ptr = ptr->next;
    }
    sleep(2);
  }

  return 0;
}
