/*
 * pumpservice.h
 *
 * Created: 14.01.2017 22:06:18
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef PUMPSERVICE_H_
#define PUMPSERVICE_H_

#include <stdint.h>

typedef enum {
	PUMP_RUNNING,
	PUMP_OFF,
	PUMP_MALFUNCTION,
	PUMP_INVALID,

} PUMPSTATE_t;

void voPSE_TaskInit( void );
void voPSE_Task( void );
PUMPSTATE_t emGetPumpState( void );
void voSetPumpState( PUMPSTATE_t PumpState);
uint32_t u32PSE_GetCurrentRuntimeMS( void );



#endif /* PUMPSERVICE_H_ */