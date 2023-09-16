/*
 * CMDI_AUTOPOWER.c
 *
 * Created: 15.09.2023 19:12:04
 *  Author: CalM
 */ 

#include "stdbool.h"
#include <avr/pgmspace.h>
#include "../../coffee_fsm/coffee_fsm.h"
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_AUTOPOWER.h"


void voCMDI_AUTOPOWER(CMDI_CMD_DATA_t COMMAND){
	if(COMMAND.DIR==CMDI_GET){
		if(boGetConnected()==true){
			FILE* USBSTREAM = (FILE*)GetUSBStream();
			

			fprintf(USBSTREAM,"OK:%i\n\r", u8GetAutoPowerOff());
		}
		
		} else if(COMMAND.DIR==CMDI_SET) {
		voSetAutoPowerOff((uint8_t)COMMAND.u32CmdValue);
		voResponseOk();
		
		} else if(COMMAND.DIR == CMDI_HELP){
		/* Print Help Text here */
		if(boGetConnected()==true){
			FILE* USBSTREAM = (FILE*)GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("USAGE: 'SET AUTOOFF ON', 'SET AUTOOFF OFF' or 'SET AUTOOFF OFF' \n\r"));
		}
		} else {
		voResponseParserErr();
	}
	return;
}