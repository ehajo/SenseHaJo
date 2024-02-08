/*
 * boilertempcontroller.c
 *
 * Created: 15.01.2017 17:32:44
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#include "../heaterpowercontroller/heaterpowercontroller.h"
#include "../boilertemp/boilertemp.h"
#include "../../driver/gpio/gpio.h"
#include "boilertempcontroller.h"


static volatile int16_t s16BoilerSetpoint;

/* PID Regulatorparts */



void voBTC_TaskInit(void ){
	voHPC_SetPowerOutput( 0 ); /* This will switch off the Boiler */
	s16BoilerSetpoint=0;
	
}

void voBTC_Task( void ){
static int16_t s16_FeedBackerrorSum=0;
static int16_t s16_LastTempValue=0;

const float fltP = 0.1;
const float fltI = 0.1;
const float fltD = 0.1;
	
	int16_t s16_ProportionalPart=0;
	int16_t s16_IntegralPart=0;
	int16_t s16_DifferentialPart=0;
	
	int16_t s16LocBoilerTemp = s16BLTMP_GetBoilerTemp();
	
/* We use here the fast way and do a two point controller */

if(s16LocBoilerTemp < s16BoilerSetpoint ){
	
	voSetHeaterPowerState(POWER_ON);
	
} else {
	
	voSetHeaterPowerState(POWER_OFF);
	
}

/*   */

return;

	int16_t FeedbackError = 0;
	int16_t s16D_Error=0;
	
	int32_t s32Result =0;
	
	/* Limit Error to be within int16_t Range */
	
	if( ( s16BoilerSetpoint - s16LocBoilerTemp ) > INT16_MAX ){
		FeedbackError = INT16_MAX;
	} else if ( ( s16BoilerSetpoint - s16LocBoilerTemp ) < INT16_MIN){
		FeedbackError = INT16_MIN;
	} else {
		FeedbackError = ( s16BoilerSetpoint - s16LocBoilerTemp );
	}
	
	
	s16_ProportionalPart =  (  FeedbackError * fltP ) ; 
	
	/* Next is the I Part */ 
	if( ( s16_FeedBackerrorSum + s16_FeedBackerrorSum ) > INT16_MAX ) {
		
		s16_FeedBackerrorSum = INT16_MAX;
	
	} else if  ( ( s16_FeedBackerrorSum + s16_FeedBackerrorSum ) < INT16_MIN ) {
		
		s16_FeedBackerrorSum = INT16_MIN;
	} else {
		
		s16_FeedBackerrorSum =  s16_FeedBackerrorSum + s16_FeedBackerrorSum;
	}
	
	s16_IntegralPart = s16_FeedBackerrorSum * fltI;
	
	/* Last is the D Part */
	
	if( ( s16_LastTempValue - s16LocBoilerTemp ) > INT16_MAX ){
		s16D_Error = INT16_MAX;
		} else if ( ( s16_LastTempValue - s16LocBoilerTemp ) < INT16_MIN){
		s16D_Error = INT16_MIN;
		} else {
		s16D_Error = ( s16_LastTempValue - s16LocBoilerTemp ) ;
	}
	
	s16_DifferentialPart = s16D_Error*fltD;
	
	
	/* Last is to put all error together */
	
	s32Result = s16_ProportionalPart + s16_IntegralPart + s16_DifferentialPart ;
	
	/* we now limit the value to be processable */ 
	if ( s32Result > 100 ){
		voHPC_SetPowerOutput( 100 );
	} else if ( s32Result < 0 ) {
		voHPC_SetPowerOutput( 0 );
	} else {
		voHPC_SetPowerOutput( (uint8_t)(s32Result) );
	}
	
}

void voBTC_SetBoilerSetTemp( int16_t  s16BoilerTemp ){
	
	s16BoilerSetpoint=s16BoilerTemp;
}

int16_t s16BTC_GetBoilerSetTemp( void ){
	
	return s16BoilerSetpoint;
}

int16_t s16BTC_GetCurrentBoilerTemp( void ){
	return s16BLTMP_GetBoilerTemp();	
}

bool boBTC_GetIsHeating( void ){
	if(emGetHeaterPowerState()!=POWER_OFF){
		return true; 
	} else {
		return false;
	}
}