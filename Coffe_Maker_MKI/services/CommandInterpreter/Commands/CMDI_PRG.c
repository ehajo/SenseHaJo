/*
 * CMD_Temp.c
 *
 * Created: 13.09.2023 20:26:22
 *  Author: CalM
 */ 

#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../coffee_fsm/coffee_fsm.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_TEMP.h"



void voCMDI_PRG( CMDI_CMD_DATA_t COMMAND ){
		/* Change PRG to Load */
		if(COMMAND.DIR==CMDI_SET){
			
			if(boCoffeeFSM_LoadProgramm(COMMAND.u32CmdValue)==true){
				voResponseOk();
			} else {
				voResponseParamErr();
			}
		} else if (COMMAND.DIR==CMDI_GET){
			/* We shall return the curren Programm */
			if(boGetConnected()==true){
				FILE* USBSTREAM = GetUSBStream();
				fprintf_P(USBSTREAM,PSTR("OK:%i\n\r"),u8GetCurrenProgrammIdx());
			}
			
		} else if(COMMAND.DIR == CMDI_HELP){
			/* Print Help Text here */
			if(boGetConnected()==true){
				FILE* USBSTREAM = GetUSBStream();
				fprintf_P(USBSTREAM,PSTR("USAGE: 'GET PRG' or 'SET PRG XXXX' with range of 0 to 7\n\r"));
			}
		} else {
			voResponseParserErr();
		}
	return;
}