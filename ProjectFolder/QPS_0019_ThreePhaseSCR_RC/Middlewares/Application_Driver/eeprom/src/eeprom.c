/*
 * eeprom.c
 *
 *  Created on: Feb 23, 2020
 *      Author: vishweshgm
 */
#define EEPROM_USED 1
#if EEPROM_USED

#include "stateMachine.h"
#include "appl_diag.h"
#include "eeprom.h"
#include "eeprom_external.h"
void MapVishwastructtoEEP(void);
void MapStEEPtoVishwastruct(void);

/*extern variables*/
extern SM_MainApplParameters SM_ApplParam;
extern DiagFaultTable Appl_diagActiveFault;
static uint32_t EEP_ErrorCounter;
ExtEEP_Error EEP_Error;

extern I2C_HandleTypeDef hi2c1;
/*STM32 EEP*/
/*EEP*/
/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NB_OF_VAR];
uint16_t VarDataTab[NB_OF_VAR];
FLASH_Status Appl_enumStatusAPIEEP;
uint16_t Appl_u16EEPError;

EEP_DataStructure Appl_EEPParam = {1};
EEP_DataStructure Appl_EEPParam_Mirror = {0}; /* defined in-equal purposefully*/

void vishwaEEp_Init(void){
	   /* Unlock the Flash Program Erase controller */
	HAL_FLASH_Unlock();
	Appl_enumStatusAPIEEP = EE_Init();
	  if(Appl_enumStatusAPIEEP != FLASH_COMPLETE){
		  Appl_u16EEPError++;
	  }
}
void Update_Usage_hour_eep(void){

	Appl_EEPParam.stm32.EEP_Usg_Sec++;
	if(Appl_EEPParam.stm32.EEP_Usg_Sec>=60){
		Appl_EEPParam.stm32.EEP_Usg_Sec = 0;
		Appl_EEPParam.stm32.EEP_Usg_Min++;

	}

	if(Appl_EEPParam.stm32.EEP_Usg_Min >= 60){
		Appl_EEPParam.stm32.EEP_Usg_Min = 0;
		if(Appl_EEPParam.stm32.EEP_Usg_HourL >= 0xFF){
			Appl_EEPParam.stm32.EEP_Usg_HourL = 0;
			Appl_EEPParam.stm32.EEP_Usg_HourH++;
		}
		else{
			Appl_EEPParam.stm32.EEP_Usg_HourL++;
		}
	}
}

void Update_RunningTime_eep(void){

	Appl_EEPParam.stm32.EEP_Run_Sec++;
	if(Appl_EEPParam.stm32.EEP_Run_Sec>=60){
		Appl_EEPParam.stm32.EEP_Run_Sec = 0;
		Appl_EEPParam.stm32.EEP_Run_Min++;
		WriteEEPData();
	}

	if(Appl_EEPParam.stm32.EEP_Run_Min >= 60){
		Appl_EEPParam.stm32.EEP_Run_Min = 0;
		if(Appl_EEPParam.stm32.EEP_Run_Hour >= 0xFF){
			Appl_EEPParam.stm32.EEP_Run_Hour = 0;
		}
		else{
			Appl_EEPParam.stm32.EEP_Run_Hour++;
			CaptureExtEEPSnapshot_SingleShot(STOP_RECORD_FLAG,PauseReasonEnum_1HrLogTimeSave);
			WriteEEPData();
			CaptureExtEEPSnapshot_SingleShot(START_RECORD_FLAG,0);
		}
	}
}

void WriteEEPData(void){

	MapStEEPtoVishwastruct();
	if(memcmp(&Appl_EEPParam_Mirror.stm32, &Appl_EEPParam.stm32, sizeof(Internal_EEP_DataStructure)) != 0){
		memcpy(&Appl_EEPParam_Mirror.stm32, &Appl_EEPParam.stm32, sizeof(Internal_EEP_DataStructure));
		for(int i=0; i<NB_OF_VAR; i++){
			Appl_enumStatusAPIEEP = EE_WriteVariable(VirtAddVarTab[i], VarDataTab[i]);
			if(Appl_enumStatusAPIEEP != FLASH_COMPLETE)Appl_u16EEPError++;
		}
	}
}

void MapStEEPtoVishwastruct(void){
	uint16_t TempU16;

	for(int i=0; i<NB_OF_VAR ; i++){
		VirtAddVarTab[i] = i+1;
	}

	Appl_EEPParam.stm32.EEP_UserSetTargetCurrent = (uint8_t)SM_ApplParam.UserSetCurrent;
	Appl_EEPParam.stm32.EEP_SetOverCurrentThreshold =(uint8_t) SM_ApplParam.OverCurrentThreshold;
	TempU16 = (uint16_t)SM_ApplParam.OverVoltageThreshold;
	Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdH = TempU16>>8;
	Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdL = TempU16 & 0xFF;
	Appl_EEPParam.stm32.EEP_SetUnderVoltageThreshold = (uint8_t)SM_ApplParam.UnderVoltageThreshold;

	VarDataTab[0] = Appl_EEPParam.stm32.EEP_Usg_Sec;
	VarDataTab[1] = Appl_EEPParam.stm32.EEP_Usg_Min;
	VarDataTab[2] = Appl_EEPParam.stm32.EEP_Usg_HourL;
	VarDataTab[3] = Appl_EEPParam.stm32.EEP_Usg_HourH;

	VarDataTab[4] = Appl_EEPParam.stm32.EEP_UserSetTargetCurrent;
	VarDataTab[5] = Appl_EEPParam.stm32.EEP_SetOverCurrentThreshold ;

	VarDataTab[6] = Appl_EEPParam.stm32.EEP_Run_Sec;
	VarDataTab[7] = Appl_EEPParam.stm32.EEP_Run_Min;
	VarDataTab[8] = Appl_EEPParam.stm32.EEP_Run_Hour;

	VarDataTab[9] = Appl_EEPParam.stm32.EEP_PowerFailedLastTime;
	VarDataTab[10] = Appl_EEPParam.stm32.EEP_TimerSetHours;
	VarDataTab[11] = Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdH;

	VarDataTab[12] = Appl_EEPParam.stm32.EEP_SetUnderVoltageThreshold;

	VarDataTab[13] = Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH;
	VarDataTab[14] = Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL;
	VarDataTab[15] = Appl_EEPParam.stm32.ExtEepAddress;
	VarDataTab[16] = Appl_EEPParam.stm32.ExtEepPage;
	VarDataTab[17] = Appl_EEPParam.stm32.ExtEepOffset;
	VarDataTab[18] = Appl_EEPParam.stm32.EEP_CustomerNumber;
	VarDataTab[19] = (uint8_t)Appl_EEPParam.stm32.EEP_RejuvenationCycleID;

	VarDataTab[20] = (uint8_t)Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdL;
}

void ReadEEPData(void){
	/*Vishwesh:
	 * Update EEP Data from STM32 Emulated Flash
	 */
	for(int i=0; i<NB_OF_VAR ; i++){
		VirtAddVarTab[i] = i+1;
	}

	if(memcmp(&Appl_EEPParam_Mirror.stm32, &Appl_EEPParam.stm32, sizeof(Appl_EEPParam_Mirror.stm32)) != 0){
		for(int i=0; i<NB_OF_VAR; i++){
			Appl_enumStatusAPIEEP = EE_ReadVariable(VirtAddVarTab[i], &VarDataTab[i]);
			if(Appl_enumStatusAPIEEP != 0)Appl_u16EEPError++;
		}
		MapVishwastructtoEEP();
	}

}


void MapVishwastructtoEEP(void){

	uint16_t TempU16;
	Appl_EEPParam.stm32.EEP_Usg_Sec = VarDataTab[0];
	Appl_EEPParam.stm32.EEP_Usg_Min = VarDataTab[1];
	Appl_EEPParam.stm32.EEP_Usg_HourL = VarDataTab[2];
	Appl_EEPParam.stm32.EEP_Usg_HourH = VarDataTab[3];

	Appl_EEPParam.stm32.EEP_UserSetTargetCurrent = VarDataTab[4];
	Appl_EEPParam.stm32.EEP_SetOverCurrentThreshold = VarDataTab[5];

	Appl_EEPParam.stm32.EEP_Run_Sec = VarDataTab[6];
	Appl_EEPParam.stm32.EEP_Run_Min = VarDataTab[7];
	Appl_EEPParam.stm32.EEP_Run_Hour = VarDataTab[8];

	Appl_EEPParam.stm32.EEP_PowerFailedLastTime = VarDataTab[9];
	Appl_EEPParam.stm32.EEP_TimerSetHours = VarDataTab[10];

	Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdH = VarDataTab[11];
	Appl_EEPParam.stm32.EEP_SetUnderVoltageThreshold = VarDataTab[12];

	Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH = VarDataTab[13];
	Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL = VarDataTab[14];
	Appl_EEPParam.stm32.ExtEepAddress = VarDataTab[15];
	Appl_EEPParam.stm32.ExtEepPage = VarDataTab[16];
	Appl_EEPParam.stm32.ExtEepOffset  = VarDataTab[17];

	Appl_EEPParam.stm32.EEP_CustomerNumber = VarDataTab[18];
	Appl_EEPParam.stm32.EEP_RejuvenationCycleID = VarDataTab[19];

	Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdL = VarDataTab[20];

	SM_ApplParam.UserSetCurrent = (float)Appl_EEPParam.stm32.EEP_UserSetTargetCurrent;
	SM_ApplParam.OverCurrentThreshold = (float)Appl_EEPParam.stm32.EEP_SetOverCurrentThreshold;
	TempU16 = (((uint16_t)Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdH<<8) | Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdL);
	SM_ApplParam.OverVoltageThreshold = (float)TempU16;
	SM_ApplParam.UnderVoltageThreshold = (float)Appl_EEPParam.stm32.EEP_SetUnderVoltageThreshold;
	memcpy(&Appl_EEPParam_Mirror.stm32, &Appl_EEPParam.stm32, sizeof(Appl_EEPParam_Mirror.stm32));
}




void CaptureExtEEPSnapshot_SingleShot(uint8_t start_stop_Flag, PauseReason reason){
	uint16_t TempU16;
	TempU16 = (uint16_t)SM_ApplParam.Realtime_floatingVoltage;

	if(start_stop_Flag == START_RECORD_FLAG){
		Appl_EEPParam.stm32.ExtEepAddress = 0xA0;
		Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL++;
		if(Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL == 0xFF){
			Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL = 0;
			Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH++;
		}


		Appl_EEPParam.transfer.extRecord.ExtEEP_SlNumber_HighByte = Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH;
		Appl_EEPParam.transfer.extRecord.ExtEEP_SlNumber_LowByte = Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL;
		Appl_EEPParam.transfer.extRecord.ExtEEP_CustIDNum = Appl_EEPParam.stm32.EEP_CustomerNumber;
		Appl_EEPParam.transfer.extRecord.ExtEEP_RejCycleID = Appl_EEPParam.stm32.EEP_RejuvenationCycleID;
		Appl_EEPParam.transfer.extRecord.ExtEEP_StartTime_H = Appl_EEPParam.stm32.EEP_Run_Hour;
		Appl_EEPParam.transfer.extRecord.ExtEEP_StartTime_M = Appl_EEPParam.stm32.EEP_Run_Min;
#if ALACRITAS_UNIT_NUMBER == 1
		Appl_EEPParam.transfer.extRecord.ExtEEP_DeviceId = 0x1;
#elif ALACRITAS_UNIT_NUMBER == 2
		Appl_EEPParam.transfer.extRecord.ExtEEP_DeviceId = 0x2;
#elif ALACRITAS_UNIT_NUMBER == 3
		Appl_EEPParam.transfer.extRecord.ExtEEP_DeviceId = 0x3;
#endif
		Appl_EEPParam.transfer.extRecord.ExtEEP_StopTime_H = 0;
		Appl_EEPParam.transfer.extRecord.ExtEEP_StopTime_M = 0;
		Appl_EEPParam.transfer.extRecord.ExtEEP_StopReason = 0;
		Appl_EEPParam.transfer.extRecord.ExtEEP_BatteryVoltageH = TempU16>>8;
		Appl_EEPParam.transfer.extRecord.ExtEEP_CurrentConsumption = 0;
		Appl_EEPParam.transfer.extRecord.ExtEEP_KWh = 0;
		Appl_EEPParam.transfer.extRecord.ExtEEP_BatteryVoltageL = TempU16 & 0xFF;
		Appl_EEPParam.transfer.extRecord.dummy_L = 0;
	}
	if(start_stop_Flag == STOP_RECORD_FLAG){
		Appl_EEPParam.transfer.extRecord.ExtEEP_BatteryVoltageH = TempU16>>8;
		Appl_EEPParam.transfer.extRecord.ExtEEP_BatteryVoltageL = TempU16 & 0xFF;
		Appl_EEPParam.transfer.extRecord.ExtEEP_CurrentConsumption = SM_ApplParam.RealtimeCurrentFiltered;
		Appl_EEPParam.transfer.extRecord.ExtEEP_StopTime_H = Appl_EEPParam.stm32.EEP_Run_Hour;
		Appl_EEPParam.transfer.extRecord.ExtEEP_StopTime_M = Appl_EEPParam.stm32.EEP_Run_Min;
		Appl_EEPParam.transfer.extRecord.ExtEEP_StopReason = reason;
		if(Appl_diagActiveFault.EEPLogMemoryFull!= 1){
			do{
				EEP_Error = EEPROM_Write(Appl_EEPParam.stm32.ExtEepPage,Appl_EEPParam.stm32.ExtEepOffset,&Appl_EEPParam.transfer.bytes[0],sizeof(Appl_EEPParam.transfer),Appl_EEPParam.stm32.ExtEepAddress);

				if(EEP_Error != EXTEEP_OK){
					EEP_ErrorCounter++;
					/*ResetI2c*/
					(&hi2c1)->Instance->CR1 |= I2C_CR1_SWRST;
					(&hi2c1)->Instance->CR1 &= ~I2C_CR1_SWRST;
				}
				else{
					EEP_ErrorCounter = 0xFF;
				}
			}while(EEP_ErrorCounter<10);
		}

//		if(Appl_EEPParam.stm32.ExtEepOffset >= ATMEL_EEP_CHIP_PAGE_SIZE){
//			Appl_EEPParam.stm32.ExtEepOffset = 0;
//			Appl_EEPParam.stm32.ExtEepPage++;
//			if(Appl_EEPParam.stm32.ExtEepPage == ATMEL_EEP_CHIP_PAGE_NUM){
//				Appl_EEPParam.stm32.ExtEepPage = 0;
//				Appl_EEPParam.stm32.ExtEepAddress++;
//				if(Appl_EEPParam.stm32.ExtEepAddress > 0xA4){
//					Appl_EEPParam.stm32.ExtEepAddress = 0xA4;
//					Appl_diagActiveFault.EEPLogMemoryFull = 1;
//				}
//			}
//		}
		if(Appl_EEPParam.stm32.ExtEepOffset >= 240){
			Appl_EEPParam.stm32.ExtEepPage++;
			if(Appl_EEPParam.stm32.ExtEepPage > 0xFE){
				Appl_EEPParam.stm32.ExtEepPage = 0xFE;
				Appl_diagActiveFault.EEPLogMemoryFull = 1;
			}
		}
		/*Note that resetting of Appl_EEPParam.stm32.ExtEepOffset is done via overflow method.
		 * i.e 240+16 on  8-byte variable overflows as 0;
		 * Upper logic block ensures when this happen page number is incremented.
		 *
		 */
		Appl_EEPParam.stm32.ExtEepOffset +=NUM_OF_BYTES_IN_A_EXTEEP_RECORD;
	}
}

void ResetRecords(void){
	Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL = 0;
	Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH = 0;
	Appl_EEPParam.stm32.ExtEepPage = 1;
	Appl_EEPParam.stm32.ExtEepOffset = 0;
	Appl_diagActiveFault.EEPLogMemoryFull = 0;
}
#endif
