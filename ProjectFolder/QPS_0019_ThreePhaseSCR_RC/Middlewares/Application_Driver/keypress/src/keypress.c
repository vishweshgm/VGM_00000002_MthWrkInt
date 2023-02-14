/*
 * keypress.c
 *
 *  Created on: Dec 22, 2021
 *      Author: vishweshgm
 */
#define KEYPRESS_C 1
#if KEYPRESS_C == 1

#include "keypress.h"


keypress_EnumButton SM_KeyPressed;
CMD_Parameter CMD_Structure[CMD_Enum_MAX] = KEYPRESS_INIT;
extern uint8_t switchOn;

void keypressInit(void){

}

void CommandLoop(uint8_t loopms){
	uint8_t command = 0;

	for(uint8_t i = 0; i<(sizeof(CMD_Structure)/sizeof(CMD_Parameter)); i++){
		CMD_Structure[i].CMD_DebounceCONSTValue = DEBOUNCE_MS/loopms;
		CMD_Structure[i].CMD_ContinuousModeDebounceValue = CONTINUOUSMODE_DEBOUNCE_MS/loopms;
	}

	if(HAL_GPIO_ReadPin(uC_B1_GPIO_Port, uC_B1_Pin) == GPIO_PIN_SET){
		command = 0xA4;
		updateCommand(command,loopms);
	}

	if(HAL_GPIO_ReadPin(uC_B2_GPIO_Port, uC_B2_Pin) == GPIO_PIN_SET){
		command = 0xA3;
		updateCommand(command,loopms);
	}

	if(HAL_GPIO_ReadPin(uC_B3_GPIO_Port, uC_B3_Pin) == GPIO_PIN_SET){
		command = 0xA2;
		updateCommand(command,loopms);
	}

	if(HAL_GPIO_ReadPin(uC_B4_GPIO_Port, uC_B4_Pin) == GPIO_PIN_SET){
		command = 0xA1;
		updateCommand(command,loopms);
	}
	synchronizeCommands();
	acquire_appl_keydemand();

}


void updateCommand(uint16_t Command, uint8_t loopms){
	for(uint8_t i = 0; i<(sizeof(CMD_Structure)/sizeof(CMD_Parameter)); i++){
		if(CMD_Structure[i].CMD_Command == Command){
			CMD_Structure[i].CMD_CurrentState = 1;
			CMD_Structure[i].CMD_synchro++;
			if(CMD_Structure[i].CMD_lastState == 1){
				if((CMD_Structure[i].CMD_DebouncedState == 0) && (CMD_Structure[i].CMD_ContinuousPressCount == 0) && (CMD_Structure[i].CMD_ContinuousModeEnabled == 0)){
					CMD_Structure[i].CMD_DebounceCount++;
					if(CMD_Structure[i].CMD_DebounceCount >= CMD_Structure[i].CMD_DebounceCONSTValue){
						CMD_Structure[i].CMD_DebounceCount = CMD_Structure[i].CMD_DebounceCONSTValue;
						CMD_Structure[i].CMD_DebouncedState = 1;
						CMD_Structure[i].CMD_ContinuousPressCount = 0xFF;
					}
				}
				else if((CMD_Structure[i].CMD_DebouncedState == 0) && (CMD_Structure[i].CMD_ContinuousPressCount == 0) && (CMD_Structure[i].CMD_ContinuousModeEnabled == 1)){
					CMD_Structure[i].CMD_DebounceCount++;
					if(CMD_Structure[i].CMD_DebounceCount >= CMD_Structure[i].CMD_DebounceCONSTValue){
						CMD_Structure[i].CMD_DebounceCount = CMD_Structure[i].CMD_DebounceCONSTValue;
						CMD_Structure[i].CMD_DebouncedState = 1;
						CMD_Structure[i].CMD_ContinuousPressCount = 0xFE;
					}
				}
				else if((CMD_Structure[i].CMD_DebouncedState == 0) && (CMD_Structure[i].CMD_ContinuousPressCount > 0)){
					if((CMD_Structure[i].CMD_ContinuousModeEnabled == 1) && (CMD_Structure[i].CMD_ContinuousPressCount == 0xFF)){
						CMD_Structure[i].CMD_ContinuousPressCount = 1;
					}
					else if((CMD_Structure[i].CMD_ContinuousModeEnabled == 1) && (CMD_Structure[i].CMD_ContinuousPressCount >= 1) && (CMD_Structure[i].ContinuousCountFlag == 0)){
						CMD_Structure[i].CMD_ContinuousPressCount++;
						if(CMD_Structure[i].CMD_ContinuousPressCount >= (1000/loopms))CMD_Structure[i].ContinuousCountFlag = 1;
					}

					if(CMD_Structure[i].ContinuousCountFlag == 1){
						if((CMD_Structure[i].CMD_ContinuousModeEnabled == 1) && (CMD_Structure[i].CMD_ContinuousPressCount > 0)){
							CMD_Structure[i].CMD_ContinuousPressCount++;
							if(CMD_Structure[i].CMD_ContinuousPressCount>= CMD_Structure[i].CMD_ContinuousModeDebounceValue){
								CMD_Structure[i].CMD_ContinuousPressCount = 0;
								CMD_Structure[i].CMD_DebouncedState = 1;
							}
						}
					}
				}
				else if(CMD_Structure[i].CMD_DebouncedState == 1){
					CMD_Structure[i].CMD_DebouncedState = 0;
					if(CMD_Structure[i].CMD_ContinuousModeEnabled == 1){
						CMD_Structure[i].CMD_ContinuousPressCount++;
					}
				}
			}
			else{
				CMD_Structure[i].CMD_ContinuousPressCount = 0;
				CMD_Structure[i].CMD_DebounceCount = 0;
				CMD_Structure[i].CMD_lastState = 1;
				CMD_Structure[i].ContinuousCountFlag = 0;
			}
			break;
		}
	}
	return;
}


void synchronizeCommands(void){
	for(uint8_t i = 0; i<(sizeof(CMD_Structure)/sizeof(CMD_Parameter)); i++){
		if(CMD_Structure[i].CMD_synchro_old == CMD_Structure[i].CMD_synchro){
			CMD_Structure[i].CMD_synchro = 0;
			CMD_Structure[i].CMD_synchro_old = 0;
			CMD_Structure[i].CMD_DebouncedState = 0;
			CMD_Structure[i].CMD_ContinuousPressCount = 0;
			CMD_Structure[i].CMD_CurrentState = 0;
			CMD_Structure[i].CMD_lastState = 0;
			CMD_Structure[i].CMD_DebounceCount = 0;
			CMD_Structure[i].ContinuousCountFlag = 1;
		}
		else{
			CMD_Structure[i].CMD_synchro_old = CMD_Structure[i].CMD_synchro;
		}
	}
}

void acquire_appl_keydemand(void){
	if(CMD_Structure[CMD_Enum_B1].CMD_DebouncedState == 1){
		SM_KeyPressed = CMD_Enum_B1;
	}
	else if(CMD_Structure[CMD_Enum_B2].CMD_DebouncedState == 1){
		SM_KeyPressed = CMD_Enum_B2;
	}
	else if(CMD_Structure[CMD_Enum_B3].CMD_DebouncedState == 1){
		SM_KeyPressed = CMD_Enum_B3;
	}
	else if(CMD_Structure[CMD_Enum_B4].CMD_DebouncedState == 1){
		SM_KeyPressed = CMD_Enum_B4;
	}
	else{
		SM_KeyPressed = CMD_Enum_DEFAULT;
	}
}


#endif
