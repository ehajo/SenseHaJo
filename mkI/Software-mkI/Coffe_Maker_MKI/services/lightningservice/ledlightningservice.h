 /*
 * ledlighntningservice.h
 *
 * Created: 29.01.2017 12:30:05
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef LEDLIGHNTNINGSERVICE_H_
#define LEDLIGHNTNINGSERVICE_H_

#include "ledlightnoingservice_enum.h"

void LLS_voInitTask( void );
void LLS_voTask( void );

void LLS_voSetLedLightning( LED_t emLed , LED_BLINK_Frq_t emFreq );
LED_BLINK_Frq_t LLS_getLedLightning( LED_t emLed);



#endif /* LEDLIGHNTNINGSERVICE_H_ */