#ifndef STATEMACHINE_INC_STATEMACHINE_H_
#define STATEMACHINE_INC_STATEMACHINE_H_

#ifdef __cplusplus
extern "C"{
#endif
/* USER-#defines*/
#include "main.h"

/* USER- TYPES*/
typedef enum SM_ECUState_Enum{
	SM_enum_ECU_Init = 0,
	SM_enum_ECU_StandByMode,
	SM_enum_ECU_CurrentModeSettings,
	SM_enum_ECU_VoltageModeSettings,
	SM_enum_ECU_CVPreparing,
	SM_enum_ECU_CCPreparing,
	SM_enum_ECU_SettingMenu,
	SM_enum_ECU_SettingMenu_Transfer,
	SM_enum_ECU_SettingMenu_Erase,
	SM_enum_ECU_SettingMenu_Completed,
	SM_enum_ECU_CCPauseMode,
	SM_enum_ECU_CVPauseMode,
	SM_enum_ECU_AutoModeUserInput,
	SM_enum_ECU_CCRun_Slow,
	SM_enum_ECU_CCRun_Stable,
	SM_enum_ECU_CVRun_Slow,
	SM_enum_ECU_CVRun_Stable,
	SM_enum_ECU_SelfTest,
	SM_enum_ECU_StatusDisplay,
	SM_enum_ECU_Info,
	SM_enum_ECU_ContinueFromLastPowerOff,
	SM_enum_ECU_VishwatestMode
}SM_ECUState_Enum;

typedef enum PauseReasonTag{
	PauseReasonEnum_Default = 1,
	PauseReasonEnum_UserPaused,
	PauseReasonEnum_TimerComplete,
	PauseReasonEnum_1HrLogTimeSave,
	PauseReasonEnum_PowerFailZ1,
	PauseReasonEnum_HighTemp,
	PauseReasonEnum_PhaseReversal,
	PauseReasonEnum_BatteryReversal,
	PauseReasonEnum_EEPLogMemoryFull,
	PauseReasonEnum_CutOffVoltage,
	PauseReasonEnum_UnderVoltage,
	PauseReasonEnum_CutOffCurrent,
	PauseReasonEnum_FilterCircuitOpen,
	PauseReasonEnum_OverVoltage,
	PauseReasonEnum_OverCurrent,
	PauseReasonEnum_Max
}PauseReason;


typedef struct SM_MainApplParamsTag{



	float Realtime_floatingVoltage_vrms;
	float Realtime_floatingVoltage_vavg;

	float RealtimeVoltage;
	float RealtimeCurrent;


	float RealtimeShuntCurrent_vrms;
	float RealtimeShuntCurrent_vavg;

	float ap_fltCCSlowGain;
	float ap_fltCCFastGain;
	float ap_fltCVSlowGain;
	float ap_fltCVFastGain;

	float InitialBatteryVoltage;
	float PreparePercentage;

	float ap_fltDataPercent;


	uint8_t ap_PrepareMode;
	uint32_t ap_PrepareCounter0;
	uint32_t ap_PrepareCounter1;

	uint32_t Transfer_PageloopCount;
	uint32_t Transfer_OffsetCounter;

	uint16_t ExtEepTotalRecordsTotransferred;
	PauseReason Pausereason;
	uint16_t Appl_u16PulseSCRWidth_us;
	uint32_t Appl_u32_TriggersCountForG1;
	uint32_t Appl_u32_TriggersCountForG2;
	uint32_t Appl_u32_TriggersCountForG3;

	uint8_t u8ApplSCRTrggerOffG1;
	uint8_t u8ApplSCRTrggerOffG2;
	uint8_t u8ApplSCRTrggerOffG3;

	uint32_t BuzzerCounter;
	uint8_t Buzzertoggle;

	uint8_t u8SkipT1;
	uint8_t u8SkipT2;
	uint8_t u8SkipT3;

}SM_MainApplParameters;


typedef enum VishFuncError_tag{
	VISH_OK = 0,
	VISH_ERR
}VishFuncError;


extern volatile SM_ECUState_Enum Appl_enumECUState;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;

void SM_ECUInit(void);
void SM_StateMachineHandler(uint8_t loopms);
void SM_ListenFromDisplay(uint8_t loopms);
void SM_SpeakToDisplay(uint8_t loopms);
void Error_Handler(void);
void SM_10msTask(void);
void process_output(float Fnclocal_fltOutputTargedPIDVoltage);
uint32_t tick_get(void);
float CurrentRMS(float* arr, int n);
float movingAvg(uint32_t *ptrArrNumbers, uint32_t *ptrSum, uint8_t pos, uint16_t len, uint32_t nextNum);
void Appl_fncCalculateFeedbackVoltage(void);
void Appl_fncCalculateFeedbackShuntCurrent(void);
void Appl_fncCalculateFeedbackHallCurrent(void);
void TriggerWidth(void);
void TIM3_Callback(void);
void Appl_fncResetStateOfPID(void);
void delay_us (uint16_t us);
void Appl_AdcPhaseAngleDetermination(void);
float PID_Current(float gain);
float PID_Voltage(float gain);
float PID_PrepareVoltage(void);
float PID_CVMode_CCRun(float gain);
float PID_CCMode_CVRun(float gain);



uint32_t ADC_filter_noise(uint32_t Fnclocal_u32AdcVoltageValue);
uint32_t ADC_normal_avg(uint32_t Appl_u32loaclvolt);
float ADC_filter_noise2(float Fnclocal_AdcVoltageValue);
float ADC_filter_noise3(float Fnclocal_AdcVoltageValue);
void SafeRunCVMonitoring(void);
void SafeRunCCMonitoring(void);
void TIM5_Callback(void);
void TIM6_Callback(void);
void TIM7_Callback(void);

void StartCurrentProvidingLoop(void);
void StartVoltageProvidingLoop(void);
void StartSelftestCCPrepare(uint8_t loopms);
void StartSelftestCVPrepare(uint8_t loopms);
void StopCurrentProvidingLoop(PauseReason reason);
void StopVoltageProvidingLoop(PauseReason reason);
void switchOnBuzzer(void);


extern TIM_HandleTypeDef* phtim8;

#ifdef __cplusplus
}
#endif

#endif /* STATEMACHINE_INC_STATEMACHINE_H_ */
