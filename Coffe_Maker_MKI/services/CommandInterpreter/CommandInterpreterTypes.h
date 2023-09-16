/*
 * CommandInterpretertypeDefs.h
 *
 * Created: 13.09.2023 20:34:51
 *  Author: CalM
 */ 


#ifndef COMMANDINTERPRETERTYPEDEFS_H_
#define COMMANDINTERPRETERTYPEDEFS_H_
#include <stdio.h>
typedef enum {
	CMDI_SET,
	CMDI_GET,
	CMDI_CMD,
	CMDI_HELP,
	CMDI_BEER, /* For 99 Bottels of beer */
	CMDI_DIR_CNT,
	CMDI_EOF_DIR,
} CMDI_CMD_DIR_t;

typedef enum {
	CMDI_TEMP,
	CMDI_PREWATER,
	CMDI_WATER,
	CMDI_WAIT,
	CMDI_PRG,
	CMDI_CHPRG,
	CMDI_NONE,
	CMDI_MES_POWER,
	CMDI_BOILERTEMP,
	CMDI_CUP_LEVEL,
	CMDI_AUTOPOWER,
	CMDI_STBY_TEMP,
	CMDI_SYS_HELP,
	CMDI_ECHO,
	CMDI_GPIO,
	/* Commands for CMD */
	CMDI_MAKE,
	CMDI_POWER,

}CMDI_CMD_t;


typedef struct{
	CMDI_CMD_DIR_t DIR;
	CMDI_CMD_t CMD;
	uint32_t u32CmdValue;
} CMDI_CMD_DATA_t;


typedef struct {
	uint8_t bfEcho: 1;
	uint8_t bfReserved0: 1;
	uint8_t bfReserved1: 1;
	uint8_t bfReserved2: 1;
	uint8_t bfReserved3: 1;
	uint8_t bfReserved4: 1;
	uint8_t bfReserved5: 1;
	uint8_t bfReserved6: 1;
} CmdConfigBit_t;


#endif /* COMMANDINTERPRETERTYPEDEFS_H_ */