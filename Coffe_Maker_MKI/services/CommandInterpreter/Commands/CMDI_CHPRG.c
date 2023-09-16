/*
 * CMDI_CHPRG.c
 *
 * Created: 15.09.2023 18:42:54
 *  Author: CalM
 */ 


#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../../eepromFileSystem/eepromFileSystem.h"
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "COMMON_VALUE_STORE.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_CHPRG.h"
void voCMDI_CHPRG(CMDI_CMD_DATA_t COMMAND){
	/* Set the PRG to Change */
	if(COMMAND.DIR==CMDI_SET){
		if(COMMAND.u32CmdValue<8){
			uint8_t u8ProgToChange=COMMAND.u32CmdValue;
			voSetProgToChange(u8ProgToChange);
			voResponseOk();
			} else {
			voResponseParamErr();
		}
	} else if(COMMAND.DIR==CMDI_GET) {
		if(boGetConnected()==true){
			uint8_t u8ProgToChange = u8GetProgToChange();
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("OK:%i\n\r"),u8ProgToChange);
		}
		
	} else if(COMMAND.DIR == CMDI_HELP){
		/* Print Help Text here */
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("USAGE: 'GET CHPRG' or 'SET CHPRG XXXX' with range of 0 to 7\n\r"));
		}
	} else {
		voResponseParserErr();
	}
	return;
}