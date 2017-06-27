/*
 * ramfs.h
 *
 * Created: 24.03.2017 17:28:18
 *  Author: calm
 */ 

  /*
  * www.eHaJo.de
  * All rights reserved
  * (C) 2016-2017
  */


#ifndef RAMFS_H_
#define RAMFS_H_

#include "../services/eepromFileSystem/eepromFilesSystemTypes.h"
#include <stdbool.h>
#include <inttypes.h>


void voRFS_Init(void);
bool boRFS_WriteUserSettings(usersettings_t* Inputsettings );
bool boRFS_ReadUserSettings(usersettings_t* UserSettings );

uint32_t boRFS_ReadUserProgrammParam(USERPROGPARAM_t param );
void boRFS_UpdateUserProgramm(USERPROGPARAM_t param, uint32_t Value );


#endif /* RAMFS_H_ */