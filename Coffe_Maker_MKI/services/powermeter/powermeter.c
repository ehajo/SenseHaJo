/*
 * powermeter.c
 *
 * Created: 01.01.2017 01:45:32
 *  Author: CalM
 */

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */ 

#include "powermeter.h"

static volatile uint16_t u16AvgPower=0;
static volatile uint32_t u32RawValue=0;
static volatile uint16_t  u8MessurmentCounter=0;
//We add 256 Messurments and devide by Zero

void voPWRMTR_InitPowermeter()
{
	u16AvgPower=0;
}


void voPWRMTR_AddNewMessurment(uint16_t RawValue ){



u8MessurmentCounter++;
if(u8MessurmentCounter>=(1<<12)){
	u16AvgPower=u32RawValue>>12;
	/* We put that to a mA Value */
	/* We have 2.5V for 20A witch lead to 8mA per mV and we have 0.0762mV per Bit */
	/* With 2.5V Offset we have 610uA per Bit * 2^15 as max Value */
	u8MessurmentCounter=0;
	
	
	u32RawValue=0;
}

	if( (RawValue&0x8000)!=0 ){

		/* Okay Positive current value */
		u32RawValue	+= ( ( RawValue & ~( 0x8000 )  )   ) ; /* Remove Offset of VCC/2 */

	} else {

		//Negative current value
		u32RawValue	+= ( (0x8000 - RawValue ) ) ; /* Use Delta betwin VCC/2 and messurment Value */

	}
	
}

uint16_t u16PWRMTR_GetAvgPower(){
	return u16AvgPower;
}

uint16_t u16PWRMTR_GetRawPower(){
	return u32RawValue;
}

uint32_t u32PWRMTR_GetuAmp(){
	return u16AvgPower*610;	
}

uint32_t u32PWRMTR_GetmAmp(){
	return ( (u16AvgPower*610)/1000 );	
}

uint32_t u32PWRMTR_GetAmp(){
	return ( (u16AvgPower*610)/1000000 );	
}
