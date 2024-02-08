/*
 * MCP3202.h
 *
 * Created: 30.12.2016 20:41:04
 *  Author: CalM
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef MCP3202_H_
#define MCP3202_H_

#define CURRENT_SENS 1
#define TEMPERATUR_SENS 0

void voMCP3202_Init( void );
uint16_t u16MCP3202_ReadChannel(uint8_t Channel);
uint8_t u8MCP3202_ReadChannel(uint8_t Channel);

#endif /* MCP3202_H_ */