/*
 * CMDI_GPIO.c
 *
 * Created: 16.09.2023 11:27:49
 *  Author: CalM
 */ 

#include "stdbool.h"
#include <avr/pgmspace.h>
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"
#include "../../driver/gpio/gpio.h"
#include "../../driver/senseo_button/senseo_button.h"
#include "../../driver/gpio/gpio_typedef.h"
#include "PARSE_RESPONSE.h"
#include "CMDI_GPIO.h"

void voCMDI_GPIO(CMDI_CMD_DATA_t COMMAND){
	if(COMMAND.DIR==CMDI_GET){
		switch(COMMAND.u32CmdValue){
			case 0:
			case 1:
			case 2:{
				if(boGetConnected()==true){
					emSWITCHSTATE emReturnValue=SWITCH_RELEASED;
					emReturnValue = emGetSwitchState( COMMAND.u32CmdValue );
					FILE* USBSTREAM = GetUSBStream();
					if(SWITCH_RELEASED == emReturnValue){
						fprintf(USBSTREAM,"SWITCH %lu RELEASED\n\r",COMMAND.u32CmdValue);
						} else {
						fprintf(USBSTREAM,"SWITCH %lu PRESSED\n\r",COMMAND.u32CmdValue);
					}
				}
			}break;
					
			case 3:{
				if(boGetConnected()==true){
					emSWITCHSTATE emReturnValue=SWITCH_RELEASED;
					emReturnValue = SBNT_emGetState();
					FILE* USBSTREAM = GetUSBStream();
					if(SWITCH_RELEASED == emReturnValue){
						fprintf(USBSTREAM,"DISPLAY BTN RELEASED\n\r");
						} else {
						fprintf(USBSTREAM,"DISPLAY BTN PRESSED\n\r");
					}
				}
			}break;
				/*
				#define LOW_LEVEL_SENSOR ( 0 )
				#define HIGH_LEVEL_SENSOR ( 1 )
				*/
			case 4:{
				if(boGetConnected()==true){
					emSWITCHSTATE SwitchState=SWITCH_RELEASED;
					SwitchState = emGetHallSwitchState( 0 );
					FILE* USBSTREAM = GetUSBStream();
					if(SWITCH_RELEASED == SwitchState){
						fprintf(USBSTREAM,"WATER LOW LEVEL LOW \n\r");
						} else {
						fprintf(USBSTREAM,"WATER LOW LEVEL HIGH\n\r");
					}
				}
						
						
			}break;
					
			case 5:{
				if(boGetConnected()==true){
					emSWITCHSTATE SwitchState=SWITCH_RELEASED;
					SwitchState = emGetHallSwitchState( 1 );
					FILE* USBSTREAM = GetUSBStream();
					if(SWITCH_RELEASED == SwitchState){
						fprintf(USBSTREAM,"WATER HIGH LEVEL LOW\n\r");
						} else {
						fprintf(USBSTREAM,"WATER HIGH LEVEL HIGH\n\r");
					}
				}
			}break;
					
			default:{
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf_P(USBSTREAM,PSTR(	"GPIO - Help\n\r"
					"------------------------------------------------------------------------------------------\n\r"
					"GPIO index out of range, see HELP GPIO for more info"
					"-------------------------------------------------------------------------------------------\n\r"
					));
				}
			} break;
		} //END Switch
			
	} else if(COMMAND.DIR==CMDI_SET){
			if(boGetConnected()==true){
				FILE* USBSTREAM = GetUSBStream();
				fprintf_P(USBSTREAM,PSTR(	"GPIO - Help\n\r"
				"------------------------------------------------------------------------------------------\n\r"
				"Only GET is supported for GPIO , see HELP GPIO for more info"
				"-------------------------------------------------------------------------------------------\n\r"
				));
			}
	} else if (COMMAND.DIR==CMDI_HELP) {
		if(boGetConnected()==true){
			FILE* USBSTREAM = GetUSBStream();
			fprintf_P(USBSTREAM,PSTR(	"GPIO - Help\n\r"
			"------------------------------------------------------------------------------------------\n\r"
			"Use GET to read or set the status of GPIOs\n\r"
			"The IO maps as follows:"
			"0 = SWITCH 0"
			"1 = SWITCH 1"
			"2 = SWITCH 2"
			"3 = Display button"
			"4 = Water sensor low level"
			"5 = Water sensor high level"
			"Example: 'GET GPIO 0' will return the current level of SWITCH 0"
			"-------------------------------------------------------------------------------------------\n\r"
			));
		}
	} else {
		voResponseParamErr();
	}
	return;
}