/*
 * CMD_SYS_HELP.c
 *
 * Created: 13.09.2023 20:28:11
 *  Author: CalM
 */ 

#include <avr/pgmspace.h>
#include "stdbool.h"
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "CMDI_SYS_HELP.h"
#include "PARSE_RESPONSE.h"

void voCMDI_SYS_HELP( CMDI_CMD_DATA_t COMMAND ){
	static volatile uint8_t u8BottelsOfBeer=99;
	if(COMMAND.DIR==CMDI_HELP){
		/* We print Out the Help here */
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR(	"SenseHaJo - Help\n\r"
			"------------------------------------------------------------------------------------------\n\r"
			"Use SET, GET, CMD and HELP\n\r"
			"Commands are:\n\r"
			"GPIO - Get GPIO State	(Can only use GET)\n\r"
			"TEMP - Set or Get Prog. Temp.\n\r"
			"PRG - Set or Get current Prog.\n\r"
			"CHPRG - Set or Get the current Prog. to modify\n\r"
			"PRWA - Set or Get the amount of water used to wet the pad\n\r"
			"WATER - Set or Get the amount of water used to brew the coffee\n\r"
			"WAIT - Set or Get the amount of time to wait between wet the pad and make the coffee\n\r"
			"CURPWR - Get the current power used\n\r"
			"BOILERTMP - Get the current boiler temp.\n\r"
			"LEVEL - Set or Get the current fill level of the Cup\n\r"
			"AUTOOF - Set or Get the Time in Minutes until Auto Off\n\r"
			"STBYTEMP - Set or Get the standby temp.\n\r"
			"MAKE (ONECUP , TWOCUP) used to make coffee\n\r"
			"ECHO - enable or disable serial ECHO\n\r"
			"After changes to the Program reload with CHPRG"
			"For more Help type HELP <COMMAND>\n\r"
			"-------------------------------------------------------------------------------------------\n\r"
			));
		}
	} else if(COMMAND.DIR==CMDI_BEER){
		if(u8BottelsOfBeer>0){
			if(boGetConnected()==true){
				FILE* USBSTREAM = GetUSBStream();
				fprintf_P(USBSTREAM,PSTR("%i Bottle(s) of beer on the wall, %i bottle(s) of beer\n\r"
				"Take one down and pass it around,\n\r"
				"%i bottle(s) of beer on the wall\n\r"),u8BottelsOfBeer,u8BottelsOfBeer,u8BottelsOfBeer-1);
				u8BottelsOfBeer--;	
			}
		} else { /* Bottels are empty */
			if(boGetConnected()==true){
				FILE* USBSTREAM = GetUSBStream();
				fprintf_P(USBSTREAM,PSTR("No (more) bottles of beer on the wall,\n\r"
				"No (more) bottles of beer.\n\r"
				"Go to the store and buy some more,\n\r"
				"Ninety-nine bottles of beer on the wall.\n\r")
				);
				u8BottelsOfBeer=99;			
			}
		}
	} else {
		voResponseParserErr();
	}
	return;	
}