/*
 * gpio.h
 *
 * Created: 20.09.2013 14:53:45
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef GPIO_H_
#define GPIO_H_

#include <avr/io.h>
#include "gpio_typedef.h"

void GPIO_Init(void);

void voSetLedState(uint8_t LedIdx, emLEDSATATE );
emLEDSATATE emGetLedState(uint8_t LedIdx);
emSWITCHSTATE emGetSwitchState(uint8_t SwitchIdx);
emSWITCHSTATE emGetHallSwitchState(uint8_t SwitchIdx);

void voSetHeaterPowerState(emPOWERSTATE);
emPOWERSTATE emGetHeaterPowerState( void );

void voSetPumpPowerState(emPOWERSTATE);
emPOWERSTATE emGetPumpPowerState( void );


#endif /* GPIO_H_ */