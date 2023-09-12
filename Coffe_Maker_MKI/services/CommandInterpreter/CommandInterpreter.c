/*
 * CommandInterpreter.c
 *
 * Created: 26.04.2017 20:29:59
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include <string.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "CommandInterpreter.h"
#include "../coffee_fsm/coffee_fsm.h"
#include "../eepromFileSystem/eepromFileSystem.h"
#include "../powermeter/powermeter.h"
#include "../boilertempcontroller/boilertempcontroller.h"
#include "../../driver/gpio/gpio_typedef.h"

#define INPUTBUFFERSIZE ( 64 )
#define LINE_END '\r'

volatile char chInputBuffer[INPUTBUFFERSIZE+1];
volatile uint8_t u8InputBufferWriteIdx;

volatile uint8_t u8ProgToChange;

typedef struct {
	uint8_t bfEcho: 1;
	uint8_t bfReserved0: 1;
	uint8_t bfReserved1: 1;
	uint8_t bfReserved2: 1;
	uint8_t bfReserved3: 1;
	uint8_t bfReserved4: 1;
	uint8_t bfReserved5: 1;
	uint8_t bfReserved6: 1;
} CmdConfigBit_t;

volatile CmdConfigBit_t CmdConfig;
volatile uint8_t u8BottelsOfBeer=99;

typedef enum {
	CMDI_SET,
	CMDI_GET,
	CMDI_CMD,
	CMDI_HELP,
	CMDI_BEER, /* For 99 Bottels of beer */
	CMDI_DIR_CNT,
	CMDI_EOF_DIR,
} CMDI_CMD_DIR_t;

typedef enum {
	CMDI_TEMP,
	CMDI_PREWATER,
	CMDI_WATER,
	CMDI_WAIT,
	CMDI_PRG,
	CMDI_CHPRG,
	CMDI_NONE,
	CMDI_MES_POWER,
	CMDI_BOILERTEMP,
	CMDI_CUP_LEVEL,
	CMDI_AUTOPOWER,
	CMDI_STBY_TEMP,
	CMDI_SYS_HELP,
	CMDI_ECHO,
	CMDI_GPIO,
	/* Commands for CMD */
	CMDI_MAKE,
	CMDI_POWER,

}CMDI_CMD_t;

			
typedef struct{
CMDI_CMD_DIR_t DIR;
CMDI_CMD_t CMD;
uint32_t u32CmdValue;
} CMDI_CMD_DATA_t; 





void voProcessData( void );
void voResponseParamErr( void );
void voResponseOk( void );
void voResponseParserErr( void );

extern bool boCoffeeFSM_LoadProgramm(uint8_t idx);
extern bool boGetConnected( void );
extern FILE* GetUSBStream( void );

void voCleanBuffer( void ){
	for(uint8_t i =0;i<INPUTBUFFERSIZE;i++){
		chInputBuffer[i]=0;
	}
	u8InputBufferWriteIdx=0;
	
}

void CMDI_voInit(){

	CmdConfig.bfEcho=0;
	u8ProgToChange=0;
	voCleanBuffer();
	
	
}

bool CMDI_boAddCharToBuffer(char Element){
	bool boWriteDone = false;
	
	if(u8InputBufferWriteIdx<INPUTBUFFERSIZE){
		chInputBuffer[u8InputBufferWriteIdx]=Element;
		u8InputBufferWriteIdx++;
		boWriteDone = true;
	} else {
		chInputBuffer[(INPUTBUFFERSIZE-1)]=Element;
	}
	if(CmdConfig.bfEcho!=0){
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf_P(USBSTREAM,PSTR("%c"),Element);
				}
	}
	return boWriteDone; 
}


void CMDI_voTask(){
	/* This will check if we have a return in out buffer at the end */
	chInputBuffer[INPUTBUFFERSIZE]='\0';
	if(u8InputBufferWriteIdx>0){
		if(chInputBuffer[u8InputBufferWriteIdx-1]==LINE_END){
			chInputBuffer[u8InputBufferWriteIdx-1]='\0';
			/* we need to process the data */
			/* we need to remove any \r in front of the data : TBD */
			voProcessData();
		}
	}
	
	
}


bool StringToUint(char* elements, uint32_t* u32converted){
	
	bool boHasError=false;

	(*u32converted)=0;
	/* Seach for presign */
	if( (*elements == '+') || (*elements=='-') ) {
		if(*elements=='-'){
			/* We do unsigned here we report an error */
			boHasError=true;
		} else {
			/* Do nothing here */
		}
		elements++;
	}
	
	while( (*elements!='\0')&&(boHasError==false)){
			if( (*elements>='0') && (*elements<='9') ){
				if(*u32converted<(UINT32_MAX/10))
				{
					*u32converted=(*u32converted) * 10;	
					*u32converted=(*u32converted)+( (*elements)-'0');
					elements++;
				} else {
					boHasError=true;
				}
			} else {
				//parsererror
				boHasError=true;
			}
	}
	return boHasError;
}


char* finddelimiter(char* input, char delimiter, uint8_t* size){
	char* searchptr=input;
	uint8_t u8pos=0;
	bool boFound=false;
	while( (*searchptr!='\0' ) && ( u8pos<*size) && (boFound==false)){
		if(*searchptr==delimiter){
			boFound=true;
			*searchptr='\0';
		}
		searchptr++;
		u8pos++;
	}
	*size-=u8pos;
	
if(boFound==false){
	searchptr=NULL;
}	
return searchptr;	
}

void voProcessData(){
	
	CMDI_CMD_DATA_t COMMAND;
	
	COMMAND.CMD=CMDI_NONE;
	COMMAND.DIR=CMDI_GET;
	COMMAND.u32CmdValue=0;
	
	bool boStringEnd=false;
	uint8_t u8TokenNo=0;
	uint8_t u8BufferLenght=64;
	char* cptr = (char*)chInputBuffer;
	char* cpreptr = (char*)chInputBuffer;
	/* we need no to parse the string and may  find elements we may know */
	while(boStringEnd==false){
		cptr = finddelimiter(cptr, ' ', &u8BufferLenght);
		/* We need to check if we are at the string end or found something */
		if(cpreptr!=NULL){
			switch(u8TokenNo){
			
				case 0:{
					
					if(strcmp_P(cpreptr,PSTR("SET"))==0){
						/* This is a SET */
						COMMAND.DIR=CMDI_SET;
						
					} else if(strcmp_P(cpreptr,PSTR("GET"))==0){
						/* This is a GET */
						COMMAND.DIR=CMDI_GET;
					
					} else if(strcmp_P(cpreptr,PSTR("CMD"))==0){
						/* This is a GET */
						COMMAND.DIR=CMDI_CMD;
					} else if(strcmp_P(cpreptr,PSTR("HELP"))==0){
					/* This is a GET */
						COMMAND.DIR=CMDI_HELP;					
						COMMAND.CMD=CMDI_SYS_HELP;
					} else if (strcmp_P(cpreptr,PSTR("BEER"))==0){
						COMMAND.DIR=CMDI_BEER;	
						COMMAND.CMD=CMDI_SYS_HELP;
					} else {
						/* We don't know the command and need to prepare an error */
						COMMAND.DIR=CMDI_NONE;
						
					}
								
				} break;
				
				case 1:{
					if( (COMMAND.DIR==CMDI_HELP) ||(COMMAND.DIR==CMDI_SET) || ( COMMAND.DIR==CMDI_GET) ) {

						if(strcmp_P(cpreptr,PSTR("TEMP"))==0){
							/* TEMP */
							COMMAND.CMD=CMDI_TEMP;
						
						} else if(strcmp_P(cpreptr,PSTR("PRWA"))==0){
							/* PRWA */
							COMMAND.CMD=CMDI_PREWATER;
						
						} else if(strcmp_P(cpreptr,PSTR("WATER"))==0){
							/* WATER */
							COMMAND.CMD=CMDI_WATER;
						
						} else if(strcmp_P(cpreptr,PSTR("WAIT"))==0){
							/* WAIT */
							COMMAND.CMD=CMDI_WAIT;
						
						} else if(strcmp_P(cpreptr,PSTR("PRG"))==0){
							/* PRG */
							COMMAND.CMD=CMDI_PRG;
						} else if(strcmp_P(cpreptr,PSTR("CHPRG"))==0){
							/* SET Prog to Change */
							COMMAND.CMD=CMDI_CHPRG;
						} else if(strcmp_P(cpreptr,PSTR("CURPWR"))==0){
								/* Ausgabe Leistungsmessung */
								COMMAND.CMD=CMDI_MES_POWER;
						} else if(strcmp_P(cpreptr,PSTR("BOILERTMP"))==0){
								/* Ausgabe Boiler Temp */
								COMMAND.CMD=CMDI_BOILERTEMP;								
						} else if(strcmp_P(cpreptr,PSTR("LEVEL"))==0){
								/* CUP Level */
								COMMAND.CMD=CMDI_CUP_LEVEL;
						} else if(strcmp_P(cpreptr,PSTR("AUTOOFF"))==0){
							 COMMAND.CMD=CMDI_AUTOPOWER;
						}else if(strcmp_P(cpreptr,PSTR("STBYTEMP"))==0){
							COMMAND.CMD=CMDI_STBY_TEMP;	
						}else if(strcmp_P(cpreptr,PSTR("ECHO"))==0){
							COMMAND.CMD=CMDI_ECHO;	
						} else if (strcmp_P(cpreptr,PSTR("GPIO"))==0){
							COMMAND.CMD=CMDI_GPIO;	
						} else {
							/* We don't know the command and need to prepare an error */
							COMMAND.CMD=CMDI_NONE;
						
						}
					} 
					
					else if (COMMAND.DIR==CMDI_CMD){
						if(strcmp_P(cpreptr,PSTR("MAKE"))==0){
							/* TEMP */
							COMMAND.CMD=CMDI_MAKE;
						
						} else if(strcmp_P(cpreptr,PSTR("POWER"))==0){
							/* PRWA */
							COMMAND.CMD=CMDI_POWER;
						
						} else {
							COMMAND.CMD=CMDI_NONE;							
						}
					} else {
							COMMAND.CMD=CMDI_NONE;							
					}
				} break;
				
				case 2:{
					if( (COMMAND.DIR==CMDI_SET) || (COMMAND.DIR == CMDI_GET) ){
						if(COMMAND.DIR!=CMDI_GET){
							if(StringToUint(cpreptr,&COMMAND.u32CmdValue)==false){
								//All good here  
							} else {
								COMMAND.CMD=CMDI_NONE; //Parser Error
							}
						
						} else {
							if( (CMDI_GET==COMMAND.DIR) && ( CMDI_GPIO == COMMAND.CMD) ){
								if(StringToUint(cpreptr,&COMMAND.u32CmdValue)==false){
									//All good here
								} else {
									COMMAND.CMD=CMDI_NONE; //Parser Error
								}		
							} else {
								/* We don't know the command and need to prepare an error */
								COMMAND.CMD=CMDI_NONE;
							}
						
						}
					}
					
					if(COMMAND.DIR==CMDI_CMD){
						
						if(COMMAND.CMD==CMDI_MAKE){
							if(strcmp_P(cpreptr,PSTR("ONECUP"))==0){
								COMMAND.u32CmdValue=0;
						
							} else if(strcmp_P(cpreptr,PSTR("TWOCUP"))==0){
								COMMAND.u32CmdValue=1;
							
							} else {
								COMMAND.CMD=CMDI_NONE;							
							}
						} else if(COMMAND.CMD==CMDI_POWER){
							if(strcmp_P(cpreptr,PSTR("ON"))==0){
								COMMAND.u32CmdValue=1;
						
							} else if(strcmp_P(cpreptr,PSTR("OFF"))==0){
								COMMAND.u32CmdValue=0;
							
							} else {
								COMMAND.CMD=CMDI_NONE;							
							}
						}
					}
					
				}break;
				
				default:{
				/* Wir sollten hier nur ankommen wenn es einen Fehler gibt */
								
				} break;
				
			}
			/* cptr now contains our first token */
			u8TokenNo++;
		} else {
			boStringEnd = true;
		}
		cpreptr=cptr;
	}
	
	/* Now we need to process the input and what we found now */
	switch(COMMAND.CMD){
		
		case CMDI_SYS_HELP:{
			
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
		
		} break;
		
		case CMDI_TEMP:{
			/* Change Temp */
			if(COMMAND.DIR==CMDI_SET){
				boEEFS_UpdateUserProgramm(u8ProgToChange,USRPRG_HEATERTEMP,COMMAND.u32CmdValue);
				voResponseOk();
			} else if(COMMAND.DIR==CMDI_GET) {
				if(boGetConnected()==true){
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
			
			
		} break;
		
		case CMDI_PREWATER:{
			/* Change PreWater Time */	
			if(COMMAND.DIR==CMDI_SET){
				boEEFS_UpdateUserProgramm(u8ProgToChange,USRPRG_PREWATERLEVEL,COMMAND.u32CmdValue);
				voResponseOk();
			} else if(COMMAND.DIR==CMDI_GET) {
					if(boGetConnected()==true){
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
		} break;
		
		case CMDI_WATER:{
			/* Change Water time */
						if(COMMAND.DIR==CMDI_SET){
							boEEFS_UpdateUserProgramm(u8ProgToChange,USRPRG_WATERLEVELFULL,COMMAND.u32CmdValue);
							voResponseOk();
						} else if(COMMAND.DIR==CMDI_GET) {
							if(boGetConnected()==true){
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
		
		} break;
		
		case CMDI_WAIT:{
			/* Change Wait time */
			if(COMMAND.DIR==CMDI_SET){
				boEEFS_UpdateUserProgramm(u8ProgToChange,USRPRG_PREWATERWAIT,COMMAND.u32CmdValue);
				voResponseOk();
				} else if(COMMAND.DIR==CMDI_GET) {
				if(boGetConnected()==true){
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
			
			
		} break;
		
		case CMDI_PRG:{
			/* Change PRG to Load */
			if(COMMAND.DIR==CMDI_SET){
				
				if(boCoffeeFSM_LoadProgramm(COMMAND.u32CmdValue)==true){
					voResponseOk();
				} else {
					voResponseParamErr();
				}
			} else if (COMMAND.DIR==CMDI_GET){
				/* We shall return the curren Programm */
					if(boGetConnected()==true){
						FILE* USBSTREAM = GetUSBStream();
						fprintf_P(USBSTREAM,PSTR("OK:%i\n\r"),u8GetCurrenProgrammIdx());
					}
				
			} else if(COMMAND.DIR == CMDI_HELP){
			/* Print Help Text here */
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf_P(USBSTREAM,PSTR("USAGE: 'GET PRG' or 'SET PRG XXXX' with range of 0 to 7\n\r"));
				}
			} else {
				voResponseParserErr();
			}
					
		}break;
		
		case CMDI_CHPRG:{
			/* Set the PRG to Change */
			if(COMMAND.DIR==CMDI_SET){
				if(COMMAND.u32CmdValue<8){
						u8ProgToChange=COMMAND.u32CmdValue;
						voResponseOk();
				} else {
					voResponseParamErr();
				}
			} else if(COMMAND.DIR==CMDI_GET) {
					if(boGetConnected()==true){
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
			
			
		}break;
		
		case CMDI_MES_POWER:{
			if(COMMAND.DIR==CMDI_GET){
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf_P(USBSTREAM,PSTR("OK:%i\n\r"), u32PWRMTR_GetmAmp());
				}
			
			} else if(COMMAND.DIR==CMDI_SET) {
					voResponseParserErr();
			} else if(COMMAND.DIR == CMDI_HELP){
			/* Print Help Text here */
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf_P(USBSTREAM,PSTR("USAGE: 'GET CURPWR' to read the current power used in mA\n\r"));
				}
			} else {
				voResponseParserErr();
			}
			
		} break;
		
		case CMDI_BOILERTEMP:{
			if(COMMAND.DIR==CMDI_GET){
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf_P(USBSTREAM,PSTR("OK:%i\n\r"), s16BTC_GetCurrentBoilerTemp());
				}
			
			} else if(COMMAND.DIR==CMDI_SET) {
				voResponseParserErr();	
			} else if(COMMAND.DIR == CMDI_HELP){
			/* Print Help Text here */
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf_P(USBSTREAM,PSTR("USAGE: 'GET BOILERTMP' to read the current boiler temp in 1/100 degree C.\n\r"));
				}
			} else {
				voResponseParserErr();
			}
			
			
		} break;
		
		case CMDI_CUP_LEVEL:{
			if(COMMAND.DIR==CMDI_GET){
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf(USBSTREAM,"OK:%i\n\r", u8GetCupLevel());
				}
			
			} else if(COMMAND.DIR==CMDI_SET) {
					voSetCupLevel((uint8_t)COMMAND.u32CmdValue);
					voResponseOk();
					
			} else if(COMMAND.DIR == CMDI_HELP){
			/* Print Help Text here */
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf_P(USBSTREAM,PSTR("USAGE: 'GET LEVLE' or 'SET LEVEL XXXX' with range of 0 to 2\n\r"));
				}
			} else {
				voResponseParserErr();
			}
			
			
			
			
		} break;
		
		case CMDI_AUTOPOWER:{
			if(COMMAND.DIR==CMDI_GET){
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					

					fprintf(USBSTREAM,"OK:%i\n\r", u8GetAutoPowerOff());
				}
			
			} else if(COMMAND.DIR==CMDI_SET) {
					voSetAutoPowerOff((uint8_t)COMMAND.u32CmdValue);
					voResponseOk();
					
			} else if(COMMAND.DIR == CMDI_HELP){
				/* Print Help Text here */
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf_P(USBSTREAM,PSTR("USAGE: 'SET AUTOOFF ON', 'SET AUTOOFF OFF' or 'SET AUTOOFF OFF' \n\r"));
				}
			} else {
				voResponseParserErr();
			}
			
			
		} break;
		
		case CMDI_STBY_TEMP:{
			if(COMMAND.DIR==CMDI_GET){
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf(USBSTREAM,"OK:%i\n\r", u16GetStandbyTemp());
				}
				
			else if(COMMAND.DIR==CMDI_SET) {
				voSetStandbyTemp((uint16_t)COMMAND.u32CmdValue);
				voResponseOk();
			} else if(COMMAND.DIR == CMDI_HELP){
			/* Print Help Text here */
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf_P(USBSTREAM,PSTR("USAGE: 'GET STBYTMP', 'SET STBYTMP XXXX\n\r"));
				}
			} else {
				voResponseParserErr();
			}	
			
		} break;
		
		case CMDI_ECHO:{
			if(COMMAND.DIR==CMDI_GET){
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf(USBSTREAM,"OK:%i\n\r", CmdConfig.bfEcho);
				}
				
			} else if(COMMAND.DIR==CMDI_SET) {
				if(COMMAND.u32CmdValue>0){
					CmdConfig.bfEcho=1;
				} else {
					CmdConfig.bfEcho=0;
				}
				voResponseOk();
			} else if(COMMAND.DIR == CMDI_HELP){
			/* Print Help Text here */
				if(boGetConnected()==true){
					FILE* USBSTREAM = GetUSBStream();
					fprintf_P(USBSTREAM,PSTR("USAGE: 'GET ECHO', 'SET ECHO XXXX' with 0 = OFF and else ON\n\r"));
				}
			} else {
				voResponseParserErr();
			}	
			
		} break;
		
		case CMDI_NONE:{
			voResponseParserErr();
		} break;
		
		case CMDI_POWER:{
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
			
			}
			} break;
			
			case CMDI_MAKE:{
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
					if(boCoffeeFSM_MakeCoffee(1)==true){
						if(boGetConnected()==true){
							FILE* USBSTREAM = GetUSBStream();
							fprintf(USBSTREAM,"OK:BRETWO\n\r");
						}
					} else {
						if(boGetConnected()==true){
							FILE* USBSTREAM = GetUSBStream();
							fprintf(USBSTREAM,"ERR:BREWERR\n\r");
						}
					}
				} break;
				
				case CMDI_GPIO:{
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
										fprintf(USBSTREAM,"SWITCH %i RELEASED\n\r",COMMAND.u32CmdValue);
									} else {
										fprintf(USBSTREAM,"SWITCH %i PRESSED\n\r",COMMAND.u32CmdValue);
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
							}
						}
					
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
				}
				
				default:{
					voResponseParamErr();
				} break;
			   }//End switch
			  } else if(COMMAND.DIR==CMDI_HELP){
					if(boGetConnected()==true){
						FILE* USBSTREAM = GetUSBStream();
						fprintf(USBSTREAM,"USAGE: 'MAKE ONECUP', 'MAKE TWOCUP' for one or two cups\n\r");
					}
				  
			  }
			
	} break;
		
		default:{
			
		} break;
	}	
	
	
voCleanBuffer();
	
}

void voResponseParamErr(){
	if(boGetConnected()==true){
		FILE* USBSTREAM = GetUSBStream();
		fprintf_P(USBSTREAM,PSTR("ERR:PARAM\n\r"));
	}	
}

void voResponseParserErr(){
	if(boGetConnected()==true){
		FILE* USBSTREAM = GetUSBStream();
		fprintf_P(USBSTREAM,PSTR("ERR:PARSER\n\r"));
	}	
}

void voResponseOk( ){
	if(boGetConnected()==true){
		FILE* USBSTREAM = GetUSBStream();
		fprintf_P(USBSTREAM,PSTR("OK\n\r"));
	}
}