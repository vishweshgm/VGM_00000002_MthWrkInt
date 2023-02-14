/*
 * display_state.h
 *
 *  Created on: Mar 6, 2022
 *      Author: vishweshgm
 */

#ifndef APPLDRIVER_DISPLAY_STATE_INC_DISPLAY_STATE_H_
#define APPLDRIVER_DISPLAY_STATE_INC_DISPLAY_STATE_H_
#include "main.h"
#include "u8g2.h"
typedef enum SM_DisplayState_Enum{
	SM_enum_Display_POWERON_LOGO =0,
	SM_enum_Display_MAIN_MENU,
	SM_enum_Display_RejuvenationMenu,
	SM_enum_Display_SelfTest,
	SM_enum_Display_Settings_Menu,
	SM_enum_Display_Settings_Menu_Transfer,
	SM_enum_Display_Settings_Menu_Erase,
	SM_enum_Display_Settings_Menu_Completed,
	SM_enum_Display_Settings_Menu_MaxVoltage,
	SM_enum_Display_MM_CurrentloopRunning_Slow,
	SM_enum_Display_MM_CurrentloopRunning_Stable,
	SM_enum_Display_PoweringOffWindow,
	SM_enum_Display_VishwaTest,
	SM_enum_Display_AutoModeUserInput,
	SM_enum_Display_StatusDisplay,
	SM_enum_Display_Info,
	SM_enum_Display_Pause,
	SM_enum_Display_StoppedWhileRunning,
	SM_enum_Display_Blank,
}SM_DisplayState_Enum;

typedef struct SM_DisplayParamTag{
	uint8_t SelectedIndexIntheList;
	uint8_t MaximumIndexInTheList;
	uint8_t SelectedSubIndex;
	uint8_t PowerOnCounter;
	uint8_t PowerOffCounter;

}SM_DisplayParam;


extern SM_DisplayState_Enum SM_enumDisplayState;
extern uint8_t Fnlocal_u8dispint;
extern uint8_t Fnlocal_u8dispintmax;
extern uint8_t Fnlocal_u8dispintmin;
extern SPI_HandleTypeDef hspi2;

void SM_DrawTask(u8g2_t *u8g2, uint8_t loopms);
void SM_DispInit(void);
void FilterCurrent_forLogging(void);
void DisplayLoop(uint8_t loopms);
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,void *arg_ptr);
void SerialCommunication(uint8_t loopms);
void vishwa_u8g2Animation(const uint8_t *bitmap, uint8_t w, uint8_t h);
void vishwa_u8g2drawcircleAnime(u8g2_t* u8g2);
void splashScreen(u8g2_t *u8g2);

/*VISHWA DESIGN LIBRARY*/

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#endif /* APPLDRIVER_DISPLAY_STATE_INC_DISPLAY_STATE_H_ */
