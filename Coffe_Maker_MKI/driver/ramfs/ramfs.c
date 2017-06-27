/*
 * ramfs.c
 *
 * Created: 24.03.2017 17:28:04
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

/* this is for all values we store in ram */

#include "ramfs.h"
#include <util/crc16.h>
#include <string.h>

volatile usersettings_t tUSBProgSettings;

void voRFS_Init(){
	tUSBProgSettings.u16CRC16=0;
	tUSBProgSettings.u16PreWaterLevel=0;
	tUSBProgSettings.u16PreWaterWait=0;
	tUSBProgSettings.u16Reserved_1=0;
	tUSBProgSettings.u16Reserved_2=0;
	tUSBProgSettings.u16Reserved_3=0;
	tUSBProgSettings.u16WaterLevelFull=0;
	tUSBProgSettings.u32HeaterTemp=0;	
	/* This will produce a invalid usb prog */
	
}

bool boRFS_WriteUserSettings(usersettings_t* Inputsettings ){
	/* we copy the new settings into ram */
	/* first we need to check if the crc is okay */
	//Okay now we need to Build our CRC
	bool boReturnval=false;
	uint8_t* ptr=(uint8_t*)Inputsettings;
	uint16_t crc_calc=0xFFFF;
	uint16_t lenght=sizeof(systemsettings_t);
	for(uint16_t i=0;i<(lenght);i++)
	{
		crc_calc = _crc_ccitt_update(crc_calc,*ptr);
		ptr++;
	}
	
	if(crc_calc == Inputsettings->u16CRC16){
		//And now everything must back to the EEPROM
		boReturnval = true;
		memcpy((void*)&tUSBProgSettings,(const void*)Inputsettings,sizeof(systemsettings_t));	
		
	}
	
	return boReturnval;
}

bool boRFS_ReadUserSettings(usersettings_t* UserSettings ){
	
	bool boReturnval=false;
	uint8_t* ptr=(uint8_t*)&tUSBProgSettings;
	uint16_t crc_calc=0xFFFF;
	uint16_t lenght=sizeof(systemsettings_t);
	for(uint16_t i=0;i<(lenght);i++)
	{
		crc_calc = _crc_ccitt_update(crc_calc,*ptr);
		ptr++;
	}
	
	if(crc_calc == tUSBProgSettings.u16CRC16){
		//And now everything must back to the EEPROM
		boReturnval = true;
		memcpy(UserSettings, (void*)&tUSBProgSettings, sizeof(systemsettings_t));
	}
	
	return boReturnval;
	
}

uint32_t boRFS_ReadUserProgrammParam(USERPROGPARAM_t param ){
	uint32_t u32ReturnVal=0;
	switch( param ){
		case USRPRG_HEATERTEMP:{
				u32ReturnVal=tUSBProgSettings.u32HeaterTemp;
		} break;
		
		case USRPRG_PREWATERLEVEL:{
			u32ReturnVal=tUSBProgSettings.u16PreWaterLevel;
		} break;
		
		case USRPRG_PREWATERWAIT:{
			u32ReturnVal=tUSBProgSettings.u16PreWaterWait;
		} break;
		
		case USRPRG_WATERLEVELFULL:{
			u32ReturnVal=tUSBProgSettings.u16WaterLevelFull;
		} break;
		
		default:{
			
		} break;
	}
	
	return u32ReturnVal;
}

void boRFS_UpdateUserProgramm(USERPROGPARAM_t param, uint32_t Value ){
	
		switch( param ){
			case USRPRG_HEATERTEMP:{
				
			} break;
			
			case USRPRG_PREWATERLEVEL:{
				
			} break;
			
			case USRPRG_PREWATERWAIT:{
				
			} break;
			
			case USRPRG_WATERLEVELFULL:{
				
			} break;
			
			default:{
				
			} break;
		}
		
		
}