/*
 * heaterpowercontroller.h
 *
 * Created: 15.01.2017 17:14:11
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef HEATERPOWERCONTROLLER_H_
#define HEATERPOWERCONTROLLER_H_

#include <stdlib.h>
#include <stdint.h>

void voHPC_TaskInit( void );
void voHPC_Task( void );
void voHPC_SetPowerOutput( uint8_t u8Power );
uint8_t u8HPC_GetPowerOutput( void );



#endif /* HEATERPOWERCONTROLLER_H_ */