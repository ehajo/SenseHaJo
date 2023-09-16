/*
 * usb_cdc_helper.h
 *
 * Created: 16.09.2023 12:21:02
 *  Author: CalM
 */ 


#ifndef USB_CDC_HELPER_H_
#define USB_CDC_HELPER_H_

#include "stdbool.h"
#include "stdio.h"

bool boGetConnected( void );
void voSetConnected(bool con);
FILE* GetUSBStream( void );
void voSetUSBStream(FILE Stream);

#endif /* USB_CDC_HELPER_H_ */