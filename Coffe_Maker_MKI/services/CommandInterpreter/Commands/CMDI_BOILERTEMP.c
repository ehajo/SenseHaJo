/*
 * CMDI_BOILERTEMP.c
 *
 * Created: 15.09.2023 18:56:01
 *  Author: CalM
 */ 

#include "stdbool.h"
#include <avr/pgmspace.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../services/boilertempcontroller/boilertempcontroller.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_BOILERTEMP.h"

void voCMDI_BOILERTEMP(CMDI_CMD_DATA_t COMMAND){
	
	if(COMMAND.DIR==CMDI_GET){
		if(boGetConnected()==true){
			FILE* USBSTREAM = (FILE*)GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("OK:%i\n\r"), s16BTC_GetCurrentBoilerTemp());
		}
		
	} else if(COMMAND.DIR==CMDI_SET) {
		voResponseParserErr();
	} else if(COMMAND.DIR == CMDI_HELP){
		/* Print Help Text here */
		if(boGetConnected()==true){
			FILE* USBSTREAM = (FILE*)GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("USAGE: 'GET BOILERTMP' to read the current boiler temp in 1/100 degree C.\n\r"));
		}
	} else {
		voResponseParserErr();
	}
	return;
}