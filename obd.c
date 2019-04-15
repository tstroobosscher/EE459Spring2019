/*
 *  USC EE459 Spring 2019 Team 17 - OBD Communication Routines
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obd.h"
#include "list.h"
#include "elm.h"
#include "utils.h"

static const char *NO_DATA = "NO DATA";
static const char *NO_CONNECT = "UNABLE TO CONNECT";
static const char *GET_DEVS = "0100\r";

struct obd_cmd obd_cmds[] = {
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
        "0101\r",
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
        /* UNITS_NONE, */
        NULL,
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
        /* UNITS_NONE, */
        NULL,
        "011C\r",
        "OBD STANDARDS",
        NULL,
    },
    {
        /* lol what car has 4 banks */
        TOTAL_OXYGEN_SENSORS_4_BANKS,
        1,
        /* UNITS_NONE, */
        NULL,
        "011D\r",
        "TOTAL OXYGEN SENSORS 4 BANKS",
        NULL,
    },
    {
        AUXILIARY_INPUT_STATUS,
        1,
        /* UNITS_NONE, */
        NULL,
        "011E\r",
        "AUXILIARY INPUT STATUS",
        NULL,
    },
    {
        RUN_TIME_ENGINE_START,
        2,
        /* UNITS_SEC, */
        "s",
        "011F\r",
        "RUN TIME ENGINE START",
        NULL,
    },
};

int obd_print_cmd(struct obd_cmd *cmd) { UART_DBG(cmd->cmd_str); UART_DBG("\r\n"); return 0;}

void obd_get_engine_load(char* buf, void *dat, int bytes, struct obd_ctx *ctx) {

  unsigned char *res = (unsigned char *) dat;

  float calc = res[2]/2.55;

  sprintf(buf, "%.2f", calc);
}

void obd_get_coolant_temp(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {
  /* can be negative */
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2] - 40;

  sprintf(buf, "%d", calc);
}

void obd_get_fuel_trim(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  float calc = res[2]/1.28 - 100;

  sprintf(buf, "%.2f", calc);
}

void obd_get_fuel_pressure(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2] * 3;

  sprintf(buf, "%d", calc);
}

void obd_get_manifold_pressure(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2];

  sprintf(buf, "%d", calc);
}

void obd_get_engine_rpm(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = (res[2] * 256 + res[3])/4;

  sprintf(buf, "%d", calc);
}

void obd_get_vehicle_speed(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2];

  char *ret;

  sprintf(buf, "%d", calc);
}

void obd_get_timing_advance(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2]/2 - 64;

  sprintf(buf, "%d", calc);
}

void obd_get_air_temp(char *buf, void *dat, int bytes, struct obd_ctx *ctx){
  /* can be negative */
  unsigned char *res = (unsigned char *) dat;

  int calc = res[2] - 40;

  sprintf(buf, "%d", calc);
}

void obd_get_maf_rate(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int calc = (res[2] * 256 + res[3])/100;

  sprintf(buf, "%d", calc);
}

void obd_get_throttle_pos(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  float calc = res[2]/2.55;

  sprintf(buf, "%.2f", calc);
}

void obd_get_oxygen_sensors(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  int bank_1 = (res[2] >> 0) & 0x0F;
  int bank_2 = (res[2] >> 4) & 0x0F;

  int calc = bank_1 + bank_2;

  sprintf(buf, "%d", calc);
}

void obd_get_oxygen_data(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {
  unsigned char *res = (unsigned char *) dat;

  /* this one can have multiple responses if permitted, % fuel trim and voltage */
  float calc = res[2]/200;

  sprintf(buf, "%.2f", calc);
}

void obd_set_supported_ops(char *buf, void *dat, int bytes, struct obd_ctx *ctx) {

  uint32_t res = *(uint32_t *)dat;

  ctx->linked_list = NULL;

  uint32_t mask = 0x80000000;

  /* skip the 0 pid, not in the return data */
  for(int i = 1; i < 0x20; i++) {

    /* 
     *  the msb in the res corresponds to the lowest order pid, not the 00 pid
     * 
     *  offset for the 0 pid
     *  
     *  The 1 pid corresponds to the 32nd bitmask on the res32, which would be
     *  shifted over 31 times, 0x20 - pid
     */

    if(res & mask) {
      list_push_head(&ctx->linked_list, (struct obd_cmd *) &obd_cmds[i], sizeof(struct obd_cmd));
    }

    mask = (mask >> 1);
  }
}

char *obd_resp(const char *cmd, char *buf) {
  /*
   *    find the beginning of the response data
   */

  char pream[5];

  if ((strlen(cmd) != OBD_CMD_LEN)) {
    DBG("obd_resp: strlen error\n");
    return NULL;
  }

  if (!strncpy(pream, cmd, 2)) {
    DBG("obd_resp: strncpy error\n");
    return NULL;
  }

  /* NULL terminate substr, strncpy might already do this */
  pream[2] = '\0';

  /* The obd standard responds with the mode arg added to 0x40 */
  snprintf(pream, 3, "%02lX", 0x40 + strtol(pream, 0, 16));

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

int obd_command(struct obd_ctx *obd, const char *cmd, void *dat, int size) {
  /*
   *    send cmd of 4 bytes to device, expect size bytes and put into dat
   *
   *    cmds are NULL terminated
   *
   *    cmds are 4 digit hex values passed as ascii
   *
   *    string ops are just easier and more efficient with dynamic allocation.
   *    but we really shouldnt need dynamic allocation. all the bytes are held
   *    in a butter right off the bat, and all the operations we are doing
   *    are monotonically increasing through the index. hmmm
   *
   *    return a buffer with the actual (not ascii) hex data in order
   *    by reference
   *
   *    return a buffer of the response size in bytes with the acutal data
   */

  /* BUF_SIZE seems arbitrary, can be a varying number of whitespaces */
  char buf[64];
  char *data;
  unsigned char *ret = (unsigned char *)dat;

  if ((strlen(cmd) != OBD_CMD_LEN)) {
    DBG("obd_command: incorrect command format\n");
    return -1;
  }

  /* send the OBD cmd to the ELM, put the response in the buffer */
  if (elm_command(obd->elm, cmd, strlen(cmd), buf, 64) < 0) {
    DBG("obd_command: elm device failure\n");
    return -1;
  }

  /* did the OBD CMD fail? */
  if (strstr(buf, NO_DATA) || strstr(buf, NO_CONNECT)) {
    DBG("obd_command: OBD not configured\n");
    return -1;
  }

  /* get the pointer to the beginning of the response */
  if (!(data = obd_resp(cmd, buf))) {
    DBG("obd_command: unable to find responses\n");
    return -1;
  }

  /* place all the hex chars into sequential order with no whitespace */
  if(obd_fmt_resp(data) < 0 ) {
    DBG("obd_command: unable to format response\n");
    return -1;
  }

   /* there should only be an even number of hex char's now */
  for (int i = 0; i < strlen(data)/2; i++) {

    /* grab each byte string, convert to unsigned char */
    char ch[3] = {data[i * 2], data[i * 2 + 1], '\0'};

    /* strlen(data)/2 should also be equal to size NO! size is the max buf*/
    ret[i] = (unsigned char) strtol(ch, 0, HEX_BASE);
  }

  return 0;
}

int initialize_obd(struct elm_ctx *elm, struct obd_ctx *ctx) {

  ctx->elm = elm;

  uint8_t dat[BUF_SIZE];

  if (obd_command(ctx, obd_cmds[PIDS_SUPPORTED_01_02].obd_cmd, dat, sizeof(dat)) < 0)
    return -1;

  /*
   *  build the 32 bit response from the char array, 0 and 1 are preamble
   *  This is not going to work on 8 bit architecture, need to assign to larger
   *  buffers first
   */

  uint32_t new_buf[BUF_SIZE];
  for(int i = 0; i < ARRAY_SIZE(dat); i++) {
    DBG("obd: dat: 0x%x\r", dat[i]);
    new_buf[i] = dat[i];
  }

  uint32_t res = (new_buf[2] << 24) | (new_buf[3] << 16) | (new_buf[4] << 8) | (new_buf[5] << 0);

  DBG("obd: res = %d\n", res);

  obd_set_supported_ops(NULL, &res, sizeof(res), ctx);

  list_dump(ctx->linked_list, (void *) obd_print_cmd);

  return 0;
}
