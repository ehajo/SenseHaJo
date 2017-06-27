/*
 * senseodisplay.c
 *
 * Created: 25.01.2017 21:12:56
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */

#include "../driver/SWI2C_special/i2cmaster.h"
#include "senseodisplay.h"
#include "../driver/senseo_button/senseo_button.h"

#define DISPLAY_ADDR 0x7E

/* We need some defines here for the ICONS */

#define DISP_ICON_CUPTWO				( 0 )
#define DISP_ICON_CUPTHREE				( 1 )
#define DISP_ICON_CALC					( 2 )
#define DISP_ICON_CUPONE				( 3 )
#define DISP_ICON_NOWATER				( 4 )
#define DISP_ICON_ARROWS				( 5 )
#define DISP_ICON_THERMOMETEROUT		( 6 )
#define DISP_ICON_THERMOMETERINNER		( 7 )

#define DISP_BACKLIGHT					( 15 )
#define DISP_NOT_ICON_THERMOMETEROUT	( 14 )
#define DISP_NOT_ICON_ARROW				( 13 )
#define DISP_NOT_ICON_NOWATER			( 12 )
#define DISP_NOT_CUPONE					( 11 )
#define DISP_NOT_CALC					( 10 )
#define DISP_NOT_CUPTHREE				( 9 )
#define DISP_NOT_CUPTWO					( 8 )

#define DISP_CHECKSUM_MASK				( 0x7F00 )


#include "../senseo_display/senseodisplay.h"

typedef struct {
	union{
		uint16_t Word;
		uint8_t Byte[2];
	} Data;
	
	uint16_t PrevWord;
	/* More may to come */
} DisplayData_t;

static volatile DisplayData_t DisplayData;

void voCalcCheckSum(void);

void SDP_voInit(){	/* This will initialize the Display */
	DisplayData.Data.Word=0x0FF80;
	DisplayData.PrevWord=0x0000;
	voCalcCheckSum();
	SDP_voRefreshDisplay();
	/* Set SCL to Input */
	
}

void SDP_voRefreshDisplay(){ 	/* This will write new Data to the Display */
	
	voCalcCheckSum();
	
	if(DisplayData.Data.Word != DisplayData.PrevWord){
		
		i2c_init(); /* Reinit the pins here */
		
		if( 0 == i2c_start(DISPLAY_ADDR + I2C_WRITE )  ){
			//i2c_write(DisplayData.Data.Word&0xFF);
			//i2c_write(DisplayData.Data.Word>>8);
			//i2c_write(0xFF);
			//i2c_write(0x80);
			i2c_write(DisplayData.Data.Byte[0]);
			i2c_write(DisplayData.Data.Byte[1]);
			
			
			DisplayData.PrevWord = DisplayData.Data.Word;
		}
		i2c_stop();
		
	}
	/* Display has new Data, but this will take 4ms to be finished */
	SBNT_SetSCLtoInput();

}
void SDP_voSetIconVisibility( SENSEOICON_t ICON, ICONVISIBLITY_t Visible ){
	
	switch(ICON){
		case ARROWS:{
			
			if(VISIBLE == Visible ) {
				DisplayData.Data.Word |= ( 1 << DISP_ICON_ARROWS );
			} else if (TOGGLE == Visible ) {
				DisplayData.Data.Word ^= ( 1 << DISP_ICON_ARROWS);
			} else {
				DisplayData.Data.Word &= ~( 1 << DISP_ICON_ARROWS);
			}
			
		} break;
		
		case CALC:{
			if(VISIBLE == Visible ) {
				DisplayData.Data.Word |= ( 1 << DISP_ICON_CALC );
			} else if (TOGGLE == Visible ) {
				DisplayData.Data.Word ^= ( 1 << DISP_ICON_CALC);
			} else {
				DisplayData.Data.Word &= ~( 1 << DISP_ICON_CALC);
			}
			
		}break;
		
		case THERMOMETER_INNER:{
			if(VISIBLE == Visible ) {
				DisplayData.Data.Word |= ( 1 << DISP_ICON_THERMOMETERINNER );
			} else if (TOGGLE == Visible ) {
				DisplayData.Data.Word ^= ( 1 << DISP_ICON_THERMOMETERINNER);
			} else {
				DisplayData.Data.Word &= ~( 1 << DISP_ICON_THERMOMETERINNER);
			}
		}break;
		
		case THERMOMETER_OUTER:{
			if(VISIBLE == Visible ) {
				DisplayData.Data.Word |= ( 1 << DISP_ICON_THERMOMETEROUT );
			} else if (TOGGLE == Visible ) {
				DisplayData.Data.Word ^= ( 1 << DISP_ICON_THERMOMETEROUT);
			} else {
				DisplayData.Data.Word &= ~( 1 << DISP_ICON_THERMOMETEROUT);
			}
		}break;
		
		case WATER:{
			if(VISIBLE == Visible ) {
				DisplayData.Data.Word |= ( 1 << DISP_ICON_NOWATER );
			} else if (TOGGLE == Visible ) {
				DisplayData.Data.Word ^= ( 1 << DISP_ICON_NOWATER);
			} else {
				DisplayData.Data.Word &= ~( 1 << DISP_ICON_NOWATER);
			}			
		}break;
		
		case CUP_ONE:{
			if(VISIBLE == Visible ) {
				DisplayData.Data.Word |= ( 1 << DISP_ICON_CUPONE );
			} else if (TOGGLE == Visible ) {
				DisplayData.Data.Word ^= ( 1 << DISP_ICON_CUPONE);
			} else {
				DisplayData.Data.Word &= ~( 1 << DISP_ICON_CUPONE);
			}
		}break;
		
		case CUP_TWO:{
			if(VISIBLE == Visible ) {
				DisplayData.Data.Word |= ( 1 << DISP_ICON_CUPTWO );
			} else if (TOGGLE == Visible ) {
				DisplayData.Data.Word ^= ( 1 << DISP_ICON_CUPTWO);
			} else {
				DisplayData.Data.Word &= ~( 1 << DISP_ICON_CUPTWO);
			}
		}break;
		
		case CUP_THREE:{
		if(VISIBLE == Visible ) {
			DisplayData.Data.Word |= ( 1 << DISP_ICON_CUPTHREE );
			} else if (TOGGLE == Visible ) {
			DisplayData.Data.Word ^= ( 1 << DISP_ICON_CUPTHREE);
			} else {
			DisplayData.Data.Word &= ~( 1 << DISP_ICON_CUPTHREE);
		}
		}break;
			
		case BACKLIGHT:{
		if(VISIBLE == Visible ) {
			DisplayData.Data.Word |= ( 1 << DISP_BACKLIGHT );
			} else if (TOGGLE == Visible ) {
			DisplayData.Data.Word ^= ( 1 << DISP_BACKLIGHT);
			} else {
			DisplayData.Data.Word &= ~( 1 << DISP_BACKLIGHT);
		}	
			
		}	
		break;
		
		default:{
			/* Not supported ICON do nothing */
		}
	
	} /* End of Switch */
}

void SDP_voSetBackLight( BACKLIGHTSTATE_t LightState ){
	
	switch (LightState){
		case BACKLIGHT_ON:{
			DisplayData.Data.Word |= ( 1 << DISP_BACKLIGHT );
		} break;
		
		case BACKLIGHT_OFF:{
			DisplayData.Data.Word &= ~( 1 << DISP_BACKLIGHT );
		}break;
		
		case BACKLIGHT_TOGGLE:{
			DisplayData.Data.Word ^= ( 1 << DISP_BACKLIGHT );
		}break;
		
		default: {
			/* Do nothing */
			
		}
	}
	
}


void voCalcCheckSum( void ){
	/* We need to Invert a few Bits and make a CheckSum at the end */
	
	DisplayData.Data.Word = DisplayData.Data.Word & 0x80FF ; 
	
	uint16_t u16toinvert= DisplayData.Data.Word&0x00FF;
	u16toinvert = ~u16toinvert; /* Bits are now inverted */
	u16toinvert = u16toinvert << 8 ; 
	u16toinvert = u16toinvert & 0x7F00;
	
	DisplayData.Data.Word |= u16toinvert;
		
	/* We have here still a bug inside */
}

BACKLIGHTSTATE_t SDP_emGetBackLight( void ) {
	BACKLIGHTSTATE_t emReturn = BACKLIGHT_OFF;
	
	if( 0 != (DisplayData.Data.Word & (1<<DISP_BACKLIGHT) ) ) {
		emReturn = BACKLIGHT_ON;	
	} else {
		emReturn = BACKLIGHT_OFF;
	}
	
	return emReturn;
	
}

ICONVISIBLITY_t SDP_emGetIconVisibility(SENSEOICON_t ICON) {
	
	ICONVISIBLITY_t emReturn = INVISIBLE;
	if(ICON < NO_OF_ICONS){
		if( 0 != (DisplayData.PrevWord & (1<<ICON) ) ) {
			emReturn = VISIBLE;
			} else {
			emReturn = INVISIBLE;
		}
	}
	return emReturn;
}