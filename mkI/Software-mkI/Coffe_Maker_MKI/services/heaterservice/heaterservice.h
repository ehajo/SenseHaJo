/*
 * heatservice.h
 *
 * Created: 14.01.2017 22:05:31
 *  Author: calm
 */ 


#ifndef HEATSERVICE_H_
#define HEATSERVICE_H_

#include <stdlib.h>
#include <stdint.h>

typedef enum {
	HEATER_RUNNING,
	HEATER_OFF,
	HEATER_MALFUNCTION,
	HEATER_INVALID,

} HEATERSTATE_t;

void voHSE_TaskInit( void );
void voHSE_Task( void );
HEATERSTATE_t emGetHeaterState( void );
void voSetHeaterState( HEATERSTATE_t HeaterState);
uint32_t u32HSE_GetCurrentRuntimeMS( void );



#endif /* HEATSERVICE_H_ */