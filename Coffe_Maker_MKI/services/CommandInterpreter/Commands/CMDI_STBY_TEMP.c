/*
 * CMDI_STBY_TEMP.c
 *
 * Created: 15.09.2023 19:15:41
 *  Author: CalM
 */ 
#include "stdint.h"
#include "stdbool.h"
#include <avr/pgmspace.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../coffee_fsm/coffee_fsm.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_STBY_TEMP.h"


void voCMDI_STBY_TEMP(CMDI_CMD_DATA_t COMMAND){
	
		if(COMMAND.DIR==CMDI_GET){
			if(boGetConnected()==true){
				FILE* USBSTREAM = GetUSBStream();
				fprintf(USBSTREAM,"OK:%i\n\r", u16GetStandbyTemp());
			}
		} else if(COMMAND.DIR==CMDI_SET) {
			voSetStandbyTemp((uint16_t)COMMAND.u32CmdValue);
			voResponseOk();
		} else if(COMMAND.DIR == CMDI_HELP){
			/* Print Help Text here */
			if(boGetConnected()==true){
				FILE* USBSTREAM = GetUSBStream();
				fprintf_P(USBSTREAM,PSTR("USAGE: 'GET STBYTMP', 'SET STBYTMP XXXX\n\r"));
			}
		} else {
			voResponseParserErr();
		}
	return;
}