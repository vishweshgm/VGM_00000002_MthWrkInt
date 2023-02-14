/*
 * eeprom.c
 *
 *  Created on: Feb 23, 2020
 *      Author: vishweshgm
 */
#define EEPROM_USED 1
#if EEPROM_USED

#include "stateMachine.h"
#include "cmsis_os.h"
#include "appl_diag.h"
#include "eeprom.h"
#include "eeprom_external.h"
#include "display_state.h"
void MapVishwastructtoEEP(void);
void MapStEEPtoVishwastruct(void);

/*extern variables*/
extern SM_MainApplParameters SM_ApplParam;
extern DiagFaultTable Appl_diagActiveFault;
extern I2C_HandleTypeDef hi2c1;
extern External_EEP_DataStructure VishExtEepRead;
extern SM_DisplayParam Appl_Display;
/*STM32 EEP*/
/*EEP*/
/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NB_OF_VAR];
uint16_t VarDataTab[NB_OF_VAR];
FLASH_Status Appl_enumStatusAPIEEP;
uint16_t Appl_u16EEPError;

EEP_DataStructure Appl_EEPParam = {1};
EEP_DataStructure Appl_EEPParam_Mirror = {0}; /* defined in-equal purposefully*/

/*Vishwesh GM  : I'll use this for dynamic memory allocation*/
int8_t* UsbDataBuf;
int8_t* usbTraceBuffer;


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
		CaptureExtEEPSnapshot_SingleShot(PauseReasonEnum_1HrLogTimeSave);
		WriteEEPData();
	}

	if(Appl_EEPParam.stm32.EEP_Run_Min >= 60){
		Appl_EEPParam.stm32.EEP_Run_Min = 0;
		if(Appl_EEPParam.stm32.EEP_Run_Hour >= 0xFF){
			Appl_EEPParam.stm32.EEP_Run_Hour = 0;
		}
		else{
			Appl_EEPParam.stm32.EEP_Run_Hour++;

			WriteEEPData();
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


	for(int i=0; i<NB_OF_VAR ; i++){
		VirtAddVarTab[i] = i+1;
	}


	VarDataTab[0]  = Appl_EEPParam.stm32.EEP_Usg_Sec;
	VarDataTab[1]  = Appl_EEPParam.stm32.EEP_Usg_Min;
	VarDataTab[2]  = Appl_EEPParam.stm32.EEP_Usg_HourL;
	VarDataTab[3]  = Appl_EEPParam.stm32.EEP_Usg_HourH;
	VarDataTab[4]  = Appl_EEPParam.stm32.EEP_CC_SetCurrent;
	VarDataTab[5]  = Appl_EEPParam.stm32.EEP_CV_CutOffCurrent ;
	VarDataTab[6]  = Appl_EEPParam.stm32.EEP_Run_Sec;
	VarDataTab[7]  = Appl_EEPParam.stm32.EEP_Run_Min;
	VarDataTab[8]  = Appl_EEPParam.stm32.EEP_Run_Hour;
	VarDataTab[9]  = Appl_EEPParam.stm32.EEP_PowerFailedLastTime;
	VarDataTab[10] = Appl_EEPParam.stm32.EEP_TimerSetHours;
	VarDataTab[11] = Appl_EEPParam.stm32.EEP_CC_CutOffVoltage;
	VarDataTab[12] = Appl_EEPParam.stm32.EEP_ChargeRunNumber;
	VarDataTab[13] = Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH;
	VarDataTab[14] = Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL;
	VarDataTab[15] = Appl_EEPParam.stm32.ExtEepAddress;
	VarDataTab[16] = Appl_EEPParam.stm32.ExtEepPage;
	VarDataTab[17] = Appl_EEPParam.stm32.ExtEepOffset;
	VarDataTab[18] = Appl_EEPParam.stm32.EEP_CustomerNumber;
	VarDataTab[19] = (uint8_t)Appl_EEPParam.stm32.EEP_ModeSelected;
	VarDataTab[20] = (uint8_t)Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdL;
	VarDataTab[21] = (uint8_t)Appl_EEPParam.stm32.EEP_CV_SetVoltage;
}

VishFuncError MapExteepToVishwaStruct(void){
	int8_t UsbdataVLength = 0;
	unionfloatData unionfloattemp;
	uint16_t Fnclocal_u16Temp;
	uint8_t Fnclocal_u8Temp;

	UsbDataBuf = (int8_t*)malloc(512);
	if(!UsbDataBuf)return VISH_ERR;
	usbTraceBuffer = UsbDataBuf;

	Fnclocal_u16Temp = (VishExtEepRead.extRecord.ExtEEP_SlNumber_HighByte << 8) | VishExtEepRead.extRecord.ExtEEP_SlNumber_LowByte;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%d|",Fnclocal_u16Temp);

	Fnclocal_u8Temp = VishExtEepRead.extRecord.ExtEEP_CustIDNum;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%d|",Fnclocal_u8Temp);

	Fnclocal_u8Temp = VishExtEepRead.extRecord.ExtEEP_ChargeRunNumber;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%d|",Fnclocal_u8Temp);

	Fnclocal_u8Temp = VishExtEepRead.extRecord.ExtEEP_Time_H;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%d|",Fnclocal_u8Temp);

	Fnclocal_u8Temp = VishExtEepRead.extRecord.ExtEEP_Time_M;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%d|",Fnclocal_u8Temp);

	Fnclocal_u8Temp = Appl_EEPParam.transfer.extRecord.ExtEEP_ChargeMode;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%d|",Fnclocal_u8Temp);

	unionfloattemp.c[0] = VishExtEepRead.extRecord.ExtEEP_BatteryVoltagec0;
	unionfloattemp.c[1] = VishExtEepRead.extRecord.ExtEEP_BatteryVoltagec1;
	unionfloattemp.c[2] = VishExtEepRead.extRecord.ExtEEP_BatteryVoltagec2;
	unionfloattemp.c[3] = VishExtEepRead.extRecord.ExtEEP_BatteryVoltagec3;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%.2f|",unionfloattemp.f);

	unionfloattemp.c[0] = VishExtEepRead.extRecord.ExtEEP_CurrentConsumptionc0;
	unionfloattemp.c[1] = VishExtEepRead.extRecord.ExtEEP_CurrentConsumptionc1;
	unionfloattemp.c[2] = VishExtEepRead.extRecord.ExtEEP_CurrentConsumptionc2;
	unionfloattemp.c[3] = VishExtEepRead.extRecord.ExtEEP_CurrentConsumptionc3;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%.2f|",unionfloattemp.f);

	Fnclocal_u8Temp = VishExtEepRead.extRecord.ExtEEP_StopReason;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%d|",Fnclocal_u8Temp);

	Fnclocal_u8Temp = VishExtEepRead.extRecord.ExtEEP_CCModeCurrent;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%d|",Fnclocal_u8Temp);

	Fnclocal_u8Temp = VishExtEepRead.extRecord.ExtEEP_CCModeVoltage;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%d|",Fnclocal_u8Temp);

	Fnclocal_u8Temp = VishExtEepRead.extRecord.ExtEEP_CVModeCurrent;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%d|",Fnclocal_u8Temp);

	Fnclocal_u8Temp = VishExtEepRead.extRecord.ExtEEP_CVModeVoltage;
	UsbdataVLength += sprintf(usbTraceBuffer+UsbdataVLength,"%d\n",Fnclocal_u8Temp);

	CDC_Transmit_HS((uint8_t*)usbTraceBuffer,UsbdataVLength);

	free(UsbDataBuf);

	return VISH_OK;

}

void MapVishwaStructToExteep(PauseReason reason){

	unionfloatData unionfloattemp;
	Appl_EEPParam.transfer.extRecord.ExtEEP_SlNumber_HighByte = Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH;
	Appl_EEPParam.transfer.extRecord.ExtEEP_SlNumber_LowByte = Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL;
	Appl_EEPParam.transfer.extRecord.ExtEEP_CustIDNum = Appl_EEPParam.stm32.EEP_CustomerNumber;
	Appl_EEPParam.transfer.extRecord.ExtEEP_ChargeRunNumber = Appl_EEPParam.stm32.EEP_ChargeRunNumber;
	Appl_EEPParam.transfer.extRecord.ExtEEP_Time_H = Appl_EEPParam.stm32.EEP_Run_Hour;
	Appl_EEPParam.transfer.extRecord.ExtEEP_Time_M = Appl_EEPParam.stm32.EEP_Run_Min;
	Appl_EEPParam.transfer.extRecord.ExtEEP_DeviceId = 51;

	unionfloattemp.f = Appl_Display.flt_dv;
	Appl_EEPParam.transfer.extRecord.ExtEEP_BatteryVoltagec0 = unionfloattemp.c[0];
	Appl_EEPParam.transfer.extRecord.ExtEEP_BatteryVoltagec1 = unionfloattemp.c[1];
	Appl_EEPParam.transfer.extRecord.ExtEEP_BatteryVoltagec2 = unionfloattemp.c[2];
	Appl_EEPParam.transfer.extRecord.ExtEEP_BatteryVoltagec3 = unionfloattemp.c[3];

	unionfloattemp.f = Appl_Display.flt_di;
	Appl_EEPParam.transfer.extRecord.ExtEEP_CurrentConsumptionc0 = unionfloattemp.c[0];
	Appl_EEPParam.transfer.extRecord.ExtEEP_CurrentConsumptionc1 = unionfloattemp.c[1];
	Appl_EEPParam.transfer.extRecord.ExtEEP_CurrentConsumptionc2 = unionfloattemp.c[2];
	Appl_EEPParam.transfer.extRecord.ExtEEP_CurrentConsumptionc3 = unionfloattemp.c[3];


	Appl_EEPParam.transfer.extRecord.ExtEEP_StopReason = reason;

	Appl_EEPParam.transfer.extRecord.ExtEEP_ChargeMode = Appl_EEPParam.stm32.EEP_ModeSelected;
	Appl_EEPParam.transfer.extRecord.ExtEEP_CCModeCurrent = Appl_EEPParam.stm32.EEP_CC_SetCurrent;
	Appl_EEPParam.transfer.extRecord.ExtEEP_CCModeVoltage = Appl_EEPParam.stm32.EEP_CC_CutOffVoltage;
	Appl_EEPParam.transfer.extRecord.ExtEEP_CVModeCurrent = Appl_EEPParam.stm32.EEP_CV_SetVoltage;
	Appl_EEPParam.transfer.extRecord.ExtEEP_CVModeVoltage = Appl_EEPParam.stm32.EEP_CV_CutOffCurrent;
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


	Appl_EEPParam.stm32.EEP_Usg_Sec                         = VarDataTab[0];
	Appl_EEPParam.stm32.EEP_Usg_Min                         = VarDataTab[1];
	Appl_EEPParam.stm32.EEP_Usg_HourL                       = VarDataTab[2];
	Appl_EEPParam.stm32.EEP_Usg_HourH                       = VarDataTab[3];
	Appl_EEPParam.stm32.EEP_CC_SetCurrent                   = VarDataTab[4];
	Appl_EEPParam.stm32.EEP_CV_CutOffCurrent                = VarDataTab[5];
	Appl_EEPParam.stm32.EEP_Run_Sec                         = VarDataTab[6];
	Appl_EEPParam.stm32.EEP_Run_Min                         = VarDataTab[7];
	Appl_EEPParam.stm32.EEP_Run_Hour                        = VarDataTab[8];
	Appl_EEPParam.stm32.EEP_PowerFailedLastTime             = VarDataTab[9];
	Appl_EEPParam.stm32.EEP_TimerSetHours                   = VarDataTab[10];
	Appl_EEPParam.stm32.EEP_CC_CutOffVoltage                = VarDataTab[11];
	Appl_EEPParam.stm32.EEP_ChargeRunNumber			        = VarDataTab[12];
	Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH  = VarDataTab[13];
	Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL  = VarDataTab[14];
	Appl_EEPParam.stm32.ExtEepAddress                       = VarDataTab[15];
	Appl_EEPParam.stm32.ExtEepPage                          = VarDataTab[16];
	Appl_EEPParam.stm32.ExtEepOffset                        = VarDataTab[17];
	Appl_EEPParam.stm32.EEP_CustomerNumber                  = VarDataTab[18];
	Appl_EEPParam.stm32.EEP_ModeSelected                    = VarDataTab[19];
	Appl_EEPParam.stm32.EEP_SetOverVoltageThresholdL        = VarDataTab[20];
	Appl_EEPParam.stm32.EEP_CV_SetVoltage                   = VarDataTab[21];

	memcpy(&Appl_EEPParam_Mirror.stm32, &Appl_EEPParam.stm32, sizeof(Appl_EEPParam_Mirror.stm32));
}




void CaptureExtEEPSnapshot_SingleShot(PauseReason reason){
	uint16_t TempU16;




	ExtEEP_Error EEP_Error;
	uint32_t EEP_ErrorCounter;


		Appl_EEPParam.stm32.ExtEepAddress = 0xA0;
		Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL++;
		if(Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL == 0xFF){
			Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL = 0;
			Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH++;
		}
		MapVishwaStructToExteep(reason);


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
		if(Appl_EEPParam.stm32.ExtEepOffset >= PAGEINCREMENT_OFFSET_THRESHOLD){
			Appl_EEPParam.stm32.ExtEepPage++;
			if(Appl_EEPParam.stm32.ExtEepPage > 0xFE){
				Appl_EEPParam.stm32.ExtEepPage = 0xFE;
				Appl_diagActiveFault.EEPLogMemoryFull = 1;
			}
		}
		/*Note that resetting of Appl_EEPParam.stm32.ExtEepOffset is done via overflow method.
		 * i.e 224+32 on  8-byte variable overflows as 0;
		 * Upper logic block ensures when this happen page number is incremented.
		 *
		 */
		Appl_EEPParam.stm32.ExtEepOffset +=NUM_OF_BYTES_IN_A_EXTEEP_RECORD;

}

void ResetRecords(void){
	Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL = 0;
	Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH = 0;
	Appl_EEPParam.stm32.ExtEepPage = 1;
	Appl_EEPParam.stm32.ExtEepOffset = 0;
	Appl_diagActiveFault.EEPLogMemoryFull = 0;
}
#endif
