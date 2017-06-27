/*
 * senseodisplay.h
 *
 * Created: 25.01.2017 21:13:08
 *  Author: calm
 */ 

 /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef SENSEODISPLAY_H_
#define SENSEODISPLAY_H_

#include "senseodisplay_enum.h"

void SDP_voInit();	/* This will initialize the Display */
void SDP_voRefreshDisplay(); 	/* This will write new Data to the Display */
void SDP_voSetIconVisibility(SENSEOICON_t ICON, ICONVISIBLITY_t Visible);
ICONVISIBLITY_t SDP_emGetIconVisibility(SENSEOICON_t ICON);
void SDP_voSetBackLight(BACKLIGHTSTATE_t LightState);
BACKLIGHTSTATE_t SDP_emGetBackLight( void );
	
	




#endif /* SENSEODISPLAY_H_ */