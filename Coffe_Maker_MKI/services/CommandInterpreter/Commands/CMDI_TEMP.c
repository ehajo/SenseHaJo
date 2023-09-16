/*
 * CMD_Temp.c
 *
 * Created: 13.09.2023 20:26:22
 *  Author: CalM
 */ 
#include <avr/pgmspace.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../eepromFileSystem/eepromFileSystem.h"
#include "COMMON_VALUE_STORE.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_TEMP.h"

void voCMDI_TEMP( CMDI_CMD_DATA_t COMMAND ){
	/* Change Temp */
	if(COMMAND.DIR==CMDI_SET){
		uint8_t u8ProgToChange = u8GetProgToChange();
		boEEFS_UpdateUserProgramm(u8ProgToChange,USRPRG_HEATERTEMP,COMMAND.u32CmdValue);
		voResponseOk();
	} else if(COMMAND.DIR==CMDI_GET) {
		if(boGetConnected()==true){
			uint8_t u8ProgToChange = u8GetProgToChange();
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("OK:%i\n\r"),boEEFS_ReadUserProgrammParam(u8ProgToChange,USRPRG_HEATERTEMP));
		}
	} else if(COMMAND.DIR == CMDI_HELP){
		/* Print Help Text here */
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("USAGE: 'GET TEMP' or 'SET TEMP XXXX' with range of 0 to 9999\n\r"));
		}
	
	
	} else {
		voResponseParserErr();
	}
	return;
}