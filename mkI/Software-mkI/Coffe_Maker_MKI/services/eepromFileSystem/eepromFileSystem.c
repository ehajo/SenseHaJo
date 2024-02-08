/*
 * eepromFileSystem.c
 *
 * Created: 14.11.2014 13:45:55
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/crc16.h>

#include "eepromFileSystem.h"

//#ifdef USE_WLF_CC

//EEPROM Memory Map
/* This is a small sort of eeprom file system */
/* We only got 1024 byte of EEPROM for our settings */


/* Defines for the eeprom status */

#define INVALID_BLOCK		( 0xFF )
#define CURRENT		 		( 0x81 )
#define PREVIOUS			( 0x99 )	


typedef struct {
uint8_t  entrystatus;
uint16_t value;
uint8_t  blockcnt;
} coffecounter_t;

typedef struct {
	uint8_t MagicKey[5];
	uint8_t InvertedMagicKey[5];
} flashmagic_t;


/* EEPROM will be :

000..255  [8 Times UserSettings  ]
256..511  [CoffeCounterWL System ]
512..513  [CoffeeCounter     	 ]
513..899  [Reserved				 ]
899..1013[Systemsettings        ]
1014..1023[Flash Init Magic Value]

*/
#define USRSETADDR ( 0 )
#define USRSETSIZE ( 256 )
#define CCWLADDR ( USRSETADDR + USRSETSIZE )
#define CCWLSIZE ( 256 )
#define RESERVDADDR ( CCWLADDR + CCWLSIZE )
#define RESERVSIZE ( 386 )
#define SYSSETADD ( RESERVDADDR + RESERVSIZE )

#define MAGICKEY_STARTADD ( 1014 )

uint8_t const MagicFlashKey[5] PROGMEM = {'e','H','a','J','o'};

void voWriteMagicKey( void );
void voInitCoffeeCounter( void );


void voSetDefaultSystemSettingsValues(systemsettings_t* DefaultSystemsettings){
	DefaultSystemsettings->u8ConfigBits.bfBoilerFilled=0; /* Boiler not filled */
	DefaultSystemsettings->u8ConfigBits.bfCoinMode=0; /* CoinMode off */
	DefaultSystemsettings->u8ConfigBits.bfDualHalSensor=1; /* We have two hall sensors here */
	DefaultSystemsettings->u8ConfigBits.bfLcdAttached=1; /* We have a LCD attached */
	DefaultSystemsettings->u8ConfigBits.bfReserved0=0;
	DefaultSystemsettings->u8ConfigBits.bfReserved1=0;
	DefaultSystemsettings->u8ConfigBits.bfReserved2=0;
	DefaultSystemsettings->u8ConfigBits.bfReserved3=0;
	DefaultSystemsettings->u8NetFrequency=50; /*50Hz*/
	DefaultSystemsettings->u8NetVoltage=230; /* 325V Peak  230V eff*/
	DefaultSystemsettings->u16StandbyTemp=7500;
	DefaultSystemsettings->u8coffeelevel=2;
	DefaultSystemsettings->u8SelectedProg=0;
}

void voSetDefaultUserSettingsValues( usersettings_t* DefaultUserSettings){
	DefaultUserSettings->u16PreWaterLevel=1000;	/* 1,5S PreFill */
	DefaultUserSettings->u16PreWaterWait=3500;	/* 3,5S Wait */
	DefaultUserSettings->u16WaterLevelFull=7500; /* 7.5S Water */
	DefaultUserSettings->u32HeaterTemp=9000; /* 90 Degree C */
	DefaultUserSettings->u16Reserved_1=0xAAAA;
	DefaultUserSettings->u16Reserved_2=0x5555;
	DefaultUserSettings->u16Reserved_3=0xAAAA;
	DefaultUserSettings->u16CRC16=0;
	
}

void voEEFS_Init()
{
	uint8_t u8MagigKeyError=0;
	/* First Read the magic key and see if there is any */
	flashmagic_t MagicKey;
	uint8_t* MagicKeyPtr = (uint8_t*)&MagicKey;
	for(uint8_t i=0;i<sizeof(flashmagic_t);i++){
		*MagicKeyPtr=eeprom_read_byte((uint8_t *)(  MAGICKEY_STARTADD + i ) );
		MagicKeyPtr++;
	}
	/* Okay the magickey is in Memory */
	/* The value shall be "eHaJo" */ 
	for(uint8_t i=0;i<(sizeof(flashmagic_t)/2);i++){
		volatile uint8_t u8Value = MagicKey.MagicKey[i];
		volatile uint8_t u8InvertedValue = ~(MagicKey.InvertedMagicKey[i]);
		volatile uint8_t u8FlashValue = pgm_read_byte(&(MagicFlashKey[i]));
		
		if( (u8Value == u8InvertedValue) && ( u8FlashValue == u8Value) ){
			
		} else {
			u8MagigKeyError++;
		}
		
	
	}
	
	if(0==u8MagigKeyError){
		/* Okay Magic is correct , seems flash has been written */
		return;
	}
	
	
	systemsettings_t DefaultSystemsettings;
	voSetDefaultSystemSettingsValues(&DefaultSystemsettings);
	
	/* We need to write the Defaultsettings */
	voEEFS_WriteSystemSettings(&DefaultSystemsettings);
	
	usersettings_t DefaultUserSettings;
	voSetDefaultUserSettingsValues(&DefaultUserSettings);
	
	
	/* The Defaultconfig is now in place we need now to store the user programms */
	for (uint8_t i =0 ;i<8; i++){
	
		/*We need to write one Programm to the EEPROM */
		voEEFS_WriteUserProgramm(i, &DefaultUserSettings);
	}
	
	voInitCoffeeCounter();
	
	voWriteMagicKey();
}

void voWriteMagicKey( ) {
	
	
	flashmagic_t FlashMagic;
	FlashMagic.MagicKey[0]='e';
	FlashMagic.MagicKey[1]='H';
	FlashMagic.MagicKey[2]='a';
	FlashMagic.MagicKey[3]='J';
	FlashMagic.MagicKey[4]='o';
	FlashMagic.InvertedMagicKey[0]=~FlashMagic.MagicKey[0];
	FlashMagic.InvertedMagicKey[1]=~FlashMagic.MagicKey[1];
	FlashMagic.InvertedMagicKey[2]=~FlashMagic.MagicKey[2];
	FlashMagic.InvertedMagicKey[3]=~FlashMagic.MagicKey[3];
	FlashMagic.InvertedMagicKey[4]=~FlashMagic.MagicKey[4]; 
	uint8_t* ptr=(uint8_t*)&FlashMagic;
	
	for(uint8_t i=0;i<sizeof(flashmagic_t);i++){
		eeprom_update_byte((uint8_t *)(MAGICKEY_STARTADD + i),*ptr);
		
		ptr++;	
	}
	
	
	
}

void voEEFS_WriteSystemSettings(systemsettings_t* Systemsettings){
	//Okay now we need to Build our CRC
	uint8_t* ptr=(uint8_t *)(Systemsettings);
	uint16_t crc_calc=0xFFFF;
	uint16_t lenght=sizeof(systemsettings_t);
	if(lenght>2){
		lenght=lenght-2;
	}
	for(uint16_t i=0;i<(lenght);i++)
	{
		crc_calc = _crc_ccitt_update(crc_calc,*ptr);
		ptr++;
	}
	Systemsettings->u16CRC=crc_calc;
	ptr=(uint8_t *)(Systemsettings);
	//And now everything must back to the EEPROM
	for(uint16_t i=0;i<(lenght+2);i++)
	{
		eeprom_update_byte((uint8_t *)i+(SYSSETADD) , *ptr);
		ptr++;
	}
	
}

bool voEEFS_ReadSystemSettings( systemsettings_t* Settings){
	
	bool boReturnval = false;
	
		uint8_t* ptr=(uint8_t *)(Settings);
		uint16_t lenght=sizeof(systemsettings_t);
		uint16_t crc_calc=0xFFFF;
		
		if(lenght>2){
			lenght=lenght-2;
		}
		for(uint16_t i=0;i<(lenght);i++)
		{
			*ptr=eeprom_read_byte((uint8_t *)i+(SYSSETADD));
			crc_calc = _crc_ccitt_update(crc_calc,*ptr);
			ptr++;
		}
		
		for(uint16_t i=lenght;i<lenght+2;i++){
			*ptr=eeprom_read_byte((uint8_t *)i+(SYSSETADD));
			ptr++;
		}
		
		
		//Lets see if the EEPROM has valid data
		if(crc_calc!=Settings->u16CRC)
		{
			/* CRC is broken we need to return a false */
			boReturnval=false;
			} else {
			/* CRC match , return okay */
			boReturnval=true;
		}

	return boReturnval;
	
}


void voEEFS_WriteUserProgramm(uint8_t u8Idx, usersettings_t* Settings){
	
	uint16_t u16offset = sizeof(usersettings_t) * u8Idx;
	uint8_t* ptr=(uint8_t *)(Settings);
	uint16_t crc_calc=0xFFFF;
	uint16_t lenght=sizeof(usersettings_t);
	if(lenght>2){
		lenght=lenght-2;
	}
    		
	if(u8Idx < 8) {
		
		for(uint16_t i=0;i<(lenght);i++)
		{
			crc_calc = _crc_ccitt_update(crc_calc,*ptr);
			ptr++;
		}
		
		Settings->u16CRC16=crc_calc;
		ptr=(uint8_t *)(Settings);
		for(uint16_t i=0;i<(lenght+2);i++)
		{
			eeprom_update_byte((uint8_t *)i+(USRSETADDR)+u16offset , *ptr);
			ptr++;
		}
	} else {
		/* Error happend */
	}
	
}

bool boEEFS_ReadUserProgramm(uint8_t u8Idx, usersettings_t* Settings)
{
	bool boReturnval=false;
	
	volatile uint16_t u16offset = sizeof(usersettings_t) * u8Idx;
	volatile uint16_t crc_calc=0xFFFF;
	volatile uint16_t u16eepindex=0;
		
	volatile uint8_t* ptr=(uint8_t *)(Settings);
	volatile uint16_t lenght=sizeof(usersettings_t);
	
	if(u8Idx < 8) {
		if(lenght>2){
			lenght=lenght-2; /* last two bytes are crc */
		}
			
		
		for(uint16_t i=0;i<(lenght);i++)
		{
			u16eepindex = i+(USRSETADDR)+u16offset;
			*ptr=eeprom_read_byte((uint8_t *)(u16eepindex) );
			ptr++;
		}
		
		
		ptr=(uint8_t *)(Settings);;
		for(uint16_t i=0;i<(lenght);i++){	
			crc_calc = _crc_ccitt_update(crc_calc,*ptr);
			ptr++;
		}
		
		
		/* We need to load the CRC from the eeprom but dont use it for the crc here */
		for(uint16_t i=(sizeof(usersettings_t)-2);i<sizeof(usersettings_t);i++)
		{
			
			u16eepindex = i+(USRSETADDR)+u16offset;
			*ptr=eeprom_read_byte((uint8_t *)(u16eepindex) );
 			ptr++;
		}
		
		
		//Lets see if the EEPROM has valid data
		if(crc_calc!=Settings->u16CRC16)
		{
			/* CRC is broken we need to return a false */
			boReturnval=false;
		} else {
			/* CRC match , return okay */
			boReturnval=true;
		}
		
	} else {
		/* Index out of range we need to return a false */
		boReturnval=false;
	}
	
	return boReturnval;
}

void boEEFS_UpdateUserProgramm(uint8_t idx, USERPROGPARAM_t param, uint32_t Value ){
	usersettings_t TemUserSettings;
	boEEFS_ReadUserProgramm(idx,&TemUserSettings);
	switch(param){
		case USRPRG_HEATERTEMP:{
			if(Value<UINT16_MAX){
				TemUserSettings.u32HeaterTemp=(uint16_t)Value;				
				voEEFS_WriteUserProgramm(idx,&TemUserSettings);
			}
		} break;
		
		case USRPRG_PREWATERLEVEL:{
			if(Value<UINT16_MAX){
				TemUserSettings.u16PreWaterLevel=(uint16_t)Value;				
				voEEFS_WriteUserProgramm(idx,&TemUserSettings);
			}
		} break;
		
		case USRPRG_PREWATERWAIT:{
			if(Value<UINT16_MAX){
				TemUserSettings.u16PreWaterWait=(uint16_t)Value;				
				voEEFS_WriteUserProgramm(idx,&TemUserSettings);
			}
		} break;
		
		case USRPRG_WATERLEVELFULL:{
			if(Value<UINT16_MAX){
				TemUserSettings.u16WaterLevelFull=(uint16_t)Value;				
				voEEFS_WriteUserProgramm(idx,&TemUserSettings);
			}
		} break;
		
		default:{
			
		} break;
	
	}
	
	
}

uint32_t boEEFS_ReadUserProgrammParam(uint8_t idx, USERPROGPARAM_t param ){
	usersettings_t TemUserSettings;
	uint32_t u32ReturnVal=0;
	boEEFS_ReadUserProgramm(idx,&TemUserSettings);
	switch(param){
		case USRPRG_HEATERTEMP:{
			u32ReturnVal=TemUserSettings.u32HeaterTemp;
		} break;
		
		case USRPRG_PREWATERLEVEL:{
			u32ReturnVal = TemUserSettings.u16PreWaterLevel;
		} break;
		
		case USRPRG_PREWATERWAIT:{
			u32ReturnVal = TemUserSettings.u16PreWaterWait;
		} break;
		
		case USRPRG_WATERLEVELFULL:{
			u32ReturnVal = TemUserSettings.u16WaterLevelFull;
		} break;
	}
	
	return u32ReturnVal;
}










#ifdef USE_WLF_CC

/* We do here the wearleveling for the CoffeeCounter */
/*
	We need to implement Increment , SetZero, GetCount 
*/
void voWriteCoffeeCounter( uint8_t CellIdx, coffecounter_t* CounterItem );


void voInitCoffeeCounter( void ){
	
	coffecounter_t DefaultCounter;
	DefaultCounter.blockcnt=0;
	DefaultCounter.entrystatus=INVALID_BLOCK;
	DefaultCounter.value=0;
	
	for(uint8_t i=0;i<64;i++){
		voWriteCoffeeCounter(i,&DefaultCounter);
	}
	/* Counter shall be empty so we need to write the First one to Zero now */
	DefaultCounter.entrystatus=PREVIOUS;
	DefaultCounter.value=UINT16_MAX;
	voWriteCoffeeCounter(0,&DefaultCounter);
	
	DefaultCounter.entrystatus=CURRENT;
	DefaultCounter.value=0;
	voWriteCoffeeCounter(1, &DefaultCounter);
	
	/* The EEPROM shall be ready now */
	
}




void voWriteCoffeeCounter( uint8_t CellIdx, coffecounter_t* CounterItem ){
	
	//Okay now we need to Build our CRC
	uint8_t* ptr=(uint8_t *)(CounterItem);
	uint16_t lenght=sizeof(coffecounter_t);
	
	if(CellIdx < 64 ){
		for(uint16_t i=0;i<lenght;i++)
		{
			*ptr=eeprom_read_byte((uint8_t *)i+(CCWLADDR)+lenght*CellIdx);
			ptr++;
		}
	}
}

void voReadCoffeeCounter( uint8_t CellIdx, coffecounter_t* CounterItem){
	
	uint8_t* ptr=(uint8_t *)(CounterItem);
	uint16_t lenght=sizeof(coffecounter_t);
	
	if(CellIdx<64){
		for(uint16_t i=0;i<lenght;i++)
		{
			eeprom_update_byte((uint8_t *)i+(CCWLADDR)+lenght*CellIdx , *ptr);
			ptr++;
		}
	}
}


void voRestoreCounterFromRevious(uint8_t CellIdxPrev){
	coffecounter_t CounterValue;
	
	/* We take the prevoius idx +1 and try to do a rebuild */
	/* The value will be this time +2 and the dest will be idx+2 */
	voReadCoffeeCounter(CellIdxPrev, &CounterValue);
				
	CellIdxPrev=( ( CellIdxPrev + 1 ) & ( 0x3F ) );
	CounterValue.value++;
	CounterValue.blockcnt++;
	CounterValue.entrystatus=CURRENT;
	voWriteCoffeeCounter(CellIdxPrev,&CounterValue);
	/* Restore shall be done now */
				
}

uint8_t u8RecoverCounterFromBlockCnt(){
	
	uint8_t CellIdxMax=0;
	uint8_t CellIdxBlockCnt=0;
	coffecounter_t CounterValue;
	
	for(uint8_t i=0;i<64 ;i++){
		/* we need to find the entry with previous and current */
		
		voReadCoffeeCounter(i,&CounterValue);
		if( CounterValue.blockcnt >= CellIdxBlockCnt ){
			CellIdxMax=i;
		}
	}
	
	/* We try to do a restore from the Index */
	CellIdxMax=( ( CellIdxMax + 1 ) & ( 0x3F ) );
	CounterValue.value++;
	CounterValue.blockcnt++;
	CounterValue.entrystatus=PREVIOUS;
	voWriteCoffeeCounter(CellIdxMax,&CounterValue);

	CellIdxMax=( ( CellIdxMax + 1 ) & ( 0x3F ) );
	CounterValue.value++;
	CounterValue.blockcnt++;
	CounterValue.entrystatus=CURRENT;
	voWriteCoffeeCounter(CellIdxMax,&CounterValue);
	
	/* Restore shall be done now */
	return CellIdxMax;
}



void voFindCoffeeCounterEntrys(uint8_t* CurrentIdx, uint8_t*PreviusIdx){
	
		/* We need to find the Current entry in the EEPROM */
		uint8_t u8idx_current=255;
		uint8_t u8idx_previous=255;
		coffecounter_t CounterValue;
		
		for(uint8_t i=0;i<64; i++){
			/* we need to find the entry with previous and current */
			
			voReadCoffeeCounter(i,&CounterValue);
			/* We need to Check what we found */
			if( CounterValue.entrystatus == PREVIOUS ){
				u8idx_previous=i;
			}
			
			if( CounterValue.entrystatus == CURRENT ){
				u8idx_current=i;
			}
			
		}
		
		/* Next is to Check if we found valid Data in the EEPROM */
		if(  u8idx_current != ( (u8idx_previous + 1 ) & ( 0x3F ) ) ) {
			/* Okay this is fitting we take the Data from the current one */
			
			if( u8idx_previous == 255 ) {
				/* Okay previous value is also gone */
				 u8idx_current= u8RecoverCounterFromBlockCnt();
				 u8idx_previous = ( ( u8idx_current -1 ) & ( 0x3F ) );
				
				} else {
				 voRestoreCounterFromRevious(u8idx_previous);
				 u8idx_current=( ( u8idx_previous + 1 ) & ( 0x3F ) );
			    }
		}
		/* Okay now everything is in place we can do the increment now */
		*CurrentIdx =  u8idx_current;
		*PreviusIdx = u8idx_previous;
		
}



uint16_t EEFS_u16GetCoffeeCounter(){
		uint8_t u8idx_current=1;
		uint8_t u8idx_previous=0;
		coffecounter_t CounterValue;
		voFindCoffeeCounterEntrys(&u8idx_current,&u8idx_previous);
		voReadCoffeeCounter(u8idx_current,&CounterValue);
		return CounterValue.value;

}


void EEFS_voIncrementCoffeeCounter( void ){
	
	uint8_t u8idx_current=1;
	uint8_t u8idx_previous=0;
	coffecounter_t CounterValue;
	voFindCoffeeCounterEntrys(&u8idx_current,&u8idx_previous);
	
	/* We Read the Current Value */
	voReadCoffeeCounter(u8idx_current,&CounterValue);
	CounterValue.entrystatus=PREVIOUS;
	/* And set it to previous */
	voWriteCoffeeCounter(u8idx_current,&CounterValue);
	/* We update the values for the new vlaues */
	CounterValue.blockcnt++;
	CounterValue.value++;
	CounterValue.entrystatus=CURRENT;
	u8idx_current=( ( u8idx_current + 1 ) & ( 0x3F ) );
	/* Next is to write it to the eeprom */
	voWriteCoffeeCounter( u8idx_current, &CounterValue);
	/* Next is to invalidate the previous settiung */
	voReadCoffeeCounter(u8idx_previous,&CounterValue);
	CounterValue.entrystatus=INVALID_BLOCK;
	voWriteCoffeeCounter(u8idx_previous,&CounterValue);
	/* We should be done by now */
}

#else 

void voInitCoffeeCounter(){
	eeprom_update_word((uint16_t*)(512),0x0000);
}

void EEFS_voIncrementCoffeeCounter( void ){
	
	uint16_t u16current_value = eeprom_read_word((uint16_t*)(512));	
	u16current_value++;
	eeprom_update_word((uint16_t*)(512),u16current_value);

}

uint16_t EEFS_u16GetCoffeeCounter( void ){
	
	uint16_t u16current_value = eeprom_read_word((uint16_t*)(512));	
	return u16current_value;
	
}

#endif
