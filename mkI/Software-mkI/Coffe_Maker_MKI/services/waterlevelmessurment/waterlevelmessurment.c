/*
 * waterlevelmessurment.c
 *
 * Created: 14.01.2017 15:05:48
 *  Author: calm
 */

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */ 

#include "waterlevelmessurment.h"
#include "../../driver/gpio/gpio.h"

#define LOW_LEVEL_SENSOR ( 0 )
#define HIGH_LEVEL_SENSOR ( 1 )

#define DEBOUNCE_HIGH ( 10 )
#define DEBOUNCE_LOW ( 3 )

volatile emWaterLevel emDetectedWarterLevel = LEVEL_INVALID;

volatile emSWITCHSTATE emSwitchDebouncedLow=SWITCH_RELEASED;
volatile emSWITCHSTATE emSwitchDebouncedHigh=SWITCH_RELEASED;

volatile uint8_t u8switch_high_debounce=0;
volatile uint8_t u8switch_low_debounce=0;

void WLM_voTask(){
	/* We need to debounce the switch in the "valid" direction" */
	/* Also we do this in the Empty direction as well but not that hard */
	
	emSWITCHSTATE emLowLevelSensor=SWITCH_RELEASED;
	emSWITCHSTATE emHighLevelSensor=SWITCH_RELEASED;
	
	emLowLevelSensor = emGetHallSwitchState( LOW_LEVEL_SENSOR );
	emHighLevelSensor = emGetHallSwitchState( HIGH_LEVEL_SENSOR );
	
	if(emSwitchDebouncedLow!=emLowLevelSensor){
		if(emLowLevelSensor==SWITCH_PRESSED){
			if(u8switch_low_debounce < DEBOUNCE_HIGH){
				u8switch_low_debounce++;
			} else {
				emSwitchDebouncedLow=emLowLevelSensor; /* SW will detect a transition if */
			}
		} else {
			if(u8switch_low_debounce < DEBOUNCE_LOW){
				u8switch_low_debounce++;
				} else {
				emSwitchDebouncedLow=emLowLevelSensor; /* SW will detect a transition if */
			}
		}
	} else {
		u8switch_low_debounce=0;
	}
	
	if(emSwitchDebouncedHigh!=emHighLevelSensor){
		if(emHighLevelSensor==SWITCH_PRESSED){
			if(u8switch_high_debounce < DEBOUNCE_HIGH){
				u8switch_high_debounce++;
				} else {
				emHighLevelSensor=emHighLevelSensor; /* SW will detect a transition if */
				}
			} else {
				if(u8switch_low_debounce < DEBOUNCE_LOW){
					u8switch_high_debounce++;
				} else {
					emHighLevelSensor=emHighLevelSensor; /* SW will detect a transition if */
				}
			}
		} else {
			u8switch_high_debounce=0;
	}
	
	/* This may save some RAM but is hard to read */
	
	/* If we have only one sensor inside we will only use one sensor */
	emSwitchDebouncedHigh = emSwitchDebouncedLow ;
	
	if( ( emSwitchDebouncedHigh == SWITCH_PRESSED )  && ( emSwitchDebouncedLow == SWITCH_PRESSED) ) {
		/* Wasser ist ausreichen für einen Becher mit Maximalbefüllung */
		emDetectedWarterLevel = LEVEL_OKAY;
		
	} else if( ( emSwitchDebouncedHigh == SWITCH_RELEASED )  && ( emSwitchDebouncedLow == SWITCH_PRESSED) ){ 
		/* Wasser ist ausreichen für einen Becher mit Minimalbefüllung */
		emDetectedWarterLevel = LEVEL_MIN;
	} else if( ( emSwitchDebouncedHigh == SWITCH_RELEASED )  && ( emSwitchDebouncedLow == SWITCH_RELEASED) ) {
		/* Wasser ist ausreichen für einen Becher mit Minimalbefüllung */
		emDetectedWarterLevel = LEVEL_NOT_OKAY;
	} else {
		/* REED Broken */
		/* Wasser ist ausreichen für einen Becher mit Minimalbefüllung */
		emDetectedWarterLevel = LEVEL_INVALID;
	}
	
	
}

emWaterLevel WLM_emGetLevel(){
	return emDetectedWarterLevel;
}
