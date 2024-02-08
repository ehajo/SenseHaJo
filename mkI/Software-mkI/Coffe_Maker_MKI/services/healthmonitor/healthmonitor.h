/*
 * healthmonitor.h
 *
 * Created: 24.03.2017 19:08:59
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef HEALTHMONITOR_H_
#define HEALTHMONITOR_H_

typedef struct {
	
	uint8_t WaterLevelLowFault; /* Water is empty */
	uint8_t WaterLevelLowWarning; /* Water will only last for one cup */
	uint8_t OverTemperaturFault;
	uint8_t BoilerDrawsNoCurrentFault;
	uint8_t Reserved;
	
} FaultAndWarning_t;

void voHMon_Init( void );
void voHMon_Task( void );

FaultAndWarning_t HMon_tGetFaultAndWarning( void );

#endif /* HEALTHMONITOR_H_ */