/*
 * CMDI_WAIT.c
 *
 * Created: 14.09.2023 19:36:42
 *  Author: CalM
 */ 

#include <avr/pgmspace.h>
#include <stdbool.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../eepromFileSystem/eepromFileSystem.h"
#include "PARSE_RESPONSE.h"
#include "COMMON_VALUE_STORE.h"
#include "CMDI_WAIT.h"


void voCMDI_WAIT( CMDI_CMD_DATA_t COMMAND ){
	/* Change Wait time */
	if(COMMAND.DIR==CMDI_SET){
		uint8_t u8ProgToChange = u8GetProgToChange();
		boEEFS_UpdateUserProgramm(u8ProgToChange,USRPRG_PREWATERWAIT,COMMAND.u32CmdValue);
		voResponseOk();
	} else if(COMMAND.DIR==CMDI_GET) {
		if(boGetConnected()==true){
			uint8_t u8ProgToChange = u8GetProgToChange();
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("OK:%i\n\r"),boEEFS_ReadUserProgrammParam(u8ProgToChange,USRPRG_PREWATERWAIT));
		}
	} else if(COMMAND.DIR == CMDI_HELP){
		/* Print Help Text here */
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("USAGE: 'GET WAIT' or 'SET WAIT XXXX' with range of 0 to 65535\n\r"));
		}
	} else {
		voResponseParserErr();
	}
	return;
}