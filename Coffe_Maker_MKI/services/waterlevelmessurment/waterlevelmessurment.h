/*
 * waterlevelmessurment.h
 *
 * Created: 14.01.2017 15:06:00
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef WATERLEVELMESSURMENT_H_
#define WATERLEVELMESSURMENT_H_

typedef enum {

	LEVEL_OKAY,
	LEVEL_MIN,
	LEVEL_NOT_OKAY,
	LEVEL_INVALID,	

} emWaterLevel;

void WLM_voTask( void );
emWaterLevel WLM_emGetLevel( void ) ;



#endif /* WATERLEVELMESSURMENT_H_ */