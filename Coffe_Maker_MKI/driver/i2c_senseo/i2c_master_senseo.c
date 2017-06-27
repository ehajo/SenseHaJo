/*
 * i2c_master_senseo.c
 *
 * Created: 06.01.2017 08:45:22
 *  Author: calm
 */ 

#include <stdlib.h>
#include <stdio.h>

#define SDC_PIN 
#define SDC_PORT
#define SDC_DDR

#define SDA_PIN
#define SDA_PORT
#define SDA_DDR

/* We need to use 8kHz at max to drive the LCD */
#define I2C_CLOCK 8000UL 
/* We need the Quarter for our wafeform generation */
#define I2C_CLOCK_Q_MS (1000 / ( I2C_CLOCK / 4 ) )


void __attribute__((always_inline)) voSDATriState();
void __attribute__((always_inline)) voSDCTriState();

void __attribute__((always_inline)) voSDALow();
void __attribute__((always_inline)) voSDCLow();

void __attribute__((always_inline)) voSDCHigh();
void __attribute__((always_inline)) voSDAHigh();

void __attribute__((always_inline)) voSDAPullupHighInput();
void __attribute__((always_inline)) voSDCPullupHighInput();


void voI2cInit(){
	/* This will init out I2C Pins and also be used for the Button on the clock line */
	
	/* We set the Pins to be tristate with pullup here */
	voSDAPullupHighInput();
	voSDCPullupHighInput();

}

void voI2cStart(){
	/* This will issue a start condition on the Bus */
	voSDALow();
	_delay_ms(I2C_CLOCK_Q_MS);
	/* This should issue a start condition on the Bus */
}

uint8_t voI2cWrite(uint8_t Data){
	/* This will write a Byte to the I2C Bus */
	uint8_t u8Ack=0;
	for(uint8_t i=0;i<8;i++) {
		voSDAPullupHighInput();
		voSDCLow();
		_delay_ms(I2C_CLOCK_Q_MS);
		/* But Data to the Bus */
		if( (Data&0x01)==0){
			voSDALow();
		} else {
			voSDAPullupHighInput();
		}
		voSDCPullupHighInput();
		_delay_ms(I2C_CLOCK_Q_MS);
		Data=Data>>1;
	}
	/* Last is to read the ACK / NACK Bit from the Slave */
	voSDAPullupHighInput();
	voSDCLow();
	_delay_ms(I2C_CLOCK_Q_MS);
	voSDCPullupHighInput();
	/* We now will see if the slave send an ack to us */
	// u8Ack =  ( ( ( SDA_PIN & ( 1 << SDA_PIN ) ) >> SDA_PIN  ) ) ;
	_delay_ms(I2C_CLOCK_Q_MS);
	voSDCLow();
	return u8Ack;
	
}

uint8_t voI2cRead(uint8_t* Data, uint8_t SendAck){
	/* This will write a Byte to the I2C Bus */
	uint8_t u8Ack=0;
	for(uint8_t i=0;i<8;i++) {
		voSDAPullupHighInput();
		voSDCLow();
		_delay_ms(I2C_CLOCK_Q_MS);
		/* Get Data from the Bus */
		*Data=(*Data)>>1;
		
		/* if( ( SDA_PIN & ( 1 << SDA_PIN) ) > 0 ){
			*Data |= 0x80;
		} 
		*/
		/* Data has been read */
		voSDCPullupHighInput();
		_delay_ms(I2C_CLOCK_Q_MS);
		
	}
	/* Last is to read the ACK / NACK Bit from the Slave */
	voSDAPullupHighInput();
	voSDCLow();
	_delay_ms(I2C_CLOCK_Q_MS);
	voSDCPullupHighInput();
	/* We now will see if the slave send an ack to us */
	if(SendAck>0){
		voSDAPullupHighInput();
	} else {
		voSDALow();
	}
	_delay_ms(I2C_CLOCK_Q_MS);
	voSDCLow();
	return u8Ack;
	
}


void __attribute__((always_inline)) voSDATriState(){
	/* This will Tristate in Inputstate */
	// SDA_DDR  &= ~(1<<SDA_PIN);
	// SDA_PORT &= ~(1<<SDA_PIN);
}


void __attribute__((always_inline)) voSDCTriState(){
/* This will Tristate in Inputstate */
	// SDC_DDR  &= ~(1<<SDC_PIN);
	// SDC_PORT &= ~(1<<SDC_PIN);
}

void __attribute__((always_inline)) voSDALow(){
	/*This drives the PIN as Output LOW */
	// SDA_PORT &= ~(1<<SDA_PIN);
	// SDA_DDR |=   (1<<SDA_PIN);
}

void __attribute__((always_inline)) voSDCLow(){
	/*This drives the PIN as Output LOW */
	SDC_PORT &= ~(1<<SDC_PIN);
	// SDC_DDR |=   (1<<SDC_PIN);
}

void __attribute__((always_inline)) voSDCHigh{
	/*This drives the PIN as Output HIGH */
	SDC_PORT |=   (1<<SDC_PIN);
	SDC_DDR  |=   (1<<SDC_PIN);
}

void __attribute__((always_inline)) voSDAHigh{
	/*This drives the PIN as Output HIGH */
	SDA_PORT |=   (1<<SDA_PIN);
	SDA_DDR  |=   (1<<SDA_PIN);
}

void __attribute__((always_inline)) voSDAPullupHighInput{
	/*This will set the Pin to Input with Pullup enable */
	SDA_DDR  &=  ~(1<<SDA_PIN);
	SDA_PORT |=   (1<<SDA_PIN);
	
}

void __attribute__((always_inline)) voSDCPullupHighInput{
	/*This will set the Pin to Input with Pullup enable */
	SDC_DDR  &=  ~(1<<SDC_PIN);
	SDC_PORT |=   (1<<SDC_PIN);
}