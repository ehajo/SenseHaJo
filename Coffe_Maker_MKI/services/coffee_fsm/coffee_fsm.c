/*
 * coffee_fsm.c
 *
 * Created: 14.01.2017 18:57:26
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include "../../services/buttondriver/buttondriver.h"
#include "../../services/boilertemp/boilertemp.h"
#include "../../services/pumpservice/pumpservice.h"
#include "../../services/boilertempcontroller/boilertempcontroller.h"
#include "../../services/TimerService/TimerService.h"
#include "../../services/displayservice/displayservice.h"
#include "../../driver/ramfs/ramfs.h"
#include "../../services/healthmonitor/healthmonitor.h"
#include "../../services/lightningservice/ledlightningservice.h"
#include "../../services/lightningservice/ledlightnoingservice_enum.h"
#include "../../services/eepromFileSystem/eepromFileSystem.h"

#include <string.h>
#include <stdio.h>

/* Internal States for now */
typedef enum {
	coffeemaker_idle,
	coffeemaker_preheat,
	coffeemaker_brewcoffee,
	coffeemaker_fillcup,
	coffeemaker_wait_xsec_cup,
	coffeemaker_cont_fill_cup,
	coffeemaker_cont_calc_cup,
} coffeefsm_t;

typedef struct {
	uint8_t bIsBrewing:1;
	uint8_t bWaterEmpty:1;
	uint8_t bFillTwoCups:1;
	uint8_t bReserved:5;
	
} FSM_STATUSBITS_t;

static volatile coffeefsm_t emFSMState;

/* We need a few variabels to the FSM to get the userprogramm up and running */
static volatile systemsettings_t tFSMSettings; /* This will eat up 8 Bytes RAM */
static volatile usersettings_t	tFSMCoffeProg; /* This will eat up 16 Bytes RAM */

static volatile uint32_t u32FillTime = 0;
static volatile uint8_t u8ProgrammLoaded=0;
static volatile FSM_STATUSBITS_t tStatusBits;
/* prototypes for our internal functions */
void voCoffeeFSM_DrawCup( uint8_t Level );
void voCoffeeFSM_DrawArrows( bool boBrewing );
void voCoffeeFSM_DrawWaterEmpty( bool boEmpty );
void voCoffeeFSM_DrawThermometer( bool boHeating, bool boOvertemp );
void voProcessLidKey(coffeefsm_t emFSMState);

uint16_t u16GetStandbyTemp( void ){
	return tFSMSettings.u16StandbyTemp;
}

void voSetStandbyTemp( uint16_t Temp ){
	if(Temp>9999){
		Temp=9999;
	}
	tFSMSettings.u16StandbyTemp=Temp;
	voEEFS_WriteSystemSettings((systemsettings_t*)&tFSMSettings);
}

void voCoffeeFSM_TaskInit( void ){
	
	/* We need to Init the FSM to a known state */
	emFSMState = coffeemaker_idle;
	
}

bool boCoffeeFSM_LoadProgramm(uint8_t idx){

bool boReturnval=false;
bool boLoadVal=false;
usersettings_t tCoffeProg;

	if( (emFSMState==coffeemaker_idle) || (emFSMState==coffeemaker_preheat) ){
		/* idx 0 to 7 are the eeprom programms */
		if(8 > idx ){
			/* We have a idx in the range 0 to 7 */
			/* Programm will be loaded from eeprom */
			
			boLoadVal=boEEFS_ReadUserProgramm(idx,&tCoffeProg);
			if(boLoadVal==true){
				/* Copy the new coffeepro to ram for the fsm */
				memcpy((usersettings_t*)&tFSMCoffeProg,&tCoffeProg, sizeof(usersettings_t));
				boReturnval = true;	
				u8ProgrammLoaded=idx;
				
			} else {
				/* return false */
				boReturnval = false;
			}
			
		//} else if ( 8== idx ) {
		//	/* idx 8 is the RAM Settings for USB Programms */
		//	if(boRFS_ReadUserSettings(&tCoffeProg)==true){
		//		memcpy((usersettings_t*)&tFSMCoffeProg,&tCoffeProg, sizeof(usersettings_t));
		//		boReturnval = true;
		//		u8ProgrammLoaded=idx;
		//	} else {
		//		voSetDefaultUserSettingsValues(&tCoffeProg);
		//		boReturnval = false;
		//	} 
			
						
		} else {
			/* We can't change the program now as we brew coffee */
			boReturnval = false;
		}
	}
		
	return boReturnval;
}

uint8_t u8GetCurrenProgrammIdx( void ){
	return u8ProgrammLoaded;
}

bool voCoffeeFSM_LoadSystemSettings(){
	bool boRetunrVal=false;
	/* This is something that we can do at any state */
	systemsettings_t tSystemSettings;
	if(voEEFS_ReadSystemSettings(&tSystemSettings)!=false){
		//we have loaded the settings now we need to apply them to the fsm used one
		memcpy((systemsettings_t*)&tFSMSettings,&tSystemSettings, sizeof(systemsettings_t));
		boRetunrVal=true;
		/* Settings are now copied */
	} else {
		/* Loading has failed we need to provide packup values */
		voSetDefaultSystemSettingsValues((systemsettings_t*)&tFSMSettings);
		boRetunrVal=false;
		
	}
	
	return boRetunrVal;
}

bool boCoffeeFSM_MakeCoffee(uint8_t Cups){
	bool boStart=false;
	if( (emFSMState==coffeemaker_idle) || (emFSMState==coffeemaker_preheat) ){
		/* We can change to brew coffe here */
		boStart=true;
		if(Cups>1){
			tStatusBits.bFillTwoCups=1;
		} else {
			tStatusBits.bFillTwoCups=0;
		}
		emFSMState = coffeemaker_brewcoffee;
		
	}
	return boStart;
}

void voCoffeeFSM_SetPower(bool boOn){
	if(boOn==true){
		if(emFSMState==coffeemaker_idle){
			emFSMState=coffeemaker_preheat;
		}
	} else {
		emFSMState=coffeemaker_idle;
	}
}

void voSetCupLevel(uint8_t u8Level){
	if(u8Level>2){
		u8Level=2;
	}
	if( (emFSMState==coffeemaker_idle) || (emFSMState==coffeemaker_preheat)){
		tFSMSettings.u8coffeelevel=u8Level;
		/* We need to write the new setting into eeprom */
		voEEFS_WriteSystemSettings((systemsettings_t*)&tFSMSettings);
		
	}
}

uint8_t u8GetCupLevel(void){
	return tFSMSettings.u8coffeelevel;
}

void voSetAutoPowerOff(uint8_t u8TimeMin){
	if(u8TimeMin>60){
		u8TimeMin=60;
	}
	tFSMSettings.u8AutoPowerOffTime=u8TimeMin;
	voEEFS_WriteSystemSettings((systemsettings_t*)&tFSMSettings);
}

uint8_t u8GetAutoPowerOff(void){
	return tFSMSettings.u8AutoPowerOffTime;
}




void voCoffeeFSM_Task( void ){
	
	/* Cyclic FSM Task for CoffeeProcessing */
	
	
	/* We get here our Keyevents */
	KeyEvent_t PowerButtonKeyevent = strGetKeyEvent(POWER_KEY);
	KeyEvent_t OneCupButtonKeyevent = strGetKeyEvent(ONECUP_KEY);
	KeyEvent_t TwoCupButtonKeyevent = strGetKeyEvent(TWOCUP_KEY);	
	
	/* Set the internal Bits for processing to a known value  */
	tStatusBits.bIsBrewing=false;
	
	FaultAndWarning_t HealthState = HMon_tGetFaultAndWarning();
	
	/* What ever happens if we have a fault we go to off or idle depening on the current state */
	
	/* Check for HW faults an react */
	if( (HealthState.OverTemperaturFault!=0) || (HealthState.WaterLevelLowFault!=0)){
		/* We need to stop what ever we are doing and return to off or idle */
		if(emFSMState!=coffeemaker_idle){
			emFSMState=coffeemaker_preheat;
		} else {
			emFSMState=coffeemaker_idle;
		}
		
	}
	
	
	
	
	
	switch( emFSMState ){
		case coffeemaker_idle:{
			u32FillTime = 0;
			/* Ensure Heater is off and pump nor running */
			voBTC_SetBoilerSetTemp(INT16_MIN); /* PowerDown */
			voSetPumpPowerState(POWER_OFF);
			LLS_voSetLedLightning(LED_ONECUP,LED_STATIC_OFF);
			LLS_voSetLedLightning(LED_TWOCUP,LED_STATIC_OFF);
			LLS_voSetLedLightning(LED_POWER,LED_STATIC_OFF);
			/* Check for Keyevents 
			- if we have a PowerUp Button hit we wake up and do a pre heat to a default degree value 
			- if we have a OneCup Button pressed we will brew a Coffee 
			- if we have a TwoCup Button pressed we will brew two Coffee 
			*/
			tStatusBits.bFillTwoCups=0;
			
			/* We leave IDEL only if we have water in the Tank */
			
					if( (KeyChanged == PowerButtonKeyevent.KeyChangeState) & ( SWITCH_PRESSED == PowerButtonKeyevent.SwitchState ) ){
						/* Okay at least we got an Power UP */
				
						/* We need to start the auto power off timer */
						voSetTimeOut(AUTO_POWER_OFFTIMER, TMS_MIN( tFSMSettings.u8AutoPowerOffTime ) );
						emFSMState = coffeemaker_preheat;
					} 
			
					/* KeyPrioritys if a Coffee is wished this has priority and we will skip the Preheat ! */
					if( (KeyChanged == OneCupButtonKeyevent.KeyChangeState) & ( SWITCH_PRESSED == OneCupButtonKeyevent.SwitchState ) ){
						emFSMState = coffeemaker_brewcoffee;
						tStatusBits.bFillTwoCups=0;
					} 
			
					if( (KeyChanged == TwoCupButtonKeyevent.KeyChangeState) & ( SWITCH_PRESSED == TwoCupButtonKeyevent.SwitchState ) ){
						emFSMState = coffeemaker_brewcoffee;
						tStatusBits.bFillTwoCups=1;
					}
			
			
			
			
			
		}
		break;
		
		case coffeemaker_preheat:{
			LLS_voSetLedLightning(LED_ONECUP,LED_STATIC_OFF);
			LLS_voSetLedLightning(LED_TWOCUP,LED_STATIC_OFF);
			/* We start heating until we got 75°C this will at least kill the legionella bakteria in the boiler */
			
			
			/*  We need to switch off the pump  */
			voSetPumpState(PUMP_OFF);
			
			tStatusBits.bFillTwoCups=0;
			//voBTC_SetBoilerSetTemp(7500); /* PowerDown */
			/* We read the Preheat Temp form the Programm */
			if(HealthState.WaterLevelLowFault!=false){
				/* Water is empty we stop heating here */
				voBTC_SetBoilerSetTemp(INT16_MIN); /* 0°C */
				LLS_voSetLedLightning(LED_POWER,LED_BLINK_10Hz);
			} else {
				
				voBTC_SetBoilerSetTemp(tFSMSettings.u16StandbyTemp);
				/* We read the Boiler Temp */
				if(s16BTC_GetCurrentBoilerTemp()<tFSMSettings.u16StandbyTemp){
					LLS_voSetLedLightning(LED_POWER,LED_BLINK_1Hz);
				} else {
					LLS_voSetLedLightning(LED_POWER,LED_STATIC_ON);
				}
			
			}
			
			if( (KeyChanged == PowerButtonKeyevent.KeyChangeState) & ( SWITCH_PRESSED == PowerButtonKeyevent.SwitchState ) ){
				/* Okay at least we got an Power DOWN */
				emFSMState = coffeemaker_idle;
			}
			
			/* KeyPrioritys if a Coffee is wished this has priority and we will skip the Preheat ! */
			if( (KeyChanged == OneCupButtonKeyevent.KeyChangeState) & ( SWITCH_PRESSED == OneCupButtonKeyevent.SwitchState ) ){
				emFSMState = coffeemaker_brewcoffee;
				tStatusBits.bFillTwoCups=0;
			}
			
			if( (KeyChanged == TwoCupButtonKeyevent.KeyChangeState) & ( SWITCH_PRESSED == TwoCupButtonKeyevent.SwitchState ) ){
				emFSMState = coffeemaker_brewcoffee;
				tStatusBits.bFillTwoCups=1;
			}		
			
			/* We need a autopower off timer here */
			if(tFSMSettings.u8AutoPowerOffTime!=0){
				if( u8GetTimeElapsed(AUTO_POWER_OFFTIMER)==TMS_ELAPSED){
					emFSMState = coffeemaker_idle;
				}
			}
			
		}
		break;
				
		case coffeemaker_brewcoffee:{
			/* Statusbits setzen */
			tStatusBits.bIsBrewing=1;
			
			/* We set the LEDs for the Front */
			
			LLS_voSetLedLightning(LED_POWER,LED_STATIC_OFF);
			/* Set ONE Cup off */
			/* Let two Cup Blink */
			if(tStatusBits.bFillTwoCups==0){
				LLS_voSetLedLightning(LED_TWOCUP,LED_STATIC_OFF);
				LLS_voSetLedLightning(LED_ONECUP,LED_BLINK_2Hz);
				
			} else {
				LLS_voSetLedLightning(LED_ONECUP,LED_STATIC_OFF);
				LLS_voSetLedLightning(LED_TWOCUP,LED_BLINK_2Hz);
			}
			
			
			
			
			/* We need to heat up to 90°C */
			voBTC_SetBoilerSetTemp(tFSMCoffeProg.u32HeaterTemp+(300)); /* Set to +3 and hope to reach it */
			int16_t LocBoilerTemp = s16BTC_GetCurrentBoilerTemp(); /* We got the temp in 1/100°C */
			if( (int32_t)(LocBoilerTemp) < (int32_t)(tFSMCoffeProg.u32HeaterTemp )){
				/* We need to wait untill we get up to 90°C */
				/* If we can't reach within 5 Minutes we stop the heatup */
				asm("nop");
					
				
				/* and we also need to set a Timout if we can't reach out temp */
			} else {
				/* Next is to start the Pump and deliver on Coffee */
				voSetPumpState(PUMP_RUNNING);	
				emFSMState=coffeemaker_fillcup;
			}
			
			/* Remark we need a fault state if we can't reach 90°C after a while */
			if( (KeyChanged == PowerButtonKeyevent.KeyChangeState) & ( SWITCH_PRESSED == PowerButtonKeyevent.SwitchState ) ){
				/* Okay at least we got an Power UP */
				emFSMState = coffeemaker_idle;
			}
			
			
		}break;
		
		case coffeemaker_fillcup: {
		
			LLS_voSetLedLightning(LED_POWER,LED_STATIC_OFF);
			if(tStatusBits.bFillTwoCups==0){
				LLS_voSetLedLightning(LED_TWOCUP,LED_STATIC_OFF);
				LLS_voSetLedLightning(LED_ONECUP,LED_BLINK_2Hz);
			} else {
				LLS_voSetLedLightning(LED_ONECUP,LED_STATIC_OFF);
				LLS_voSetLedLightning(LED_TWOCUP,LED_BLINK_2Hz);
			}
			
			
			if(u32PSE_GetCurrentRuntimeMS()<tFSMCoffeProg.u16PreWaterLevel){
				/* Do nothing */
			} else {
				/*  Coffee is ready  */
				emFSMState =coffeemaker_wait_xsec_cup;
				voSetPumpState(PUMP_OFF);
				/* Reads now user prog */
				voSetTimeOut(PRE_HEAT_TIMER, tFSMCoffeProg.u16PreWaterWait );
			}
			if( (KeyChanged == PowerButtonKeyevent.KeyChangeState) & ( SWITCH_PRESSED == PowerButtonKeyevent.SwitchState ) ){
				/* Okay at least we got an Power UP */
				emFSMState = coffeemaker_idle;
			}
			
		} break;
		
		case coffeemaker_wait_xsec_cup:{
			
			
			LLS_voSetLedLightning(LED_POWER,LED_STATIC_OFF);
			
			if(tStatusBits.bFillTwoCups==0){
				LLS_voSetLedLightning(LED_TWOCUP,LED_STATIC_OFF);
				LLS_voSetLedLightning(LED_ONECUP,LED_BLINK_2Hz);
			} else {
				LLS_voSetLedLightning(LED_ONECUP,LED_STATIC_OFF);
				LLS_voSetLedLightning(LED_TWOCUP,LED_BLINK_2Hz);
			}
			
			if(u8GetTimeElapsed(PRE_HEAT_TIMER) == TMS_NOT_ELAPSED){
				/* Keep in this state */
				asm("nop");
			} else {
			   /* move to next one */
			    emFSMState = coffeemaker_cont_calc_cup;
			}
			
		} break;
		
		case coffeemaker_cont_calc_cup:{
			
			u32FillTime = tFSMCoffeProg.u16WaterLevelFull  ;
			if(tStatusBits.bFillTwoCups!=0){
				u32FillTime=u32FillTime*2;
			}
			
			switch(tFSMSettings.u8coffeelevel){
				case 0:{
					u32FillTime=u32FillTime*1/3;
				} break;
				
				case 1:{
					u32FillTime=u32FillTime*2/3;
				}break;
				
				case 2:{
					u32FillTime=u32FillTime;
				}break;
				
				default:{
					u32FillTime=u32FillTime;
				}break;
			}
			
			emFSMState =coffeemaker_cont_fill_cup;
			voSetTimeOut(PUMP_TIMER,u32FillTime);
		    voSetPumpState(PUMP_RUNNING);	   	
		} break;
		
		case coffeemaker_cont_fill_cup:{
			
					
			LLS_voSetLedLightning(LED_POWER,LED_STATIC_OFF);
			/* We need to see if we have one or two cups filling */
			
			
			if(tStatusBits.bFillTwoCups!=0){
				LLS_voSetLedLightning(LED_TWOCUP,LED_BLINK_2Hz);
				LLS_voSetLedLightning(LED_ONECUP,LED_STATIC_OFF);
				
			} else {
				LLS_voSetLedLightning(LED_ONECUP,LED_BLINK_2Hz);
				LLS_voSetLedLightning(LED_TWOCUP,LED_STATIC_OFF);
				
			}
			
			
			
			
			if(u8GetTimeElapsed(PUMP_TIMER)==TMS_NOT_ELAPSED){
				/* Do nothing */
			} else {
				/*  Coffee is ready  */
				voSetPumpState(PUMP_OFF);
				voSetTimeOut(AUTO_POWER_OFFTIMER, TMS_MIN( tFSMSettings.u8AutoPowerOffTime ) );
				emFSMState =coffeemaker_preheat;
				
			}
			
			
		}break;
		
		
		default:{
			emFSMState = coffeemaker_idle;
		}
	}
	
	
	voProcessLidKey(emFSMState);
	voCoffeeFSM_DrawArrows(tStatusBits.bIsBrewing);
	voCoffeeFSM_DrawCup(tFSMSettings.u8coffeelevel);
	voCoffeeFSM_DrawWaterEmpty(HealthState.WaterLevelLowFault);
	voCoffeeFSM_DrawThermometer(boBTC_GetIsHeating(),HealthState.OverTemperaturFault);
}

/* The following functions will set and handle different parts of the screen */

/* This will handle the coffecup symbols and set the accordingly */
void voCoffeeFSM_DrawCup(uint8_t Level){
	/* We draw the cup and according to the level button the corresponding level */
	DPS_SetIconVisibility(VISIBLE,CUP_ONE);
	switch(Level){
	    case 0:{
			/* We are fine here */
			DPS_SetIconVisibility(INVISIBLE,CUP_TWO);
			DPS_SetIconVisibility(INVISIBLE,CUP_THREE);
		}break;
		case 1:{
			/* We set also CUP_TWO */
			DPS_SetIconVisibility(VISIBLE,CUP_TWO);
			DPS_SetIconVisibility(INVISIBLE,CUP_THREE);
		}break;
		
		case 2:{
			DPS_SetIconVisibility(VISIBLE,CUP_TWO);
			DPS_SetIconVisibility(VISIBLE,CUP_THREE);
		}break;	
		
		default:{
			DPS_SetIconVisibility(VISIBLE,CUP_TWO);
			DPS_SetIconVisibility(VISIBLE,CUP_THREE);
		} break;
	}
	/* Okay visibility is set here */
}

void voProcessLidKey(coffeefsm_t emFSMState){

	/* We need also the events of the top key */
	KeyEvent_t LidKey = strGetKeyEvent(LID_KEY);
	
	if( (emFSMState==coffeemaker_idle) || ( emFSMState == coffeemaker_preheat ) ){
			/* If nothing is pressed we stay here and wait to be called */
			if ( (KeyChanged == LidKey.KeyChangeState) & ( SWITCH_PRESSED == LidKey.SwitchState) ) {
				tFSMSettings.u8coffeelevel++;
				if(tFSMSettings.u8coffeelevel>2){
					tFSMSettings.u8coffeelevel=0;
				}
				voEEFS_WriteSystemSettings((systemsettings_t*)&tFSMSettings);
			}
	}
}


void voCoffeeFSM_DrawArrows(bool boBrewing){
	
	if(true == boBrewing ){
		DPS_SetIconVisibility(INVISIBLE,ARROWS);		
	} else {
		DPS_SetIconVisibility(VISIBLE,ARROWS);
	}
	
}



void voCoffeeFSM_DrawWaterEmpty( bool boEmpty ){
	/* we let the symbol blink if have a Empty */
	switch (boEmpty){
		case true:{
			DPS_SetBlinkFreq(BLINK_1Hz,WATER);
			
		} break;
		
		case false:{
			DPS_SetIconVisibility(INVISIBLE,WATER);
			DPS_SetBlinkFreq(BLINK_OFF,WATER);
		} break;
		
	}
}

void voCoffeeFSM_DrawThermometer( bool boHeating, bool boOvertemp ){
	/* we need to combine both bool to up to four states */
	
	if(true == boOvertemp){
		/* We don't care if we are heating we let the thermometer blink as fast as possible */
		DPS_SetBlinkFreq(BLINK_10Hz,THERMOMETER_INNER);
		DPS_SetBlinkFreq(BLINK_10Hz,THERMOMETER_OUTER);
		
		
	} else {
		
		if ( true == boHeating ){
			
			DPS_SetIconVisibility(VISIBLE,THERMOMETER_OUTER);
			DPS_SetBlinkFreq(BLINK_OFF,THERMOMETER_OUTER);
			DPS_SetBlinkFreq(BLINK_1Hz,THERMOMETER_INNER);
				
		} else {
			
			DPS_SetIconVisibility(INVISIBLE,THERMOMETER_OUTER);
			DPS_SetIconVisibility(INVISIBLE,THERMOMETER_INNER);
			DPS_SetBlinkFreq(BLINK_OFF,THERMOMETER_OUTER);
			DPS_SetBlinkFreq(BLINK_OFF,THERMOMETER_INNER);
			
		}
	}
	
	
}


