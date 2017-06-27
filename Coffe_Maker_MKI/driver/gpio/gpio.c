/*
 * gpio.c
 *
 * Created: 20.09.2013 14:53:27
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include "gpio.h"
#include "gpio_loc.h"
#include <avr/io.h>

#define LED_ON_STATE(LEDPORT,LED) {LEDPORT &= ~(1 << LED); }
#define LED_OFF_STATE(LEDPORT,LED) {LEDPORT |= (1 << LED); }
#define LED_TOGGLE_STATE(LEDPORT,LED) {LEDPORT ^= (1 << LED ); }	
	
void GPIO_Init()
{
	//Einmal in den Tristate Zustand
	
	HEATER_PORT |= ( 1<<HEATER );
	HEATER_DDR  |= ( 1<<HEATER );
	
	PUMP_PORT |= ( 1<<PUMP );
	PUMP_DDR  |= ( 1<<PUMP );
	
	/* The LEDs are low active we need to set the output to high */
	LED1_PORT |= ( 1<<LED1 );
	LED1_DDR  |= ( 1<<LED1 );
	
	LED2_PORT |= ( 1<<LED2 );
	LED2_DDR  |= ( 1<<LED2 );
	
	LED3_PORT |= ( 1<<LED3 );
	LED3_DDR  |= ( 1<<LED3 );
	
	/* Leds are now ready */
	
	SWITCH1_DDR  &= ~( 1<<SWITCH1 ); /* Is now input */
	SWITCH1_PORT |=  ( 1<<SWITCH1 ); /* Pullup Enable */
	
	SWITCH2_DDR  &= ~( 1<<SWITCH2 ); /* Is now input */
	SWITCH2_PORT |=  ( 1<<SWITCH2 ); /* Pullup Enable */
	
	SWITCH3_DDR  &= ~( 1<<SWITCH3 ); /* Is now input */
	SWITCH3_PORT |=  ( 1<<SWITCH3 ); /* Pullup Enable */
	
	/* Switches are now ready */
	HALL_SW1_DDR  &= ~( 1<<HALL_SW1 ); /* Is now input */
	HALL_SW1_PORT |= (1<<HALL_SW1);	/* PULUP enable */	
		
	HALL_SW2_DDR  &= ~( 1<<HALL_SW2 ); /* Is now input */
	HALL_SW2_PORT |= (1<<HALL_SW2);	/* PULUP enable */




}

void voSetLedState(uint8_t LedIdx, emLEDSATATE LedState){
	switch(LedIdx){
		case 0: {
			if(LedState == LED_ON) {
				LED_ON_STATE(LED1_PORT,LED1);
			} else if ( LedState == LED_OFF) {
				LED_OFF_STATE(LED1_PORT,LED1);
			} else {
				LED_TOGGLE_STATE(LED1_PORT,LED1);
			}
			
		} break;
		
		case 1:{
			if(LedState == LED_ON){
				LED_ON_STATE(LED2_PORT,LED2);
				} else if (LedState == LED_OFF){
				LED_OFF_STATE(LED2_PORT,LED2);
				} else {
				LED_TOGGLE_STATE(LED2_PORT,LED2);
			}
		}break;
	
		case 2:{
			if(LedState == LED_ON){
				LED_ON_STATE(LED3_PORT,LED3);
			} else if(LedState == LED_OFF) {
				LED_OFF_STATE(LED3_PORT,LED3);
			} else {
				LED_TOGGLE_STATE(LED3_PORT,LED3);
			}
		
		}break;
			
		default:{
			/* We do nothing here */
		}break;
	}
}

emLEDSATATE emGetLedState(uint8_t LedIdx){
	emLEDSATATE LedState=LED_OFF;
	switch(LedIdx){
		case 0: {
			if(((LED1_PIN & (1<<LED1))>>LED1)==0){
				LedState=LED_ON;
			}else{
				LedState=LED_OFF;
			}
			
			
		} break;
		
		case 1:{
			if(((LED2_PIN & (1<<LED2))>>LED2)==0){
				LedState=LED_ON;
				}else{
				LedState=LED_OFF;
			}
			
		}break;
		
		case 2:{
			if(((LED3_PIN & (1<<LED3))>>LED3)==0){
				LedState=LED_ON;
				}else{
				LedState=LED_OFF;
			}
		}break;
		
		default:{
			/* We do nothing here */
		}break;
	}
	
	return LedState;
}


emSWITCHSTATE emGetSwitchState(uint8_t SwitchIdx){
	emSWITCHSTATE SwitchState=SWITCH_RELEASED;
	switch(SwitchIdx){
		case 0: {
			if(((SWITCH1_PIN & (1<<SWITCH1))>>SWITCH1)==0){
				SwitchState=SWITCH_PRESSED;
				}else{
				SwitchState=SWITCH_RELEASED;
			}
			
			
		} break;
		
		case 1:{
			if(((SWITCH2_PIN & (1<<SWITCH2))>>SWITCH2)==0){
				SwitchState=SWITCH_PRESSED;
				}else{
				SwitchState=SWITCH_RELEASED;
			}
			
		}break;
		
		case 2:{
			if(((SWITCH3_PIN & (1<<SWITCH3))>>SWITCH3)==0){
				SwitchState=SWITCH_PRESSED;
				}else{
				SwitchState=SWITCH_RELEASED;
			}
			
		}break;
		
		default:{
			/* We do nothing here */
		}break;
	}
	
	return SwitchState;
}

emSWITCHSTATE emGetHallSwitchState(uint8_t SwitchIdx){
	emSWITCHSTATE SwitchState=SWITCH_RELEASED;
	switch(SwitchIdx){
		case 0: {
			if(((HALL_SW1_PIN & (1<<HALL_SW1))>>HALL_SW1)==0){
				SwitchState=SWITCH_PRESSED;
				}else{
				SwitchState=SWITCH_RELEASED;
			}
			
			
		} break;
		
		case 1:{
			if(((HALL_SW2_PIN & (1<<HALL_SW2))>>HALL_SW2)==0){
				SwitchState=SWITCH_PRESSED;
				}else{
				SwitchState=SWITCH_RELEASED;
			}
			
			
		}break;
		
		default:{
			/* We do nothing here */
		}break;
	}
	
	return SwitchState;
}


void voSetHeaterPowerState(emPOWERSTATE PwSt){
	switch(PwSt){
		case POWER_ON:{
			HEATER_PORT |= ( 1 << HEATER );
			
		}break;
		
		case POWER_OFF:{
			HEATER_PORT &= ~( 1 << HEATER );
		} break;
		
		default:{
			HEATER_PORT &= ~( 1 << HEATER );
		} break;
		
	}
	
}
emPOWERSTATE emGetHeaterPowerState( void ){
		if(((HEATER_PIN & (1<<HEATER))>>HEATER)==0){
			return POWER_OFF;
			}else{
			return POWER_ON;
		}
}

void voSetPumpPowerState(emPOWERSTATE PwSt){
	switch(PwSt){
		case POWER_ON:{
			PUMP_PORT |= ( 1 << PUMP );
			
		}break;
		
		case POWER_OFF:{
			PUMP_PORT &= ~( 1 << PUMP );
		} break;
		
		default:{
			PUMP_PORT &= ~( 1 << PUMP );
		} break;
		
	}
	
}

emPOWERSTATE emGetPumpPowerState( void ){
	if(((PUMP_PIN & (1<<PUMP))>>PUMP)==0){
		return POWER_OFF;
		}else{
		return POWER_ON;
	}
}

