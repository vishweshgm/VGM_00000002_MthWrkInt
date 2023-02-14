/*
 * keypress.h
 *
 *  Created on: Dec 22, 2021
 *      Author: vishweshgm
 */

#ifndef APPLICATION_DRIVER_KEYPRESS_INC_KEYPRESS_H_
#define APPLICATION_DRIVER_KEYPRESS_INC_KEYPRESS_H_

#include "main.h"

#define DEBOUNCE_MS	100
#define CONTINUOUSMODE_DEBOUNCE_MS	100
#define KEPRESS_DEBOUNCE_CONST_VALUE	5
#define CONTINUOUSMODE_DEBOUNCE_VALUE	10

/*APPLICATION SPECIFIC PART*/
typedef enum keypress_EnumButton_Tag{
	CMD_Enum_DEFAULT	,
	CMD_Enum_B1	,
	CMD_Enum_B2	,
	CMD_Enum_B3,
	CMD_Enum_B4,
	CMD_Enum_MAX
}keypress_EnumButton;

typedef struct CMD_ButtonParameter_Tag{
	const uint16_t CMD_Command;
	uint8_t CMD_ContinuousPressCount;
	uint8_t CMD_DebounceCount;
	uint16_t CMD_synchro;
	uint16_t CMD_synchro_old;
	uint8_t CMD_DebounceCONSTValue;
	uint8_t CMD_CurrentState; /* 0 - Not pressed , 1 - Pressed*/
	uint8_t CMD_lastState;
	uint8_t CMD_ContinuousModeEnabled;
	uint8_t CMD_DebouncedState;
	uint8_t CMD_ContinuousModeDebounceValue;
	uint8_t ContinuousCountFlag;
	keypress_EnumButton CMD_enum;
}CMD_Parameter;




#define KEYPRESS_INIT \
{\
	{0x00,0,0,0,0,KEPRESS_DEBOUNCE_CONST_VALUE,0,0,0,0,CONTINUOUSMODE_DEBOUNCE_VALUE,0,CMD_Enum_DEFAULT},\
	{0xA1,0,0,0,0,KEPRESS_DEBOUNCE_CONST_VALUE,0,0,0,0,CONTINUOUSMODE_DEBOUNCE_VALUE,0,CMD_Enum_B1},\
	{0xA2,0,0,0,0,KEPRESS_DEBOUNCE_CONST_VALUE,0,0,0,0,CONTINUOUSMODE_DEBOUNCE_VALUE,0,CMD_Enum_B2},\
	{0xA3,0,0,0,0,KEPRESS_DEBOUNCE_CONST_VALUE,0,0,1,0,CONTINUOUSMODE_DEBOUNCE_VALUE,0,CMD_Enum_B3},\
	{0xA4,0,0,0,0,KEPRESS_DEBOUNCE_CONST_VALUE,0,0,1,0,CONTINUOUSMODE_DEBOUNCE_VALUE,0,CMD_Enum_B4}\
}
void keypressInit(void);
void CommandLoop(uint8_t loopms);
void updateCommand(uint16_t Command, uint8_t loopms);
void synchronizeCommands(void);
void acquire_appl_keydemand(void);


#endif /* APPLICATION_DRIVER_KEYPRESS_INC_KEYPRESS_H_ */
