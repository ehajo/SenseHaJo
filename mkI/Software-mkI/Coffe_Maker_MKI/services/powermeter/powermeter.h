/*
 * powermeter.h
 *
 * Created: 01.01.2017 01:45:44
 *  Author: CalM
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef POWERMETER_H_
#define POWERMETER_H_

#include <stdlib.h>
#include <avr/io.h>

void voPWRMTR_InitPowermeter();
void voPWRMTR_AddNewMessurment(uint16_t RawValue );
uint16_t u16PWRMTR_GetAvgPower();
uint16_t u16PWRMTR_GetRawPower();

uint32_t u32PWRMTR_GetuAmp();
uint32_t u32PWRMTR_GetmAmp();
uint32_t u32PWRMTR_GetAmp();


#endif /* POWERMETER_H_ */