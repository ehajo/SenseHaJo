/*
 * boilertemp.h
 *
 * Created: 01.01.2017 02:00:04
 *  Author: CalM
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef BOILERTEMP_H_
#define BOILERTEMP_H_

#include <stdlib.h>
#include <avr/io.h>

void voBLTMP_InitBoilerTemp();
void voBLTMP_AddNewRawValue(uint16_t RawValue);
int16_t s16BLTMP_GetBoilerTemp();
uint16_t u16BLTMP_GetBoilerRawValue();

#endif /* BOILERTEMP_H_ */