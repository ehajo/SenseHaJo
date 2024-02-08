/*
 * HW_USART_SPI.c
 *
 * Created: 04.03.2013 12:30:53
 *  Author: CalM
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include "HW_USART_SPI.h"
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>


/*ATMEGA32U2 USART0*/

#define DDR_HW_CS DDRC
#define PORT_HW_CS PORTC
#define PIN_HW_CS  PC2

#define DDR_HW_XCKL DDRD
#define PORT_HW_XCKL PORTD
#define PIN_HW_XCKL PD5

static SPI_Transfer_t* TransferPtr=NULL;
static volatile uint8_t TransferLen=0;

void USART_Chip_Select()
{
	PORT_HW_CS&=~(1<<PIN_HW_CS);
}

void USART_Chip_Deselect()
{
	PORT_HW_CS|=(1<<PIN_HW_CS);
}

void USART_SPI_init()
{
	UBRR1 = 0;
	//Generischer Code
	PORT_HW_CS|=(1<<PIN_HW_CS);
	DDR_HW_CS |=(1<<PIN_HW_CS);
	
	PORT_HW_XCKL|=(1<<PIN_HW_XCKL);
	DDR_HW_XCKL|=(1<<PIN_HW_XCKL);
		
	/* Set MSPI mode of operation and SPI data mode 0. */
	//UCSR1C = (1<<UMSEL11)|(1<<UMSEL10)|(1<<UCPHA1)|(1<<UCPOL1);
	
	/*Setup MSPI mode of operation and SPI data mode */
	//UCSR1C = (1<<UMSEL11)|(1<<UMSEL10)|(0<<UCPHA1)|(0<<UCPOL1);
	
	UCSR1C = (1<<UMSEL11)|(1<<UMSEL10)|(0<<1)|(0<<UCPOL1);
	
	
	/* Baudrate = Fosc / ( 2 * ( UBBRn + 1 ) ) */
	/* with UBRR1 = 5 we got fCpu / 2 * ( 5+1 ) */
	/* We need 1.8 to 0.9 MHz here */
	//UBRR1 = 5; //Results in 1.33Mhz @16MHz
	UBRR1 = 4; //Results in 1.6Mhz @16MHz
	/* Enable receiver and transmitter. */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	
}

uint8_t USART_SPI_Transmitt(uint8_t data)
{
	
	/* Wait for empty transmit buffer */
	while( !( UCSR1A & (1<<UDRE1)) );
	/* Put data into buffer, sends the data */
	UDR1 = data;
	/* Wait for data to be received */
	while( !(UCSR1A & (1<<RXC1)) );
	/* Get and return received data from buffer */
	uint8_t temp=UDR1;
	return	temp;
}

void USART_SPI_Start_Transmitt(SPI_Transfer_t* Transfer)
{
	Transfer->TransferDone=0;
	TransferLen=Transfer->BufferLen;
	TransferPtr = Transfer;
	USART_Chip_Select();
	UCSR1B |= (1<<RXCIE1) | (1<<UDRE1) ; /* Enable TX Done and RX Done ISR */
}

ISR(USART1_UDRE_vect) /*Data Register Empty */
{
	if(TransferLen>0){
		TransferLen--;
		if(TransferPtr->TX_Buffer!=NULL) {
			UDR1=*TransferPtr->TX_Buffer;
		} else {
			UDR1=0xAA;	/* Dummy Write */
		}
	
	} else {
      /* Transfer is done */
	  UCSR1B &= ~( (1<<RXCIE1) | (1<<TXCIE1) | (1<<UDRE1) ); /* Disable ISRs */
	  USART_Chip_Deselect();
	}
	
}


ISR(USART1_RX_vect){
	/* Read Datat to Buffer */
	/* If last byte Received notify that data is ready */
	
	if(TransferPtr->RX_Buffer!=NULL){
		*TransferPtr->RX_Buffer=UDR1;
		TransferPtr->RX_Buffer++;
	} else {
		__attribute__ ((unused)) uint8_t  temp=UDR1;	/* Dummy Read */
	}
	
}