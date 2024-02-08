/*
 * CMDI_CUP_LEVEL.c
 *
 * Created: 15.09.2023 19:01:22
 *  Author: CalM
 */ 

#include "stdbool.h"
#include <avr/pgmspace.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../services/coffee_fsm/coffee_fsm.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_CUP_LEVEL.h"

void voCMDI_CUP_LEVEL(CMDI_CMD_DATA_t COMMAND){
		if(COMMAND.DIR==CMDI_GET){
			if(boGetConnected()==true){
				FILE* USBSTREAM = GetUSBStream();
				fprintf(USBSTREAM,"OK:%i\n\r", u8GetCupLevel());
			}
			
		} else if(COMMAND.DIR==CMDI_SET) {
			voSetCupLevel((uint8_t)COMMAND.u32CmdValue);
			voResponseOk();
			
		} else if(COMMAND.DIR == CMDI_HELP){
			/* Print Help Text here */
			if(boGetConnected()==true){
				FILE* USBSTREAM = GetUSBStream();
				fprintf_P(USBSTREAM,PSTR("USAGE: 'GET LEVLE' or 'SET LEVEL XXXX' with range of 0 to 2\n\r"));
			}
		} else {
			voResponseParserErr();
		}
		return;
}