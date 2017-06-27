/*
 * buttondriver.c
 *
 * Created: 14.01.2017 17:11:32
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include "buttondriver.h"
#include "../../driver/senseo_button/senseo_button.h"

#define Debouncelimit 5
#define NoOfKeys 4

typedef enum {
	KEYSTATE_READ,
	KEYSTATE_NOT_READ
}KEYSTATEREAD_t; 

typedef struct {

	KeyEvent_t KeyEvent;
	uint8_t Debouncecounter;
	emSWITCHSTATE IntermediateSwitchstate;
	KEYSTATEREAD_t KeyStateRead;
}KeyEventRegister_t;

static volatile KeyEventRegister_t astrKeyEventRegister[4];

emSWITCHSTATE emGetSwitchStateHAL( uint8_t index );


void voKeyEvent_TaskInit( void ){
	
	for(uint8_t i=0; i < NoOfKeys ; i++){
		astrKeyEventRegister[i].Debouncecounter=0;
		astrKeyEventRegister[i].IntermediateSwitchstate=SWITCH_RELEASED;
		astrKeyEventRegister[i].KeyEvent.KeyChangeState=KeyInvalide;
		astrKeyEventRegister[i].KeyEvent.SwitchState=SWITCH_RELEASED;
	}
	
}

void voKeyEvent_Task( void ){
	
	/* We need to check each key here */
	
	for(uint8_t i=0;i<NoOfKeys;i++){
		
		emSWITCHSTATE State = emGetSwitchStateHAL(i);
		
		if(State != astrKeyEventRegister[i].IntermediateSwitchstate ){
			astrKeyEventRegister[i].Debouncecounter=0;
			astrKeyEventRegister[i].IntermediateSwitchstate = State;
			
			if(astrKeyEventRegister[i].KeyStateRead==KEYSTATE_READ){
				astrKeyEventRegister[i].KeyEvent.KeyChangeState = KeyUnchanged;
			}	
		} else {
			if( astrKeyEventRegister[i].Debouncecounter == Debouncelimit ){
				
				/* We have found our Event */
				astrKeyEventRegister[i].KeyEvent.SwitchState=State;
				astrKeyEventRegister[i].KeyEvent.KeyChangeState=KeyChanged;
				astrKeyEventRegister[i].KeyStateRead = KEYSTATE_NOT_READ;
				astrKeyEventRegister[i].Debouncecounter++;	
				
			} else if ( ( astrKeyEventRegister[i].Debouncecounter > Debouncelimit) && (astrKeyEventRegister[i].KeyStateRead == KEYSTATE_READ ) ) {
				
				astrKeyEventRegister[i].KeyEvent.KeyChangeState = KeyUnchanged;
			
			} else {
				if(astrKeyEventRegister[i].Debouncecounter<255){
					astrKeyEventRegister[i].Debouncecounter++;	
				}
				
			}
			
		}
		
	}	
}

emSWITCHSTATE emGetSwitchStateHAL( uint8_t index ){
	
	emSWITCHSTATE emReturnValue=SWITCH_RELEASED;
	/* This will grab the keys from the different driver and map them in the array */
	switch(index){
		case 0:
		case 1:
		case 2:
		{
			emReturnValue = emGetSwitchState( index );
			
		} break;
		
		case 3:{
			emReturnValue = SBNT_emGetState();
		} break;
		
		default:{
			/* do nothing here */
		}
	}
	
	return emReturnValue;
	
};

KeyEvent_t strGetKeyEvent( uint8_t KeyIdx ){
	
	if(KeyIdx < NoOfKeys) {
		astrKeyEventRegister[KeyIdx].KeyStateRead=KEYSTATE_READ;
		return astrKeyEventRegister[KeyIdx].KeyEvent;
	} else {
		return astrKeyEventRegister[NoOfKeys-1].KeyEvent;
	}
	
}

