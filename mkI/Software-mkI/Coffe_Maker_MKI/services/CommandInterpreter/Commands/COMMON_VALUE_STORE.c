/*
 * COMMON_VALUE_STORE.c
 *
 * Created: 16.09.2023 11:46:20
 *  Author: CalM
 */ 
#include "COMMON_VALUE_STORE.h"

volatile uint8_t u8ProgToChange;

uint8_t u8GetProgToChange(void){
	return u8ProgToChange;
}

void voSetProgToChange(uint8_t value){
	u8ProgToChange=value;
}