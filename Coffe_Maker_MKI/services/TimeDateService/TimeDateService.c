/*
 * TimeDateService.c
 *
 * Created: 02.06.2017 16:49:56
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include "TimeDateServiceTypes.h"

/* Ticks per Second */
#define TICKRATE ( 1 )
#if TICKRATE < 1
  #error "TICKRATE must be >= 1"
#endif

volatile DATETIME_t LocalTime;


void voAddOneSecondToTime( void );
void voAddOneMinuteToTime( void );
void voAddOneHourToTime( void );

void voAddOneDayToDate( void );
void voAddOneMonthToDate ( void );
void voAddOneYearToDate ( void ); 

void voTDS_Init(){
	LocalTime.Date.Day=0;
	LocalTime.Date.DayOfWeek=0;
	LocalTime.Date.Month=0;
	LocalTime.Date.Year=0;
	
	LocalTime.Time.Hours=0;
	LocalTime.Time.Minutes=0;
	LocalTime.Time.Seconds=0;
	
	/* Struct is initialized */
	/* We will now try to load Data from the RTC if the I2C is up and running */
		
	/*   */
}


void voTDS_Task(){
#if TICKRATE > 1
static uint8_t divider = 0;
/* We need also a tickrate for the RTC */	
	if(divider >= ( TICKRATE -1 ) ){ 	
#endif
	/* We will keep the internal time for at least 30 Minutes and will the sync with the RTC */	
	/* This will happen at the full hour and every half */
	if( (0 == LocalTime.Time.Minutes) && ( 0 == LocalTime.Time.Seconds) ){
		/* We need to do a resync if a rtc is arround */
		
		/* Currently there is no RTC driver */
			voAddOneSecondToTime();
		/*									*/
	} else {
		/* We don't need to sync an count */
		voAddOneSecondToTime();
		/* Next is to see if we need to update Date */
		/* Why here? If we have to much calls stacked in each other we will run out of stack */
		if( (LocalTime.Time.Hours == 0) && ( LocalTime.Time.Minutes == 0) && ( LocalTime.Time.Seconds == 0 ) ){
			/* We have a date chang here */
			
		}
	}
	
#if TICKRATE > 1
	divider=0;

} else {
	/* We need to wait a bit */
	divider++;	
}
#endif	
	
}


void voAddOneSecondToTime(){
	
	if(LocalTime.Time.Seconds >= 59){
		LocalTime.Time.Seconds=0;
		/* We need to add a Miunte to Time */
		voAddOneMinuteToTime();
	} else {
		LocalTime.Time.Seconds++;
	}
	
}


void voAddOneMinuteToTime(){
	
	if(LocalTime.Time.Minutes>=59){
		/* We need to add one Hour to Time */
		LocalTime.Time.Minutes=0;
		voAddOneHourToTime();
	} else {
		LocalTime.Time.Minutes++
	}
}

void voAddOneHourToTime(){
	if(LocalTime >= 23 ){
		/* We need to add a day to Time */
		LocalTime.Time.Hours=0;
	} else {
		LocalTime.Time.Hours++;
	}
}

void voAddOneDayToDate( ){
	/*  This is a bit tricky as we have 30 , 31 , 28 and 29 days in a month */
	if (LocalTime.Date.DayOfWeek != 0) {
		if( LocalTime.Date.DayOfWeek >= 7 ){
			LocalTime.Date.DayOfWeek=1;
		} else {
			LocalTime.Date.DayOfWeek++;
		}
	
	
		switch (LocalTime.Date.Month){
			case 0:{
				/* We have no month 0 and won't count the Date */
			} break;
		
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
			{
				if(LocalTime.Date.Day >= 31){
					/* We need to change the Month */
					LocalTime.Date.Day=1;
					voAddOneMonthToDate();
				} else {
					LocalTime.Date.Day++;
				}
			} break;
		
			case 2:{
				/* Wenn /4 ohne rest und nict /100 aber /400 dann schaltjahr */
				if( ( ( (LocalTime.Date.Year % 4) == 0) && ( (LocalTime.Date.Year % 100) != 0) ) || ( ( LocalTime.Date.Year % 400) == 0 )){
					/* 29 Days */
					if(LocalTime.Date.Day >= 29){
						/* We need to change the Month */
						voAddOneMonthToDate();
						LocalTime.Date.Day=1;
					} else {
						LocalTime.Date.Day++;
					}
				} else {
					/* 28 Days */
					if(LocalTime.Date.Day >= 28){
						/* We need to change the Month */
						LocalTime.Date.Day=1;
						voAddOneMonthToDate();
						} else {
						LocalTime.Date.Day++;
					}
				}
			} break;
		
		
		   case 4:
		   case 6:{
		   case 9:{
		   case 11:{
			   if(LocalTime.Date.Day >= 30){
				   /* We need to change the Month */
				   voAddOneMonthToDate();
				   LocalTime.Date.Day=1;
			   
				   } else {
				   LocalTime.Date.Day++;
			   }
		   } break;
	   
		   default:{
			   /* This is strange */
			   /* We will set the Date to Zero */
			   LocalTime.Date.Day=0;
		   } break;
     }
}

void voAddOneMonthToDate ( ){
	/* We have 12 Months a year */
	if(LocalTime.Date.Month>0){
	if(LocalTime.Date.Month>=12){
		LocalTime.Date.Month=1;
		voAddOneYearToDate();
	} else {
		LocalTime.Date.Month++;
	}
	
	}
	
	
}
void voAddOneYearToDate ( ){
	/* */
	if( (LocalTime.Date.Year<UINT16_MAX) && ( LocalTime.Date.Year > 0) ){
		LocalTime.Date.Year++;
	}
}
