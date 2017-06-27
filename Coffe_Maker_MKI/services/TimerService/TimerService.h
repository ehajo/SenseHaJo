/*
 * TimerService.h
 *
 * Created: 17.01.2017 22:07:51
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef TIMERSERVICE_H_
#define TIMERSERVICE_H_

#define TMS_ELAPSED ( 1 )
#define TMS_NOT_ELAPSED ( 0 )

#include <stdlib.h>
#include <stdint.h>

/* For easier "use" we have out defines for the timerservices here */
#define PRE_HEAT_TIMER		( 0 )
#define AUTO_POWER_OFFTIMER ( 1 )
#define PUMP_TIMER			( 2 )
#define TMS_SEC( x ) ( x * 1000 )
#define TMS_MIN( x ) ( x * 60000UL )

void voTMS_InitTask( void );
void voTMS_Task( void );

void voSetTimeOut( uint8_t idx, uint32_t TimeOutMS );
uint8_t u8GetTimeElapsed( uint8_t idx );

#endif /* TIMERSERVICE_H_ */