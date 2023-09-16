/*
 * CMDI_MES_POWER.c
 *
 * Created: 15.09.2023 18:47:53
 *  Author: CalM
 */ 

#include "stdbool.h"
#include <avr/pgmspace.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../powermeter/powermeter.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_MES_POWER.h"

void voCMDI_MES_POWER(CMDI_CMD_DATA_t COMMAND){
	if(COMMAND.DIR==CMDI_GET){
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("OK:%lu\n\r"), u32PWRMTR_GetmAmp());
		}			
	}else if(COMMAND.DIR == CMDI_HELP){
		/* Print Help Text here */
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("USAGE: 'GET CURPWR' to read the current power used in mA\n\r"));
		}
	} else {
		voResponseParserErr();
	}
	return;			
}