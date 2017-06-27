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

#include "../../services/boilertemp/boilertemp.h"
#include "../../services/powermeter/powermeter.h"

#include "../../services/buttondriver/buttondriver.h"
#include "../../services/pumpservice/pumpservice.h"
#include "../../services/heaterservice/heaterservice.h"
#include "../../services/coffee_fsm/coffee_fsm.h"
#include "../../services/boilertempcontroller/boilertempcontroller.h"
#include "../../services/heaterpowercontroller/heaterpowercontroller.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another. This is for the first CDC interface,
 *  which sends strings to the host for each joystick movement.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial1_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_CDC1_CCI,
				.DataINEndpoint           =
					{
						.Address          = CDC1_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC1_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC1_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another. This is for the second CDC interface,
 *  which echos back all received data from the host.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial2_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_CDC2_CCI,
				.DataINEndpoint           =
					{
						.Address          = CDC2_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC2_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC2_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},

			},
	};


/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs.
 */
void voScheduler( void );

FILE USBSerialStream_1;
static FILE USBSerialStream_2;
volatile uint8_t connected=0;


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();
	
	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial1_CDC_Interface, &USBSerialStream_1);
	CDC_Device_CreateStream(&VirtualSerial2_CDC_Interface, &USBSerialStream_2);
	GlobalInterruptEnable();
	timer1_setup();
	
	/* Service init */
	voBLTMP_InitBoilerTemp();
	voPWRMTR_InitPowermeter();
	
	/* Setup the Tasks */
	voHPC_TaskInit();
	voKeyEvent_TaskInit();
	voPSE_TaskInit();
	voCoffeeFSM_TaskInit();
	voBTC_TaskInit();
	
	while( 1==1 ) 
	{
		
	voScheduler();
	
		
	}
}


uint8_t  voCheckIfRunRequiered(uint16_t delta , uint16_t u16_current_time , uint16_t* u16_lastrun ) 
{																						
	if(*u16_lastrun > u16_current_time ){

		if( ( ( UINT16_MAX - *u16_lastrun) +u16_current_time  ) > 10 ){
			*u16_lastrun = u16_current_time;
			return 1;
		}
		
	} else {

		if( ( u16_current_time -*u16_lastrun  ) > 10 ){
			return 1;
			*u16_lastrun = u16_current_time;
		}

	}
	
	return 1;
}
void voScheduler( ) {
static uint16_t u16_10ms_lastrun=0;
static uint16_t u16_100ms_lastrun=0;
static uint16_t u16_200ms_lastrun=0;

struct {
	uint8_t bf10msRunRequiered:1;
	uint8_t bf100msRunRequiered:1;
	uint8_t bf200msRunRequiered:1;
	uint8_t bfReserved:5;
} RunFlags;

uint16_t u16_current_time=u16GetSystemTicksMS();	
/* Here we execute the Coffemaker parts if requiered */


/* Everything here is run at 10ms Slots */
if(u16_10ms_lastrun > u16_current_time ){
	/* our timedelta is now  ( UINT16_MAX - u16_10ms_lastrun ) + u16_current_time */
	if( ( ( UINT16_MAX - u16_10ms_lastrun) +u16_current_time  ) > 10 ){
		RunFlags.bf10msRunRequiered=1;
		u16_10ms_lastrun = u16_current_time;
	} 
} else {
	/* our delta is u16current_time - u16_10ms_lastrun */
	if( ( u16_current_time -u16_10ms_lastrun  ) > 10 ){
		RunFlags.bf10msRunRequiered=1;
		u16_10ms_lastrun = u16_current_time;
	}
}

/* Everything here is run at 10ms Slots */
if(u16_100ms_lastrun > u16_current_time ){
	/* our timedelta is now  ( UINT16_MAX - u16_10ms_lastrun ) + u16_current_time */
	if( ( ( UINT16_MAX - u16_100ms_lastrun) +u16_current_time  ) > 100){
		RunFlags.bf100msRunRequiered=1;
		u16_100ms_lastrun = u16_current_time;
	}
	} else {
	/* our delta is u16current_time - u16_10ms_lastrun */
	if( ( u16_current_time -u16_100ms_lastrun  ) > 100 ){
		RunFlags.bf100msRunRequiered=1;
		u16_100ms_lastrun = u16_current_time;
	}
}

/* Everything here is run at 10ms Slots */
if(u16_200ms_lastrun > u16_current_time ){
	/* our timedelta is now  ( UINT16_MAX - u16_10ms_lastrun ) + u16_current_time */
	if( ( ( UINT16_MAX - u16_200ms_lastrun) +u16_current_time  ) > 10 ){
		RunFlags.bf200msRunRequiered=1;
		u16_200ms_lastrun = u16_current_time;
	}
	} else {
	/* our delta is u16current_time - u16_10ms_lastrun */
	if( ( u16_current_time -u16_200ms_lastrun  ) > 10 ){
		RunFlags.bf200msRunRequiered=1;
		u16_200ms_lastrun = u16_current_time;
	}
}


if(RunFlags.bf10msRunRequiered != 0){
	
	RunFlags.bf10msRunRequiered=0;
}

if(RunFlags.bf100msRunRequiered != 0){
	
	
	voKeyEvent_Task();
	voPSE_Task();
	voCoffeeFSM_Task();
	
	RunFlags.bf100msRunRequiered=0;
	
}


if(RunFlags.bf200msRunRequiered != 0){
	
	//fprintf(&USBSerialStream_1, "Temp:%i\n\r", s16BTC_GetCurrentBoilerTemp());
	voBTC_Task();
	RunFlags.bf200msRunRequiered=0;
}
/* Everything here is run at 100ms Slots */








/* This is the run always part for the USB Stack */

		/* Must throw away unused bytes from the host, or it will lock up while waiting for the device */
		int16_t Data = CDC_Device_ReceiveByte(&VirtualSerial1_CDC_Interface);
		if(Data >= 0)
		{
			/* We need to colect the Data for processing here */
			
			/* First we put the Data to the FiFo and execute the Data if we find an Enter */
		}
		/* Must throw away unused bytes from the host, or it will lock up while waiting for the device */
		Data = CDC_Device_ReceiveByte(&VirtualSerial2_CDC_Interface);
		if(Data >= 0)
		{
			/* We need to colect the Data for processing here */
			
			/* First we put the Data to the FiFo and execute the Data if we find an Enter */
		}
		
		CDC_Device_USBTask(&VirtualSerial1_CDC_Interface);
		CDC_Device_USBTask(&VirtualSerial2_CDC_Interface);
		USB_USBTask();
		
	
	
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

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial1_CDC_Interface);
	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial2_CDC_Interface);

	
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial1_CDC_Interface);
	CDC_Device_ProcessControlRequest(&VirtualSerial2_CDC_Interface);
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
