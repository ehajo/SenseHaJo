/*
 * eepromFileSystem.h
 *
 * Created: 05.03.2017 12:18:28
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef EEPROMFILESYSTEM_H_
#define EEPROMFILESYSTEM_H_

#include "eepromFilesSystemTypes.h"
#include <stdbool.h>

void voEEFS_Init( void ); /* Init for the Driver */

void voEEFS_WriteSystemSettings(systemsettings_t* Systemsettings);  
bool voEEFS_ReadSystemSettings(systemsettings_t* Systemsettings);
void voEEFS_WriteUserProgramm(uint8_t u8Idx, usersettings_t* Settings);
bool boEEFS_ReadUserProgramm(uint8_t u8Idx, usersettings_t* Settings);

void voSetDefaultSystemSettingsValues(systemsettings_t* DefaultSystemsettings);
void voSetDefaultUserSettingsValues( usersettings_t* DefaultUserSettings);

void EEFS_voIncrementCoffeeCounter( void );
uint16_t EEFS_u16GetCoffeeCounter( void );

uint32_t boEEFS_ReadUserProgrammParam(uint8_t idx, USERPROGPARAM_t param );
void boEEFS_UpdateUserProgramm(uint8_t idx, USERPROGPARAM_t param, uint32_t Value );
#endif /* EEPROMFILESYSTEM_H_ */