/*
 * appl_diag.h
 *
 *  Created on: Dec 23, 2021
 *      Author: vishweshgm
 */

#ifndef APPLICATION_DRIVER_APPL_DIAG_INC_APPL_DIAG_H_
#define APPLICATION_DRIVER_APPL_DIAG_INC_APPL_DIAG_H_


#include "main.h"
#define DEBOUNCE_VALUE_OVERCURRENT 2000
#define DEBOUNCE_VALUE_UNDERCURRENT 2000

typedef enum Diag_FaultTableEnumTag{
	Diag_enum_Powerfail_Z1 = 0,
	Diag_enum_Powerfail_Z2,
	Diag_enum_Powerfail_Z3,
	Diag_enum_CCcutOffVoltage,
	Diag_enum_CVcutOffCurrent,
	Diag_enum_UnderVoltage,
	Diag_enum_HighTemperatureSCR,
	Diag_enum_PhaseReversal,
	Diag_enum_BatteryReversal,
	Diag_enum_EEPLogMemoryFull,
	Diag_enum_FilterCktOpen,
	Diag_enum_OverVOltage,
	Diag_enum_OverCurrent,

	Diag_enum_MaxFault
}Diag_FaultTable_enum;

typedef struct DiagFaultTablestructtag{
	uint8_t PowerFail_Z1;
	uint8_t PowerFail_Z2;
	uint8_t PowerFail_Z3;
	uint8_t CCcutOffVoltage;
	uint8_t CVcutOffCurrent;
	uint8_t UnderVoltage;
	uint8_t HighTempperatureSCR;
	uint8_t PhaseReversal;
	uint8_t BatteryReversal;
	uint8_t EEPLogMemoryFull;
	uint8_t FiltercktOpen;
	uint8_t OverVoltage;
	uint8_t OverCurrent;
}DiagFaultTable;

typedef struct Diagfaultparamtabletag{
	uint16_t DebouncefaultCounter;
	uint16_t DebouncehealCounter;
	uint16_t faultconfirmThreshold;
	uint16_t healconfirmThreshold;
}DiagFaultParamTable;

#define FAULT_PARAM_TABLE_INIT \
{{0,0,10,50},/*PowerFail_Z1*/\
{0,0,10,50},/*PowerFail_Z2*/\
{0,0,10,50},/*PowerFail_Z3*/\
{0,0,10,10},/*CCcutOffVoltage*/\
{0,0,10,10},/*CVcutOffCurrent*/\
{0,0,10,10},/*UnderVoltage*/\
{0,0,10,10},/*HighTempperatureSCR*/\
{0,0,50,50},/*PhaseReversal*/\
{0,0,50,50},/*BatteryReversal */\
{0,0,10,10},/*EEPLogMemoryFull*/\
{0,0,10,10},/*FiltercktOpen*/\
{0,0,10,10},/*OverVoltage*/\
{0,0,10,10}/*OverCurrent*/\
}



void RunDiagnostics(void);
#endif /* APPLICATION_DRIVER_APPL_DIAG_INC_APPL_DIAG_H_ */
