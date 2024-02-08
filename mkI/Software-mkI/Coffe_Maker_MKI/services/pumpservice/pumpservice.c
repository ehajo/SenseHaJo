/*
 * pumpservice.c
 *
 * Created: 14.01.2017 22:05:11
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include "pumpservice.h"
#include "../../driver/gpio/gpio.h"

static uint16_t u16PumpRuntime;
static PUMPSTATE_t emPumpState;
void voPSE_TaskInit( void ){
	u16PumpRuntime=0;
	emPumpState=PUMP_INVALID;
}

void voPSE_Task( void ){ /* Needs to be called every 100ms */
	if(emGetPumpPowerState()==POWER_OFF){
		u16PumpRuntime=0;
		emPumpState=PUMP_OFF;
	} else {
		
		if(u16PumpRuntime<UINT16_MAX){
			u16PumpRuntime++;
		}
	}
	
}

PUMPSTATE_t emGetPumpState( void ){
	return emPumpState;
}

void voSetPumpState( PUMPSTATE_t PumpState){
	/* We accept here only Running and Off */
	if(PumpState==PUMP_RUNNING){
		voSetPumpPowerState(POWER_ON);
	} else if (PumpState == PUMP_OFF){
		u16PumpRuntime=0;
		voSetPumpPowerState(POWER_OFF);
	} else {
		/* Do nothing */
	}
}

uint32_t u32PSE_GetCurrentRuntimeMS( void ){
	return u16PumpRuntime*100;
}
