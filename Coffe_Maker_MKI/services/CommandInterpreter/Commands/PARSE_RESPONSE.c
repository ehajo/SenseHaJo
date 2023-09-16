/*
 * PARSE_ERROR.h
 *
 * Created: 13.09.2023 20:40:25
 *  Author: CalM
 */ 


#ifndef PARSE_ERROR_H_
#define PARSE_ERROR_H_

#include <stdio.h>
#include "stdbool.h"
#include <avr/pgmspace.h>
#include "../../usb_cdc_helper/usb_cdc_helper.h"

#include "PARSE_RESPONSE.h"

void voResponseParamErr(){
	if(boGetConnected()==true){
		FILE* USBSTREAM = GetUSBStream();
		fprintf_P(USBSTREAM,PSTR("ERR:PARAM\n\r"));
	}
}

void voResponseParserErr(){
	if(boGetConnected()==true){
		FILE* USBSTREAM = GetUSBStream();
		fprintf_P(USBSTREAM,PSTR("ERR:PARSER\n\r"));
	}
}

void voResponseOk( ){
	if(boGetConnected()==true){
		FILE* USBSTREAM = GetUSBStream();
		fprintf_P(USBSTREAM,PSTR("OK\n\r"));
	}
}



#endif /* PARSE_ERROR_H_ */