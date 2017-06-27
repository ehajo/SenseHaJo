/*
 * HW_USART_SPI.h
 *
 * Created: 04.03.2013 12:32:27
 *  Author: CalM
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#ifndef HW_USART_SPI_H_
#define HW_USART_SPI_H_
//Achtung: Im .c File noch die Defines anpassen!
#include <stdlib.h>
#include <avr/io.h>
//#warning "USART as SPI included"

typedef struct {
	uint8_t* TX_Buffer;
	uint8_t* RX_Buffer;
	uint8_t BufferLen;
	uint8_t TransferDone;
} SPI_Transfer_t;

void USART_Chip_Select(void);
void USART_Chip_Deselect(void) ;
void USART_SPI_init();
uint8_t USART_SPI_Transmitt(uint8_t data);

#endif /* HW_USART_SPI_H_ */