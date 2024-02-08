/*
 * CMDI_PREWATER.c
 *
 * Created: 13.09.2023 21:02:08
 *  Author: CalM
 */ 
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../eepromFileSystem/eepromFileSystem.h"
#include "COMMON_VALUE_STORE.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_PREWATER.h"

void voCMDI_PREWATER(CMDI_CMD_DATA_t COMMAND){
	/* Change PreWater Time */
	if(COMMAND.DIR==CMDI_SET){
		uint8_t u8ProgToChange = u8GetProgToChange();
		boEEFS_UpdateUserProgramm(u8ProgToChange,USRPRG_PREWATERLEVEL,COMMAND.u32CmdValue);
		voResponseOk();
	} else if(COMMAND.DIR==CMDI_GET) {
		if(boGetConnected()==true){
			uint8_t u8ProgToChange = u8GetProgToChange();
			FILE* USBSTREAM = GetUSBStream();
			fprintf(USBSTREAM,"OK:%ld\n\r",boEEFS_ReadUserProgrammParam(u8ProgToChange,USRPRG_PREWATERLEVEL));
		}
	} else if(COMMAND.DIR == CMDI_HELP){
		/* Print Help Text here */
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR("USAGE: 'GET PRWA' or 'SET PRWA XXXX' with range of 0 to 65535\n\r"));
		}
	} else {
		voResponseParserErr();
	}
	return;
}