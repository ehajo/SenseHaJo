/*
 * CommandInterpreter.h
 *
 * Created: 26.04.2017 20:30:15
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef COMMANDINTERPRETER_H_
#define COMMANDINTERPRETER_H_

#include <stdbool.h>

void CMDI_voInit( void );
void CMDI_voTask( void ) ;
bool CMDI_boAddCharToBuffer(char Element);


#endif /* COMMANDINTERPRETER_H_ */