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
void Update_RunningTime_eep(void);
void CaptureExtEEPSnapshot_SingleShot(uint8_t start_stop_Flag, PauseReason reason);
void MapVishwastruct_to_ExtEEP(void);
void ResetRecords(void);


typedef struct {
	uint8_t ExtEEP_SlNumber_HighByte;
	uint8_t ExtEEP_SlNumber_LowByte;
	uint8_t ExtEEP_CustIDNum;
	uint8_t ExtEEP_RejCycleID;
	uint8_t ExtEEP_StartTime_H;
	uint8_t ExtEEP_StartTime_M;
	uint8_t ExtEEP_DeviceId;
	uint8_t ExtEEP_StopTime_H;
	uint8_t ExtEEP_StopTime_M;
	uint8_t ExtEEP_StopTime_S;
	uint8_t ExtEEP_StopReason;
	uint8_t ExtEEP_CurrentConsumption;
	uint8_t ExtEEP_BatteryVoltageH;
	uint8_t ExtEEP_KWh;
	uint8_t ExtEEP_BatteryVoltageL;
	uint8_t dummy_L;
}External_EEP_DataStruct;

typedef union {
	uint8_t bytes[NUM_OF_BYTES_IN_A_EXTEEP_RECORD];
	External_EEP_DataStruct extRecord;
}External_EEP_DataStructure;

typedef struct{
	uint8_t EEP_Usg_Sec;
	uint8_t EEP_Usg_Min;
	uint8_t EEP_Usg_HourL;
	uint8_t EEP_Usg_HourH;
	uint8_t EEP_UserSetTargetCurrent;
	uint8_t EEP_SetOverCurrentThreshold;
	uint8_t EEP_SetOverVoltageThresholdH;
	uint8_t EEP_SetOverVoltageThresholdL;
	uint8_t EEP_SetUnderVoltageThreshold;
	uint8_t EEP_PowerFailedLastTime;
	uint8_t EEP_Run_Sec;
	uint8_t EEP_Run_Min;
	uint8_t EEP_Run_Hour;
	uint8_t EEP_TimerSetHours;
	uint8_t EEP_CustomerNumber;
	RejuvenationCycleID EEP_RejuvenationCycleID;
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


union {
	char c[4];
	float f;
} unionData;


extern FLASH_Status Appl_enumStatusAPIEEP;
#endif /* EEPROM_INC_EEPROM_H_ */
