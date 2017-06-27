/*
 * displayservice.h
 *
 * Created: 28.01.2017 09:06:37
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef DISPLAYSERVICE_H_
#define DISPLAYSERVICE_H_

#include <stdio.h>
#include "../../driver/senseo_display/senseodisplay_enum.h"


typedef enum {
	BLINK_10Hz,
	BLINK_5Hz,
	BLINK_2Hz,
	BLINK_1Hz,
	BLINK_OFF
} ICON_BLINK_FREQ_t;

/* We support here our 9 "ICONS" witch include the BackLight */

/* This here will basically do a refrsh for the I2C Display but will take at least 5ms to be completed */
/* We will also do the Blinky stuff here and support 10Hz 2Hz and 0 Hz */

void DPS_Init( void) ;
void DPS_voTask( void );

void DPS_SetBlinkFreq(ICON_BLINK_FREQ_t Frq, SENSEOICON_t Icon);
void DPS_SetIconVisibility(ICONVISIBLITY_t Visibility, SENSEOICON_t Icon);
ICONVISIBLITY_t DPS_GetIconVisibility( SENSEOICON_t Icon);


#endif /* DISPLAYSERVICE_H_ */