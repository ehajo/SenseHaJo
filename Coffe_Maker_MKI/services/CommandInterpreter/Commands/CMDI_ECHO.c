/*
 * CMDI_ECHO.c
 *
 * Created: 15.09.2023 19:21:18
 *  Author: CalM
 */ 
#include "stdbool.h"
#include <avr/pgmspace.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_ECHO.h"
#include "../CommandInterpreterInternalVars.h"

void voCMDI_ECHO(CMDI_CMD_DATA_t COMMAND){
	if(COMMAND.DIR==CMDI_GET){
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf(USBSTREAM,"OK:%i\n\r", CmdConfig.bfEcho);
		}
	
	} else if(COMMAND.DIR==CMDI_SET) {
		if(COMMAND.u32CmdValue>0){
			CmdConfig.bfEcho=1;
			} else {
			CmdConfig.bfEcho=0;
		}
		voResponseOk();
	} else if(COMMAND.DIR == CMDI_HELP){
		/* Print Help Text here */
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("USAGE: 'GET ECHO', 'SET ECHO XXXX' with 0 = OFF and else ON\n\r"));
		}
	} else {
		voResponseParserErr();
	}
	return;
}
