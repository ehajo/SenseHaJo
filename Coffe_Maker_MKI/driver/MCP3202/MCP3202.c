/*
 * MCP3202.c
 *
 * Created: 30.12.2016 20:40:51
 *  Author: CalM
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include "../USART_SPI_Master/HW_USART_SPI.h"

#define MCP3202_SGL 7
#define MCP3202_ODD 6
#define MCP3202_MSBF 5

void voMCP3202_Init( ){
	USART_SPI_init();
}

uint16_t u16MCP3202_ReadChannel(uint8_t Channel)
{

/*
	union {
		uint16_t u16adc_value;
		struct {
			uint8_t u8adc_lowvalue;
			uint8_t u8acd_highvalue;
		}
	} adc_value;
*/	
	uint16_t rawvalue = 0 ; 
	USART_Chip_Select();
	USART_SPI_Transmitt(0x01); /* Start Bit But we need to waste 7 additinal Bits */
	if( 0 == Channel ){
		//adc_value.u8acd_highvalue=USART_SPI_Transmitt( ( 1 << MCP3202_SGL ) |  (1 << MCP3202_ODD) | (1 << MCP3202_MSBF)  ); //SGN=1,ODD=0 -> CH0 SGN=1,ODD=1 -> CH1 ,...MSBF1
		rawvalue=USART_SPI_Transmitt( ( 1 << MCP3202_SGL ) |  (1 << MCP3202_ODD) | (1 << MCP3202_MSBF)  ); //SGN=1,ODD=0 -> CH0 SGN=1,ODD=1 -> CH1 ,...MSBF1
		rawvalue=rawvalue<<8;
		} else {
		//adc_value.u8acd_highvalue=USART_SPI_Transmitt( ( 1 << MCP3202_SGL ) |  (0 << MCP3202_ODD) | (1 << MCP3202_MSBF)  ); //SGN=1,ODD=0 -> CH0 SGN=1,ODD=1 -> CH1 ,...MSBF1
		rawvalue = USART_SPI_Transmitt( ( 1 << MCP3202_SGL ) |  (0 << MCP3202_ODD) | (1 << MCP3202_MSBF)  ); //SGN=1,ODD=0 -> CH0 SGN=1,ODD=1 -> CH1 ,...MSBF1
		rawvalue=rawvalue<<8;
	}
		
	//adc_value.u8adc_lowvalue = USART_SPI_Transmitt(0xAA); //SGN=1,ODD=0 -> CH0 SGN=1,ODD=1 -> CH1 ,...MSBF1
	rawvalue |= USART_SPI_Transmitt(0xAA); //SGN=1,ODD=0 -> CH0 SGN=1,ODD=1 -> CH1 ,...MSBF1
	USART_Chip_Deselect();
	/* Last is to normalize the value to 16Bit */
	//adc_value.u16adc_value = adc_value.u16adc_value << 4;
	rawvalue= rawvalue <<4;
		
	//return adc_value.u16adc_value;
	return rawvalue;
}

uint8_t u8MCP3202_ReadChannel(uint8_t Channel){
	
	return (uint8_t)(u16MCP3202_ReadChannel(Channel)>>8);
}