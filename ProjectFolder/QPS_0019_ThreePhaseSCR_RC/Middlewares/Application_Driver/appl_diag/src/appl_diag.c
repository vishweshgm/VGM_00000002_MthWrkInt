/*
 * appl_diag.c
 *
 *  Created on: Dec 23, 2021
 *      Author: vishweshgm
 */
#define APPL_DIAG_C 1
#if APPL_DIAG_C == 1

#include "stateMachine.h"
#include "appl_diag.h"

/*extern variables*/
extern SM_MainApplParameters SM_ApplParam;
extern volatile SM_ECUState_Enum SM_enumECUState;

/*global variables*/
DiagFaultTable Appl_diagActiveFault;
DiagFaultParamTable FaultParamTable[Diag_enum_MaxFault] = FAULT_PARAM_TABLE_INIT;

void RunDiagnostics(void){

	/*Diag_enum_OverCurrent*/
	if(GPIO_PIN_RESET == DC_CONTACTOR_STATE()){
		if(SM_ApplParam.RealtimeCurrent > SM_ApplParam.OverCurrentThreshold){
			FaultParamTable[Diag_enum_OverCurrent].DebouncefaultCounter++;
			if(FaultParamTable[Diag_enum_OverCurrent].DebouncefaultCounter >= FaultParamTable[Diag_enum_OverCurrent].faultconfirmThreshold){
				FaultParamTable[Diag_enum_OverCurrent].DebouncefaultCounter = FaultParamTable[Diag_enum_OverCurrent].faultconfirmThreshold;
				Appl_diagActiveFault.OverCurrent = 1;
				FaultParamTable[Diag_enum_OverCurrent].DebouncehealCounter = 0;
			}
		}
		else{
			FaultParamTable[Diag_enum_OverCurrent].DebouncehealCounter++;
			if(FaultParamTable[Diag_enum_OverCurrent].DebouncehealCounter > FaultParamTable[Diag_enum_OverCurrent].healconfirmThreshold){
				FaultParamTable[Diag_enum_OverCurrent].DebouncefaultCounter = 0;
				FaultParamTable[Diag_enum_OverCurrent].DebouncehealCounter = FaultParamTable[Diag_enum_OverCurrent].healconfirmThreshold;
				Appl_diagActiveFault.OverCurrent = 0;
			}
		}



		if(SM_ApplParam.RealtimeVoltage > SM_ApplParam.OverVoltageThreshold){
			FaultParamTable[Diag_enum_OverVoltage].DebouncefaultCounter++;
			if(FaultParamTable[Diag_enum_OverVoltage].DebouncefaultCounter >= FaultParamTable[Diag_enum_OverVoltage].faultconfirmThreshold){
				FaultParamTable[Diag_enum_OverVoltage].DebouncefaultCounter = FaultParamTable[Diag_enum_OverVoltage].faultconfirmThreshold;
				Appl_diagActiveFault.OverVoltage = 1;
				FaultParamTable[Diag_enum_OverVoltage].DebouncehealCounter = 0;
			}
		}
		else{
			FaultParamTable[Diag_enum_OverVoltage].DebouncehealCounter++;
			if(FaultParamTable[Diag_enum_OverVoltage].DebouncehealCounter > FaultParamTable[Diag_enum_OverVoltage].healconfirmThreshold){
				FaultParamTable[Diag_enum_OverVoltage].DebouncefaultCounter = 0;
				FaultParamTable[Diag_enum_OverVoltage].DebouncehealCounter = FaultParamTable[Diag_enum_OverVoltage].healconfirmThreshold;
				Appl_diagActiveFault.OverVoltage = 0;
			}
		}




		if(SM_ApplParam.RealtimeVoltage < SM_ApplParam.UnderVoltageThreshold){
			FaultParamTable[Diag_enum_UnderVoltage].DebouncefaultCounter++;
			if(FaultParamTable[Diag_enum_UnderVoltage].DebouncefaultCounter >= FaultParamTable[Diag_enum_UnderVoltage].faultconfirmThreshold){
				FaultParamTable[Diag_enum_UnderVoltage].DebouncefaultCounter = FaultParamTable[Diag_enum_UnderVoltage].faultconfirmThreshold;
				Appl_diagActiveFault.UnderVoltage = 1;
				FaultParamTable[Diag_enum_UnderVoltage].DebouncehealCounter = 0;
			}
		}
		else{
			FaultParamTable[Diag_enum_UnderVoltage].DebouncehealCounter++;
			if(FaultParamTable[Diag_enum_UnderVoltage].DebouncehealCounter > FaultParamTable[Diag_enum_UnderVoltage].healconfirmThreshold){
				FaultParamTable[Diag_enum_UnderVoltage].DebouncefaultCounter = 0;
				FaultParamTable[Diag_enum_UnderVoltage].DebouncehealCounter = FaultParamTable[Diag_enum_UnderVoltage].healconfirmThreshold;
				Appl_diagActiveFault.UnderVoltage = 0;
			}
		}
	}
	else{
		Appl_diagActiveFault.UnderVoltage = 0;
		Appl_diagActiveFault.OverVoltage = 0;
		Appl_diagActiveFault.OverCurrent = 0;
	}



		if(HAL_GPIO_ReadPin(uC_SCRTemp_GPIO_Port,uC_SCRTemp_Pin) == GPIO_PIN_RESET){
			FaultParamTable[Diag_enum_HighTemperatureSCR].DebouncefaultCounter++;
			if(FaultParamTable[Diag_enum_HighTemperatureSCR].DebouncefaultCounter >= FaultParamTable[Diag_enum_HighTemperatureSCR].faultconfirmThreshold){
				FaultParamTable[Diag_enum_HighTemperatureSCR].DebouncefaultCounter = FaultParamTable[Diag_enum_HighTemperatureSCR].faultconfirmThreshold;
				Appl_diagActiveFault.HighTempperatureSCR = 1;
				FaultParamTable[Diag_enum_HighTemperatureSCR].DebouncehealCounter = 0;
			}
		}
		else{
			FaultParamTable[Diag_enum_HighTemperatureSCR].DebouncehealCounter++;
			if(FaultParamTable[Diag_enum_HighTemperatureSCR].DebouncehealCounter > FaultParamTable[Diag_enum_HighTemperatureSCR].healconfirmThreshold){
				FaultParamTable[Diag_enum_HighTemperatureSCR].DebouncefaultCounter = 0;
				FaultParamTable[Diag_enum_HighTemperatureSCR].DebouncehealCounter = FaultParamTable[Diag_enum_HighTemperatureSCR].healconfirmThreshold;
				Appl_diagActiveFault.HighTempperatureSCR = 0;
			}
		}


	/*Diag_enum_Powerfail_Z1*/
	/*
	 * Let's increase this every 50ms and reset it in 20ms interrupts
	 * If any phase is unavailable, that particular fault counter increases beyond threshold.
	 * */
	FaultParamTable[Diag_enum_Powerfail_Z1].DebouncefaultCounter++;
	if(FaultParamTable[Diag_enum_Powerfail_Z1].DebouncefaultCounter > FaultParamTable[Diag_enum_Powerfail_Z1].faultconfirmThreshold){
		FaultParamTable[Diag_enum_Powerfail_Z1].DebouncefaultCounter = FaultParamTable[Diag_enum_Powerfail_Z1].faultconfirmThreshold;
		Appl_diagActiveFault.PowerFail_Z1 = 1;
		FaultParamTable[Diag_enum_Powerfail_Z1].DebouncehealCounter = 0;
	}
	else if(FaultParamTable[Diag_enum_Powerfail_Z1].DebouncehealCounter >= FaultParamTable[Diag_enum_Powerfail_Z1].healconfirmThreshold){

		Appl_diagActiveFault.PowerFail_Z1 = 0;
	}

	/*Diag_enum_Powerfail_Z2*/
	/*
	 * Let's increase this every 50ms and reset it in 20ms interrupts
	 * If any phase is unavailable, that particular fault counter increases beyond threshold.
	 * */
	FaultParamTable[Diag_enum_Powerfail_Z2].DebouncefaultCounter++;
	if(FaultParamTable[Diag_enum_Powerfail_Z2].DebouncefaultCounter > FaultParamTable[Diag_enum_Powerfail_Z2].faultconfirmThreshold){
		FaultParamTable[Diag_enum_Powerfail_Z2].DebouncefaultCounter = FaultParamTable[Diag_enum_Powerfail_Z2].faultconfirmThreshold;
		Appl_diagActiveFault.PowerFail_Z2 = 1;
		FaultParamTable[Diag_enum_Powerfail_Z2].DebouncehealCounter = 0;
	}
	else if(FaultParamTable[Diag_enum_Powerfail_Z2].DebouncehealCounter >= FaultParamTable[Diag_enum_Powerfail_Z2].healconfirmThreshold){

		Appl_diagActiveFault.PowerFail_Z2 = 0;
	}

	/*Diag_enum_Powerfail_Z3*/
	/*
	 * Let's increase this every 50ms and reset it in 20ms interrupts
	 * If any phase is unavailable, that particular fault counter increases beyond threshold.
	 * */
	FaultParamTable[Diag_enum_Powerfail_Z3].DebouncefaultCounter++;
	if(FaultParamTable[Diag_enum_Powerfail_Z3].DebouncefaultCounter > FaultParamTable[Diag_enum_Powerfail_Z3].faultconfirmThreshold){
		FaultParamTable[Diag_enum_Powerfail_Z3].DebouncefaultCounter = FaultParamTable[Diag_enum_Powerfail_Z3].faultconfirmThreshold;
		Appl_diagActiveFault.PowerFail_Z3 = 1;
		FaultParamTable[Diag_enum_Powerfail_Z3].DebouncehealCounter = 0;
	}
	else if(FaultParamTable[Diag_enum_Powerfail_Z3].DebouncehealCounter >= FaultParamTable[Diag_enum_Powerfail_Z3].healconfirmThreshold){
		Appl_diagActiveFault.PowerFail_Z3 = 0;
	}


	if(HAL_GPIO_ReadPin(uC_BattteryReverse_GPIO_Port,uC_BattteryReverse_Pin) == GPIO_PIN_RESET){

		Appl_diagActiveFault.BatteryReversal = 1;
	}
	else{
		Appl_diagActiveFault.BatteryReversal = 0;
	}
}


void Run_Diagnostic_Actions(void){
	if(SM_enumECUState != SM_enum_ECU_SelfTest){
		if((Appl_diagActiveFault.PhaseReversal == 1) || \
				Appl_diagActiveFault.HighTempperatureSCR == 1){
			BLUELEDON();
		}
		else{
			BLUELEDOFF();
		}
	}

}
#endif
