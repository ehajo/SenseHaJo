/*
 * eepromFilesySystem.h
 *
 * Created: 14.11.2014 13:46:22
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef EEPROMFILESYSYSTEM_H_
#define EEPROMFILESYSYSTEM_H_

#include <stdbool.h>
#include <inttypes.h>


typedef struct {
	uint16_t u32HeaterTemp;			/* Heater Temp in 1/100 Degree Celsius */
	uint16_t u16WaterLevelFull;		/* Time in Sec for Water */
	uint16_t u16PreWaterLevel;		/* Time in Sec for Pre-Brew */
	uint16_t u16PreWaterWait;		/* Time we wait for the Pre-Brew */
	uint16_t u16Reserved_1;			/* Reserved */
	uint16_t u16Reserved_2;			/* Reserved */
	uint16_t u16Reserved_3;			/* Reserved */
	uint16_t u16CRC16;				/* Checksum */
	} usersettings_t;  /* Will take now 16Byte per Setting */

	/* We use 128Byte for Settings means we will end up with 8 Settings in EEPROM */
	typedef struct {
		uint8_t bfBoilerFilled: 1;
		uint8_t bfCoinMode: 1;
		uint8_t bfLcdAttached: 1;
		uint8_t bfDualHalSensor: 1 ;
		uint8_t bfReserved0: 1;
		uint8_t bfReserved1: 1;
		uint8_t bfReserved2: 1;
		uint8_t bfReserved3: 1;
	} ConfigBit_t;

	typedef struct {
		uint8_t u8AutoPowerOffTime;	/* Time till the Machine auto power off in Minutes */
		uint8_t u8NetFrequency;		/* Will be 50Hz */
		uint8_t u8NetVoltage;		/* Will be 230 AC */
		ConfigBit_t u8ConfigBits;		/*Enables CoinMode for Payed Coffee */
		uint8_t u8coffeelevel;
		uint16_t u16StandbyTemp;
		uint8_t u8SelectedProg;	
		uint16_t u16CRC;
	} systemsettings_t;


	typedef enum {
		USRPRG_HEATERTEMP,
		USRPRG_WATERLEVELFULL,
		USRPRG_PREWATERLEVEL,
		USRPRG_PREWATERWAIT,
	} USERPROGPARAM_t;	


#endif /* EEPROMFILESYSYSTEM_H_ */

	uint16_t u32HeaterTemp;			/* Heater Temp in 1/100 Degree Celsius */
	uint16_t u16WaterLevelFull;		/* Time in Sec for Water */
	uint16_t u16PreWaterLevel;		/* Time in Sec for Pre-Brew */
	uint16_t u16PreWaterWait;		/* Time we wait for the Pre-Brew */
	uint16_t u16Reserved_1;			/* Reserved */
	uint16_t u16Reserved_2;			/* Reserved */
	uint16_t u16Reserved_3;			/* Reserved */
	uint16_t u16CRC16;				/* Checksum */
	