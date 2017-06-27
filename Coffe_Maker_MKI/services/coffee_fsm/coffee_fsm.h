/*
 * coffee_fsm.h
 *
 * Created: 14.01.2017 18:57:44
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef COFFEE_FSM_H_
#define COFFEE_FSM_H_

#include <stdbool.h>

void voCoffeeFSM_TaskInit( void );
void voCoffeeFSM_Task( void );
uint8_t u8GetCurrenProgrammIdx( void );

bool boCoffeeFSM_LoadProgramm(uint8_t idx);
void voCoffeeFSM_LoadSystemSettings( void );

bool boCoffeeFSM_MakeCoffee(uint8_t Cups);
void voCoffeeFSM_SetPower(bool boOn);

void voSetCupLevel(uint8_t u8Level);
uint8_t u8GetCupLevel(void);

void voSetAutoPowerOff(uint8_t u8TimeMin);
uint8_t u8GetAutoPowerOff(void);

void voSetStandbyTemp( uint16_t Temp );
uint16_t u16GetStandbyTemp( void );


#endif /* COFFEE_FSM_H_ */