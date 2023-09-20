/*
 * main.c
 *
 * Created: 24.03.2017 19:08:59
 * Author: Mathias Clauﬂen aka calm
 */ 

/*
* www.eHaJo.de
* All rights reserved
* (C) 2016-2017
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
#include "../../services/usb_cdc_helper/usb_cdc_helper.h"

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



/* Main entry point, this also start the scheduler for some tasks

While we have no RTOS on this MCU, we still need to execute some
task in an organized fashion. This is done by the "scheduler" but
not one you would find in an RTOS. This one is very simple and
will require things to be handled in the super loop. 

What it is doing is calling a set of functions at a given point
in time to produce some cooperative tasking. 

TIMEROBJ_t Timer_OBJ[] holds an array of functions to be called at
certain intervals (in ms). In these functions all tasks that need
to be run in certain intervals will be called.

All is organized by voScheduler() that uses a timer to keep track
which task needs to be run next
 
*/

int main(void)
{
	SetupHardware();
	//We need to wait a few seconds until all pins are stable
	for(uint8_t i=0; i<20;i++ ){
		_delay_ms(100);
	}
	//Next is to power up the machine
	voEEFS_Init();
	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	FILE USBSerialStream;
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);
	voSetUSBStream(USBSerialStream);
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
			/* our time delta is now  ( UINT16_MAX - u16_lastrun ) + u16_current_time */
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

/* Little dirty define for Timer */
#define TIMER( Period , CallBack) {0, Period , 0 , CallBack }
/* The Table needs to be placed in RAM as we store some additional information during runtime in it */
volatile TIMEROBJ_t Timer_OBJ[] = {
	
	TIMER(1,   voMS1_Task),
	TIMER(10,  voMS10_Tasks  ),		/* 10ms Timer */
	TIMER(100, voMS100_Tasks ),		/* 100ms Timer */
	TIMER(50,  voMS50_Tasks  ),		/* 50ms Timer */
	TIMER(1000, voMS1000_Tasks ),	/* 1000ms Timer */

};


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
	
}

void voMS50_Tasks(void){
	/* We will update the Display here and may even handle Blinking ICONs */
	DPS_voTask();
	LLS_voTask();
	
	
}

void voMS1000_Tasks(){
	
}


/* Configuration for the MkI Hardware */
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
	voSetConnected(true);
	
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	voSetConnected(false);
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
