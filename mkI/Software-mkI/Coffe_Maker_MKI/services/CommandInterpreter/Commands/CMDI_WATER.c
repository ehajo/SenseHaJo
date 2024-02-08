/*
 * CMDI_WATER.c
 *
 * Created: 13.09.2023 21:00:00
 *  Author: CalM
 */ 

#include <avr/pgmspace.h>
#include <stdbool.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../eepromFileSystem/eepromFileSystem.h"
#include "COMMON_VALUE_STORE.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_WATER.h"

/* Change Water time */
void voCMDI_WATRER(CMDI_CMD_DATA_t COMMAND){
	if(COMMAND.DIR==CMDI_SET){
		uint8_t u8ProgToChange = u8GetProgToChange();
		boEEFS_UpdateUserProgramm(u8ProgToChange,USRPRG_WATERLEVELFULL,COMMAND.u32CmdValue);
		voResponseOk();
	} else if(COMMAND.DIR==CMDI_GET) {
		if(boGetConnected()==true){
			uint8_t u8ProgToChange = u8GetProgToChange();
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("OK:%i\n\r"),boEEFS_ReadUserProgrammParam(u8ProgToChange,USRPRG_WATERLEVELFULL));
		
		}
	} else if(COMMAND.DIR == CMDI_HELP){
		/* Print Help Text here */
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("USAGE: 'GET WATER' or 'SET WATER XXXX' with range of 0 to 65535\n\r"));
		}
	} else {
		voResponseParserErr();
	}
	return;
}