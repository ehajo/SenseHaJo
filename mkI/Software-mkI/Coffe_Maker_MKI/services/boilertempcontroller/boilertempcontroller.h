/*
 * boilertempcontroller.h
 *
 * Created: 15.01.2017 17:33:05
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef BOILERTEMPCONTROLLER_H_
#define BOILERTEMPCONTROLLER_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define BTC_OFF ( INT16_MIN );

void voBTC_TaskInit(void );
void voBTC_Task( void );
void voBTC_SetBoilerSetTemp( int16_t  u16BoilerTemp );
int16_t s16BTC_GetBoilerSetTemp( void );
int16_t s16BTC_GetCurrentBoilerTemp( void );
bool boBTC_GetIsHeating( void );

#endif /* BOILERTEMPCONTROLLER_H_ */