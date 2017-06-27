/*
 * buttondriver.h
 *
 * Created: 14.01.2017 17:11:47
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef BUTTONDRIVER_H_
#define BUTTONDRIVER_H_

#include "../../driver/gpio/gpio.h"

#define POWER_KEY ( 1 )
#define ONECUP_KEY ( 0 )
#define TWOCUP_KEY ( 2 )

#define LID_KEY ( 3 )

typedef enum {
	KeyUnchanged,
	KeyChanged,
	KeyInvalide
} KeyChangeState_t;

typedef struct {
	KeyChangeState_t KeyChangeState;
	emSWITCHSTATE	SwitchState;
} KeyEvent_t;

void voKeyEvent_TaskInit( void );
void voKeyEvent_Task( void );
KeyEvent_t strGetKeyEvent( uint8_t KeyIdx );

	








#endif /* BUTTONDRIVER_H_ */