/*
 * senseodisplay_enum.h
 *
 * Created: 28.01.2017 09:19:53
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef SENSEODISPLAY_ENUM_H_
#define SENSEODISPLAY_ENUM_H_

typedef enum{
	ARROWS=0,
	CALC,
	WATER,
	THERMOMETER_OUTER,
	THERMOMETER_INNER,
	CUP_ONE,
	CUP_TWO,
	CUP_THREE,
	BACKLIGHT,
	NO_OF_ICONS /* This will give us to amount of icons used */
} SENSEOICON_t;

typedef enum{
	VISIBLE,
	INVISIBLE,
	TOGGLE
} ICONVISIBLITY_t;

typedef enum {
	BACKLIGHT_ON,
	BACKLIGHT_OFF,
	BACKLIGHT_TOGGLE
}BACKLIGHTSTATE_t;



#endif /* SENSEODISPLAY_ENUM_H_ */