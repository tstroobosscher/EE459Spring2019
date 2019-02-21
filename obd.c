/*
 *  USC EE459 Spring 2019 Team 17 - OBD Communication routines
 */

#include <obd.h>



/*

	static function_util_sensor(raw data)
	
	struct sensor {

		// name of the sensor (O2, Fuel Trim, etc.)
		name,

		// AT command used to retrieve sensor data
		command,

		// function used to translate raw dato to process
		function pointer,

		// units of the data if there are any
		units
	}

SENSORS = [
    Sensor("pids"                  , "Supported PIDs"				, "0100" , hex_to_bitstring ,""       ), 
    Sensor("dtc_status"            , "S-S DTC Cleared"				, "0101" , dtc_decrypt      ,""       ),    
    Sensor("dtc_ff"                , "DTC C-F-F"					, "0102" , cpass            ,""       ),      
    Sensor("fuel_status"           , "Fuel System Stat"				, "0103" , cpass            ,""       ),
    Sensor("load"                  , "Calc Load Value"				, "01041", percent_scale    ,""       ),    
    Sensor("temp"                  , "Coolant Temp"					, "0105" , temp             ,"F"      ),
    Sensor("short_term_fuel_trim_1", "S-T Fuel Trim"				, "0106" , fuel_trim_percent,"%"      ),
    Sensor("long_term_fuel_trim_1" , "L-T Fuel Trim"				, "0107" , fuel_trim_percent,"%"      ),
    Sensor("short_term_fuel_trim_2", "S-T Fuel Trim"				, "0108" , fuel_trim_percent,"%"      ),
    Sensor("long_term_fuel_trim_2" , "L-T Fuel Trim"				, "0109" , fuel_trim_percent,"%"      ),
    Sensor("fuel_pressure"         , "FuelRail Pressure"			, "010A" , cpass            ,""       ),
    Sensor("manifold_pressure"     , "Intk Manifold"				, "010B" , intake_m_pres    ,"psi"    ),
    Sensor("rpm"                   , "Engine RPM"					, "010C1", rpm              ,""       ),
    Sensor("speed"                 , "Vehicle Speed"				, "010D1", speed            ,"MPH"    ),
    Sensor("timing_advance"        , "Timing Advance"				, "010E" , timing_advance   ,"degrees"),
    Sensor("intake_air_temp"       , "Intake Air Temp"				, "010F" , temp             ,"F"      ),
    Sensor("maf"                   , "AirFlow Rate(MAF)"			, "0110" , maf              ,"lb/min" ),
    Sensor("throttle_pos"          , "Throttle Position"			, "01111", throttle_pos     ,"%"      ),
    Sensor("secondary_air_status"  , "2nd Air Status"				, "0112" , cpass            ,""       ),
    Sensor("o2_sensor_positions"   , "Loc of O2 sensors"			, "0113" , cpass            ,""       ),
    Sensor("o211"                  , "O2 Sensor: 1 - 1"				, "0114" , fuel_trim_percent,"%"      ),
    Sensor("o212"                  , "O2 Sensor: 1 - 2"				, "0115" , fuel_trim_percent,"%"      ),
    Sensor("o213"                  , "O2 Sensor: 1 - 3"				, "0116" , fuel_trim_percent,"%"      ),
    Sensor("o214"                  , "O2 Sensor: 1 - 4"				, "0117" , fuel_trim_percent,"%"      ),
    Sensor("o221"                  , "O2 Sensor: 2 - 1"				, "0118" , fuel_trim_percent,"%"      ),
    Sensor("o222"                  , "O2 Sensor: 2 - 2"				, "0119" , fuel_trim_percent,"%"      ),
    Sensor("o223"                  , "O2 Sensor: 2 - 3"				, "011A" , fuel_trim_percent,"%"      ),
    Sensor("o224"                  , "O2 Sensor: 2 - 4"				, "011B" , fuel_trim_percent,"%"      ),
    Sensor("obd_standard"          , "OBD Designation"				, "011C" , cpass            ,""       ),
    Sensor("o2_sensor_position_b"  , "Loc of O2 sensor" 			, "011D" , cpass            ,""       ),
    Sensor("aux_input"             , "Aux input status"				, "011E" , cpass            ,""       ),
    Sensor("engine_time"           , "Engine Start MIN"				, "011F" , sec_to_min       ,"min"    ),
    Sensor("engine_mil_time"       , "Engine Run MIL"				, "014D" , sec_to_min       ,"min"    ),
    ]

    struct dtc {

    	// standardized string representation (PXXXX)
		SAE name,

		// string representing error cause
		cause,
    }


 */