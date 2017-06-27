/*
 * ledlightnoingservice_enum.h
 *
 * Created: 29.01.2017 12:32:48
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef LEDLIGHTNOINGSERVICE_ENUM_H_
#define LEDLIGHTNOINGSERVICE_ENUM_H_

typedef enum {

LED_BLINK_10Hz,
LED_BLINK_5Hz,
LED_BLINK_2Hz,
LED_BLINK_1Hz,
LED_FADE_IN,  /* IF LED IS OFF THIS WILL DO A FADEOUT */
LED_FASE_OUT, /* IF LED IS ON THIS WILL DO A FADEOUT */
LED_FADE_IN_OUT, /* FADEIN / FADEOUT Effect */
LED_BLINK_OFF,
LED_STATIC_OFF,
LED_STATIC_ON,
	
} LED_BLINK_Frq_t;

typedef enum {
	LED_ONECUP,
	LED_POWER,
	LED_TWOCUP,
} LED_t;



#endif /* LEDLIGHTNOINGSERVICE_ENUM_H_ */