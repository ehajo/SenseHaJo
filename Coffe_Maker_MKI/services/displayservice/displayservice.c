/*
 * displayservice.c
 *
 * Created: 28.01.2017 09:06:21
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include "displayservice.h"
#include "../../driver/senseo_display/senseodisplay.h"

static ICON_BLINK_FREQ_t emIconBlinkFreq[NO_OF_ICONS];
static ICON_BLINK_FREQ_t emIconBlinkFreqPrevious[NO_OF_ICONS];
static ICONVISIBLITY_t emIconInitialVisibiliyNow[NO_OF_ICONS];


static uint8_t u8_10Hz_Cnt=0;
static uint8_t u8_5Hz_Cnt=0;
static uint8_t u8_2Hz_Cnt=0;
static uint8_t u8_1Hz_Cnt=0;


void DPS_Init( void) {
	/* Set ALL variables to a default state */
	for(uint8_t i=0;i<NO_OF_ICONS;i++){
		emIconBlinkFreq[i]=BLINK_OFF;
		emIconBlinkFreqPrevious[i]=BLINK_OFF;
		SDP_voSetIconVisibility(i,INVISIBLE);
	}
	/* Write to Display */
	SDP_voRefreshDisplay();
}

void DPS_SetDisplayOff( void ){
	/* Set ALL variables to a default state */
	for(uint8_t i=0;i<NO_OF_ICONS;i++){
		emIconBlinkFreq[i]=BLINK_OFF;
		emIconBlinkFreqPrevious[i]=BLINK_OFF;
		SDP_voSetIconVisibility(i,INVISIBLE);
	}
	/* Write to Display */
	SDP_voRefreshDisplay();
}

/* This needs to be called every 10ms -> 100 Times a Second */
void DPS_voTask( void ){
	/* Blink and Refesh */
	/* As we got 10Hz, 5Hz, 2Hz and OFF we need to track that */
	/* This will be called at 20Hz */
	u8_10Hz_Cnt++;
	u8_5Hz_Cnt++;
	u8_2Hz_Cnt++;
	u8_1Hz_Cnt++;
	
	/* Check for overflow, we will use the 0 to Signal a Run is requierd, withc makes Comparison easyer for the MCU */
	if(u8_10Hz_Cnt>=1){
		u8_10Hz_Cnt=0;
	}
	
	if(u8_5Hz_Cnt>=2){
		u8_5Hz_Cnt=0;
	}
	
	if(u8_2Hz_Cnt>=5){
		u8_2Hz_Cnt=0;
	}
	
	if(u8_1Hz_Cnt>=10){
		u8_1Hz_Cnt=0;
	}
	
	for(uint8_t i=0;i<NO_OF_ICONS;i++){
		
		if( (emIconBlinkFreqPrevious[i]!=emIconBlinkFreq[i]) && ( emIconBlinkFreqPrevious[i] == BLINK_OFF) ){
			/* Okay something in the BlinkFreq has changed, and only if we are going from OFF to !=OFF we need to care */
			emIconBlinkFreqPrevious[i] = emIconBlinkFreq[i];
			emIconInitialVisibiliyNow [i]= SDP_emGetIconVisibility(i);
		}  /* Okay Visibility is updated */
		else if( (emIconBlinkFreqPrevious[i]!=emIconBlinkFreq[i]) && ( emIconBlinkFreq[i] == BLINK_OFF) ){
			/* Okay something in the BlinkFreq has changed, and only if we are going from OFF to !=OFF we need to care */
			emIconBlinkFreqPrevious[i] = emIconBlinkFreq[i];
			SDP_voSetIconVisibility((SENSEOICON_t)(i),emIconInitialVisibiliyNow[i]);
		}  /* Okay Visibility is updated */
		else {
			/* We dont care for now its a speed change only */
		}
		/* Next is to keep Track if and how we need to Blink */
		
		switch(emIconBlinkFreq[i]){
			
			case BLINK_10Hz:{
				if(0==u8_10Hz_Cnt){
					SDP_voSetIconVisibility(i,TOGGLE);
				}
			}break;
			
			
			case BLINK_5Hz:{
				if(0==u8_5Hz_Cnt){
					SDP_voSetIconVisibility(i,TOGGLE);
				}
			} break;
			
			case BLINK_2Hz:{
				if(0==u8_2Hz_Cnt){
					SDP_voSetIconVisibility(i,TOGGLE);
				}
			}break;
			
			case BLINK_1Hz:{
				if(0==u8_1Hz_Cnt){
					SDP_voSetIconVisibility(i,TOGGLE);
				}
			}break;
			
			default:{
				 /* We do nothing here */
				
								
			}break;
		}
		
	}
    SDP_voRefreshDisplay(); /* Display Update */
}

void DPS_SetBlinkFreq(ICON_BLINK_FREQ_t Frq, SENSEOICON_t Icon){
	if(Icon < NO_OF_ICONS){
		if(emIconBlinkFreq[Icon]!=Frq){
			emIconBlinkFreq[Icon] = Frq;
		}
	}
}


void DPS_SetIconVisibility(ICONVISIBLITY_t Visibility, SENSEOICON_t Icon){
	SDP_voSetIconVisibility(Icon,Visibility);
}

ICONVISIBLITY_t DPS_GetIconVisibility(SENSEOICON_t Icon){
	return SDP_emGetIconVisibility(Icon);
}
