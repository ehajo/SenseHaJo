/*
 * CMDI_MAEK.c
 *
 * Created: 16.09.2023 11:32:42
 *  Author: CalM
 */ 
#include "stdbool.h"
#include <avr/pgmspace.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../coffee_fsm/coffee_fsm.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_MAKE.h"

void voCMDI_MAKE(CMDI_CMD_DATA_t COMMAND){
	
	if(COMMAND.DIR==CMDI_CMD) {
		switch(COMMAND.u32CmdValue){
			case 0:{
				if(boCoffeeFSM_MakeCoffee(1)==true){
					if(boGetConnected()==true){
						FILE* USBSTREAM = GetUSBStream();
						fprintf(USBSTREAM,"OK:BREWONE\n\r");
					}
				} else {
					if(boGetConnected()==true){
						FILE* USBSTREAM = GetUSBStream();
						fprintf(USBSTREAM,"ERR:BREWERR\n\r");
					}
				}
			} break;
				
			case 1:{
				/* Power On */
				if(boCoffeeFSM_MakeCoffee(2)==true){
					if(boGetConnected()==true){
						FILE* USBSTREAM = GetUSBStream();
						fprintf(USBSTREAM,"OK:BREWTWO\n\r");
					}
				} else {
					if(boGetConnected()==true){
						FILE* USBSTREAM = GetUSBStream();
						fprintf(USBSTREAM,"ERR:BREWERR\n\r");
					}
				}
			} break;
			
			default:{
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf(USBSTREAM,"ERR:BREWERR CNT\n\r");
				}
			} break;
		}
			
	} else if(COMMAND.DIR==CMDI_HELP){
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf(USBSTREAM,"USAGE: 'MAKE ONECUP', 'MAKE TWOCUP' for one or two cups\n\r");
		}
			
	}
	return;
}
