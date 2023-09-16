/*
 * CMDI_POWER.c
 *
 * Created: 15.09.2023 19:06:38
 *  Author: CalM
 */ 

#include "stdbool.h"
#include <avr/pgmspace.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../coffee_fsm/coffee_fsm.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_POWER.h"

void voCMDI_POWER(CMDI_CMD_DATA_t COMMAND){
	if(COMMAND.DIR==CMDI_CMD) {
		switch(COMMAND.u32CmdValue){
			case 0:{
				/* Power Off */
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf(USBSTREAM,"OK:POWERDOWN\n\r");
					voCoffeeFSM_SetPower(false);
				}
			} break;
					
			case 1:{
				/* Power On */
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf(USBSTREAM,"OK:POWERON\n\r");
					voCoffeeFSM_SetPower(true);
				}
			} break;
					
			default:{
				voResponseParamErr();
			} break;
		}//End Switch
	} else if(COMMAND.DIR==CMDI_HELP) {
		voResponseParamErr();
	}
	return;		
}