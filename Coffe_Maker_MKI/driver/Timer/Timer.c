/*
 * Timer.c
 *
 * Created: 31.12.2016 01:38:57
 *  Author: CalM
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../driver/MCP3202/MCP3202.h"
#include "../../services/powermeter/powermeter.h"
#include "../../services/boilertemp/boilertemp.h"
#include "../../driver/gpio/gpio.h"

static volatile uint16_t Ticks=0;
/* For the Systemticks the 8Bit timer is used as we do a "PWM" with the 16Bit Timer ISR */
void timer1_setup( void );
void timer0_setup( void );

ISR(TIMER0_COMPA_vect) /* This will run with 1kHz */
{
	uint16_t Current=0;
	uint16_t Temp=0;
	Ticks++;
		
	if(Ticks%2==0){
		
		Current=u16MCP3202_ReadChannel(CURRENT_SENS); /*Results in 500Hz */
		/* The Current needs to be processed */
		voPWRMTR_AddNewMessurment(Current);
	}
	
	
	if(Ticks%128==0){
		Temp=u16MCP3202_ReadChannel(TEMPERATUR_SENS); /* Results in 7,8125Hz */
		voBLTMP_AddNewRawValue(Temp);
	}
}

ISR(TIMER1_COMPA_vect){

}



ISR(TIMER1_COMPB_vect){
/* SoftwarePWM Unit */
/* This will be run at 10kHz witch means every 1600 Clocks */

/* SoftwPWM will be Called here */



}

ISR(TIMER1_COMPC_vect){
/* SoftwarePWM Unit */
/* This will be run at 10kHz witch means every 1600 Clocks */

/* SoftwPWM will be Called here */



}


void voInit_HwTimer(){
	timer0_setup();
	timer1_setup();
}


void timer0_setup(){
	TCCR0A = ( 1<< WGM01); /* CTC MODE */
	TCCR0B = ( 0<<CS02) | (1<<CS01) | (1 << CS00)  ; /* Prescale 64 */
	OCR0A =  250;
	TIMSK0 = (1<<OCIE0A);
}

void timer1_setup() /* 10kHz Soft/HW PWM Setup */
{

//	TCCR1A=0;
//	TCCR1B=(1<<WGM12)|(1<<CS10)|(0<<CS12)|(1<<CS11); //64 Preescaler
//	OCR1A = 25; /* Sollte dann 0.1ms ergeben	*/
//	OCR1B = 25; /* Sollte dann 0.1ms ergeben	*/
//	OCR1C = 25; /* Sollte dann 0.1ms ergeben	*/
//	TIMSK1=( (1<<OCIE1A) | ( 1<<OCIE1B ) | (1<<OCIE1C) );

}

uint16_t u16GetSystemTicks(){
	
	return Ticks;	

}

uint16_t u16GetSystemTicksMS(){
	
	return Ticks;

}