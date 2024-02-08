/*
 * heaterservice.c
 *
 * Created: 14.01.2017 22:04:54
 *  Author: calm
 */ 

#include "heaterservice.h"
#include "../../driver/gpio/gpio.h"

static uint16_t u16RunTime;
static HEATERSTATE_t emHeaterState;

void voHSE_TaskInit( void ){
	emHeaterState=HEATER_INVALID;
	u16RunTime=0;
}

void voHSE_Task( void ){
	if(emGetHeaterPowerState()==POWER_OFF){
		u16RunTime=0;
		emHeaterState=HEATER_OFF;
	} else {
		emHeaterState=HEATER_RUNNING;
		if(u16RunTime<UINT16_MAX){
			u16RunTime++;
		}
	}
	
}

HEATERSTATE_t emGetHeaterState( void ){
	return emHeaterState;
}

void voSetHeaterState( HEATERSTATE_t HeaterState){
	if(HeaterState==HEATER_OFF){
		voSetHeaterPowerState(POWER_OFF);
	} else if( HeaterState == HEATER_RUNNING){
		voSetHeaterPowerState(POWER_ON);
	} else {
		/* Do nothing */
	}
}

uint32_t u32HSE_GetCurrentRuntimeMS( void ){
	return u16RunTime*100;
}