/*
 * senseo_button.c
 *
 * Created: 25.01.2017 23:04:03
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include <avr/io.h>
#include "senseo_button.h"

#define SCL_PIN 4
#define SCL_DDR DDRC
#define SCL_PINREG PINC
#define SCL_PORT PORTC

void SBNT_voInit( void ){
	/* We don't need a Init Here in this HW Version */
	
}

void SBNT_SetSCLtoInput( void ){
	
	SCL_DDR &= ~(1<<SCL_PIN);
	SCL_PORT &= ~(1<<SCL_PIN);
	SCL_PINREG &= ~(1<<SCL_PIN);
	
}
emSWITCHSTATE SBNT_emGetState( void ){
	
	
	//uint8_t SCL_DDR_BACKUP = SCL_DDR;
	//uint8_t SCL_PORT_BACKUP = SCL_PORT;
	uint8_t PIN_VALUE=0;
	emSWITCHSTATE emReturnvalue = SWITCH_RELEASED;	
	
	/* We need to modify the I2C SCL PIN for this */
	//SCL_DDR &= ~(1<<SCL_PIN); /* Switch to IN */
	/* PORT WILL by HIGH by Default */	
	//SCL_PORT &= ~(1<<SCL_PIN);
	/* PIN will be INPUT now */	
	PIN_VALUE = SCL_PINREG;
	/* We may need some Nops here */
	if( 0 != ( PIN_VALUE & ( 1 << SCL_PIN ) ) ) {
			emReturnvalue = SWITCH_RELEASED;	
	} else {
			emReturnvalue = SWITCH_PRESSED;
	}
	 
	/* Restore PORT and DDR */
	//SCL_PORT = SCL_PORT_BACKUP;
	//SCL_DDR = SCL_DDR_BACKUP ;
	
	return emReturnvalue;
	
}
