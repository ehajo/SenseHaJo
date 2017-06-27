/*
 * senseo_button.h
 *
 * Created: 25.01.2017 23:04:31
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#ifndef SENSEO_BUTTON_H_
#define SENSEO_BUTTON_H_

#include "../gpio/gpio_typedef.h"

void SBNT_voInit( void );
emSWITCHSTATE SBNT_emGetState( void );

void SBNT_SetSCLtoInput( void );


#endif /* SENSEO_BUTTON_H_ */