/*
 * ledlightningservice.c
 *
 * Created: 29.01.2017 12:29:48
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include "ledlightningservice.h"
#include "../../driver/gpio/gpio_typedef.h"
#include "../../driver/gpio/gpio.h"

/* We got here currently 3LEDs */
typedef struct{
	
	LED_BLINK_Frq_t CurrenFreq;
	LED_BLINK_Frq_t PrevFreq;
	emLEDSATATE InitialState;	
	
} LED_STATUS_t;

LED_STATUS_t Led_State[3];

volatile uint8_t u8_10Hz_Counter;
volatile uint8_t u8_5Hz_Counter;
volatile uint8_t u8_2Hz_Counter;
volatile uint8_t u8_1Hz_Counter;


void LLS_voInitTask( void ){
	
	uint8_t u8Elements = sizeof(Led_State)/sizeof(LED_STATUS_t);
	
	for(uint8_t u8i=0;u8i<u8Elements;u8i++){
		
		Led_State[u8i].CurrenFreq=LED_BLINK_OFF;
		Led_State[u8i].PrevFreq=LED_BLINK_OFF;
		Led_State[u8i].InitialState=LED_OFF;
	
	}
	
	u8_10Hz_Counter=0;
	u8_1Hz_Counter=0;
	u8_2Hz_Counter=0;
	u8_5Hz_Counter=0;
	
	/* Init done for now */
}

void LLS_voTask( void ){
	/* We need to Check what to do */
	uint8_t u8Elements = sizeof(Led_State)/sizeof(LED_STATUS_t);
		
	/* Keep track of the Timing */
	u8_10Hz_Counter++;
	u8_5Hz_Counter++;
	u8_2Hz_Counter++;
	u8_1Hz_Counter++;
	
	if(u8_10Hz_Counter>0){
		u8_10Hz_Counter=0;
	}
	
	if(u8_5Hz_Counter>1){
		u8_5Hz_Counter=0;
	}
	
	if(u8_2Hz_Counter>4){
		u8_2Hz_Counter=0;
	}
	
	
	if(u8_1Hz_Counter>9){
		u8_1Hz_Counter=0;
	}
	
	for(uint8_t u8i=0;u8i<u8Elements;u8i++){
		
		if(Led_State[u8i].CurrenFreq != Led_State[u8i].PrevFreq){
			Led_State[u8i].PrevFreq = Led_State[u8i].CurrenFreq;
			if(Led_State[u8i].CurrenFreq!=LED_BLINK_OFF){
				Led_State[u8i].InitialState=emGetLedState(u8i);
			} else {
				voSetLedState(u8i,Led_State[u8i].InitialState);
			}
		}
		
		switch(Led_State[u8i].CurrenFreq){
			
			case LED_BLINK_10Hz:{
				if(u8_10Hz_Counter==0){
					voSetLedState(u8i,LED_TOGGLE);
				}
			
			}
				
			case LED_BLINK_5Hz:{
				if(u8_5Hz_Counter==0){
					voSetLedState(u8i,LED_TOGGLE);
				}
			} break;

			case LED_BLINK_2Hz:{
				if(u8_2Hz_Counter==0){
					voSetLedState(u8i,LED_TOGGLE);
				}
			} break;

			case LED_BLINK_1Hz:{
				if(u8_1Hz_Counter==0){
					voSetLedState(u8i,LED_TOGGLE);
				}
			} break;							
			
			case LED_STATIC_OFF:{
				voSetLedState(u8i,LED_OFF);
			}break;
			
			case LED_STATIC_ON:{
				voSetLedState(u8i,LED_ON);
			}break;
			
			default:{
				/* We do nothing here as feading is not implimentend */
				
			}

		}
		
	}
	
	
}

void LLS_voSetLedLightning( LED_t emLed , LED_BLINK_Frq_t emFreq ){
	
	/* we need to MAP the enum to the LED Index here */
	
	switch(emLed){
		
		case LED_ONECUP:{
			/* This should be 0 */
			if(Led_State[0].CurrenFreq!=emFreq){
				Led_State[0].CurrenFreq=emFreq;
			}
			
		} break;
		
		
		case LED_POWER:{
			/* This is 1 */
			if(Led_State[1].CurrenFreq!=emFreq){
				Led_State[1].CurrenFreq=emFreq;
			}
			
		}break;
		
		
		case LED_TWOCUP:{
			/* And this should be 2 */
			if(Led_State[2].CurrenFreq!=emFreq){
				Led_State[2].CurrenFreq=emFreq;
			}
			
			
		}break;
		
		default:{
			
			/* Do nothing as we don't know the LED */
			
		}break;
		
	}
	
	
	
	
}
LED_BLINK_Frq_t LLS_getLedLightning( LED_t emLed){
				
		/* we need to MAP the enum to the LED Index here */
		
		LED_BLINK_Frq_t emReturn=LED_BLINK_OFF;
		
		switch(emLed){
			
			case LED_ONECUP:{
				/* This should be 0 */
				emReturn = Led_State[0].CurrenFreq;
				
			} break;
			
			
			case LED_POWER:{
				/* This is 1 */
				emReturn = Led_State[1].CurrenFreq;
				
			}break;
			
			
			case LED_TWOCUP:{
				/* And this should be 2 */
				emReturn = Led_State[2].CurrenFreq;
				
				
			}break;
			
			default:{
				
				/* Do nothing as we don't know the LED */
				
			}break;
			
		}
	
	return emReturn;
}