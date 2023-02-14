/*
 * eeprom.h
 *
 *  Created on: Feb 23, 2020
 *      Author: vishweshgm
 */

#ifndef EEPROM_INC_EEPROM_H_
#define EEPROM_INC_EEPROM_H_
#include "main.h"
#include "eeprom_stm32f4xx.h"
#include "stateMachine.h"
void Update_Usage_hour_eep(void);
void WriteEEPData(void);
void vishwaEEp_Init(void);
void ReadEEPData(void);
VishFuncError MapExteepToVishwaStruct(void);
void MapVishwaStructToExteep(PauseReason reason);
void Update_RunningTime_eep(void);
void CaptureExtEEPSnapshot_SingleShot(PauseReason reason);
void MapVishwastruct_to_ExtEEP(void);
void ResetRecords(void);


typedef struct {
/*byte 00*/	uint8_t ExtEEP_SlNumber_HighByte;
/*byte 01*/	uint8_t ExtEEP_SlNumber_LowByte;
/*byte 02*/	uint8_t ExtEEP_CustIDNum;
/*byte 03*/	uint8_t ExtEEP_ChargeRunNumber;
/*byte 04*/	uint8_t ExtEEP_Time_H;
/*byte 05*/	uint8_t ExtEEP_Time_M;
/*byte 06*/	uint8_t ExtEEP_DeviceId;
/*byte 07*/	uint8_t ExtEEP_BatteryVoltagec0;
/*byte 08*/	uint8_t ExtEEP_BatteryVoltagec1;
/*byte 09*/	uint8_t ExtEEP_BatteryVoltagec2;
/*byte 10*/	uint8_t ExtEEP_BatteryVoltagec3;
/*byte 11*/	uint8_t ExtEEP_CurrentConsumptionc0;
/*byte 12*/	uint8_t ExtEEP_CurrentConsumptionc1;
/*byte 13*/	uint8_t ExtEEP_CurrentConsumptionc2;
/*byte 14*/	uint8_t ExtEEP_CurrentConsumptionc3;
/*byte 15*/	uint8_t ExtEEP_StopReason;
/*byte 16*/	uint8_t ExtEEP_ChargeMode;
/*byte 17*/	uint8_t ExtEEP_CVModeCurrent;
/*byte 18*/	uint8_t ExtEEP_CVModeVoltage;
/*byte 19*/	uint8_t ExtEEP_CCModeCurrent;
/*byte 20*/	uint8_t ExtEEP_CCModeVoltage;
/*byte 21*/	uint8_t unused1;
/*byte 22*/	uint8_t unused2;
/*byte 23*/	uint8_t unused3;
/*byte 24*/	uint8_t unused4;
/*byte 25*/	uint8_t unused5;
/*byte 26*/	uint8_t unused6;
/*byte 27*/	uint8_t unused7;
/*byte 28*/	uint8_t unused8;
/*byte 29*/	uint8_t unused9;
/*byte 30*/	uint8_t unused10;
/*byte 31*/	uint8_t unused11;
}External_EEP_DataStruct;

typedef union {
	uint8_t bytes[NUM_OF_BYTES_IN_A_EXTEEP_RECORD];
	External_EEP_DataStruct extRecord;
}External_EEP_DataStructure;

/*Vishwesh:
 * If you are changing this structure change NB_OF_VAR constant
 *
 */
typedef struct{
	uint8_t EEP_Usg_Sec;
	uint8_t EEP_Usg_Min;
	uint8_t EEP_Usg_HourL;
	uint8_t EEP_Usg_HourH;
	uint8_t EEP_CC_SetCurrent;
	uint8_t EEP_CV_SetVoltage;
	uint8_t EEP_CV_CutOffCurrent;
	uint8_t EEP_CC_CutOffVoltage;
	uint8_t EEP_SetOverVoltageThresholdL;
	uint8_t EEP_ChargeRunNumber;
	uint8_t EEP_PowerFailedLastTime;
	uint8_t EEP_Run_Sec;
	uint8_t EEP_Run_Min;
	uint8_t EEP_Run_Hour;
	uint8_t EEP_TimerSetHours;
	uint8_t EEP_CustomerNumber;
	uint8_t EEP_ModeSelected;
	uint8_t EEP_ExternalEEPRecordSerialNumberH;
	uint8_t EEP_ExternalEEPRecordSerialNumberL;
	uint8_t ExtEepAddress;
	uint8_t ExtEepPage;
	uint8_t ExtEepOffset;
}Internal_EEP_DataStructure;

typedef struct EEP_DataStructure_Tag{
	Internal_EEP_DataStructure stm32;
	External_EEP_DataStructure  transfer;
}EEP_DataStructure;


typedef union {
	char c[4];
	float f;
} unionfloatData;


extern FLASH_Status Appl_enumStatusAPIEEP;
#endif /* EEPROM_INC_EEPROM_H_ */
