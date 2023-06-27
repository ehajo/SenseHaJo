/*
             LUFA Library
     Copyright (C) Dean Camera, 2015.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2015  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the VirtualSerial demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "CoffeeMaker.h"

#include "./driver/MCP3202/MCP3202.h"
#include "./driver/gpio/gpio.h"
#include "./driver/USART_SPI_Master/HW_USART_SPI.h"
#include "./driver/Timer/Timer.h"
#include "./driver/SWI2C_special/i2cmaster.h"


#include "../../services/boilertemp/boilertemp.h"
#include "../../services/powermeter/powermeter.h"

#include "../../services/eepromFileSystem/eepromFileSystem.h"
#include "../../services/buttondriver/buttondriver.h"
#include "../../services/pumpservice/pumpservice.h"
#include "../../services/heaterservice/heaterservice.h"
#include "../../services/coffee_fsm/coffee_fsm.h"
#include "../../services/boilertempcontroller/boilertempcontroller.h"
#include "../../services/TimerService/TimerService.h"
#include "../../services/displayservice/displayservice.h"
#include "../../services/heaterpowercontroller/heaterpowercontroller.h"
#include "../../services/lightningservice/ledlightningservice.h"
#include "../../services/healthmonitor/healthmonitor.h"
#include "../../services/CommandInterpreter/CommandInterpreter.h"
#include "../../services/waterlevelmessurment/waterlevelmessurment.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs.
 */

typedef void (*VoidFnct) (void);


typedef struct {
	uint16_t u16_lastrun;
	uint16_t u16_intervall;
	uint8_t RunRequiered;
	VoidFnct fncptrCallBack;
} TIMEROBJ_t;


void voScheduler( void );
void voUSB_Task( void );
void voMS100_Tasks ( void );
void voMS10_Tasks ( void );
void voMS50_Tasks ( void );
void voMS1_Task ( void );
void voMS1000_Tasks ( void );

FILE USBSerialStream;
volatile uint8_t connected=0;


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();
	voEEFS_Init();
	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
	GlobalInterruptEnable();
	voInit_HwTimer();
	
	/* Service init */
	voBLTMP_InitBoilerTemp();
	voPWRMTR_InitPowermeter();
	
	/* Setup the Tasks */
	voHPC_TaskInit();
	voHMon_Init();
	voKeyEvent_TaskInit();
	voPSE_TaskInit();
	voCoffeeFSM_TaskInit();
	voBTC_TaskInit();
	voTMS_InitTask();
	
	LLS_voInitTask();
	DPS_Init();
	i2c_init();
	CMDI_voInit();
	
	

	
	boCoffeeFSM_LoadProgramm(0);
	voCoffeeFSM_LoadSystemSettings();
	while( 1==1 ) 
	{
		
		voScheduler();
		voUSB_Task();
		CMDI_voTask();
	}
}



void voUSB_Task(  )
{
	/* Must throw away unused bytes from the host, or it will lock up while waiting for the device */
	int16_t Data = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
	if(Data >= 0)
	{
		/* We need to colect the Data for processing here */
		CMDI_boAddCharToBuffer(Data);
		/* First we put the Data to the FiFo and execute the Data if we find an Enter */
	}
	CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
	USB_USBTask();
}

/* This is currently prone to jitter and drift*/
void voProcessTimerOBJ(volatile TIMEROBJ_t* ptrTimerOBJ ){

uint16_t u16_current_time=u16GetSystemTicksMS();
/* Here we execute the Coffemaker parts if requiered */
uint16_t u16Delta = 0;
/* We need to move the Time axis here */


uint16_t u16intervall = ptrTimerOBJ->u16_intervall;
/* Everything here is run at XXXms Slots */
	if(ptrTimerOBJ->u16_intervall>0){
		/* Everything here is run at XXXms Slots */
		if(ptrTimerOBJ->u16_lastrun > u16_current_time ){
			/* our timedelta is now  ( UINT16_MAX - u16_lastrun ) + u16_current_time */
			u16Delta = ( ( UINT16_MAX - ptrTimerOBJ->u16_lastrun) +u16_current_time  );
			if( u16Delta > u16intervall){
				ptrTimerOBJ->RunRequiered=1;
				/* ptrTimerOBJ->u16_lastrun = u16_current_time; */
				/* This will fix the drift */
				ptrTimerOBJ->u16_lastrun = ptrTimerOBJ->u16_lastrun+ptrTimerOBJ->u16_intervall;
			}
		} else {
			/* our delta is u16current_time - u16_lastrun */
			u16Delta = ( u16_current_time -ptrTimerOBJ->u16_lastrun  ) ;
			if( u16Delta > u16intervall ){
				ptrTimerOBJ->RunRequiered=1;
				/* ptrTimerOBJ->u16_lastrun = u16_current_time; */
				/* This will fix the drift */
				ptrTimerOBJ->u16_lastrun = ptrTimerOBJ->u16_lastrun+ptrTimerOBJ->u16_intervall;
			}
		}
	}
}

/* Littel dity define for Timer */
#define TIMER( Period , CallBack) {0, Period , 0 , CallBack }
/*  */
volatile TIMEROBJ_t Timer_OBJ[] = {
	
	TIMER(1,   voMS1_Task),
	TIMER(10,  voMS10_Tasks  ),		/* 10ms Timer */
	TIMER(100, voMS100_Tasks ),		/* 100ms Timer */
	TIMER(50,  voMS50_Tasks  ),		/* 50ms Timer */
	TIMER(1000, voMS1000_Tasks ),	/* 1000ms Timer */

};

bool boGetConnected( void ){
	return connected; 
}

FILE* GetUSBStream( void ){
	return &USBSerialStream;
}

/* 
if(connected!=0){
		fprintf(&USBSerialStream,"%i\t%i\n\r",s16BLTMP_GetBoilerTemp(),u16GetStandbyTemp());
	}
	
*/

void voScheduler( ) {

uint8_t u8NoOfTimer= (sizeof(Timer_OBJ)/sizeof(TIMEROBJ_t));

for(uint8_t i=0;i<u8NoOfTimer;i++){
	voProcessTimerOBJ(&Timer_OBJ[i]);
	if(Timer_OBJ[i].RunRequiered!=0){
		if(Timer_OBJ[i].fncptrCallBack!=NULL){
			Timer_OBJ[i].fncptrCallBack();
		}
		Timer_OBJ[i].RunRequiered=0;
	}
}

/* This is the run always part for the USB Stack */
voUSB_Task();
		
	
}

void voMS1_Task( void ){
	
	
	
}

void voMS10_Tasks(void){
	voKeyEvent_Task();
	
}

void voMS100_Tasks(void){
	WLM_voTask();
	voTMS_Task();
	voPSE_Task();
	voBTC_Task();
	
	voHMon_Task();
	voCoffeeFSM_Task();
	
	if(connected!=0){
	
	}
}

void voMS50_Tasks(void){
	/* We will update the Display here and may even handle Blinking ICONs */
	DPS_voTask();
	LLS_voTask();
	
	
}

void voMS1000_Tasks(){
	
}


/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
	#error "ARCH NOT SUPPORTED BY COFFEE MAKER MKI"
#endif

	/* Hardware Initialization */
	USB_Init();
	
	GPIO_Init();
	voSetPumpPowerState(POWER_OFF);
	voSetHeaterPowerState(POWER_OFF);
	
	voSetLedState(0,LED_OFF);
	voSetLedState(1,LED_OFF);
	voSetLedState(2,LED_OFF);
	
	
	voMCP3202_Init();
	
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	connected=1;
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	connected=0;
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

/** CDC class driver callback function the processing of changes to the virtual
 *  control lines sent from the host..
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo)
{
	/* You can get changes to the virtual CDC lines in this callback; a common
	   use-case is to use the Data Terminal Ready (DTR) flag to enable and
	   disable CDC communications in your application when set to avoid the
	   application blocking while waiting for a host to become ready and read
	   in the pending data from the USB endpoints.
	*/
	__attribute__ ((unused)) bool HostReady = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR) != 0;
}
