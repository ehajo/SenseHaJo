/*
 * healthmonitor.c
 *
 * Created: 24.03.2017 19:08:45
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


/* 

	This is the helathmonito witch must be cyclic called and watches over all system parameter 
		- will check if the poiler if activated draws power 
		- will check if the pump if activated draws power
		- will check if the waterlevel is okay 
*/

/* faults are reported back in a fault bitfield */
#include "../waterlevelmessurment/waterlevelmessurment.h"
#include "../boilertemp/boilertemp.h"
#include "../healthmonitor/healthmonitor.h"

volatile FaultAndWarning_t tCollectedFaults;

void voHMon_Init( void ){
	
	tCollectedFaults.BoilerDrawsNoCurrentFault=0;
	tCollectedFaults.OverTemperaturFault=0;
	tCollectedFaults.Reserved=0;
	tCollectedFaults.WaterLevelLowFault=0;
	tCollectedFaults.WaterLevelLowWarning=0;
	
	
		
}

void voHMon_Task( void ){
	
	
	tCollectedFaults.BoilerDrawsNoCurrentFault=0;
	tCollectedFaults.OverTemperaturFault=0;
	tCollectedFaults.Reserved=0;
	tCollectedFaults.WaterLevelLowFault=0;
	tCollectedFaults.WaterLevelLowWarning=0;
	
	
	/* We now need to check the errors */
	/* First  */
	
	emWaterLevel emWaterLevel = WLM_emGetLevel();
	switch (emWaterLevel){
		case 	LEVEL_OKAY:{
			tCollectedFaults.WaterLevelLowFault=0;
			tCollectedFaults.WaterLevelLowWarning=0;
		} break;
		
		case LEVEL_MIN:{
			tCollectedFaults.WaterLevelLowWarning=1;
			tCollectedFaults.WaterLevelLowFault=0;
		} break;
		
		case LEVEL_NOT_OKAY: {
			tCollectedFaults.WaterLevelLowFault=1;
			tCollectedFaults.WaterLevelLowWarning=1;
		} break;
		
		case LEVEL_INVALID: {
			tCollectedFaults.WaterLevelLowFault=1;
			tCollectedFaults.WaterLevelLowWarning=1;
		}break;
		
	}
	
	/* Next is to check if we have a overtemp means more than 100 Degree C */
	if(  s16BLTMP_GetBoilerTemp() > (int16_t)(10000) ){
		tCollectedFaults.OverTemperaturFault=1;
	} else {
		tCollectedFaults.OverTemperaturFault=0;
	}
	
	/* Next is to check if we have a current flow at a given level to avoid a overheating and faulty triac */
	tCollectedFaults.BoilerDrawsNoCurrentFault=0; /* Currently not implemented */
	/*   */
	
	/* Check is finished we need now let the other parts process the errors we found */
	
}

FaultAndWarning_t HMon_tGetFaultAndWarning( void ){
	
	return tCollectedFaults;
	
}