/*
 * gpio.h
 *
 * Created: 20.09.2013 14:53:45
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef GPIO_LOC_H_
#define GPIO_LOC_H_


/* The three Fron LEDs */
#define LED3 PORTB6
#define LED3_DDR DDRB
#define LED3_PORT PORTB
#define LED3_PIN PINB

#define LED2 PORTB4
#define LED2_DDR DDRB
#define LED2_PORT PORTB
#define LED2_PIN PINB

#define LED1 PORTD7
#define LED1_DDR DDRD
#define LED1_PORT PORTD
#define LED1_PIN PIND

/* The three Front Switches */
/* These need PullUps enabeld */
#define SWITCH1 PORTB0
#define SWITCH1_DDR DDRB
#define SWITCH1_PORT PORTB
#define SWITCH1_PIN PINB

#define SWITCH2 PORTB5
#define SWITCH2_DDR DDRB
#define SWITCH2_PORT PORTB
#define SWITCH2_PIN PINB

#define SWITCH3 PORTB7
#define SWITCH3_DDR DDRB
#define SWITCH3_PORT PORTB
#define SWITCH3_PIN PINB

/* The next two are the HAL Sensors for the Waterlevel */
/* Those need a PULLUP active */
#define HALL_SW1 PORTC6
#define HALL_SW1_DDR DDRC
#define HALL_SW1_PORT PORTC
#define HALL_SW1_PIN PINC

#define HALL_SW2 PORTC7
#define HALL_SW2_DDR DDRC
#define HALL_SW2_PORT PORTC
#define HALL_SW2_PIN PINC

#define HEATER PORTD1
#define HEATER_DDR DDRD
#define HEATER_PORT PORTD
#define HEATER_PIN PIND

#define PUMP PORTD0
#define PUMP_DDR DDRD
#define PUMP_PORT PORTD
#define PUMP_PIN PIND


/* This are the Strict GPIOs for now */
/* Other Pins will be set by there corresponding drivers */



#endif /* GPIO_LOC_H_ */