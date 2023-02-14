/*
 * display_state.h
 *
 *  Created on: Mar 6, 2022
 *      Author: vishweshgm
 */

#ifndef APPLDRIVER_DISPLAY_STATE_INC_DISPLAY_STATE_H_
#define APPLDRIVER_DISPLAY_STATE_INC_DISPLAY_STATE_H_
#include "main.h"
typedef enum SM_DisplayState_Enum{
	SM_enum_Display_POWERON_LOGO =0,
	SM_enum_Display_MAIN_MENU,
	SM_enum_Display_CurrentModeSettings,
	SM_enum_Display_VoltageModeSettings,
	SM_enum_Display_SelfTest,
	SM_enum_Display_Settings_Menu,
	SM_enum_Display_Settings_Menu_Transfer,
	SM_enum_Display_Settings_Menu_Erase,
	SM_enum_Display_Settings_Menu_Completed,
	SM_enum_Display_CCRun_Slow,
	SM_enum_Display_CCRun_Stable,
	SM_enum_Display_CVRun_Slow,
	SM_enum_Display_CVRun_Stable,
	SM_enum_Display_CVPreparing,
	SM_enum_Display_CCPreparing,
	SM_enum_Display_PoweringOffWindow,
	SM_enum_Display_VishwaTest,
	SM_enum_Display_AutoModeUserInput,
	SM_enum_Display_StatusDisplay,
	SM_enum_Display_Info,
	SM_enum_Display_CCPause,
	SM_enum_Display_CVPause,
	SM_enum_Display_StoppedWhileRunning,
	SM_enum_Display_Blank,
}SM_DisplayState_Enum;

typedef struct SM_DisplayParamTag{
	float flt_di;
	float flt_dv;
	uint8_t SelectedIndexIntheList;
	uint8_t MaximumIndexInTheList;
	uint8_t SelectedSubIndex;
	uint8_t PowerOnCounter;
	uint8_t PowerOffCounter;
	uint32_t SelfTestCounter;

}SM_DisplayParam;

#if (DGUS_DISPLAY == 1)
typedef enum DGUSERRORTAG{
	DGUS_OK = 0,
	DGUS_NORESPONSE,
	DGUS_ERR
}DGUS_ERROR;

typedef struct DGUS_Parameters{
	uint16_t PageId;
	uint16_t CustomerID;
	uint16_t unused1;
	uint16_t cutOffVoltage;
	uint16_t cutOffCurrent;
	uint16_t setCurrent;
	uint16_t setTimerHour;
	uint16_t setTimerMin;
	uint16_t setTimerSec;
	uint16_t runTimerHour;
	uint16_t ErrorCounter;
	uint16_t setVoltage;

}DGUS_ParamStruct;

typedef struct SM_Display_Tag{
	SM_DisplayState_Enum DisplayState;
	DGUS_ParamStruct DGUSParam;
	SM_DisplayParam DisplayParam;
}SM_DisplayStruct;



#endif

extern uint8_t Fnlocal_u8dispint;
extern uint8_t Fnlocal_u8dispintmax;
extern uint8_t Fnlocal_u8dispintmin;


void SM_DispInit(void);

void DisplayLoop(uint8_t loopms);

void SerialCommunication(uint8_t loopms);
void vishwa_u8g2Animation(const uint8_t *bitmap, uint8_t w, uint8_t h);
float movingAvgFlt(float *ptrArrNumbers, float *ptrSum, uint8_t pos, uint16_t len, float nextNum);

#if (DGUS_DISPLAY == 1)
DGUS_ERROR WritePageID(uint16_t PageId);
DGUS_ERROR ReadPageID(void);
DGUS_ERROR DGUS_UpdateWriteVariables(void);
DGUS_ERROR UpdateReadVariables(void);
DGUS_ERROR DGUS_Reset(void);
DGUS_ERROR DGUS_updateMessages(void);
DGUS_ERROR DGUS_UpdateWritefloatVariables(void);
DGUS_ERROR DGUS_updatePauseReason(void);
DGUS_ERROR DGUS_updateSelfTestText(uint16_t loopms);
DGUS_ERROR StartAnimation(void);

#endif
/*VISHWA DESIGN LIBRARY*/

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

#endif /* APPLDRIVER_DISPLAY_STATE_INC_DISPLAY_STATE_H_ */
