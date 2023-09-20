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
#include "CommandInterpreterTypes.h"
#include "CommandInterpreterInternalVars.h"

#include "Commands/CMDI_CHPRG.h"
#include "Commands/CMDI_PREWATER.h"
#include "Commands/CMDI_PRG.h"
#include "Commands/CMDI_SYS_HELP.h"
#include "Commands/CMDI_TEMP.h"
#include "Commands/CMDI_WAIT.h"
#include "Commands/CMDI_WATER.h"
#include "Commands/CMDI_MES_POWER.h"
#include "Commands/CMDI_CUP_LEVEL.h"
#include "Commands/CMDI_AUTOPOWER.h"
#include "Commands/CMDI_STBY_TEMP.h"
#include "Commands/CMDI_POWER.h"
#include "Commands/CMDI_ECHO.h"
#include "Commands/CMDI_GPIO.h"
#include "Commands/CMDI_BOILERTEMP.h"
#include "Commands/CMDI_MAKE.h"

#include "Commands/COMMON_VALUE_STORE.h"


#define INPUTBUFFERSIZE ( 64 )
#define LINE_END '\r'

volatile char chInputBuffer[INPUTBUFFERSIZE+1];
volatile uint8_t u8InputBufferWriteIdx;


void voProcessData( void );
void voResponseParamErr( void );
void voResponseOk( void );
void voResponseParserErr( void );

/* This functions are currently integrated into main 
need to be moved */
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
	voSetProgToChange(0);
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
				
				/*	
					Checks for first token and can be SET GET CMD HELP or BEER
					This sets the direction the command is intended to operate
				 */
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
				
				/* Second Token actually decodes what to do  */
				case 1:{
					/* This is for HELP SET and GET */
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
					
					} else if (COMMAND.DIR==CMDI_CMD){ /* Specal commands that need to be called with CMD in front */
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
				/* This is the third Token that some commands do expect */
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
					} //End of IF SET ot GET
					
					if(COMMAND.DIR==CMDI_CMD){ //If we have a CMD we may need to parse the third token
						
						if(COMMAND.CMD==CMDI_MAKE){ //If we need to make coffe 
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
					/*This will present an internal error */
					
				} break;
				
			}
			//We now increase the number of tokens we found
			u8TokenNo++;
		} else {
			boStringEnd = true;
		}
		/* cpreptr hods the current start position in out array */
		/* we move it to the next token */
		cpreptr=cptr;
	}
	
	/* Now we need to process the input and what we found now */
	switch(COMMAND.CMD){
		
		case CMDI_SYS_HELP:{
			voCMDI_SYS_HELP(COMMAND);
		} break;
		
		case CMDI_TEMP:{
			voCMDI_TEMP(COMMAND);
		} break;
		
		case CMDI_PREWATER:{
			voCMDI_PREWATER(COMMAND);
		} break;
		
		case CMDI_WATER:{
			voCMDI_WATRER(COMMAND);
		} break;
		
		case CMDI_WAIT:{
			voCMDI_WAIT(COMMAND);
		} break;
		
		case CMDI_PRG:{
			voCMDI_PRG(COMMAND);
		}break;
		
		case CMDI_CHPRG:{
			voCMDI_CHPRG(COMMAND);
		}break;
		
		case CMDI_MES_POWER:{
			voCMDI_MES_POWER(COMMAND);
		} break;
		
		case CMDI_BOILERTEMP:{
			voCMDI_BOILERTEMP(COMMAND);
		} break;
		
		case CMDI_CUP_LEVEL:{
			voCMDI_CUP_LEVEL(COMMAND);
		} break;
		
		case CMDI_AUTOPOWER:{
			voCMDI_AUTOPOWER(COMMAND);
		} break;
		
		case CMDI_STBY_TEMP:{
			voCMDI_STBY_TEMP(COMMAND);
				
		} break;
			
		case CMDI_ECHO:{
			voCMDI_ECHO(COMMAND);
		} break;
			
		case CMDI_NONE:{
			voResponseParserErr();
		} break;
			
		case CMDI_POWER:{
			voCMDI_POWER(COMMAND);
		} break;
		
		case CMDI_GPIO:{
			voCMDI_GPIO(COMMAND);
		} break;//End of case
		
		

		case CMDI_MAKE:{
			voCMDI_MAKE(COMMAND);
		} break;
		
		default:{
			/* This will be an internal error */
		} break;
	}
	
	
	voCleanBuffer();
	
}

