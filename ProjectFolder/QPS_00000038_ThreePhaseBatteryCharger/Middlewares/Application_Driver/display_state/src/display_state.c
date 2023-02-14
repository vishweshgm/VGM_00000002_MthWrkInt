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

#define TOTAL_BUFFER_SIZE 26
/*extern variables*/
extern DiagFaultTable Appl_diagActiveFault;
extern SM_MainApplParameters SM_ApplParam;
extern EEP_DataStructure Appl_EEPParam;
SM_DisplayParam Appl_Display;

float flt_movAvArray[MOVING_AVERAGE_SIZE_vArray];
float flt_movAvArraySum;
uint8_t u8_movAvArrayPos;

float flt_movAiArray[MOVING_AVERAGE_SIZE_iArray];
float flt_movAiArraySum;
uint8_t u8_movAiArrayPos;

extern UART_HandleTypeDef huart3;
/*Display Variables*/
uint8_t Fnlocal_u8dispint;
uint8_t Fnlocal_u8dispintmax;
uint8_t Fnlocal_u8dispintmin;


uint8_t uartTxBuffer[TOTAL_BUFFER_SIZE];
uint8_t switchOn = 0;
uint8_t switchOnkeypressed = 0;

#if(DGUS_DISPLAY == 1)
SM_DisplayStruct Display;
SM_DisplayStruct DisplayOld;
uint8_t uartDgusTxBuffer[DGUS_TRANSFER_BUFFER_SIZE];
uint8_t uartDgusTxBuffer2[DGUS_TRANSFER_BUFFER_SIZE];
uint8_t uartDgusRxBuffer[DGUS_RECEIVE_BUFFER_SIZE];

uint8_t uartCommandList[DGUS_TOTAL_NUM_OF_COMMANDS][DGUS_TRANSFER_BUFFER_SIZE] = DGUS_COMMAND_INIT;
#endif


extern volatile SM_ECUState_Enum SM_enumECUState;


#if (DGUS_DISPLAY == 1)

void SM_DispInit(void){
	DGUS_Reset();
}
void DisplayLoop(uint8_t loopms){
	static uint8_t timer = 0;
	static uint8_t timer2 = 0;
	static uint32_t timer_logo;
	static uint8_t startAnimationFlag = 1;
	Appl_Display.flt_dv = movingAvgFlt(&flt_movAvArray[0],&flt_movAvArraySum,u8_movAvArrayPos,MOVING_AVERAGE_SIZE_vArray,SM_ApplParam.RealtimeVoltage);
	Appl_Display.flt_di = movingAvgFlt(&flt_movAiArray[0],&flt_movAiArraySum,u8_movAiArrayPos,MOVING_AVERAGE_SIZE_iArray,SM_ApplParam.RealtimeCurrent);
	if(Appl_Display.flt_dv <5)Appl_Display.flt_dv = 0;
	if(Appl_Display.flt_di <2)Appl_Display.flt_di = 0;
	if(++u8_movAvArrayPos >= MOVING_AVERAGE_SIZE_vArray)u8_movAvArrayPos = 0;
	if(++u8_movAiArrayPos >= MOVING_AVERAGE_SIZE_iArray)u8_movAiArrayPos = 0;

	if(Display.DisplayState == SM_enum_Display_POWERON_LOGO){
		timer_logo++;
		if(timer_logo < ((2000)/loopms)){
			WritePageID(DGUS_PAGEID_SPLASHSCREEN1);
		}
		else{
			WritePageID(DGUS_PAGEID_SPLASHSCREEN2);
		}
		startAnimationFlag = 1;
	}
	if(Display.DisplayState == SM_enum_Display_MAIN_MENU){
		startAnimationFlag = 1;
		WritePageID(DGUS_PAGEID_MAINMENU);
	}
	if(Display.DisplayState == SM_enum_Display_CurrentModeSettings){
		WritePageID(DGUS_PAGEID_CURRENTMODESETTINGS);
	}
	if(Display.DisplayState == SM_enum_Display_VoltageModeSettings){
		WritePageID(DGUS_PAGEID_VOLTAGEMODESETTINGS);
	}
	if(Display.DisplayState == SM_enum_Display_CCPreparing){
		WritePageID(DGUS_PAGEID_CCPREPARING);
		if(startAnimationFlag == 1){
			startAnimationFlag = 0;
			StartAnimation();
		}
		timer++;
		if(timer>(250/loopms)){
			timer = 0;
			DGUS_UpdateWritefloatVariables();
		}
	}
	else if(Display.DisplayState == SM_enum_Display_CCRun_Slow){
		timer++;
		if(timer>(1000/loopms)){
			timer = 0;
			DGUS_UpdateWritefloatVariables();
		}

		DGUS_updateMessages();
		WritePageID(DGUS_PAGEID_CCRUNNING);
	}
	else if(Display.DisplayState == SM_enum_Display_CCRun_Stable){
		timer++;
		if(timer>(1000/loopms)){
			timer = 0;
			DGUS_UpdateWritefloatVariables();
		}

		DGUS_updateMessages();
		WritePageID(DGUS_PAGEID_CCRUNNING);
	}
	if(Display.DisplayState == SM_enum_Display_CVPreparing){
		WritePageID(DGUS_PAGEID_CVPREPARING);
		if(startAnimationFlag == 1){
			startAnimationFlag = 0;
			StartAnimation();
		}
		timer++;
		if(timer>(1000/loopms)){
			timer = 0;
			DGUS_UpdateWritefloatVariables();
		}


	}
	else if(Display.DisplayState == SM_enum_Display_CVRun_Slow){
		timer++;
		if(timer>(1000/loopms)){
			timer = 0;
			DGUS_UpdateWritefloatVariables();
		}
		DGUS_updateMessages();
		WritePageID(DGUS_PAGEID_CVRUNNING);

	}
	else if(Display.DisplayState == SM_enum_Display_CVRun_Stable){
		timer++;
		if(timer>(1000/loopms)){
			timer = 0;
			DGUS_UpdateWritefloatVariables();
			DGUS_updateMessages();
			WritePageID(DGUS_PAGEID_CVRUNNING);
		}

	}
	else if(Display.DisplayState == SM_enum_Display_SelfTest){
		DGUS_updateSelfTestText(loopms);
		DGUS_UpdateWritefloatVariables();
	}
	else if(Display.DisplayState == SM_enum_Display_CCPause){
		startAnimationFlag = 1;
		WritePageID(DGUS_PAGEID_CCPAUSED);
		DGUS_updatePauseReason();
	}
	else if(Display.DisplayState == SM_enum_Display_CVPause){
		startAnimationFlag = 1;
		WritePageID(DGUS_PAGEID_CVPAUSED);
		DGUS_updatePauseReason();
	}
	else if(Display.DisplayState == SM_enum_Display_StoppedWhileRunning){
		WritePageID(DGUS_PAGEID_RESUMINGFROMLASTPOINT);

	}
	else if(Display.DisplayState == SM_enum_Display_PoweringOffWindow){
		startAnimationFlag = 1;
		WritePageID(DGUS_PAGEID_POWERFAILURE);
	}
	if(Display.DisplayState == SM_enum_Display_Settings_Menu){
		WritePageID(DGUS_PAGEID_SETTINGS);
	}
	if(Display.DisplayState == SM_enum_Display_Settings_Menu_Completed){
		DGUS_UpdateWritefloatVariables();
	}
	if(Display.DisplayState == SM_enum_Display_Settings_Menu_Erase){

	}
	if(Display.DisplayState == SM_enum_Display_VishwaTest){
		WritePageID(DGUS_PAGEID_VISHWAMODE);
	}
	else if(Display.DisplayState == SM_enum_Display_Info){


	}

	timer2++;
	if(timer2>(750/loopms)){
		timer2 = 0;
		DGUS_UpdateWriteVariables();
	}


}

DGUS_ERROR WritePageID(uint16_t PageId){
//	static uint16_t page_old;

	memset(&uartDgusTxBuffer[0],0,sizeof(uartDgusTxBuffer));
	memset(&uartDgusRxBuffer[0],0,sizeof(uartDgusRxBuffer));

	memcpy(&uartDgusTxBuffer[0], &uartCommandList[0][0],DGUS_TRANSFER_BUFFER_SIZE);

	uartDgusTxBuffer[8] = (uint8_t)(PageId>>8);
	uartDgusTxBuffer[9] = (uint8_t)(PageId & 0x00FF);

	if(Display.DGUSParam.PageId != PageId){
		if(HAL_OK !=HAL_UART_Transmit(&huart3, &uartDgusTxBuffer[0], 10, 10)) return DGUS_ERR;
		else return DGUS_OK;
	}
	else{
		return DGUS_OK;
	}

}



DGUS_ERROR ReadPageID(void){
	uint8_t offset = 0xFF;
	uint16_t RxLen;
	memset(&uartDgusTxBuffer[0],0,sizeof(uartDgusTxBuffer));
	memset(&uartDgusRxBuffer[0],0,sizeof(uartDgusRxBuffer));

	memcpy(&uartDgusTxBuffer[0], &uartCommandList[2][0],DGUS_TRANSFER_BUFFER_SIZE);

	if(HAL_OK != HAL_UART_Transmit(&huart3, &uartDgusTxBuffer[0], 7, 10)){
		return DGUS_ERR;
	}
	else{
		//HAL_UART_Receive
		if(HAL_OK != HAL_UARTEx_ReceiveToIdle(&huart3, &uartDgusRxBuffer[0], 9+2,&RxLen, 10)){
			return DGUS_ERR;
		}
		else{
			if(RxLen == 9)offset = 0;
			if(RxLen == 10)offset = 1;
			if(RxLen == 11)offset = 2;
//			for(uint8_t i =0;i<=2;i++){if(uartDgusRxBuffer[i] == 0x5A && uartDgusRxBuffer[i+1] == 0xA5) offset = i;}
			if(offset != 0xFF){
				Display.DGUSParam.PageId = (uint16_t)((uartDgusRxBuffer[offset+7]<<8)| uartDgusRxBuffer[offset+8]);
			}
			return DGUS_OK;
		}
	}
}

DGUS_ERROR DGUS_UpdateWriteVariables(void){
	uint16_t tempu16 = 0;

	memset(&uartDgusTxBuffer[0],0,sizeof(uartDgusTxBuffer));
	memset(&uartDgusRxBuffer[0],0,sizeof(uartDgusRxBuffer));
	memcpy(&uartDgusTxBuffer[0], &uartCommandList[4][0],DGUS_TRANSFER_BUFFER_SIZE);

	/*1-5 0x1000 to 0x1004*/
	uartDgusTxBuffer[6] = 0x00;
	uartDgusTxBuffer[7] = Appl_EEPParam.stm32.EEP_CustomerNumber;

	uartDgusTxBuffer[8] = 0x00;
	uartDgusTxBuffer[9] = Appl_EEPParam.stm32.EEP_ModeSelected;

	tempu16 = (uint16_t) Appl_EEPParam.stm32.EEP_CC_CutOffVoltage;
	uartDgusTxBuffer[10] = tempu16>>8;
	uartDgusTxBuffer[11] = tempu16;

	uartDgusTxBuffer[12] = 0x00;
	uartDgusTxBuffer[13] = (uint8_t)Appl_EEPParam.stm32.EEP_CC_SetCurrent;

	uartDgusTxBuffer[14] = 0x00;
	uartDgusTxBuffer[15] = Appl_EEPParam.stm32.EEP_TimerSetHours;

	/*6-10 0x1005 to 0x1009*/
	uartDgusTxBuffer[16] = 0x00;
	uartDgusTxBuffer[17] = (uint8_t)Appl_Display.PowerOnCounter;

	uartDgusTxBuffer[18] = (0x00);
	uartDgusTxBuffer[19] = (uint8_t)Appl_Display.PowerOffCounter;;

	uartDgusTxBuffer[20] = 0x00;
	uartDgusTxBuffer[21] =  Appl_EEPParam.stm32.EEP_Run_Hour;

	uartDgusTxBuffer[22] = 0x00;
	uartDgusTxBuffer[23] =  Appl_EEPParam.stm32.EEP_Run_Min;

	uartDgusTxBuffer[24] = 0x00;
	uartDgusTxBuffer[25] =  Appl_EEPParam.stm32.EEP_Run_Sec;

	/*11-16 0x100A to 0x100F*/
	uartDgusTxBuffer[26] = Appl_EEPParam.stm32.EEP_Usg_HourH;
	uartDgusTxBuffer[27] = Appl_EEPParam.stm32.EEP_Usg_HourL;

	uartDgusTxBuffer[28] = (0x00);
	uartDgusTxBuffer[29] = (uint8_t)Appl_EEPParam.stm32.EEP_Usg_Min;

	uartDgusTxBuffer[30] = 0x00;
	uartDgusTxBuffer[31] =  Appl_EEPParam.stm32.EEP_Usg_Sec;

	uartDgusTxBuffer[32] = 0x00;
	uartDgusTxBuffer[33] = Appl_EEPParam.stm32.EEP_CV_SetVoltage;

	uartDgusTxBuffer[34] = 0x00;
	uartDgusTxBuffer[35] = Appl_EEPParam.stm32.EEP_CV_CutOffCurrent;

	uartDgusTxBuffer[36] = (uint8_t)SM_ApplParam.ExtEepTotalRecordsTotransferred >> 8;
	uartDgusTxBuffer[37] = (uint8_t)SM_ApplParam.ExtEepTotalRecordsTotransferred;;

	uartDgusTxBuffer[2] = 2*16+3;

	if(HAL_OK != HAL_UART_Transmit(&huart3, &uartDgusTxBuffer[0], (3+uartDgusTxBuffer[2]), 10)){
		return DGUS_ERR;
	}
	else{
		return DGUS_OK;
	}
}

DGUS_ERROR DGUS_UpdateWritefloatVariables(void){
	float batteryChargePercent = 0;
	unionfloatData unionfloattemp;
	memset(&uartDgusTxBuffer[0],0,sizeof(uartDgusTxBuffer));
	memset(&uartDgusRxBuffer[0],0,sizeof(uartDgusRxBuffer));
	memcpy(&uartDgusTxBuffer[0], &uartCommandList[6][0],DGUS_TRANSFER_BUFFER_SIZE);

	/*1050-1051*/
	unionfloattemp.f = Appl_Display.flt_dv;
	uartDgusTxBuffer[6] = unionfloattemp.c[3];
	uartDgusTxBuffer[7] = unionfloattemp.c[2];
	uartDgusTxBuffer[8] = unionfloattemp.c[1];
	uartDgusTxBuffer[9] = unionfloattemp.c[0];

	/*1052-1053*/
	unionfloattemp.f = Appl_Display.flt_di;
	uartDgusTxBuffer[10] = unionfloattemp.c[3];
	uartDgusTxBuffer[11] = unionfloattemp.c[2];
	uartDgusTxBuffer[12] = unionfloattemp.c[1];
	uartDgusTxBuffer[13] = unionfloattemp.c[0];

	/*1054-1055*/

	unionfloattemp.f = SM_ApplParam.ap_fltDataPercent;
	uartDgusTxBuffer[14] = unionfloattemp.c[3];
	uartDgusTxBuffer[15] = unionfloattemp.c[2];
	uartDgusTxBuffer[16] = unionfloattemp.c[1];
	uartDgusTxBuffer[17] = unionfloattemp.c[0];

	/*1056-1057*/

	unionfloattemp.f = SM_ApplParam.PreparePercentage;
	uartDgusTxBuffer[18] = unionfloattemp.c[3];
	uartDgusTxBuffer[19] = unionfloattemp.c[2];
	uartDgusTxBuffer[20] = unionfloattemp.c[1];
	uartDgusTxBuffer[21] = unionfloattemp.c[0];

	uartDgusTxBuffer[2] = 4*4+3;
	if(HAL_OK != HAL_UART_Transmit(&huart3, &uartDgusTxBuffer[0], (3+uartDgusTxBuffer[2]), 10)){
		return DGUS_ERR;
	}
	else{
		return DGUS_OK;
	}

}
DGUS_ERROR DGUS_updatePauseReason(void){
	uint8_t stringlength = 0;
	uint8_t stringlength2 = 0;

	memset(&uartDgusTxBuffer[0],0,sizeof(uartDgusTxBuffer));
	memset(&uartDgusRxBuffer[0],0,sizeof(uartDgusRxBuffer));
	memcpy(&uartDgusTxBuffer[0], &uartCommandList[5][0],DGUS_TRANSFER_BUFFER_SIZE);
	memcpy(&uartDgusTxBuffer2[0], &uartCommandList[5][0],DGUS_TRANSFER_BUFFER_SIZE);

	uartDgusTxBuffer[4] = 0x11;
	uartDgusTxBuffer[5] = 0xA0;

	uartDgusTxBuffer2[4] = 0x12;
	uartDgusTxBuffer2[5] = 0x40;

			if(SM_ApplParam.Pausereason == PauseReasonEnum_TimerComplete){
				strcpy((char*)&uartDgusTxBuffer[6],"TIMER COMPLETED");
				strcpy((char*)&uartDgusTxBuffer2[6],"NO FAULT");
				stringlength = 15;
				stringlength2 = 8;
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_UserPaused){
				strcpy((char*)&uartDgusTxBuffer[6],"USER PAUSED");
				strcpy((char*)&uartDgusTxBuffer2[6],"NO FAULT");
				stringlength = 11;
				stringlength2 = 8;
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_BatteryReversal){
				if(Appl_diagActiveFault.BatteryReversal == 1) {
					strcpy((char*)&uartDgusTxBuffer[6],"CHK BAT CONNCTION");
					strcpy((char*)&uartDgusTxBuffer2[6],"Active");
					stringlength = 17;
					stringlength2 = 6;
				}
				else {
					strcpy((char*)&uartDgusTxBuffer[6],"CHK BAT CONNCTION");
					strcpy((char*)&uartDgusTxBuffer2[6],"Healed");
					stringlength = 17;
					stringlength2 = 6;
				}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_EEPLogMemoryFull){
				if(Appl_diagActiveFault.EEPLogMemoryFull == 1) {
					strcpy((char*)&uartDgusTxBuffer[6],"LOG MEMORY FULL");
					stringlength = 15;
					strcpy((char*)&uartDgusTxBuffer2[6],"Active");
					stringlength2 = 6;
				}
				else {
					strcpy((char*)&uartDgusTxBuffer[6],"LOG MEMORY FULL");
					stringlength = 15;
					strcpy((char*)&uartDgusTxBuffer2[6],"Healed");
					stringlength2 = 6;

				}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_UnderVoltage){
				if(Appl_diagActiveFault.UnderVoltage == 1) {
					strcpy((char*)&uartDgusTxBuffer[6],"LOW VOLTAGE");
					stringlength = 11;
					strcpy((char*)&uartDgusTxBuffer2[6],"Active");
					stringlength2 = 6;
				}
				else {
					strcpy((char*)&uartDgusTxBuffer[6],"LOW VOLTAGE");
					stringlength = 11;
					strcpy((char*)&uartDgusTxBuffer2[6],"Healed");
					stringlength2 = 6;

				}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_OverCurrent){
				if(Appl_diagActiveFault.OverCurrent == 1) {
					strcpy((char*)&uartDgusTxBuffer[6],"OVER CURRENT");
					stringlength = 12;
					strcpy((char*)&uartDgusTxBuffer2[6],"Active");
					stringlength2 = 6;
				}
				else {
					strcpy((char*)&uartDgusTxBuffer[6],"OVER CURRENT");
					stringlength = 12;
					strcpy((char*)&uartDgusTxBuffer2[6],"Healed");
					stringlength2 = 6;
				}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_FilterCircuitOpen){
				if(Appl_diagActiveFault.FiltercktOpen == 1) {
					strcpy((char*)&uartDgusTxBuffer[6],"ACCUBANK CONECTIN");
					stringlength = 17;
					strcpy((char*)&uartDgusTxBuffer2[6],"Active");
					stringlength2 = 6;
				}
				else {
					strcpy((char*)&uartDgusTxBuffer[6],"ACCUBANK CONECTIN");
					stringlength = 17;
					strcpy((char*)&uartDgusTxBuffer2[6],"Healed");
					stringlength2 = 6;
				}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_OverVoltage){
				if(Appl_diagActiveFault.OverVoltage == 1) {
					strcpy((char*)&uartDgusTxBuffer[6],"OVER VOLTAGE");
					stringlength = 12;
					strcpy((char*)&uartDgusTxBuffer2[6],"Active");
					stringlength2 = 6;
				}
				else {
					strcpy((char*)&uartDgusTxBuffer[6],"OVER VOLTAGE");
					stringlength = 12;
					strcpy((char*)&uartDgusTxBuffer2[6],"Healed");
					stringlength2 = 6;
				}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_HighTemp){
				if(Appl_diagActiveFault.HighTempperatureSCR == 1) {
					strcpy((char*)&uartDgusTxBuffer[6],"HIGH TEMPERATURE");
					stringlength = 16;
					strcpy((char*)&uartDgusTxBuffer2[6],"Active");
					stringlength2 = 6;
				}
				else {
					strcpy((char*)&uartDgusTxBuffer[6],"HIGH TEMPERATURE");
					stringlength = 16;
					strcpy((char*)&uartDgusTxBuffer2[6],"Healed");
					stringlength2 = 6;
				}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_PhaseReversal){
				if(Appl_diagActiveFault.PhaseReversal == 1) {
					strcpy((char*)&uartDgusTxBuffer[6],"REVERSE PHASE");
					stringlength = 13;
					strcpy((char*)&uartDgusTxBuffer2[6],"Active");
					stringlength2 = 6;
				}
				else {
					strcpy((char*)&uartDgusTxBuffer[6],"REVERSE PHASE");
					stringlength = 13;
					strcpy((char*)&uartDgusTxBuffer2[6],"Healed");
					stringlength2 = 6;
				}
			}
			else if(SM_ApplParam.Pausereason == PauseReasonEnum_PhaseReversal){
				if(Appl_diagActiveFault.PhaseReversal == 1) {
					strcpy((char*)&uartDgusTxBuffer[6],"REVERSE PHASE");
					stringlength = 13;
					strcpy((char*)&uartDgusTxBuffer2[6],"Active");
					stringlength2 = 6;
				}
				else {
					strcpy((char*)&uartDgusTxBuffer[6],"REVERSE PHASE");
					stringlength = 13;
					strcpy((char*)&uartDgusTxBuffer2[6],"Healed");
					stringlength2 = 6;
				}
			}



			uartDgusTxBuffer[stringlength+6] = 0xFF;
			uartDgusTxBuffer[stringlength+7] = 0xFF;
			uartDgusTxBuffer[2] = 3+stringlength+2;

			uartDgusTxBuffer2[stringlength2+6] = 0xFF;
			uartDgusTxBuffer2[stringlength2+7] = 0xFF;
			uartDgusTxBuffer2[2] = 3+stringlength2+2;

			HAL_UART_Transmit(&huart3, &uartDgusTxBuffer2[0], (3+uartDgusTxBuffer2[2]), 10);
			if(HAL_OK != HAL_UART_Transmit(&huart3, &uartDgusTxBuffer[0], (3+uartDgusTxBuffer[2]), 10)){
				return DGUS_ERR;
			}
			else{
				return DGUS_OK;
			}

}
DGUS_ERROR DGUS_updateSelfTestText(uint16_t loopms){

	uint8_t stringlength = 0;


	memset(&uartDgusTxBuffer[0],0,sizeof(uartDgusTxBuffer));
	memcpy(&uartDgusTxBuffer[0], &uartCommandList[5][0],DGUS_TRANSFER_BUFFER_SIZE);

	Appl_Display.SelfTestCounter++;

	/*
	 * 11C0 - InputTerminalCheck
	 * 11D0 - AccuBankCheck
	 * 11E0 - OutputTerminal
	 * 11F0 - OverVoltage
	 * 1200 - UnderVoltage
	 * 1210 - PhaseCheck
	 * 1220 - TemperatureCheck
	 * */

	if(Appl_Display.SelfTestCounter > (1000/loopms)){
		if(Appl_diagActiveFault.PowerFail_Z1 == 0 &&\
				Appl_diagActiveFault.PowerFail_Z2 == 0 && \
				Appl_diagActiveFault.PowerFail_Z3 == 0){
			uartDgusTxBuffer[4] = 0x11;
			uartDgusTxBuffer[5] = 0xC0;
			strcpy((char*)&uartDgusTxBuffer[6],"OK");
			stringlength = 2;
			uartDgusTxBuffer[stringlength+6] = 0xFF;
			uartDgusTxBuffer[stringlength+7] = 0xFF;
			uartDgusTxBuffer[2] = 3+stringlength+2;
			SENDTEXT();
			if(Appl_Display.SelfTestCounter > (2000/loopms)){
				if(Appl_diagActiveFault.FiltercktOpen == 0){
					uartDgusTxBuffer[4] = 0x11;
					uartDgusTxBuffer[5] = 0xD0;
					strcpy((char*)&uartDgusTxBuffer[6],"OK");
					stringlength = 2;
					uartDgusTxBuffer[stringlength+6] = 0xFF;
					uartDgusTxBuffer[stringlength+7] = 0xFF;
					uartDgusTxBuffer[2] = 3+stringlength+2;
					SENDTEXT();
					if(Appl_Display.SelfTestCounter > (3000/loopms)){
						if(Appl_diagActiveFault.BatteryReversal == 0){
							uartDgusTxBuffer[4] = 0x11;
							uartDgusTxBuffer[5] = 0xE0;
							strcpy((char*)&uartDgusTxBuffer[6],"OK");
							stringlength = 2;
							uartDgusTxBuffer[stringlength+6] = 0xFF;
							uartDgusTxBuffer[stringlength+7] = 0xFF;
							uartDgusTxBuffer[2] = 3+stringlength+2;
							SENDTEXT();
							if(Appl_Display.SelfTestCounter > (4000/loopms)){
								if(Appl_diagActiveFault.OverVoltage == 0){
									uartDgusTxBuffer[4] = 0x11;
									uartDgusTxBuffer[5] = 0xF0;
									strcpy((char*)&uartDgusTxBuffer[6],"OK");
									stringlength = 2;
									uartDgusTxBuffer[stringlength+6] = 0xFF;
									uartDgusTxBuffer[stringlength+7] = 0xFF;
									uartDgusTxBuffer[2] = 3+stringlength+2;
									SENDTEXT();
									if(Appl_Display.SelfTestCounter > (5000/loopms)){
										if(Appl_diagActiveFault.UnderVoltage == 0){
											uartDgusTxBuffer[4] = 0x12;
											uartDgusTxBuffer[5] = 0x00;
											strcpy((char*)&uartDgusTxBuffer[6],"OK");
											stringlength = 2;
											uartDgusTxBuffer[stringlength+6] = 0xFF;
											uartDgusTxBuffer[stringlength+7] = 0xFF;
											uartDgusTxBuffer[2] = 3+stringlength+2;
											SENDTEXT();
											if(Appl_Display.SelfTestCounter > (6000/loopms)){
												if(Appl_diagActiveFault.PhaseReversal == 0){
													uartDgusTxBuffer[4] = 0x12;
													uartDgusTxBuffer[5] = 0x10;
													strcpy((char*)&uartDgusTxBuffer[6],"OK");
													stringlength = 2;
													uartDgusTxBuffer[stringlength+6] = 0xFF;
													uartDgusTxBuffer[stringlength+7] = 0xFF;
													uartDgusTxBuffer[2] = 3+stringlength+2;
													SENDTEXT();
													if(Appl_Display.SelfTestCounter > (7000/loopms)){
														if(Appl_diagActiveFault.HighTempperatureSCR == 0){
															uartDgusTxBuffer[4] = 0x12;
															uartDgusTxBuffer[5] = 0x20;
															strcpy((char*)&uartDgusTxBuffer[6],"OK");
															stringlength = 2;
															uartDgusTxBuffer[stringlength+6] = 0xFF;
															uartDgusTxBuffer[stringlength+7] = 0xFF;
															uartDgusTxBuffer[2] = 3+stringlength+2;
															SENDTEXT();

														}
														else{
															uartDgusTxBuffer[4] = 0x12;
															uartDgusTxBuffer[5] = 0x20;
															strcpy((char*)&uartDgusTxBuffer[6],"ERR");
															stringlength = 3;
															uartDgusTxBuffer[stringlength+6] = 0xFF;
															uartDgusTxBuffer[stringlength+7] = 0xFF;
															uartDgusTxBuffer[2] = 3+stringlength+2;
															SENDTEXT();
															Appl_Display.SelfTestCounter--;
														}
													}

												}
												else{
													uartDgusTxBuffer[4] = 0x12;
													uartDgusTxBuffer[5] = 0x10;
													strcpy((char*)&uartDgusTxBuffer[6],"ERR");
													stringlength = 3;
													uartDgusTxBuffer[stringlength+6] = 0xFF;
													uartDgusTxBuffer[stringlength+7] = 0xFF;
													uartDgusTxBuffer[2] = 3+stringlength+2;
													SENDTEXT();
													Appl_Display.SelfTestCounter--;
												}
											}
										}
										else{
											uartDgusTxBuffer[4] = 0x12;
											uartDgusTxBuffer[5] = 0x00;
											strcpy((char*)&uartDgusTxBuffer[6],"ERR");
											stringlength = 3;
											uartDgusTxBuffer[stringlength+6] = 0xFF;
											uartDgusTxBuffer[stringlength+7] = 0xFF;
											uartDgusTxBuffer[2] = 3+stringlength+2;
											SENDTEXT();
											Appl_Display.SelfTestCounter--;
										}
									}

								}
								else{
									uartDgusTxBuffer[4] = 0x11;
									uartDgusTxBuffer[5] = 0xF0;
									strcpy((char*)&uartDgusTxBuffer[6],"ERR");
									stringlength = 3;
									uartDgusTxBuffer[stringlength+6] = 0xFF;
									uartDgusTxBuffer[stringlength+7] = 0xFF;
									uartDgusTxBuffer[2] = 3+stringlength+2;
									SENDTEXT();
									Appl_Display.SelfTestCounter--;
								}
							}

						}
						else{
							uartDgusTxBuffer[4] = 0x11;
							uartDgusTxBuffer[5] = 0xE0;
							strcpy((char*)&uartDgusTxBuffer[6],"ERR");
							stringlength = 3;
							uartDgusTxBuffer[stringlength+6] = 0xFF;
							uartDgusTxBuffer[stringlength+7] = 0xFF;
							uartDgusTxBuffer[2] = 3+stringlength+2;
							SENDTEXT();
							Appl_Display.SelfTestCounter--;
						}
					}
				}
				else{
					uartDgusTxBuffer[4] = 0x11;
					uartDgusTxBuffer[5] = 0xD0;
					strcpy((char*)&uartDgusTxBuffer[6],"ERR");
					stringlength = 3;
					uartDgusTxBuffer[stringlength+6] = 0xFF;
					uartDgusTxBuffer[stringlength+7] = 0xFF;
					uartDgusTxBuffer[2] = 3+stringlength+2;
					SENDTEXT();
					Appl_Display.SelfTestCounter--;
				}
			}

		}
		else{
			uartDgusTxBuffer[4] = 0x11;
			uartDgusTxBuffer[5] = 0xC0;
			strcpy((char*)&uartDgusTxBuffer[6],"ERR");
			stringlength = 3;
			uartDgusTxBuffer[stringlength+6] = 0xFF;
			uartDgusTxBuffer[stringlength+7] = 0xFF;
			uartDgusTxBuffer[2] = 3+stringlength+2;
			SENDTEXT();
			Appl_Display.SelfTestCounter--;
		}
	}
	else{
		for(uint16_t i = 0x11C0;i<=0x1220;i+=0x10){
			uartDgusTxBuffer[4] = (i>>8);
			uartDgusTxBuffer[5] = (i&(0x00FF));
			strcpy((char*)&uartDgusTxBuffer[6],"...");
			stringlength = 3;
			uartDgusTxBuffer[stringlength+6] = 0xFF;
			uartDgusTxBuffer[stringlength+7] = 0xFF;
			uartDgusTxBuffer[2] = 3+stringlength+2;
			SENDTEXT();
		}

	}
}

DGUS_ERROR DGUS_updateMessages(void){
#if 0
	memset(&uartDgusTxBuffer[0],0,sizeof(uartDgusTxBuffer));
	memset(&uartDgusRxBuffer[0],0,sizeof(uartDgusRxBuffer));
	memcpy(&uartDgusTxBuffer[0], &uartCommandList[5][0],DGUS_TRANSFER_BUFFER_SIZE);
	uartDgusTxBuffer[4] = 0x10;
	uartDgusTxBuffer[5] = 0x80;

	if(Appl_diagActiveFault.HighTempperatureSCR == 1) {
		strcpy((char*)&uartDgusTxBuffer[6],"TEMPERATURE>65DegC");
		uartDgusTxBuffer[18+6] = 0xFF;
		uartDgusTxBuffer[18+7] = 0xFF;
		uartDgusTxBuffer[2] = 3+18+2;
	}
	else if(Appl_diagActiveFault.PhaseReversal == 1){
		strcpy((char*)&uartDgusTxBuffer[6],"PHASE  IS REVERSED");
		uartDgusTxBuffer[18+6] = 0xFF;
		uartDgusTxBuffer[18+7] = 0xFF;
		uartDgusTxBuffer[2] = 3+18+2;
	}
	else if(Appl_diagActiveFault.CVcutOffCurrent == 1){
		strcpy((char*)&uartDgusTxBuffer[6],"LIMIT CURRENT");
		uartDgusTxBuffer[9+6] = 0xFF;
		uartDgusTxBuffer[9] = 0xFF;
		uartDgusTxBuffer[2] = 3+13+2;
	}
	else if(Appl_diagActiveFault.CCcutOffVoltage == 1){
		strcpy((char*)&uartDgusTxBuffer[6],"LIMIT VOLTAGE");
		uartDgusTxBuffer[9+6] = 0xFF;
		uartDgusTxBuffer[9+7] = 0xFF;
		uartDgusTxBuffer[2] = 3+13+2;
	}
	else{
		strcpy((char*)&uartDgusTxBuffer[6],"ALL OK");
		uartDgusTxBuffer[11+6] = 0xFF;
		uartDgusTxBuffer[11] = 0xFF;
		uartDgusTxBuffer[2] = 3+6+2;
	}
	if(HAL_OK != HAL_UART_Transmit(&huart3, &uartDgusTxBuffer[0], (3+uartDgusTxBuffer[2]), 10)){
		return DGUS_ERR;
	}
	else{
		return DGUS_OK;
	}
#endif
}

DGUS_ERROR UpdateReadVariables(void){
	uint8_t numofvariablestoread = 0;

	/*0x1000 to 0x100E*/
	numofvariablestoread = 15;

	memset(&uartDgusTxBuffer[0],0,sizeof(uartDgusTxBuffer));
	memset(&uartDgusRxBuffer[0],0,sizeof(uartDgusRxBuffer));

	memcpy(&uartDgusTxBuffer[0], &uartCommandList[1][0],DGUS_TRANSFER_BUFFER_SIZE);

	uartDgusTxBuffer[6] = numofvariablestoread;

	if(HAL_OK != HAL_UART_Transmit(&huart3, &uartDgusTxBuffer[0], 7, 10)){
		return DGUS_ERR;
	}
	else{
		//
		if(HAL_OK != HAL_UART_Receive(&huart3, &uartDgusRxBuffer[0], ((numofvariablestoread*2)+7), 20)){
			return DGUS_ERR;
		}
		else{
			Display.DGUSParam.CustomerID = (uint16_t)((uartDgusRxBuffer[7]<<8)| uartDgusRxBuffer[8]);/*1000*/
			Display.DGUSParam.unused1 = (uint16_t)((uartDgusRxBuffer[9]<<8)| uartDgusRxBuffer[10]);/*1001*/
			Display.DGUSParam.cutOffVoltage = (uint16_t)((uartDgusRxBuffer[11]<<8)| uartDgusRxBuffer[12]);/*1002*/
			Display.DGUSParam.setCurrent = (uint16_t)((uartDgusRxBuffer[13]<<8)| uartDgusRxBuffer[14]);/*1003*/
			Display.DGUSParam.setTimerHour = (uint16_t)((uartDgusRxBuffer[15]<<8)| uartDgusRxBuffer[16]);/*1004*/
//			Display.DGUSParam.setTimerHour = (uint16_t)((uartDgusRxBuffer[17]<<8)| uartDgusRxBuffer[18]);/*1005*/
//			Display.DGUSParam.setTimerHour = (uint16_t)((uartDgusRxBuffer[19]<<8)| uartDgusRxBuffer[20]);/*1006*/
//			Display.DGUSParam.setTimerHour = (uint16_t)((uartDgusRxBuffer[21]<<8)| uartDgusRxBuffer[22]);/*1007*/
//			Display.DGUSParam.setTimerHour = (uint16_t)((uartDgusRxBuffer[23]<<8)| uartDgusRxBuffer[24]);/*1008*/
//			Display.DGUSParam.setTimerHour = (uint16_t)((uartDgusRxBuffer[25]<<8)| uartDgusRxBuffer[26]);/*1009*/
//			Display.DGUSParam.setTimerHour = (uint16_t)((uartDgusRxBuffer[27]<<8)| uartDgusRxBuffer[28]);/*100A*/
//			Display.DGUSParam.setTimerHour = (uint16_t)((uartDgusRxBuffer[29]<<8)| uartDgusRxBuffer[30]);/*100B*/
//			Display.DGUSParam.setTimerHour = (uint16_t)((uartDgusRxBuffer[31]<<8)| uartDgusRxBuffer[32]);/*100C*/
			Display.DGUSParam.setVoltage = (uint16_t)((uartDgusRxBuffer[33]<<8)| uartDgusRxBuffer[34]);/*100D*/
			Display.DGUSParam.cutOffCurrent = (uint16_t)((uartDgusRxBuffer[35]<<8)| uartDgusRxBuffer[36]);/*100E*/
			return DGUS_OK;
		}
	}
}

DGUS_ERROR DGUS_Reset(void){
	memset(&uartDgusTxBuffer[0],0,sizeof(uartDgusTxBuffer));
	memset(&uartDgusRxBuffer[0],0,sizeof(uartDgusRxBuffer));
	memcpy(&uartDgusTxBuffer[0], &uartCommandList[3][0],DGUS_TRANSFER_BUFFER_SIZE);
	if(HAL_OK != HAL_UART_Transmit(&huart3, &uartDgusTxBuffer[0], 10, 10)){
		return DGUS_ERR;
	}
	else{
		return DGUS_OK;
	}
}
#endif



DGUS_ERROR StartAnimation(void){

	memset(&uartDgusTxBuffer[0],0,sizeof(uartDgusTxBuffer));
	memset(&uartDgusRxBuffer[0],0,sizeof(uartDgusRxBuffer));

	memcpy(&uartDgusTxBuffer[0], &uartCommandList[7][0],DGUS_TRANSFER_BUFFER_SIZE);

	uartDgusTxBuffer[4] = 0x14;
	uartDgusTxBuffer[5] = 0x00;

	if(HAL_OK !=HAL_UART_Transmit(&huart3, &uartDgusTxBuffer[0], 8, 10)) return DGUS_ERR;
	else return DGUS_OK;
}

float movingAvgFlt(float *ptrArrNumbers, float *ptrSum, uint8_t pos, uint16_t len, float nextNum)
{
  //Subtract the oldest number from the prev sum, add the new number
  *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
  //Assign the nextNum to the position in the array
  ptrArrNumbers[pos] = nextNum;
  //return the average
  return *ptrSum / len;
}

#endif
