/*
 * usb_cdc_helper.c
 *
 * Created: 16.09.2023 12:21:16
 *  Author: CalM
 */ 

#include "usb_cdc_helper.h"

volatile FILE USBSerialStream ;
volatile bool connected = false;

bool boGetConnected( void ){
	return connected;
}

void voSetConnected(bool con){
	connected = con;
}

FILE* GetUSBStream( void ){
	return (FILE*)&USBSerialStream;
}

void voSetUSBStream(FILE Stream){
	USBSerialStream = Stream;
}