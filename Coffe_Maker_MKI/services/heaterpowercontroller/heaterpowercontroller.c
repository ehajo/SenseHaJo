/*
 * heaterpowercontroller.c
 *
 * Created: 15.01.2017 17:13:37
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#include "../../driver/gpio/gpio.h"
#include "heaterpowercontroller.h"

static volatile uint8_t u8PowerSourced;
static volatile uint8_t u8PowerLimit;
static uint8_t u8CycleCounter;

void voHPC_TaskInit( void ){
	
	u8PowerSourced = 0;
	u8PowerLimit = 0;
	u8CycleCounter=0;
	voSetHeaterPowerState(POWER_OFF);
}

void voHPC_Task( void ){ /* Needs to be called every 10ms to work */
	
	/* After 25 Cycles we reset the PowerSourced Variabel */
	if(u8CycleCounter>=25){
		u8CycleCounter=0;
		u8PowerSourced = 0;
	}
	
	if(u8PowerSourced < u8PowerLimit ){
		voSetHeaterPowerState(POWER_ON);
		
		if(u8PowerSourced < UINT8_MAX - 4){
			u8PowerSourced+=4; /* We add 4% to the Power we deliverde by now */
		
		}
		
	} else {
		voSetHeaterPowerState(POWER_OFF);
	}

	u8CycleCounter++;
}


void voHPC_SetPowerOutput( uint8_t u8Power ){
	
	if(u8Power > 100 ){
		u8PowerLimit = 100;
	} else {
		u8PowerLimit = u8Power;
	}
}

uint8_t u8HPC_GetPowerOutput( void ){
	
	return u8PowerLimit;
}