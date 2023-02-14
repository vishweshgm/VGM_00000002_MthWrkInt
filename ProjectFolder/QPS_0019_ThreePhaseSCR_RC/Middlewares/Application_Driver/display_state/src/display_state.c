/*
 * display_state.c
 *
 *  Created on: Mar 6, 2022
 *      Author: vishweshgm
 */
#define DISPLAY_STATE_C 1
#if DISPLAY_STATE_C == 1
#include "stdio.h"
#include "appl_diag.h"
#include "display_state.h"
#include "stateMachine.h"
#include "eeprom.h"
#include "u8g2.h"

/*extern variables*/
extern DiagFaultTable Appl_diagActiveFault;
extern SM_MainApplParameters SM_ApplParam;
extern EEP_DataStructure Appl_EEPParam;
SM_DisplayParam Appl_Display;
SM_DisplayState_Enum SM_enumDisplayState;
u8g2_t u8g2;
extern UART_HandleTypeDef huart5;
/*Display Variables*/
uint8_t Fnlocal_u8dispint;
uint8_t Fnlocal_u8dispintmax;
uint8_t Fnlocal_u8dispintmin;

extern volatile SM_ECUState_Enum SM_enumECUState;
static inline void Display_Buttons(u8g2_t *u8g2,SM_DisplayState_Enum disp);
static inline void Display_ErrorMessage(u8g2_t *u8g2);
static inline void Display_Voltage(u8g2_t *u8g2);
static inline void Display_Current(u8g2_t *u8g2);
static inline void Display_RunningTime(u8g2_t *u8g2);

void u8g2Init(u8g2_t *u8g2);

void SM_DispInit(void){
	u8g2Init(&u8g2);
	Appl_Display.SelectedIndexIntheList = 1;
}

void DisplayLoop(uint8_t loopms){
	  u8g2_FirstPage(&u8g2);
	  do
	  {
		  SM_DrawTask(&u8g2,loopms);
	  } while (u8g2_NextPage(&u8g2));
}
void SM_DrawTask(u8g2_t *u8g2, uint8_t loopms){
	char display_buf[40] = {0};
	char display_buf2[20] = {0};
	char display_buf3[20] = {0};
	float Fnclocal_dispfloat;
	static uint32_t timer_logo;
	static uint16_t Fnclocal_u16disp;



	FilterCurrent_forLogging();
	if(SM_enumDisplayState == SM_enum_Display_POWERON_LOGO){
		timer_logo++;
		if(timer_logo < ((2000*4)/loopms)){
			u8g2_SetFont(u8g2, u8g2_font_nokiafc22_tu);
			u8g2_DrawStr(u8g2, 0,10,"BATTERY");
			u8g2_DrawStr(u8g2, 1,20,"REJUVENATOR");
			u8g2_DrawStr(u8g2, 1,30,"DESIGNED FOR : ");
			u8g2_DrawStr(u8g2, 1,40,"ALACRITAS SOLUTIONS");
			u8g2_DrawStr(u8g2, 1,50,"PVT LTD");
			u8g2_DrawStr(u8g2, 1,60,"BENGALURU");
		}
		else{
			u8g2_SetFont(u8g2, u8g2_font_VCR_OSD_mu);/*15pixelheight*/
			u8g2_DrawStr(u8g2, 0,16,"QUAD POWER");
			u8g2_DrawStr(u8g2, 0,32,"SYSTEMS");
			u8g2_SetFont(u8g2, u8g2_font_profont11_tf);/*7pixelheight*/
			u8g2_DrawStr(u8g2, 0,48,"BENGALURU,INDIA");
			u8g2_DrawStr(u8g2, 0,60,"www.qpowersys.com");
//			splashScreen(u8g2);
//			u8g2_DrawBitmap(u8g2, 0, 0, 16, 64, image_data_qps_logo);
		}
	}

	if(SM_enumDisplayState == SM_enum_Display_MAIN_MENU){
		u8g2_SetFont(u8g2, u8g2_font_5x7_tr);
		u8g2_UserInterfaceSelectionList(u8g2, "MAIN MENU", Appl_Display.SelectedIndexIntheList,\
				"1. START\n2. SETTINGS\n3. SELFTEST\n4. INFO\n");
		Appl_Display.MaximumIndexInTheList = 4;
		Display_Buttons(u8g2,SM_enumDisplayState);
	}


	if(SM_enumDisplayState == SM_enum_Display_RejuvenationMenu){
		if(Appl_Display.SelectedIndexIntheList == 1){
			u8g2_SetFont(u8g2,u8g2_font_5x8_tr);
			strcpy(display_buf, "CUSTOMER ID NUMBER:");
			strcpy(display_buf2, "VALUE : ");
			strcpy(display_buf3, " ");
			Fnlocal_u8dispint = (uint8_t)round(Appl_EEPParam.stm32.EEP_CustomerNumber);
		}
		else if(Appl_Display.SelectedIndexIntheList == 2){
			u8g2_SetFont(u8g2, u8g2_font_5x8_tr);
			u8g2_UserInterfaceSelectionList(u8g2, "Battery Re-life cycle",Appl_EEPParam.stm32.EEP_RejuvenationCycleID,\
					"1. INITIAL CHARGE\n2. REJUVENATION 1\n3. EQ1\n4. REJUVENATION 2\n5. EQ2\n6. FINAL CHARGE");
			if(Appl_EEPParam.stm32.EEP_RejuvenationCycleID < 6){
				u8g2_SetFont(u8g2,u8g2_font_unifont_t_symbols);
				u8g2_DrawGlyph(u8g2, 1, 52, 0x25BE);
			}
		}
		else if(Appl_Display.SelectedIndexIntheList == 3){
			u8g2_SetFont(u8g2, u8g2_font_5x8_tr);
			strcpy(display_buf, "SET CURRENT");
			strcpy(display_buf2, "VALUE : ");
			strcpy(display_buf3, " A");
			Fnlocal_u8dispint = (uint8_t)round(SM_ApplParam.UserSetCurrent);
		}
		else if(Appl_Display.SelectedIndexIntheList == 4){
			u8g2_SetFont(u8g2, u8g2_font_5x8_tr);
			strcpy(display_buf, "SET TIMER");
			strcpy(display_buf2, "VALUE : ");
			strcpy(display_buf3, " HR");
			Fnlocal_u8dispint = (uint8_t)round(Appl_EEPParam.stm32.EEP_TimerSetHours);
		}
		else if(Appl_Display.SelectedIndexIntheList == 5){
			u8g2_SetFont(u8g2, u8g2_font_10x20_tf);
			u8g2_DrawStr(u8g2, 20,18,"  START?");
		}
		if((Appl_Display.SelectedIndexIntheList != 5) && (Appl_Display.SelectedIndexIntheList != 2))
		u8g2_UserInterfaceInputValue(u8g2, display_buf, display_buf2,&Fnlocal_u8dispint,1,200,3,display_buf3);
		Display_Buttons(u8g2,SM_enumDisplayState);
		Appl_Display.MaximumIndexInTheList = 5;
	}

	if(SM_enumDisplayState == SM_enum_Display_MM_CurrentloopRunning_Slow){
		u8g2_DrawHLine(u8g2,1,24,128);
		u8g2_DrawHLine(u8g2,1,36,128);
		u8g2_DrawVLine(u8g2,64,1,24);
		Display_Voltage(u8g2);
		Display_Current(u8g2);
		Display_RunningTime(u8g2);
		Display_ErrorMessage(u8g2);
		Display_Buttons(u8g2,SM_enumDisplayState);
	}

	if(SM_enumDisplayState == SM_enum_Display_MM_CurrentloopRunning_Stable){
		u8g2_DrawHLine(u8g2,1,24,128);
		u8g2_DrawHLine(u8g2,1,36,128);
		u8g2_DrawVLine(u8g2,64,1,24);
		Display_Voltage(u8g2);
		Display_Current(u8g2);
		Display_RunningTime(u8g2);
		Display_ErrorMessage(u8g2);
		Display_Buttons(u8g2,SM_enumDisplayState);
	}

	if(SM_enumDisplayState == SM_enum_Display_SelfTest){
		u8g2_SetFont(u8g2, u8g2_font_5x8_tf);
		if(Appl_diagActiveFault.BatteryReversal == 1){
			u8g2_DrawStr(u8g2, 1,11,"OUTPUT TERMINAL");
			u8g2_DrawStr(u8g2, 1,21,"IS NOT PROPER");
		}
		else if(Appl_diagActiveFault.PowerFail_Z1 == 1){
			u8g2_DrawStr(u8g2, 1,11,"INPUT TERMINAL 1(RED)");
			u8g2_DrawStr(u8g2, 1,21,"NOT CONNECTED");
		}
		else if(Appl_diagActiveFault.PowerFail_Z2 == 1){
			u8g2_DrawStr(u8g2, 1,11,"INPUT TERMINAL 2(YEL)");
			u8g2_DrawStr(u8g2, 1,21,"NOT CONNECTED");
		}
		else if(Appl_diagActiveFault.PowerFail_Z3 == 1){
			u8g2_DrawStr(u8g2, 1,11,"INPUT TERMINAL 3(BLU)");
			u8g2_DrawStr(u8g2, 1,21,"NOT CONNECTED");
		}
		else if(Appl_diagActiveFault.OverVoltage == 1){
			u8g2_DrawStr(u8g2, 1,11,"BATTERY VOLTAGE");
			u8g2_DrawStr(u8g2, 1,21,"DETECTED IS HIGH");
			Fnclocal_dispfloat = SM_ApplParam.RealtimeVoltage;
			Fnclocal_dispfloat = round(Fnclocal_dispfloat);
			gcvt(Fnclocal_dispfloat,3,display_buf);
			u8g2_DrawStr(u8g2, 1,31,display_buf);
			u8g2_DrawStr(u8g2, 65,31,"Volts");
			u8g2_DrawStr(u8g2, 1,41,"MAX LIMIT:");
			Fnclocal_dispfloat = SM_ApplParam.OverVoltageThreshold;
			Fnclocal_dispfloat = round(Fnclocal_dispfloat);
			gcvt(Fnclocal_dispfloat,3,display_buf);
			u8g2_DrawStr(u8g2, 64,41,display_buf);
			u8g2_DrawStr(u8g2, 94,41,"Volts");

		}
		else if(Appl_diagActiveFault.UnderVoltage == 1){
			u8g2_DrawStr(u8g2, 1,11,"BATTERY VOLTAGE");
			u8g2_DrawStr(u8g2, 1,21,"DETECTED IS TOO LOW");
			Fnclocal_dispfloat = SM_ApplParam.RealtimeVoltage;
			Fnclocal_dispfloat = round(Fnclocal_dispfloat);
			gcvt(Fnclocal_dispfloat,3,display_buf);
			u8g2_DrawStr(u8g2, 1,31,display_buf);
			u8g2_DrawStr(u8g2, 65,31,"Volts");
			u8g2_DrawStr(u8g2, 1,41,"MIN LIMIT:");
			Fnclocal_dispfloat = SM_ApplParam.UnderVoltageThreshold;
			Fnclocal_dispfloat = round(Fnclocal_dispfloat);
			gcvt(Fnclocal_dispfloat,3,display_buf);
			u8g2_DrawStr(u8g2, 64,41,display_buf);
			u8g2_DrawStr(u8g2, 94,41,"Volts");
		}
		else{
			u8g2_DrawStr(u8g2, 1,11,"CHECK THE FOLLOWING:");
			u8g2_DrawStr(u8g2, 1,21,"VOLTAGE: ");
			Fnclocal_dispfloat = SM_ApplParam.RealtimeVoltage;
			Fnclocal_dispfloat = round(Fnclocal_dispfloat);
			gcvt(Fnclocal_dispfloat,3,display_buf);
			u8g2_DrawStr(u8g2, 70,21,display_buf);
			u8g2_DrawStr(u8g2, 90,21,"V");
			u8g2_DrawStr(u8g2, 1,31,"ALL LEDs ARE BLINKING ");
			if(Appl_diagActiveFault.PhaseReversal == 1) u8g2_DrawStr(u8g2, 1,41,"PHASE IS REVERSE");else u8g2_DrawStr(u8g2, 1,41,"PHASE IS OK");
			if(Appl_diagActiveFault.HighTempperatureSCR == 1) u8g2_DrawStr(u8g2, 1,51,"TEMP IS HIGH");else u8g2_DrawStr(u8g2, 1,51,"TEMP IS ALRIGHT");
			u8g2_DrawStr(u8g2, 1,61,"MEMORY % : ");
			Fnclocal_dispfloat = (Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH<<8 | Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL)*100.0/4000.0;
			Fnclocal_dispfloat = round(Fnclocal_dispfloat);
			gcvt(Fnclocal_dispfloat,3,display_buf);
			u8g2_DrawStr(u8g2, 70,61,display_buf);
		}
	}

	if(SM_enumDisplayState == SM_enum_Display_Settings_Menu){
		u8g2_SetFont(u8g2, u8g2_font_5x7_tr);
		u8g2_UserInterfaceSelectionList(u8g2, "SETTINGS", Appl_Display.SelectedIndexIntheList,\
				"1. DATA_TRANSFER\n2. DATA_ERASE\n3. MAX VOLTAGE");
		Appl_Display.MaximumIndexInTheList = 3;
		Display_Buttons(u8g2,SM_enumDisplayState);
	}

	if(SM_enumDisplayState == SM_enum_Display_Settings_Menu_MaxVoltage){
		u8g2_SetFont(u8g2, u8g2_font_5x7_tr);
		Fnclocal_dispfloat = round(SM_ApplParam.OverVoltageThreshold);
		u8g2_DrawStr(u8g2, 1,20,"Max Voltage(V)");
		gcvt(Fnclocal_dispfloat,3,display_buf);
		u8g2_SetFont(u8g2, u8g2_font_courB10_tn);
		u8g2_DrawStr(u8g2, 85,25,display_buf);

		Display_Buttons(u8g2,SM_enumDisplayState);
	}

	if(SM_enumDisplayState == SM_enum_Display_Settings_Menu_Transfer){
		u8g2_SetFont(u8g2, u8g2_font_t0_11_tf);
		u8g2_DrawStr(u8g2, 10,21,"START?");
		Display_Buttons(u8g2,SM_enumDisplayState);
	}

	if(SM_enumDisplayState == SM_enum_Display_Settings_Menu_Erase){
		u8g2_SetFont(u8g2, u8g2_font_t0_11_tf);
		u8g2_DrawStr(u8g2, 10,21,"ERASE?");
		Display_Buttons(u8g2,SM_enumDisplayState);
	}

	if(SM_enumDisplayState == SM_enum_Display_Settings_Menu_Completed){
		if(Appl_Display.SelectedSubIndex == 1){
			u8g2_SetFont(u8g2, u8g2_font_5x7_tr);
			u8g2_DrawStr(u8g2, 1,10,"PROGRESS:");
			u8g2_DrawStr(u8g2, 1,30,"Transfer of");
			Fnclocal_dispfloat = SM_ApplParam.ExtEepTotalRecordsTotransferred;
			gcvt(Fnclocal_dispfloat,3,display_buf);
			u8g2_DrawStr(u8g2, 64,30,display_buf);
			u8g2_DrawStr(u8g2, 1,50,"Records Pending..");

		}
		if(Appl_Display.SelectedSubIndex == 2){
			u8g2_SetFont(u8g2, u8g2_font_5x7_tr);
			u8g2_DrawStr(u8g2, 1,10,"ERASING");
			u8g2_DrawStr(u8g2, 1,30,"Delete of");
			Fnclocal_dispfloat = SM_ApplParam.Transfer_PageloopCount;
			gcvt(Fnclocal_dispfloat,3,display_buf);
			u8g2_DrawStr(u8g2, 54,30,display_buf);
			u8g2_DrawStr(u8g2, 1,50,"% OVER..");

		}
		Display_Buttons(u8g2,SM_enumDisplayState);
	}

	if(SM_enumDisplayState == SM_enum_Display_VishwaTest){
		u8g2_DrawHLine(u8g2,1,24,128);
		u8g2_DrawHLine(u8g2,1,36,128);
		u8g2_DrawVLine(u8g2,64,1,24);
		Display_Voltage(u8g2);
		Display_Current(u8g2);
		Display_RunningTime(u8g2);
		Display_ErrorMessage(u8g2);
		Display_Buttons(u8g2,SM_enumDisplayState);
	}
	else if(SM_enumDisplayState == SM_enum_Display_Info){
		u8g2_SetFont(u8g2, u8g2_font_5x7_tr);
		u8g2_DrawStr(u8g2, 1,20,"TOTAL USGAE HOUR : ");
		Fnclocal_u16disp = (Appl_EEPParam.stm32.EEP_Usg_HourH <<8 )|Appl_EEPParam.stm32.EEP_Usg_HourL;
		Fnclocal_dispfloat = (float)Fnclocal_u16disp;
		gcvt(Fnclocal_dispfloat,4,display_buf);
		u8g2_DrawStr(u8g2, 0,40,display_buf);
		u8g2_DrawStr(u8g2, 40,40,":");
		Fnclocal_dispfloat = Appl_EEPParam.stm32.EEP_Usg_Min;
		gcvt(Fnclocal_dispfloat,3,display_buf);
		u8g2_DrawStr(u8g2, 50,40,display_buf);
		u8g2_DrawStr(u8g2, 70,40,":");
		Fnclocal_dispfloat = Appl_EEPParam.stm32.EEP_Usg_Sec;
		gcvt(Fnclocal_dispfloat,3,display_buf);
		u8g2_DrawStr(u8g2, 90,40,display_buf);
	}
	else if(SM_enumDisplayState == SM_enum_Display_Pause){
			u8g2_SetFont(u8g2, u8g2_font_sirclivethebold_tr);
			u8g2_DrawStr(u8g2, 0,10," SYSTEM PAUSE");
			u8g2_SetFont(u8g2, u8g2_font_5x7_tf);
			u8g2_DrawStr(u8g2, 0,20,"FOLLOWING REASON RECORDED:");
			if(SM_ApplParam.Pausereason == PauseReasonEnum_TimerComplete){
				u8g2_DrawStr(u8g2, 0,30,"TIME OVER");
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_UserPaused){
				u8g2_DrawStr(u8g2, 0,50,"USERPAUSE");
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_PowerFailZ1){
				if(Appl_diagActiveFault.PowerFail_Z1 == 1) {u8g2_DrawStr(u8g2, 0,30,"L1(R)POWER ZERO");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : ACTIVE");}
				else {u8g2_DrawStr(u8g2, 0,30,"L1(R)POWER ZERO");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : HEALED");}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_PowerFailZ2){
				if(Appl_diagActiveFault.PowerFail_Z2 == 1) {u8g2_DrawStr(u8g2, 0,30,"L2(Y)POWER ZERO");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : ACTIVE");}
				else {u8g2_DrawStr(u8g2, 0,30,"L2(Y)POWER ZERO");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : HEALED");}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_PowerFailZ3){
				if(Appl_diagActiveFault.PowerFail_Z3 == 1) {u8g2_DrawStr(u8g2, 0,30,"L3(B)POWER ZERO");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : ACTIVE");}
				else {u8g2_DrawStr(u8g2, 0,30,"L3(B)POWER ZERO");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : HEALED");}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_BatteryReversal){
				if(Appl_diagActiveFault.BatteryReversal == 1) {u8g2_DrawStr(u8g2, 0,30,"CHECK O/P CONNECTION CONNECTION");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : ACTIVE");}
				else {u8g2_DrawStr(u8g2, 0,30,"CHECK O/P CONNECTION");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : HEALED");}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_OverVoltage){
				if(Appl_diagActiveFault.OverVoltage == 1) {u8g2_DrawStr(u8g2, 0,30,"AVG VOLTAGE ABOVE 200V");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : ACTIVE");}
				else {u8g2_DrawStr(u8g2, 0,30,"OVER VOLTAGE");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : HEALED");}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_EEPLogMemoryFull){
				if(Appl_diagActiveFault.EEPLogMemoryFull == 1) {u8g2_DrawStr(u8g2, 0,30,"LOG MEMORY FULL");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : ACTIVE");}
				else {u8g2_DrawStr(u8g2, 0,30,"LOG MEMORY FULL");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : HEALED");}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_UnderVoltage){
				if(Appl_diagActiveFault.UnderVoltage == 1) {u8g2_DrawStr(u8g2, 0,30,"UNDER VOLTAGE or OPEN");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : ACTIVE");}
				else {u8g2_DrawStr(u8g2, 0,30,"UNDER VOLTAGE or OPEN");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : HEALED");}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_OverCurrent){
				if(Appl_diagActiveFault.UnderVoltage == 1) {u8g2_DrawStr(u8g2, 0,30,"UNDER VOLTAGE or OPEN");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : ACTIVE");}
				else {u8g2_DrawStr(u8g2, 0,30,"OVER CURRENT");u8g2_DrawStr(u8g2, 0,50,"FAULT STATE : HEALED");}
			}
			Display_Buttons(u8g2,SM_enumDisplayState);

	}
	else if(SM_enumDisplayState == SM_enum_Display_StoppedWhileRunning){
		u8g2_SetFont(u8g2, u8g2_font_5x7_tr);
		u8g2_DrawStr(u8g2, 5,17," RESUMING FROM");
		u8g2_DrawStr(u8g2, 5,35," LAST PONIT");
		u8g2_SetFont(u8g2, u8g2_font_5x7_tf);
		Fnclocal_dispfloat = (float)Appl_Display.PowerOnCounter;
		gcvt(Fnclocal_dispfloat,3,display_buf);
		u8g2_DrawStr(u8g2, 5,50,display_buf);
		Display_Buttons(u8g2,SM_enumDisplayState);
	}
	else if(SM_enumDisplayState == SM_enum_Display_PoweringOffWindow){
		u8g2_SetFont(u8g2, u8g2_font_5x7_tf);
		u8g2_DrawStr(u8g2, 1,17,"POWER DOWN");
		u8g2_DrawStr(u8g2, 1,35,"SHUT DOWN AFTER");
		u8g2_DrawStr(u8g2, 1,47,"FIVE SECONDS");
		u8g2_SetFont(u8g2, u8g2_font_5x7_tf);
		Fnclocal_dispfloat = (float)Appl_Display.PowerOffCounter;
		gcvt(Fnclocal_dispfloat,3,display_buf);
		u8g2_DrawStr(u8g2, 90,55,display_buf);
	}
}
static inline void Display_ErrorMessage(u8g2_t *u8g2){
	u8g2_SetFont(u8g2, u8g2_font_5x7_tf);
	if((Appl_diagActiveFault.PowerFail_Z1 == 1) ||\
			Appl_diagActiveFault.PowerFail_Z2 == 1 ||\
			Appl_diagActiveFault.PowerFail_Z3 == 1){

		u8g2_DrawStr(u8g2, 20,50,"ERROR : POWERFAIL");
	}
	else if(Appl_diagActiveFault.HighTempperatureSCR == 1) u8g2_DrawStr(u8g2, 20,50,"HIGH TEMPERATURE");
	else if(Appl_diagActiveFault.PhaseReversal == 1) u8g2_DrawStr(u8g2, 20,50,"REVERSE PHASE MODE");
	else if(Appl_diagActiveFault.HighTempperatureSCR == 0 && \
			Appl_diagActiveFault.OverCurrent == 0 && \
			Appl_diagActiveFault.OverVoltage == 0 && \
			Appl_diagActiveFault.PowerFail_Z1 == 0 && \
			Appl_diagActiveFault.PowerFail_Z2 == 0 && \
			Appl_diagActiveFault.PowerFail_Z3 == 0 && \
			Appl_diagActiveFault.PhaseReversal == 0 && \
			Appl_diagActiveFault.UnderVoltage == 0){

		u8g2_DrawStr(u8g2, 20,50,"NO MESSAGES");
	}
}


static inline void Display_Buttons(u8g2_t *u8g2,SM_DisplayState_Enum disp){

	float batteryChargePercent;



	if(disp == SM_enum_Display_MM_CurrentloopRunning_Slow || \
				disp == SM_enum_Display_VishwaTest || \
				disp == SM_enum_Display_MM_CurrentloopRunning_Stable){
		u8g2_DrawHLine(u8g2,90,52,128);
		u8g2_DrawVLine(u8g2,90,52,12);
	}
	else{
	u8g2_DrawHLine(u8g2,1,52,128);
	u8g2_DrawVLine(u8g2,32,52,12);
	u8g2_DrawVLine(u8g2,64,52,12);
	u8g2_DrawVLine(u8g2,96,52,12);
	}

	if((disp == SM_enum_Display_MAIN_MENU) || \
			0 || \
			0){
		u8g2_SetFont(u8g2, u8g2_font_synchronizer_nbp_tf);
		u8g2_DrawStr(u8g2, 1,63," UP");
		u8g2_DrawStr(u8g2, 33,63,"DOWN");
		u8g2_DrawStr(u8g2, 65,63," OK");
		u8g2_DrawStr(u8g2, 97,63,"INFO");
	}
	else if(disp == SM_enum_Display_MM_CurrentloopRunning_Slow || \
			disp == SM_enum_Display_VishwaTest || \
			disp == SM_enum_Display_MM_CurrentloopRunning_Stable){

		/*Vishwesh:
		 * Do determine battery charge percentage, I employ simple trick:
		 * Let's say overvoltage is set 230V, then I divide 230/2.4 = appx 96 cells.
		 * if 96 cells have lowest battery voltage of 1.5V, then 0% voltage willbe 96*1.5 = 144.
		 * Using this, I show battery percent
		 */
		batteryChargePercent =(SM_ApplParam.RealtimeVoltage*100.0)/( SM_ApplParam.OverVoltageThreshold);

		u8g2_SetFont(u8g2,u8g2_font_battery19_tn);
		if(batteryChargePercent >= 0) u8g2_DrawGlyph(u8g2, 1, 63, 0x0030);
		if(batteryChargePercent >= 60) u8g2_DrawGlyph(u8g2, 1, 63, 0x0031);
		if(batteryChargePercent >= 70) u8g2_DrawGlyph(u8g2, 1, 63, 0x0032);
		if(batteryChargePercent >= 80) u8g2_DrawGlyph(u8g2, 1, 63, 0x0033);
		if(batteryChargePercent >= 90) u8g2_DrawGlyph(u8g2, 1, 63, 0x0034);
		if(batteryChargePercent >= 95) u8g2_DrawGlyph(u8g2, 1, 63, 0x0035);

		u8g2_SetFont(u8g2, u8g2_font_5x7_tf);
		u8g2_DrawStr(u8g2, 97,63,"PAUSE");
	}
	else if(disp == SM_enum_Display_RejuvenationMenu || \
			disp == SM_enum_Display_Settings_Menu_Transfer ||\
			disp == SM_enum_Display_Settings_Menu_Erase ||\
			disp == SM_enum_Display_Settings_Menu_MaxVoltage ||\
			disp == SM_enum_Display_Settings_Menu){
		u8g2_SetFont(u8g2, u8g2_font_synchronizer_nbp_tf);
		u8g2_DrawStr(u8g2, 1,63," UP");
		u8g2_DrawStr(u8g2, 33,63,"DOWN");
		u8g2_DrawStr(u8g2, 65,63," OK");
		u8g2_DrawStr(u8g2, 97,63,"BACK");
	}
	else if(disp == SM_enum_Display_Pause || \
			disp == SM_enum_Display_StoppedWhileRunning){
		u8g2_SetFont(u8g2, u8g2_font_5x7_tf);
		u8g2_DrawStr(u8g2, 65,63,"RESUME");
		u8g2_DrawStr(u8g2, 97,63,"STOP");
//		u8g2_SetFont(u8g2,u8g2_font_battery19_tn);
//		u8g2_DrawGlyph(u8g2, 1, 63, 0x0030);
//		u8g2_DrawGlyph(u8g2, 33, 63, 0x0030);
	}
}
static inline void Display_Voltage(u8g2_t *u8g2){
	static char display_buf[40] = {0};
	static char display_buf2[40] = {0};
	float Fnclocal_dispfloat;
	static uint8_t Fnclocal_DisplayCounter;
	Fnclocal_DisplayCounter++;
	if(Fnclocal_DisplayCounter>=4*4){
		Fnclocal_DisplayCounter = 0;
		Fnclocal_dispfloat = SM_ApplParam.RealtimeVoltage;
		Fnclocal_dispfloat = round(Fnclocal_dispfloat);
		gcvt(Fnclocal_dispfloat,3,display_buf);
	}
	u8g2_SetFont(u8g2, u8g2_font_courB10_tn);
	u8g2_DrawStr(u8g2, 10,21,display_buf);
	u8g2_SetFont(u8g2, u8g2_font_luRS08_tf);
	u8g2_DrawStr(u8g2, 1,8,"Voltage");

	u8g2_SetFont(u8g2, u8g2_font_5x7_tf);
	u8g2_DrawStr(u8g2, 20,63,"MAX V: ");
	Fnclocal_dispfloat = SM_ApplParam.OverVoltageThreshold;
	Fnclocal_dispfloat = round(Fnclocal_dispfloat);
	gcvt(Fnclocal_dispfloat,3,display_buf2);
	u8g2_DrawStr(u8g2, 50,63,display_buf2);
}

static inline void Display_Current(u8g2_t *u8g2){
	char display_buf[40] = {0};
	static float Fnclocal_dispfloat;

	static uint8_t Fnclocal_DisplayCounter;
	Fnclocal_DisplayCounter++;
	if((Fnclocal_DisplayCounter>=4*4)){
		Fnclocal_DisplayCounter = 0;
		if(SM_ApplParam.RealtimeCurrentFiltered <3){
			Fnclocal_dispfloat = 0;
		}
		else{
			Fnclocal_dispfloat = SM_ApplParam.RealtimeCurrentFiltered;
		}

//		Fnclocal_dispfloat = round(Fnclocal_dispfloat);
	}
	u8g2_SetFont(u8g2, u8g2_font_luRS08_tf);
	u8g2_DrawStr(u8g2, 71,8,"Amps");
	u8g2_SetFont(u8g2, u8g2_font_courB10_tn);
	gcvt(Fnclocal_dispfloat,3,display_buf);
	u8g2_DrawStr(u8g2, 71,21,display_buf);
}

static inline void Display_RunningTime(u8g2_t *u8g2){
	char display_buf[40] = {0};
	u8g2_SetFont(u8g2, u8g2_font_5x7_tf);
	u8g2_DrawStr(u8g2, 1,35,"RUN: ");
	u8g2_DrawStr(u8g2, 25,35,(const char*)itoa(Appl_EEPParam.stm32.EEP_Run_Hour ,display_buf,10));
	u8g2_DrawStr(u8g2, 35,35," : ");
	u8g2_DrawStr(u8g2, 45,35,(const char*)itoa(Appl_EEPParam.stm32.EEP_Run_Min,display_buf,10));
	u8g2_DrawStr(u8g2, 55,35," : ");
	u8g2_DrawStr(u8g2, 65,35,(const char*)itoa(Appl_EEPParam.stm32.EEP_Run_Sec,display_buf,10));
	u8g2_DrawStr(u8g2, 80,35,"/");
	u8g2_DrawStr(u8g2, 90,35,(const char*)itoa(Appl_EEPParam.stm32.EEP_TimerSetHours,display_buf,10));
	u8g2_DrawStr(u8g2, 100,35,"HRS");
}

void splashScreen(u8g2_t *u8g2){
	vishwa_u8g2drawcircleAnime(u8g2);
}

void FilterCurrent_forLogging(void){
	static float Fnclocal_errorValue;
	Fnclocal_errorValue = SM_ApplParam.RealtimeCurrent - SM_ApplParam.RealtimeCurrentFiltered;
	SM_ApplParam.RealtimeCurrentFiltered = SM_ApplParam.RealtimeCurrentFiltered + Fnclocal_errorValue*0.4;
}
#define TOTAL_BUFFER_SIZE 26
uint8_t uartTxBuffer[TOTAL_BUFFER_SIZE];
uint8_t switchOn = 0;
uint8_t switchOnkeypressed = 0;
void SerialCommunication(uint8_t loopms){
	static uint8_t count=0;
	uint16_t TempU16;
	uint8_t statsss;
	static uint32_t keepAliveCommunication;

	if((SM_enumECUState == SM_enum_ECU_VldAC_RunCurrentLoop_increaseVoltageMode_Slow) ||
			(SM_enumECUState == SM_enum_ECU_VldAC_RunCurrentLoop_StableVoltagefloat)){
		statsss = 1;
	}
	else if((SM_enumECUState == SM_enum_ECU_PauseMode) ){
		statsss = 2;
	}
	else{
		statsss = 3;
	}
	TempU16 = (uint16_t)SM_ApplParam.Realtime_floatingVoltage;
	count++;
	if(count>=(1000/loopms)){
		count = 0;
		uartTxBuffer[0] = statsss;
		uartTxBuffer[1] = Appl_EEPParam.stm32.EEP_CustomerNumber;
		uartTxBuffer[2] = (uint8_t)Appl_EEPParam.stm32.EEP_RejuvenationCycleID;
		uartTxBuffer[3] = Appl_EEPParam.stm32.EEP_Run_Hour;
		uartTxBuffer[4] = Appl_EEPParam.stm32.EEP_Run_Min;
		uartTxBuffer[5] = Appl_EEPParam.stm32.EEP_Run_Sec;
		uartTxBuffer[6] = Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL;
		uartTxBuffer[7] = round(SM_ApplParam.RealtimeCurrent);
		uartTxBuffer[8] = TempU16>>8;
		uartTxBuffer[9] = TempU16 & 0xFF;
		uartTxBuffer[10] = Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH;
		uartTxBuffer[11] = Appl_EEPParam.transfer.extRecord.ExtEEP_StopReason;
		uartTxBuffer[12] = Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdL;
		uartTxBuffer[13] = Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdH;

		keepAliveCommunication++;
		uartTxBuffer[14] = keepAliveCommunication & 0xFF;
		uartTxBuffer[15] = (keepAliveCommunication >> 8) & 0xFF;
		uartTxBuffer[16] = (keepAliveCommunication >> 16) & 0xFF;
		uartTxBuffer[17] = (keepAliveCommunication >> 24) & 0xFF;

		uartTxBuffer[18] = Appl_EEPParam.transfer.extRecord.ExtEEP_DeviceId;
		uartTxBuffer[19] =  Appl_EEPParam.stm32.EEP_Usg_HourH;
		uartTxBuffer[20] =  Appl_EEPParam.stm32.EEP_Usg_HourL;
		uartTxBuffer[21] =  Appl_EEPParam.stm32.EEP_Usg_Min;
		uartTxBuffer[22] =  Appl_EEPParam.stm32.EEP_Usg_Sec;

		uartTxBuffer[23] =  Appl_EEPParam.stm32.EEP_TimerSetHours;

		uartTxBuffer[TOTAL_BUFFER_SIZE-2] = 0xDE;
		uartTxBuffer[TOTAL_BUFFER_SIZE-1] = 0xAD;

//		gcvt(Fnclocal_dispfloat,1,chara[0]);
		HAL_UART_Transmit(&huart5, &uartTxBuffer[0], TOTAL_BUFFER_SIZE, 100);
//		HAL_UART_Receive(&huart3, &switchOn, 1, 1);
		if(switchOn == 1){
			switchOnkeypressed = 1;
		}
	}

}

#if 0

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void vishwa_u8g2Animation(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
  }

  for(;;) { // Loop forever...
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}
#endif
#ifndef max
    #define max(a,b) ((a) > (b) ? (a) : (b))
#endif

void vishwa_u8g2drawcircleAnime(u8g2_t* u8g2) {
	static uint16_t circleradius = 0;
    u8g2_DrawCircle(u8g2,64, 32, circleradius, U8G2_DRAW_ALL);
    circleradius+=2;
}
#endif
