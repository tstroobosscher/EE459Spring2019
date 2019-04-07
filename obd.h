/*
 *	USC EE459 Spring 2019 Team 17 - OBD Header
 */

#ifndef OBD_H
#define OBD_H

#define BUF_SIZE 64
#define MAX_TRIALS 64
#define HEX_BASE 16
#define PREAM_CONST 0x40
#define OBD_CMD_LEN 4 + 1 /* +1 for carriage return */
#define OBD_PID_REGISTER_SIZE 0x20 

const char *NO_DATA = "NO DATA";
const char *OK = "OK";
const char *NO_CONNECT = "UNABLE TO CONNECT";

const char *ELM_RESET = "ATZ\r";
const char *ECHO_OFF = "ATE0\r";
const char *SEARCH_BUS = "ATSP0\r";
const char *GET_DEVS = "0100\r";

static const enum {
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
	/* 20 */	PIDS_SUPPORTED_21_40,	/* Bit encoded [A7..D0] == [PID $21..PID $40] See below */
	/* 21 */	DISTANCE_WITH_MIL_ON,	/* 256A + B [km] [0-65,535] */
	/* 22 */	FUEL_PRESSURE_REL_TO_MANIFOLD,	/* 0.079(256A + B) [kPa] [0-5177.265] */
	/* 23 */	FUEL_RAIL_PRESSURE,		/* 10(256A + B) [kPa] [0-655,350]	kPa */
	/* 24 */	OXYGEN_SENSOR_1_4_BANK_VOLTS,	/*2/65536 (256A+B), 8/65536 (256C + D); AB: Fuel–Air Equivalence Ratio, CD: Voltage; [0-2], [0-8]; [volts], [ratio]; */
	/* 25 */	OXYGEN_SENSOR_2_4_BANK_VOLTS,	/*2/65536 (256A+B), 8/65536 (256C + D); AB: Fuel–Air Equivalence Ratio, CD: Voltage; [0-2], [0-8]; [volts], [ratio]; */
	/* 26 */	OXYGEN_SENSOR_3_4_BANK_VOLTS,	/*2/65536 (256A+B), 8/65536 (256C + D); AB: Fuel–Air Equivalence Ratio, CD: Voltage; [0-2], [0-8]; [volts], [ratio]; */
	/* 27 */	OXYGEN_SENSOR_4_4_BANK_VOLTS,	/*2/65536 (256A+B), 8/65536 (256C + D); AB: Fuel–Air Equivalence Ratio, CD: Voltage; [0-2], [0-8]; [volts], [ratio]; */
	/* 28 */	OXYGEN_SENSOR_5_4_BANK_VOLTS,	/*2/65536 (256A+B), 8/65536 (256C + D); AB: Fuel–Air Equivalence Ratio, CD: Voltage; [0-2], [0-8]; [volts], [ratio]; */
	/* 29 */	OXYGEN_SENSOR_6_4_BANK_VOLTS,	/*2/65536 (256A+B), 8/65536 (256C + D); AB: Fuel–Air Equivalence Ratio, CD: Voltage; [0-2], [0-8]; [volts], [ratio]; */
	/* 2A */	OXYGEN_SENSOR_7_4_BANK_VOLTS,	/*2/65536 (256A+B), 8/65536 (256C + D); AB: Fuel–Air Equivalence Ratio, CD: Voltage; [0-2], [0-8]; [volts], [ratio]; */
	/* 2B */	OXYGEN_SENSOR_8_4_BANK_VOLTS,	/*2/65536 (256A+B), 8/65536 (256C + D); AB: Fuel–Air Equivalence Ratio, CD: Voltage; [0-2], [0-8]; [volts], [ratio]; */
	/* 2C */	COMMANDED_EGR, 			/* (100/128)A [%] [0-100] */
	/* 2D */	EGR_ERROR,				/* (100/128)A - 100, [%] [-100 - 99.2]	*/
	/* 2E */	COMMANDED_EVAPORATIVE_PURGE, /* (100/255)A [5] [0-100] */
	/* 2F */	FUEL_TANK_LEVEL_INPUT,	/* (100/255)A [%] [0-100] */
	/* 30 */	WARM_UPS_SINCE_CODES_CLEARED,	/* A [count] [0-255] */
	/* 31 */	DISTANCE_TRAVELED_SINCE_CODES_CLEARED,	/* (256A + B) [km] [0-65,535] */
	/* 32 */	EVAP_SYSTEM_VAPOR_PRESSURE_1,	/* (256A + B)/4 (2's comp signed) [Pa] [-8,192-8191.75] */
	/* 33 */	ABS_BAROMETRIC_PRESSURE, /* A [kPa] [0-255] */
	/* 34 */	OXYGEN_SENSOR_1_4_BANK_CURR,	/* (2/65536)(256A + B), (C + D/256 - 128); AB: Fuel–Air Equivalence Ratio, CD: Current; [0-2] [-128-128]; [ratio] [mA] */
	/* 35 */	OXYGEN_SENSOR_2_4_BANK_CURR, 	/* (2/65536)(256A + B), (C + D/256 - 128); AB: Fuel–Air Equivalence Ratio, CD: Current; [0-2] [-128-128]; [ratio] [mA] */
	/* 36 */	OXYGEN_SENSOR_3_4_BANK_CURR,	/* (2/65536)(256A + B), (C + D/256 - 128); AB: Fuel–Air Equivalence Ratio, CD: Current; [0-2] [-128-128]; [ratio] [mA] */
	/* 37 */	OXYGEN_SENSOR_4_4_BANK_CURR,	/* (2/65536)(256A + B), (C + D/256 - 128); AB: Fuel–Air Equivalence Ratio, CD: Current; [0-2] [-128-128]; [ratio] [mA] */
	/* 38 */	OXYGEN_SENSOR_5_4_BANK_CURR,	/* (2/65536)(256A + B), (C + D/256 - 128); AB: Fuel–Air Equivalence Ratio, CD: Current; [0-2] [-128-128]; [ratio] [mA] */
	/* 39 */	OXYGEN_SENSOR_6_4_BANK_CURR,	/* (2/65536)(256A + B), (C + D/256 - 128); AB: Fuel–Air Equivalence Ratio, CD: Current; [0-2] [-128-128]; [ratio] [mA] */
	/* 3A */	OXYGEN_SENSOR_7_4_BANK_CURR,	/* (2/65536)(256A + B), (C + D/256 - 128); AB: Fuel–Air Equivalence Ratio, CD: Current; [0-2] [-128-128]; [ratio] [mA] */
	/* 3B */	OXYGEN_SENSOR_8_4_BANK_CURR,	/* (2/65536)(256A + B), (C + D/256 - 128); AB: Fuel–Air Equivalence Ratio, CD: Current; [0-2] [-128-128]; [ratio] [mA] */
	/* 3C */	CAT_TEMP_BANK_1_SENSOR_1,		/* (256A+B)/10 - 40 [°C] [-40-6,513.5] */
	/* 3D */	CAT_TEMP_BANK_2_SENSOR_1,		/* (256A+B)/10 - 40 [°C] [-40-6,513.5] */
	/* 3E */	CAT_TEMP_BANK_1_SENSOR_2,		/* (256A+B)/10 - 40 [°C] [-40-6,513.5] */
	/* 3F */	CAT_TEMP_BANK_2_SENSOR_2, 		/* (256A+B)/10 - 40 [°C] [-40-6,513.5] */
	/* 40 */	PIDS_SUPPORTED_41_60,			/*	Bit encoded [A7..D0] == [PID $41..PID $60] See below */
	/* 41 */	MONITOR_STATUS_THIS_DRIVE_CYCLE,	/* Bit encoded. See below */
	/* 42 */	CONTROL_MODULE_VOLTAGE,		/* (256A + B)/1000 [volts] [0-65.535] */
	/* 43 */	ABSOLUTE_LOAD,	/* (100/255)(256A + B) [%] [0-25,700] */
	/* 44 */	COMMANDED_FUEL_AIR_RATIO,	/* (2/65536)(256A + B) [ratio] [0-2] */
	/* 45 */	RELATIVE_THROTTLE_POSITION,	/* (100/255)A [%] [0-100] */
	/* 46 */	AMBIENT_AIR_TEMP,	/* A - 40 [°C] [-40-215] */
	/* 47 */	ABSOLUTE_THROTTLE_POSITION_B,	/* (100/255)A [%] [0-100] */
	/* 48 */	ABSOLUTE_THROTTLE_POSITION_C,	/* (100/255)A [%] [0-100] */
	/* 49 */	ACCELORATOR_PEDAL_POSITION_D,	/* (100/255)A [%] [0-100] */
	/* 4A */	ACCELORATOR_PEDAL_POSITION_E,	/* (100/255)A [%] [0-100] */
	/* 4B */	ACCELORATOR_PEDAL_POSITION_F,	/* (100/255)A [%] [0-100] */
	/* 4C */	COMMANDED_THROTTLE_ACTUATOR,	/* (100/255)A [%] [0-100] */
	/* 4D */	TIME_RUN_WITH_MIL_ON,	/* (256A + B) [minutes] [0-65,535] */
	/* 4E */	TIME_SINCE_TROUBLE_CODES_CLEARED, /* (256A + B) [minutes] [0-65,535] */

	/*
	 *	SAE J1979 Only defines 00-4E, 4F-FF are experimental
	 */

	/* 4F */	MAX_FAR_O2V_O2C_ABS_MAN_PRES, 	/* A, B, C, D*10 [ratio, V, mA, kPa] [0, 0, 0, 0 - 255, 255, 255, 2550] */
	/* 50 */	MAX_MAF_RATE, 	/* A*10, B, C, and D are reserved for future use [g/s] [0-2550]	*/
	/* 51 */	FUEL_TYPE,	/* From fuel type table see below */
	/* 52 */	ETHANOL_FUEL_PERCENT,	/* (100/255)A [%] [0-100] */
	/* 53 */	ABS_EVAP_SYS_VAPOR_PRES,	/*	(256A + B)/200 [kPa] [0-327.675] */
	/* 54 */	EVAP_SYSTEM_VAPOR_PRESSURE_2,	/* (256A + B)-32,767 [Pa] [-32,767-32,768] */
	/* 55 */	SECONDARY_O2_TRIM_SHORT_TERM_BANK_1_3,	/* (100/128)A - 100, (100/128)B - 100; [%, %] [-100-99.2, -100-99.2] */
	/* 56 */	SECONDARY_O2_TRIM_LONG_TERM_BANK_1_3, 	/* (100/128)A - 100, (100/128)B - 100; [%, %] [-100-99.2, -100-99.2] */
	/* 57 */	SECONDARY_O2_TRIM_SHORT_TERM_BANK_2_4,	/* (100/128)A - 100, (100/128)B - 100; [%, %] [-100-99.2, -100-99.2] */
	/* 58 */	SECONDARY_O2_TRIM_LONG_TERM_BANK_2_4,	/* (100/128)A - 100, (100/128)B - 100; [%, %] [-100-99.2, -100-99.2] */
	/* 59 */	FUEL_RAIL_ABS_PRESSURE, 	/* 10(256A + B) [kPa] [0-655,350] */
	/* 5A */	RELATIVE_ACCEL_PEDAL_POS,	/* (100/255)A [%] [0-100] */
	/* 5B */	HYBRID_BATTERY_PACK_LIFE,	/* (100/255)A [%] [0-100] */
	/* 5C */	ENGINE_OIL_TEMP,	/* A - 40 [°C]	[-40-210] */
	/* 5D */	FUEL_INJECTION_TIMING,	/* ((256A + B)/128) -210 [°advance] [-210.00-301.992] */
	/* 5E */	ENGINE_FUEL_RATE_1,	/* (256A + B)/20 [L/h] [0-3212.75] */
	/* 5F */	EMISSIONS_REQUIREMENTS, 	/* Bit Encoded, See below */
	/* 60 */	PIDS_SUPPORTED_61_80,	/* Bit encoded [A7..D0] == [PID $61..PID $80] See below */
	/* 61 */	ENGINE_DEMAND_PERCENT_TORQUE, 	/* A-125 [%] [-125-130] */
	/* 62 */	ENGINE_ACTUAL_PERCENT_TORQUE,	/* A-125 [%] [-125-130]	*/
	/* 63 */	ENGINE_REFERENCE_TORQUE,	/* 256A + B [Nm] [0-65,535] */
	/* 64 */	ENGINE_DATA_PERCENT_TORQUE,	/* A-125 [%] [-125-130] A-125 Idle, B-125 Engine point 1, C-125 Engine point 2, D-125 Engine point 3, E-125 Engine point 4
	/* 65 */	AUX_IO_SUPPORTED, 	/* Bit Encoded, See below */
	/* 66 */	MAF_SENSOR, /*  */				
	/* 67 */	ENGINE_COOLANT_TEMP_2,				
	/* 68 */	INTAKE_AIR_TEMP_2,				
	/* 69 */	COMMANDED_EGR_AND_EGR_ERR,				
	/* 6A */	DIESEL_INTAKE_AIR_CONTROL,				
	/* 6B */	EXHAUST_RECIRC_TEMP,
	/* 6C */	THROTTLE_CONTROL,
	/* 6D */	FUEL_PRESSURE_CONTROL,
	/* 6E */	INJECTION_PRESSURE_CONTROL,
	/* 6F */	TURBO_INLET_PRESSURE,
	/* 70 */	BOOST_PRESSURE_CONTROL,
	/* 71 */	VAR_TURBO_CONTROL,
	/* 72 */	WASTEGATE_CONTROL,
	/* 73 */	EXHAUST_PRESSURE,
	/* 74 */	TURBO_RPM,
	/* 75 */	TURBO_TEMP_1,
	/* 76 */	TURBO_TEMP_2,
	/* 77 */	INTERCOOLER_TEMP,
	/* 78 */	EXHAUST_TEMP_BANK_1,
	/* 79 */	EXHAUST_TEMP_BANK_2
	/* 7A */	DIESEL_PARTICULATE_FILTER_1,
	/* 7B */	DIESEL_PARTICULATE_FILTER_2,
	/* 7C */	DIESEL_PARTICULATE_FILTER_TEMP,
	/* 7D */	NOX_CONTROL_STATUS,
	/* 7E */	PM_CONTROL_STATUS,
	/* 7F */	ENGINE_RUN_TIME,
	/* 80 */	PIDS_SUPPORTED_81_A0	/* Bit encoded [A7..D0] == [PID $81..PID $A0] See below */
	/* 81 */	ENGINE_RUN_TIME_AUX_EMMISSION_1,
	/* 82 */	ENGINE_RUN_TIME_AUX_EMMISSION_2,
	/* 83 */	NOX_SENSOR,
	/* 84 */	MANIFOLD_SURFACE_TEMP,
	/* 85 */	NOX_REAGENT,
	/* 86 */	PM_SENSOR,
	/* 87 */	INTAKE_MANIFOLD_ABS_PRESSURE_2,
	/* 88 */	SCR_INDUCE_SYSTEM,
	/* 89 */	RUN_TIME_FOR_AECD_11_15,
	/* 8A */	RUN_TIME_FOR_AECD_16_20,
	/* 8B */	DIESEL_AFTERTREATMENT,
	/* 8C */	OXYGEN_WIDEBAND,
	/* 8D */	THROTTLE_POSITION_2,
	/* 8E */	ENGINE_FRICTION_PERCENT_TORQUE,
	/* 8F */	PM_SENSOR_BANK_1_2,
	/* 90 */	OBD_INFORMATION_1,
	/* 91 */	OBD_INFORMATION_2,
	/* 92 */	FUEL_SYSTEM_CONTROL,
	/* 93 */	OBD_COUNTERS,
	/* 94 */	NOX_WARNGING,
	/* 98 */	EXHAUST_TEMP_1,
	/* 99 */	EXHAUST_TEMP_2,
	/* 9A */	HYBRID_EV_DATA,
	/* 9B */	DIESEL_EXHAUST_FLUID,
	/* 9C */	OXYGEN_SENSOR_DATA,
	/* 9D */	ENGINE_FUEL_RATE_2
	/* 9E */	EXHAUST_FLOW_RATE,
	/* 9F */	FUEL_SYSTEM_PERCENT,
	/* A0 */	PIDS_SUPPORTED_A1_C0,
	/* A1 */	NOX_SENSOR_CORRECTED_DATA,
	/* A2 */	CYLINDER_FUEL_RATE,
	/* A3 */	EVAP_SYSTEM_VAPOR_PRESSURE_3,
	/* A4 */	TRANSMISSION_GEAR,
	/* A5 */	DIESEL_EXHAUST_FLUID_TREATMENT,
	/* A6 */	ODOMETER,
} __attribute__ ((unused)) obd_pid;

/* callbacks */
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

#endif
