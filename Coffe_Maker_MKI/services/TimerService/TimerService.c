/*
 * TimerService.c
 *
 * Created: 17.01.2017 22:07:36
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include "TimerService.h"

#define CALLTIME ( 100 )
/* The Timer is now modified to present more than one timer object */
#define NO_OF_TIMER 3

volatile uint32_t TimoutMsCounter[NO_OF_TIMER];



void voTMS_InitTask( void ){
	
	for(uint8_t i=0;i < NO_OF_TIMER ; i++){
		TimoutMsCounter[i]=0;	
	}
	
}

void voTMS_Task( void ){
	
	for(uint8_t i=0;i<NO_OF_TIMER;i++){
		if( TimoutMsCounter[i] > CALLTIME ){
	
			TimoutMsCounter[i] -= CALLTIME;
	
		} else {
	
			TimoutMsCounter[i] = 0;
		
		}
	}
	
}


void voSetTimeOut( uint8_t u8idx, uint32_t TimeOutMS ){
	
	if(u8idx<NO_OF_TIMER){
		TimoutMsCounter[u8idx] = TimeOutMS;
	}
	
}

uint8_t u8GetTimeElapsed( uint8_t u8idx ){
	
	if(u8idx<NO_OF_TIMER){
		if(TimoutMsCounter[u8idx] > 0 ) {
			return TMS_NOT_ELAPSED;
		} else {
			return TMS_ELAPSED;
		}
	} else {
		return TMS_ELAPSED;
	}
	
	
}