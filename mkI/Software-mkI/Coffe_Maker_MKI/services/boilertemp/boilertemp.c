/*
 * boilertemp.c
 *
 * Created: 01.01.2017 02:00:16
 *  Author: CalM
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#include "boilertemp.h"

volatile int16_t BoilerTemp;
volatile uint16_t BoilerRawValue;

void voBLTMP_InitBoilerTemp(){
	
}

void voBLTMP_AddNewRawValue(uint16_t RawValue){
	
	int32_t LocBoilerTemp=0;
	
	BoilerRawValue= RawValue;
	LocBoilerTemp = (int32_t)RawValue - (int32_t)364;
	LocBoilerTemp = LocBoilerTemp * 100;
	LocBoilerTemp = LocBoilerTemp / 464;
	/* Fix it now to s16 */
	if(LocBoilerTemp > INT16_MAX ){
		BoilerTemp = INT16_MAX;
	} else if ( LocBoilerTemp < INT16_MIN){
		BoilerTemp = INT16_MIN;
	} else {
		BoilerTemp = LocBoilerTemp;
	}
	/* LocoilerTemp is now in 1/100°C and we keep away of our float */
	
}

int16_t s16BLTMP_GetBoilerTemp(){
	return BoilerTemp;		
}

uint16_t u16BLTMP_GetBoilerRawValue(){
	return BoilerRawValue;
}