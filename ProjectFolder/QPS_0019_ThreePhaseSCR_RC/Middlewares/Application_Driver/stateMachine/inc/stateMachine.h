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
	SM_enum_ECU_RejuvenationMenu,
	SM_enum_ECU_SettingMenu,
	SM_enum_ECU_SettingMenu_Transfer,
	SM_enum_ECU_SettingMenu_Erase,
	SM_enum_ECU_SettingMenu_Completed,
	SM_enum_ECU_SettingMenu_MaxVoltage,
	SM_enum_ECU_SelfTestMenu,
	SM_enum_ECU_PauseMode,
	SM_enum_ECU_AutoModeUserInput,
	SM_enum_ECU_VldAC_RunCurrentLoop_increaseVoltageMode_Slow,
	SM_enum_ECU_VldAC_RunCurrentLoop_StableVoltagefloat,
	SM_enum_ECU_SelfTest,
	SM_enum_ECU_StatusDisplay,
	SM_enum_ECU_Info,
	SM_enum_ECU_ContinueFromLastPowerOff,
	SM_enum_ECU_VishwatestMode
}SM_ECUState_Enum;

typedef enum VishFuncError_tag{
	VISH_OK = 0,
	VISH_ERR
}VishFuncError;

typedef enum PauseReasonTag{
	PauseReasonEnum_Default = 1,
	PauseReasonEnum_UserPaused,
	PauseReasonEnum_TimerComplete,
	PauseReasonEnum_1HrLogTimeSave,
	PauseReasonEnum_PowerFailZ1,
	PauseReasonEnum_PowerFailZ2,
	PauseReasonEnum_PowerFailZ3,
	PauseReasonEnum_BatteryReversal,
	PauseReasonEnum_EEPLogMemoryFull,
	PauseReasonEnum_OverVoltage,
	PauseReasonEnum_UnderVoltage,
	PauseReasonEnum_OverCurrent,
	PauseReasonEnum_Error,
	PauseReasonEnum_Max
}PauseReason;

typedef enum RejuvenationCycleIDTag{
	RejuvCycleID_InitialCharge = 1,
	RejuvCycleID_Rej1,
	RejuvCycleID_Eq1,
	RejuvCycleID_Rej2,
	RejuvCycleID_Eq2,
	RejuvCycleID_Final
}RejuvenationCycleID;

typedef struct SM_MainApplParamsTag{
	float OverVoltageThreshold;
	float UnderVoltageThreshold;
	float OverCurrentThreshold;
	float targetPIDCurrent;
	float HighTemperatureCurrent;
	float UserSetCurrent;
	float Realtime_floatingVoltage;
	float Realtime_floatingVoltage_vrms;
	float Realtime_floatingVoltage_vavg;

	float RealtimeVoltage;
	float RealtimeCurrent;
	float RealtimeCurrentFiltered;

	float RealtimeHallCurrent;
	float RealtimeHallCurrent_vrms;
	float RealtimeHallCurrent_vavg;

	float RealtimeShuntCurrent_vrms;
	float RealtimeShuntCurrent_vavg;

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

}SM_MainApplParameters;





extern volatile SM_ECUState_Enum Appl_enumECUState;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;

void SM_ECUInit(void);
void SM_StateMachineHandler(uint8_t loopms);
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
//void u8g2Init(u8g2_t *u8g2);
void MonitorTemperature(void);

uint32_t ADC_filter_noise(uint32_t Fnclocal_u32AdcVoltageValue);
uint32_t ADC_normal_avg(uint32_t Appl_u32loaclvolt);
float ADC_filter_noise2(float Fnclocal_AdcVoltageValue);
float ADC_filter_noise3(float Fnclocal_AdcVoltageValue);
void SafeRunMonitoring(void);
void TIM5_Callback(void);
void TIM6_Callback(void);
void TIM7_Callback(void);
void StartCurrentProvidingLoop(void);
void StopCurrentProvidingLoop(PauseReason reason);
void switchOnBuzzer(void);

extern TIM_HandleTypeDef* phtim8;

#ifdef __cplusplus
}
#endif

#endif /* STATEMACHINE_INC_STATEMACHINE_H_ */
