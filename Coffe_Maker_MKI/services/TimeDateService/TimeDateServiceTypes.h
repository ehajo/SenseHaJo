/*
 * TimeDateServiceTypes.h
 *
 * Created: 02.06.2017 16:50:46
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef TIMEDATESERVICETYPES_H_
#define TIMEDATESERVICETYPES_H_


typedef struct {
	uint8_t  Day;
	uint8_t  Month;
	uint16_t Year;
	uint8_t DayOfWeek;
} DATE_t;

typedef struct {
	uint8_t Seconds;
	uint8_t Minutes;
	uint8_t Hours;
} TIME_t;

typedef struct {
	DATE_t Date;
	TIME_t Time;	
} DATETIME_t;



#endif /* TIMEDATESERVICETYPES_H_ */