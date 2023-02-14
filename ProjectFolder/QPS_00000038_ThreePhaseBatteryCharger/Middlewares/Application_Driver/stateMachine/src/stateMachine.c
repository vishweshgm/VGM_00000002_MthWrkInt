/*
 * stateMachine.c
 *
 *  Created on: Feb 23, 2020
 *      Author: vishweshgm
 */
#define STATE_MACHINE
//#undef STATE_MACHINE
#ifdef STATE_MACHINE
#include "main.h"
#include "eeprom.h"
#include "display_state.h"
#include "appl_diag.h"
#include "stateMachine.h"
#include "string.h"
#include "math.h"
#include "eeprom_external.h"
#include "cmsis_os.h"
#include "vishwaAdc.h"
#include "usbd_cdc_if.h"
/*extern variables*/
extern EEP_DataStructure Appl_EEPParam;
extern DiagFaultTable Appl_diagActiveFault;
extern SM_DisplayParam Appl_Display;
extern SM_DisplayStruct Display;
extern DiagFaultParamTable FaultParamTable[Diag_enum_MaxFault];
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart5;
extern I2C_HandleTypeDef hi2c1;

volatile uint8_t Appl_u8SelectedGate;
static inline void ScheduleTrigger(uint8_t Fnclocal_u8SelectedGate);
static inline void DisplayWindowSelection(void);

/*<------------------------------------USER VARIABLES SECTION---------------------------------------------->*/

float current_offset = 0;
volatile float Appl_fltPhasePowerDelivery;
uint8_t Appl_u8ManualPhaseSetting = 1;

volatile uint32_t Appl_u32AngleInTime;


/*>>>>>>>>>SCR-Voltage & Current Control Module variables<<<<<<<<<<<<<<<<*/
uint32_t Appl_u8HighCurrentCounter = 0,Appl_u8LowCurrentCounter = 0;
volatile uint32_t Appl_u32BeginTime_Halfwave,  Appl_u32DiffTime_Halfwave;
volatile uint16_t Appl_u16ToggleGateFlag = 0;
volatile uint8_t Appl_u8ErrorCounter1, Appl_u8ErrorCounter2;
TIM_HandleTypeDef* phtim5 = &htim5;
TIM_HandleTypeDef* phtim6 = &htim6;
TIM_HandleTypeDef* phtim7 = &htim7;
TIM_HandleTypeDef* phtim8 = &htim8;

double Appl_dblPhaseTrackertime_sec, Appl_dblPhaseTrackerangle_degree;
double Appl_dblPhaseSetValue_degree = 0;
volatile uint32_t Appl_u32TimeatZCStart,Appl_u32TimeSinceLastZC, Appl_u32LastZCTime;
float Appl_fltACTimePeriod,Appl_fltFrequencyofZC;
uint32_t Appl_u32extratimetaken;
uint32_t Appl_u32LiveTimeForNoACDetection, Appl_u32DiffTimeForNoACDetection;
uint32_t Appl_u32ACDiag_DiffBtwFallEdgetime_ZC1, Appl_u32ACDiag_DiffBtwFallEdgetime_ZC2,Appl_u32ACDiag_DiffBtwFallEdgetime_ZC3;
uint8_t Appl_u8ACDiagIgnoreThisPulse_ZC1, Appl_u8ACDiagIgnoreThisPulse_ZC2, Appl_u8ACDiagIgnoreThisPulse_ZC3;


float Appl_fltRoundedAvgVoltage_V;
/*>>>>>>>>>ECU State Handling variables<<<<<<<<<<<<<<<<*/

volatile SM_ECUState_Enum SM_enumECUState;
SM_MainApplParameters SM_ApplParam;
float output;
float error;
//keypress_EnumButton SM_KeyPressed;
uint8_t Appl_u8Toggle = 0;
static uint32_t ExtEEP_ErrorCounter;
static uint32_t Malloc_ErrorCounter;
External_EEP_DataStructure VishExtEepRead;

#if 0
/*sujit office*/
float INITIAL_START_GAIN		=	(0.0000050);
float STABLE_GAIN			=		(0.000100);
#endif
#ifdef __cplusplus
extern "C"{
#endif


void SM_ECUInit(void){
	/*Vishwesh : At the start I shut off DC Contactor, All LEDs,
	 * Gates must be off - very important else transistor becomes short circuit path for current
	 * Buzzer theme is played for 2-seconds and then Splash screens displayed.
	 */
	CAPCONTACTOR_OFF();
	DC_CONTACTOR_OFF();
	REDLEDON();
	GREENLEDON();
	BLUELEDON();
	ORANGELEDON();
	G1T_OFF();
	G2T_OFF();
	G3T_OFF();
	switchOnBuzzer();
	VishwaAdcInit();

	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_Base_Start(&htim4);
//	HAL_TIM_Base_Start(&htim5);
	HAL_TIM_Base_Start(&htim8);
	vishwaEEp_Init();
	ReadEEPData();

	SM_ApplParam.Appl_u16PulseSCRWidth_us = 100;
	SM_enumECUState = SM_enum_ECU_Init;
	Appl_Display.PowerOffCounter = 6;

	HAL_Delay(1000);
	BUZZER_OFF();
	REDLEDOFF();
	GREENLEDOFF();
	BLUELEDOFF();
	ORANGELEDOFF();

	/* Vishwesh:
	 * Important: In-case of power Failure, A Li-On Battery is given
	 * This is supplied using Main Relay and switching Main relay off causes
	 * System to switch OFF. Here I am switching ON relay.
	 */
	BATTERY_RELAY_ON();
	SM_DispInit();

	/* Vishwesh:
	 * Assume there is not phase reversal in the beginning.
	 */
	Appl_diagActiveFault.PhaseReversal = 0;
	SM_ApplParam.ap_fltCCSlowGain = INITIAL_CCSLOW_GAIN;
	SM_ApplParam.ap_fltCCFastGain = INITIAL_CCFAST_GAIN;
	SM_ApplParam.ap_fltCVSlowGain = INITIAL_CVSLOW_GAIN;
	SM_ApplParam.ap_fltCVFastGain = INITIAL_CVFAST_GAIN;
	/* Vishwesh :
	 * If Modbus to be supported use following library. THank me later
	 */
//	vishwaModbusInit();
}

void SM_ListenFromDisplay(uint8_t loopms){
	static uint8_t timer;
	static uint8_t timer2;

	timer++;
	if(DGUS_OK != ReadPageID()){
		Display.DGUSParam.PageId = 0;
		Display.DGUSParam.ErrorCounter++;
	}
	if(timer >= (500/loopms)){
		timer = 0;
		if(DGUS_OK != UpdateReadVariables()){
			Display.DGUSParam.ErrorCounter++;
		}
	}

}

void SM_StateMachineHandler(uint8_t loopms){

	static uint32_t Fnclocal_u32local;
	static uint32_t localruntimrctr;
	uint8_t delimiter;
	static uint32_t TotalNumOfrecords;

	ExtEEP_Error EEP_Error;
	VishFuncError VishwaErr;

	if(((Appl_diagActiveFault.PowerFail_Z1 == 1) || \
			(Appl_diagActiveFault.PowerFail_Z2 == 1) || \
			(Appl_diagActiveFault.PowerFail_Z3 == 1))&&\
			((SM_enumECUState != SM_enum_ECU_CCRun_Slow) && \
			(SM_enumECUState != SM_enum_ECU_CCRun_Stable))){

		BUZZER_OFF();
		SM_ApplParam.BuzzerCounter++;
		if(SM_ApplParam.BuzzerCounter>(1000/loopms)){
			BUZZER_ON();
			SM_ApplParam.BuzzerCounter = 0;
			Appl_Display.PowerOffCounter--;
			if(Appl_Display.PowerOffCounter <= 0){
				Appl_Display.PowerOffCounter = 6;
				BATTERY_RELAY_OFF();
			}
		}
	}
	else
	{
		/*Vishwesh
		 * If there is no powerfail, let's charge battery and reset poweroff counter
		 */
		BATTERY_RELAY_ON();
		Appl_Display.PowerOffCounter = 6;

		if(Display.DGUSParam.PageId == DGUS_PAGEID_MAINMENU){
			SM_enumECUState = SM_enum_ECU_StandByMode;
		}

		if(SM_enumECUState == SM_enum_ECU_Init){

			Fnclocal_u32local++;
			if(Fnclocal_u32local > (5000/loopms)){

				if(Appl_EEPParam.stm32.EEP_PowerFailedLastTime == 0){
					SM_enumECUState = SM_enum_ECU_StandByMode;
					Appl_Display.SelectedIndexIntheList = 1;
				}
				else{
					SM_enumECUState = SM_enum_ECU_ContinueFromLastPowerOff;
					Appl_Display.PowerOnCounter = 20;
					output = 0;
				}
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_StandByMode){
			/*Vishwesh:
			 * Basic Behaviour of this Mode*/
			Appl_EEPParam.stm32.EEP_PowerFailedLastTime = 0;
			Appl_Display.SelfTestCounter = 0;
			WriteEEPData();
			DC_CONTACTOR_OFF();
			CAPCONTACTOR_OFF();
			BUZZER_OFF();


			/*Vishwesh:
			 * StateMachineHandle of this mode*/
			/*Display.DGUSParam.PageId is updated from display */
			if(Display.DGUSParam.PageId == DGUS_PAGEID_CURRENTMODESETTINGS){
				SM_enumECUState = SM_enum_ECU_CurrentModeSettings;
			}
			else if(Display.DGUSParam.PageId == DGUS_PAGEID_SELFTEST){
				SM_enumECUState = SM_enum_ECU_SelfTest;
			}
			else if(Display.DGUSParam.PageId == DGUS_PAGEID_SETTINGS){
				SM_enumECUState = SM_enum_ECU_SettingMenu;
			}
			else if(Display.DGUSParam.PageId == DGUS_PAGEID_VOLTAGEMODESETTINGS){
				SM_enumECUState = SM_enum_ECU_VoltageModeSettings;
			}
			else if(Display.DGUSParam.PageId == DGUS_PAGEID_VISHWAMODE){
				SM_enumECUState = SM_enum_ECU_VishwatestMode;
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_CurrentModeSettings){
			Appl_EEPParam.stm32.EEP_CustomerNumber          = (uint8_t)Display.DGUSParam.CustomerID;
			Appl_EEPParam.stm32.EEP_CC_SetCurrent           = (uint8_t)Display.DGUSParam.setCurrent;
			Appl_EEPParam.stm32.EEP_TimerSetHours           = (uint8_t)Display.DGUSParam.setTimerHour;
			Appl_EEPParam.stm32.EEP_CC_CutOffVoltage       = (uint8_t)Display.DGUSParam.cutOffVoltage;

			if(Display.DGUSParam.PageId == DGUS_PAGEID_CCPREPARING){
				Appl_EEPParam.stm32.EEP_Run_Hour = 0;
				Appl_EEPParam.stm32.EEP_Run_Min = 0;
				Appl_EEPParam.stm32.EEP_Run_Sec = 0;
				Appl_EEPParam.stm32.EEP_ModeSelected = CC_MODE;
				SM_enumECUState = SM_enum_ECU_CCPreparing;
			}
			else if(Display.DGUSParam.PageId == DGUS_PAGEID_MAINMENU){
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}

		}
		else if(SM_enumECUState == SM_enum_ECU_VoltageModeSettings){
			Appl_EEPParam.stm32.EEP_CustomerNumber         = (uint8_t)Display.DGUSParam.CustomerID;
			Appl_EEPParam.stm32.EEP_CV_SetVoltage          = (uint8_t)Display.DGUSParam.setVoltage;
			Appl_EEPParam.stm32.EEP_TimerSetHours          = (uint8_t)Display.DGUSParam.setTimerHour;
			Appl_EEPParam.stm32.EEP_CV_CutOffCurrent       = (uint8_t)Display.DGUSParam.cutOffCurrent;
			if(Display.DGUSParam.PageId == DGUS_PAGEID_CVPREPARING){
				Appl_EEPParam.stm32.EEP_Run_Hour = 0;
				Appl_EEPParam.stm32.EEP_Run_Min = 0;
				Appl_EEPParam.stm32.EEP_Run_Sec = 0;
				Appl_EEPParam.stm32.EEP_ModeSelected = CV_MODE;
				SM_enumECUState = SM_enum_ECU_CVPreparing;
			}
			else if(Display.DGUSParam.PageId == DGUS_PAGEID_MAINMENU){
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}

		}
		else if(SM_enumECUState == SM_enum_ECU_CVPreparing){
			if(Display.DGUSParam.PageId == DGUS_PAGEID_CVPREPARING){
				if(Appl_diagActiveFault.PowerFail_Z1 != 1 &&
						Appl_diagActiveFault.PowerFail_Z2 != 1 &&
						Appl_diagActiveFault.PowerFail_Z3 != 1 &&
						Appl_diagActiveFault.BatteryReversal != 1 &&
						Appl_diagActiveFault.EEPLogMemoryFull != 1 &&
						Appl_diagActiveFault.HighTempperatureSCR != 1 &&
						Appl_diagActiveFault.OverVoltage != 1 &&
						Appl_diagActiveFault.PhaseReversal != 1 &&
						Appl_diagActiveFault.FiltercktOpen !=1 &&
						Appl_diagActiveFault.UnderVoltage != 1 &&
						Appl_diagActiveFault.OverCurrent != 1){
					StartSelftestCVPrepare(loopms);
				}
				else{
					if((Appl_diagActiveFault.PowerFail_Z1 == 1) || (Appl_diagActiveFault.PowerFail_Z2 == 1) || (Appl_diagActiveFault.PowerFail_Z3 == 1))
						SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ1;
					else if(Appl_diagActiveFault.BatteryReversal == 1) SM_ApplParam.Pausereason = PauseReasonEnum_BatteryReversal;
					else if(Appl_diagActiveFault.EEPLogMemoryFull == 1) SM_ApplParam.Pausereason = PauseReasonEnum_EEPLogMemoryFull;
					else if(Appl_diagActiveFault.UnderVoltage == 1) SM_ApplParam.Pausereason = PauseReasonEnum_UnderVoltage;
					else if(Appl_diagActiveFault.FiltercktOpen == 1) SM_ApplParam.Pausereason = PauseReasonEnum_FilterCircuitOpen;
					else if(Appl_diagActiveFault.HighTempperatureSCR == 1) SM_ApplParam.Pausereason = PauseReasonEnum_HighTemp;
					else if(Appl_diagActiveFault.OverVoltage == 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverVoltage;
					else if(Appl_diagActiveFault.PhaseReversal == 1) SM_ApplParam.Pausereason = PauseReasonEnum_PhaseReversal;
					else if(Appl_diagActiveFault.OverCurrent == 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverCurrent;
					SM_enumECUState = SM_enum_ECU_CVPauseMode;
				}
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_CCPreparing){
			BUZZER_OFF();
			if(Display.DGUSParam.PageId == DGUS_PAGEID_CCPREPARING){
				if(Appl_diagActiveFault.PowerFail_Z1 != 1 &&
						Appl_diagActiveFault.PowerFail_Z2 != 1 &&
						Appl_diagActiveFault.PowerFail_Z3 != 1 &&
						Appl_diagActiveFault.BatteryReversal != 1 &&
						Appl_diagActiveFault.EEPLogMemoryFull != 1 &&
						Appl_diagActiveFault.HighTempperatureSCR != 1 &&
						Appl_diagActiveFault.OverVoltage != 1 &&
						Appl_diagActiveFault.PhaseReversal != 1 &&
						Appl_diagActiveFault.FiltercktOpen !=1 &&
						Appl_diagActiveFault.UnderVoltage != 1 &&
						Appl_diagActiveFault.OverCurrent != 1){
					StartSelftestCCPrepare(loopms);
				}
				else{
					if((Appl_diagActiveFault.PowerFail_Z1 == 1) ||
							(Appl_diagActiveFault.PowerFail_Z2 == 1) ||
							(Appl_diagActiveFault.PowerFail_Z3 == 1))
						SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ1;
					else if(Appl_diagActiveFault.BatteryReversal == 1) SM_ApplParam.Pausereason = PauseReasonEnum_BatteryReversal;
					else if(Appl_diagActiveFault.EEPLogMemoryFull == 1) SM_ApplParam.Pausereason = PauseReasonEnum_EEPLogMemoryFull;
					else if(Appl_diagActiveFault.UnderVoltage == 1) SM_ApplParam.Pausereason = PauseReasonEnum_UnderVoltage;
					else if(Appl_diagActiveFault.FiltercktOpen == 1) SM_ApplParam.Pausereason = PauseReasonEnum_FilterCircuitOpen;
					else if(Appl_diagActiveFault.HighTempperatureSCR == 1) SM_ApplParam.Pausereason = PauseReasonEnum_HighTemp;
					else if(Appl_diagActiveFault.OverVoltage == 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverVoltage;
					else if(Appl_diagActiveFault.PhaseReversal == 1) SM_ApplParam.Pausereason = PauseReasonEnum_PhaseReversal;
					else if(Appl_diagActiveFault.OverCurrent == 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverCurrent;
					SM_enumECUState = SM_enum_ECU_CCPauseMode;
				}
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_CCRun_Slow){
			localruntimrctr++;
			SM_ApplParam.BuzzerCounter++;
			if(SM_ApplParam.BuzzerCounter > (1000/loopms) && SM_ApplParam.Buzzertoggle == 0){
				BUZZER_OFF();
				SM_ApplParam.Buzzertoggle = 1;
				SM_ApplParam.BuzzerCounter = 0;
			}
			if(localruntimrctr >= (1000/loopms)){
				localruntimrctr = 0;
				Update_Usage_hour_eep();
				SafeRunCCMonitoring();
				Update_RunningTime_eep();
			}
			if(SM_enumECUState != SM_enum_ECU_CCPauseMode){

				if(Display.DGUSParam.PageId == DGUS_PAGEID_CCPAUSED){
					SM_enumECUState = SM_enum_ECU_CCPauseMode;
					SM_ApplParam.Pausereason = PauseReasonEnum_UserPaused;
					StopCurrentProvidingLoop(SM_ApplParam.Pausereason);
				}
			}

		}
		else if(SM_enumECUState == SM_enum_ECU_CCRun_Stable){
			localruntimrctr++;
			SM_ApplParam.BuzzerCounter++;
			if(SM_ApplParam.BuzzerCounter > (1000/loopms) && SM_ApplParam.Buzzertoggle == 0){
				BUZZER_OFF();
				SM_ApplParam.Buzzertoggle = 1;
				SM_ApplParam.BuzzerCounter = 0;
			}
			if(localruntimrctr >= (1000/loopms)){
				localruntimrctr = 0;
				Update_Usage_hour_eep();
				SafeRunCCMonitoring();
				Update_RunningTime_eep();
			}

			if(SM_enumECUState != SM_enum_ECU_CCPauseMode){
				if(Display.DGUSParam.PageId == DGUS_PAGEID_CCPAUSED){
					SM_enumECUState = SM_enum_ECU_CCPauseMode;
					SM_ApplParam.Pausereason = PauseReasonEnum_UserPaused;
					StopCurrentProvidingLoop(SM_ApplParam.Pausereason);
				}
			}

		}
		else if(SM_enumECUState == SM_enum_ECU_CVRun_Slow){
			localruntimrctr++;
			SM_ApplParam.BuzzerCounter++;
			if(SM_ApplParam.BuzzerCounter > (1000/loopms) && SM_ApplParam.Buzzertoggle == 0){
				BUZZER_OFF();
				SM_ApplParam.Buzzertoggle = 1;
				SM_ApplParam.BuzzerCounter = 0;
			}
			if(localruntimrctr >= (1000/loopms)){
				localruntimrctr = 0;
				Update_Usage_hour_eep();
				SafeRunCVMonitoring();
				Update_RunningTime_eep();
			}
			if(SM_enumECUState != SM_enum_ECU_CVPauseMode){
				if(Display.DGUSParam.PageId == DGUS_PAGEID_CVPAUSED){
					SM_enumECUState = SM_enum_ECU_CVPauseMode;
					SM_ApplParam.Pausereason = PauseReasonEnum_UserPaused;
					StopCurrentProvidingLoop(SM_ApplParam.Pausereason);
				}
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_CVRun_Stable){
			localruntimrctr++;
			SM_ApplParam.BuzzerCounter++;
			if(SM_ApplParam.BuzzerCounter > (1000/loopms) && SM_ApplParam.Buzzertoggle == 0){
				BUZZER_OFF();
				SM_ApplParam.Buzzertoggle = 1;
				SM_ApplParam.BuzzerCounter = 0;
			}
			if(localruntimrctr >= (1000/loopms)){
				localruntimrctr = 0;
				Update_Usage_hour_eep();
				SafeRunCVMonitoring();
				Update_RunningTime_eep();
			}
			if(SM_enumECUState != SM_enum_ECU_CVPauseMode){
				if(Display.DGUSParam.PageId == DGUS_PAGEID_CVPAUSED){
					SM_enumECUState = SM_enum_ECU_CVPauseMode;
					SM_ApplParam.Pausereason = PauseReasonEnum_UserPaused;
					StopCurrentProvidingLoop(SM_ApplParam.Pausereason);
				}
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_CCPauseMode){
			CAPCONTACTOR_OFF();
			DC_CONTACTOR_OFF();

			SM_ApplParam.ap_PrepareMode = 0;
			SM_ApplParam.ap_PrepareCounter1 = 0;
			SM_ApplParam.ap_PrepareCounter0 = 0;

			if(Display.DGUSParam.PageId == DGUS_PAGEID_SPLASHSCREEN1){
				SM_enumECUState = SM_enum_ECU_CCPreparing;
			}
			else if(Display.DGUSParam.PageId == DGUS_PAGEID_MAINMENU){
				SM_enumECUState = SM_enum_ECU_StandByMode;
				Appl_EEPParam.stm32.EEP_PowerFailedLastTime = 0;
			}
			if(SM_ApplParam.Pausereason == PauseReasonEnum_UserPaused){
				SM_ApplParam.BuzzerCounter++;
				if(SM_ApplParam.BuzzerCounter > (1000/loopms) && SM_ApplParam.Buzzertoggle == 0){
					BUZZER_OFF();
					SM_ApplParam.Buzzertoggle = 1;
					SM_ApplParam.BuzzerCounter = 0;
				}
			}
			else{
				SM_ApplParam.BuzzerCounter++;
				if((SM_ApplParam.BuzzerCounter % (100/loopms)) == 0 && SM_ApplParam.Buzzertoggle == 0){
					BUZZER_OFF();
					if(SM_ApplParam.BuzzerCounter % (200/loopms) == 0){
						BUZZER_ON();
						if(SM_ApplParam.BuzzerCounter > (2500/loopms)){
							SM_ApplParam.Buzzertoggle = 1;
							SM_ApplParam.BuzzerCounter = 0;
							BUZZER_OFF();
						}
					}
				}
			}
			WriteEEPData();
		}
		else if(SM_enumECUState == SM_enum_ECU_CVPauseMode){

			CAPCONTACTOR_OFF();
			DC_CONTACTOR_OFF();
			SM_ApplParam.ap_PrepareCounter1 = 0;
			SM_ApplParam.ap_PrepareCounter0 = 0;
			SM_ApplParam.ap_PrepareMode = 0;
			if(Display.DGUSParam.PageId == DGUS_PAGEID_SPLASHSCREEN1){
				SM_enumECUState = SM_enum_ECU_CVPreparing;
			}
			else if(Display.DGUSParam.PageId == DGUS_PAGEID_MAINMENU){
				SM_enumECUState = SM_enum_ECU_StandByMode;
				Appl_EEPParam.stm32.EEP_PowerFailedLastTime = 0;
			}
			if(SM_ApplParam.Pausereason == PauseReasonEnum_UserPaused){
				SM_ApplParam.BuzzerCounter++;
				if(SM_ApplParam.BuzzerCounter > (1000/loopms) && SM_ApplParam.Buzzertoggle == 0){
					BUZZER_OFF();
					SM_ApplParam.Buzzertoggle = 1;
					SM_ApplParam.BuzzerCounter = 0;
				}
			}
			else{
				SM_ApplParam.BuzzerCounter++;
				if((SM_ApplParam.BuzzerCounter % (100/loopms)) == 0 && SM_ApplParam.Buzzertoggle == 0){
					BUZZER_OFF();
					if(SM_ApplParam.BuzzerCounter % (200/loopms) == 0){
						BUZZER_ON();
						if(SM_ApplParam.BuzzerCounter > (2500/loopms)){
							SM_ApplParam.Buzzertoggle = 1;
							SM_ApplParam.BuzzerCounter = 0;
							BUZZER_OFF();
						}
					}
				}
			}
			WriteEEPData();
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu){
			/*Vishwesh:
			 * Basic Behaviour of this Mode*/
			WriteEEPData();
			DC_CONTACTOR_OFF();
			BUZZER_OFF();
			REDLEDON();
			GREENLEDOFF();
			ORANGELEDOFF();
			BLUELEDOFF();

			if(Display.DGUSParam.PageId == DGUS_PAGEID_SETTINGDATATRANSFER){
				SM_enumECUState = SM_enum_ECU_SettingMenu_Transfer;
			}
			else if(Display.DGUSParam.PageId == DGUS_PAGEID_SETTINGERASE){
				SM_enumECUState = SM_enum_ECU_SettingMenu_Erase;
			}
			else if(Display.DGUSParam.PageId == DGUS_PAGEID_MAINMENU){
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}

		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Transfer){
			if(Display.DGUSParam.PageId == DGUS_PAGEID_SETTIGNDATAOPERATIONON){
				SM_enumECUState = SM_enum_ECU_SettingMenu_Completed;
				Appl_Display.SelectedSubIndex = 1;
				SM_ApplParam.Transfer_PageloopCount = 1;
				SM_ApplParam.Transfer_OffsetCounter = 0;
				SM_ApplParam.ExtEepTotalRecordsTotransferred = (uint16_t)((uint16_t)Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH<<8 | Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL);
				TotalNumOfrecords = SM_ApplParam.ExtEepTotalRecordsTotransferred;
			}
			if(Display.DGUSParam.PageId == DGUS_PAGEID_SETTINGS){
				SM_enumECUState = SM_enum_ECU_SettingMenu;
			}
			if(Display.DGUSParam.PageId == DGUS_PAGEID_MAINMENU){
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Erase){
			if(Display.DGUSParam.PageId == DGUS_PAGEID_SETTIGNDATAOPERATIONON){
				Appl_Display.SelectedSubIndex = 2;
				SM_ApplParam.Transfer_PageloopCount = 1;
				SM_ApplParam.Transfer_OffsetCounter = 0;
				SM_enumECUState = SM_enum_ECU_SettingMenu_Completed;
			}
			if(Display.DGUSParam.PageId == DGUS_PAGEID_SETTINGS){
				SM_enumECUState = SM_enum_ECU_SettingMenu;
			}
			if(Display.DGUSParam.PageId == DGUS_PAGEID_MAINMENU){
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Completed){

			ORANGELEDON();
			if(Appl_Display.SelectedSubIndex == 1){

				/*Vishwesh:
				 * SM_ApplParam.Transfer_OffsetCounter is the offset address of each record in a page.
				 * For the current implementation, it can take values 0,16,32,..240.
				 * Once it reaches 240, data must be read --> then increment the page --> Reset the offset to 0
				 */


				if(SM_ApplParam.ExtEepTotalRecordsTotransferred > 0 ){
					/*Step 1: Read 1 Record*/
					Appl_EEPParam.stm32.ExtEepAddress = 0xA0;
					EEP_Error = EEPROM_Read(SM_ApplParam.Transfer_PageloopCount,SM_ApplParam.Transfer_OffsetCounter,&VishExtEepRead.bytes[0],1*NUM_OF_BYTES_IN_A_EXTEEP_RECORD,Appl_EEPParam.stm32.ExtEepAddress);
					if(EEP_Error == EXTEEP_OK){
						/*Step 2: Increment offset to next record*/
						SM_ApplParam.Transfer_OffsetCounter += NUM_OF_BYTES_IN_A_EXTEEP_RECORD;
						/*Step 3: Decrement offset to next record*/
						SM_ApplParam.ExtEepTotalRecordsTotransferred-=1;
						SM_ApplParam.ap_fltDataPercent = SM_ApplParam.ExtEepTotalRecordsTotransferred*100.0/((float)TotalNumOfrecords);
						SM_ApplParam.ap_fltDataPercent = 100-SM_ApplParam.ap_fltDataPercent;
						VishwaErr = MapExteepToVishwaStruct();

						if(VishwaErr == VISH_OK){

						}
						else{
							Malloc_ErrorCounter++;
						}

						/*step 6 : */
						if(SM_ApplParam.Transfer_OffsetCounter >= ATMEL_EEP_CHIP_PAGE_SIZE){
							SM_ApplParam.Transfer_OffsetCounter = 0;
							SM_ApplParam.Transfer_PageloopCount++;
						}
					}
					else{
						ExtEEP_ErrorCounter++;
						/*ResetI2c*/
						(&hi2c1)->Instance->CR1 |= I2C_CR1_SWRST;
						(&hi2c1)->Instance->CR1 &= ~I2C_CR1_SWRST;

						if(ExtEEP_ErrorCounter >= 30){
							ExtEEP_ErrorCounter = 0;
							SM_ApplParam.ExtEepTotalRecordsTotransferred = 0;
						}
					}

				}
				else{
					SM_enumECUState = SM_enum_ECU_SettingMenu;
				}
			}
			else if(Appl_Display.SelectedSubIndex == 2){
				ResetRecords();
				WriteEEPData();
				Appl_EEPParam.stm32.ExtEepAddress = 0xA0;
				EEP_Error = EEPROM_PageErase(SM_ApplParam.Transfer_PageloopCount,Appl_EEPParam.stm32.ExtEepAddress);
				if(EEP_Error == EXTEEP_OK){
					SM_ApplParam.Transfer_PageloopCount++;
					SM_ApplParam.ap_fltDataPercent = SM_ApplParam.Transfer_PageloopCount*100.0/255.0;
				}
				else{
					ExtEEP_ErrorCounter++;
					/*ResetI2c*/
					(&hi2c1)->Instance->CR1 |= I2C_CR1_SWRST;
					(&hi2c1)->Instance->CR1 &= ~I2C_CR1_SWRST;

					if(ExtEEP_ErrorCounter >= 30){
						ExtEEP_ErrorCounter = 0;
						SM_ApplParam.Transfer_PageloopCount = 0xFF;
					}
				}
				if(SM_ApplParam.Transfer_PageloopCount >= 0xFF){
					SM_enumECUState = SM_enum_ECU_SettingMenu;
				}

			}
		}
		else if(SM_enumECUState == SM_enum_ECU_SelfTest){
			/*Vishwesh:
			 * StateMachineHandle of this mode*/
			if(Display.DGUSParam.PageId == DGUS_PAGEID_MAINMENU){
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}

			if(Appl_Display.SelfTestCounter > (1000/loopms)){
				if(Appl_diagActiveFault.FiltercktOpen == 0){
					if(Appl_Display.SelfTestCounter > (2000/loopms)){
						if(Appl_diagActiveFault.BatteryReversal == 0){
							DC_CONTACTOR_ON();
						}
					}
				}
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_VishwatestMode){
			if(Display.DGUSParam.PageId == DGUS_PAGEID_MAINMENU){
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
			if(Appl_diagActiveFault.BatteryReversal == 0){
				DC_CONTACTOR_ON();
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			if(Display.DGUSParam.PageId == DGUS_PAGEID_MAINMENU){
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}

			BUZZER_OFF();
			SM_ApplParam.BuzzerCounter++;
			if(SM_ApplParam.BuzzerCounter>(1000/loopms)){
				BUZZER_ON();
				SM_ApplParam.BuzzerCounter =0;
				Appl_Display.PowerOnCounter--;
				if(Appl_Display.PowerOnCounter <= 0){
					if(Appl_EEPParam.stm32.EEP_ModeSelected == CC_MODE){
						SM_enumECUState = SM_enum_ECU_CCPreparing;
					}
					else{
						SM_enumECUState = SM_enum_ECU_CVPreparing;
					}
				}
			}
		}
	}
	DisplayWindowSelection();
}

static inline void DisplayWindowSelection(void){
	/* DISPLAY WINDOW SELECTION */

//	if(0){
	if((Appl_diagActiveFault.PowerFail_Z1 == 1) || \
			(Appl_diagActiveFault.PowerFail_Z3 == 1)){
		Display.DisplayState = SM_enum_Display_PoweringOffWindow;
	}
	else
		if(SM_enumECUState == SM_enum_ECU_Init){
			Display.DisplayState = SM_enum_Display_POWERON_LOGO;
		}
		else if(SM_enumECUState == SM_enum_ECU_StandByMode){
			Display.DisplayState = SM_enum_Display_MAIN_MENU;
		}
		else if((SM_enumECUState == SM_enum_ECU_CurrentModeSettings)){
			Display.DisplayState = SM_enum_Display_CurrentModeSettings;
		}
		else if(SM_enumECUState == SM_enum_ECU_CCRun_Stable){
			Display.DisplayState = SM_enum_Display_CCRun_Stable;
		}
		else if(SM_enumECUState == SM_enum_ECU_CCRun_Slow){
			Display.DisplayState = SM_enum_Display_CCRun_Slow;
		}
		else if(SM_enumECUState == SM_enum_ECU_SelfTest){
			Display.DisplayState = SM_enum_Display_SelfTest;
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu){
			Display.DisplayState = SM_enum_Display_Settings_Menu;
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu){
			Display.DisplayState = SM_enum_Display_Settings_Menu;
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Transfer){
			Display.DisplayState = SM_enum_Display_Settings_Menu_Transfer;
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Erase){
			Display.DisplayState = SM_enum_Display_Settings_Menu_Erase;
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Completed){
			Display.DisplayState = SM_enum_Display_Settings_Menu_Completed;
		}
		else if(SM_enumECUState == SM_enum_ECU_VishwatestMode){
			Display.DisplayState = SM_enum_Display_VishwaTest;
		}
		else if(SM_enumECUState == SM_enum_ECU_AutoModeUserInput){
			Display.DisplayState = SM_enum_Display_AutoModeUserInput;
		}
		else if(SM_enumECUState == SM_enum_ECU_StatusDisplay){
			Display.DisplayState = SM_enum_Display_StatusDisplay;
		}
		else if(SM_enumECUState == SM_enum_ECU_CCPauseMode){
			Display.DisplayState = SM_enum_Display_CCPause;
		}
		else if(SM_enumECUState == SM_enum_ECU_CVPauseMode){
			Display.DisplayState = SM_enum_Display_CVPause;
		}
		else if(SM_enumECUState == SM_enum_ECU_Info){
			Display.DisplayState = SM_enum_Display_Info;
		}
		else if(SM_enumECUState == SM_enum_ECU_VoltageModeSettings){
			Display.DisplayState = SM_enum_Display_VoltageModeSettings;
		}
		else if(SM_enumECUState == SM_enum_ECU_CVRun_Slow){
			Display.DisplayState = SM_enum_Display_CVRun_Slow;
		}
		else if(SM_enumECUState == SM_enum_ECU_CVRun_Stable){
			Display.DisplayState = SM_enum_Display_CVRun_Stable;
		}
		else if(SM_enumECUState == SM_enum_ECU_CVPreparing){
			Display.DisplayState = SM_enum_Display_CVPreparing;
		}
		else if(SM_enumECUState == SM_enum_ECU_CCPreparing){
			Display.DisplayState = SM_enum_Display_CCPreparing;
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			Display.DisplayState = SM_enum_Display_StoppedWhileRunning;
		}

}

void SM_SpeakToDisplay(uint8_t loopms){
	DisplayLoop(loopms);
}

void SM_10msTask(void){


}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
/*CONFIDENTIAL*/
}

static inline void ScheduleTrigger(uint8_t Fnclocal_u8SelectedGate){

	if(\
			(Appl_dblPhaseSetValue_degree > SCR_ANGLE_TRIGGER_MIN && Appl_dblPhaseSetValue_degree < SCR_ANGLE_TRIGGER_MAX) && \
			(Appl_diagActiveFault.PowerFail_Z1 == 0) && \
			(Appl_diagActiveFault.PowerFail_Z2 == 0) && \
			(Appl_diagActiveFault.PowerFail_Z3 == 0) && \
			((SM_enumECUState == SM_enum_ECU_CCRun_Slow) || \
			 (SM_enumECUState == SM_enum_ECU_CCRun_Stable) || \
			 (SM_enumECUState == SM_enum_ECU_CVRun_Slow) || \
			 (SM_enumECUState == SM_enum_ECU_CVRun_Stable) ||\
			 (SM_enumECUState == SM_enum_ECU_CVPreparing) || \
			 (SM_enumECUState == SM_enum_ECU_CCPreparing))
	){
		/*CONFIDENTIAL*/
	}
	else if(\
			(Appl_dblPhaseSetValue_degree >0 && Appl_dblPhaseSetValue_degree <= 360) && \
			(Appl_diagActiveFault.PowerFail_Z1 == 0) && \
			(Appl_diagActiveFault.PowerFail_Z2 == 0) && \
			(Appl_diagActiveFault.PowerFail_Z3 == 0) && \
			((SM_enumECUState == SM_enum_ECU_VishwatestMode))
	){
		/*CONFIDENTIAL*/
	}
	else{
		/*CONFIDENTIAL*/
	}
}

void TIM5_Callback(void){
	/*CONFIDENTIAL*/
}

void TIM6_Callback(void){
/*CONFIDENTIAL*/
}

void TIM7_Callback(void){
	/*CONFIDENTIAL*/
}

volatile float Fnclocal_fltTargetVoltage, Fnclocal_fltTargetCurrent;
void Appl_AdcPhaseAngleDetermination(void){
	/*CONFIDENTIAL*/
}

float PID_Current(float gain){
	error = (float)Appl_EEPParam.stm32.EEP_CC_SetCurrent - SM_ApplParam.RealtimeCurrent;
	output = output + error * gain;
	if(output >= PID_CURRENT_MAX) output = PID_CURRENT_MAX;
	if(output <= PID_CURRENT_MIN) output = PID_CURRENT_MIN;
	return output;
}

float PID_Voltage(float gain){

	error = (float)Appl_EEPParam.stm32.EEP_CV_SetVoltage - SM_ApplParam.RealtimeVoltage;
	output = output + error * gain;
	if(output >= PID_VOLTAGE_MAX) output = PID_VOLTAGE_MAX;
	if(output <= PID_VOLTAGE_MIN) output = PID_VOLTAGE_MIN;
	return output;
}


float PID_PrepareVoltage(void){/*CONFIDENTIAL*/}

float cvmode_cc_error;
float ccmode_cv_error;

float PID_CVMode_CCRun(float gain){

	cvmode_cc_error = (float)Appl_EEPParam.stm32.EEP_CV_CutOffCurrent - SM_ApplParam.RealtimeCurrent;

	if(cvmode_cc_error<0){
		output = output + cvmode_cc_error * gain;
	}
	else{
		return 0;
	}

	if(output >= PID_VOLTAGE_MAX) output = PID_VOLTAGE_MAX;
	if(output <= PID_VOLTAGE_MIN) output = PID_VOLTAGE_MIN;
	return output;
}

float PID_CCMode_CVRun(float gain){

	ccmode_cv_error = (float)Appl_EEPParam.stm32.EEP_CC_CutOffVoltage - SM_ApplParam.RealtimeVoltage;

	if(ccmode_cv_error<0){
		output = output + ccmode_cv_error * gain;
	}
	else{
		return 0;
	}

	if(output >= PID_VOLTAGE_MAX) output = PID_VOLTAGE_MAX;
	if(output <= PID_VOLTAGE_MIN) output = PID_VOLTAGE_MIN;
	return output;
}

void delay_us (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim4,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim4) < us);  // wait for the counter to reach the us input in the parameter
	return;
}



void SafeRunCCMonitoring(void){
	Appl_EEPParam.stm32.EEP_PowerFailedLastTime = 1;

	if(Appl_diagActiveFault.PowerFail_Z1 				!= 1 &&
			Appl_diagActiveFault.PowerFail_Z2 			!= 1 &&
			Appl_diagActiveFault.PowerFail_Z3 			!= 1 &&
			Appl_diagActiveFault.BatteryReversal 		!= 1 &&
			Appl_diagActiveFault.EEPLogMemoryFull 		!= 1 &&
			Appl_diagActiveFault.HighTempperatureSCR 	!= 1 &&
			Appl_diagActiveFault.OverVoltage 			!= 1 &&
			Appl_diagActiveFault.PhaseReversal 			!= 1 &&
			Appl_diagActiveFault.FiltercktOpen 			!= 1 &&
			Appl_diagActiveFault.UnderVoltage 			!= 1 &&
			Appl_diagActiveFault.OverCurrent 			!= 1){

		}
	else{
		if((Appl_diagActiveFault.PowerFail_Z1 == 1) ||
				(Appl_diagActiveFault.PowerFail_Z2 == 1) ||
				(Appl_diagActiveFault.PowerFail_Z3 == 1))
		SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ1;
		else if(Appl_diagActiveFault.BatteryReversal 		== 1) SM_ApplParam.Pausereason = PauseReasonEnum_BatteryReversal;
		else if(Appl_diagActiveFault.EEPLogMemoryFull 		== 1) SM_ApplParam.Pausereason = PauseReasonEnum_EEPLogMemoryFull;
		else if(Appl_diagActiveFault.UnderVoltage 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_UnderVoltage;
		else if(Appl_diagActiveFault.FiltercktOpen 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_FilterCircuitOpen;
		else if(Appl_diagActiveFault.HighTempperatureSCR 	== 1) SM_ApplParam.Pausereason = PauseReasonEnum_HighTemp;
		else if(Appl_diagActiveFault.OverVoltage 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverVoltage;
		else if(Appl_diagActiveFault.PhaseReversal 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_PhaseReversal;
		else if(Appl_diagActiveFault.OverCurrent == 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverCurrent;
		SM_enumECUState = SM_enum_ECU_CCPauseMode;
		StopCurrentProvidingLoop(SM_ApplParam.Pausereason);
	}


	if(Appl_EEPParam.stm32.EEP_Run_Hour == Appl_EEPParam.stm32.EEP_TimerSetHours){
		SM_ApplParam.Pausereason = PauseReasonEnum_TimerComplete;
		SM_enumECUState = SM_enum_ECU_CCPauseMode;
		Appl_EEPParam.stm32.EEP_Run_Hour = 0;
		Appl_EEPParam.stm32.EEP_Run_Min = 0;
		Appl_EEPParam.stm32.EEP_Run_Sec = 0;
		Appl_EEPParam.stm32.EEP_PowerFailedLastTime = 0;
		StopCurrentProvidingLoop(SM_ApplParam.Pausereason);
	}

}
void SafeRunCVMonitoring(void){
	Appl_EEPParam.stm32.EEP_PowerFailedLastTime = 1;
	if(Appl_diagActiveFault.PowerFail_Z1 				!= 1 &&
			Appl_diagActiveFault.PowerFail_Z2 			!= 1 &&
			Appl_diagActiveFault.PowerFail_Z3 			!= 1 &&
			Appl_diagActiveFault.BatteryReversal 		!= 1 &&
			Appl_diagActiveFault.EEPLogMemoryFull 		!= 1 &&
			Appl_diagActiveFault.HighTempperatureSCR 	!= 1 &&
			Appl_diagActiveFault.OverVoltage 			!= 1 &&
			Appl_diagActiveFault.PhaseReversal 			!= 1 &&
			Appl_diagActiveFault.FiltercktOpen 			!= 1 &&
			Appl_diagActiveFault.UnderVoltage 			!= 1 &&
			Appl_diagActiveFault.OverCurrent 			!= 1){

		}
	else{
		if((Appl_diagActiveFault.PowerFail_Z1 == 1) ||
				(Appl_diagActiveFault.PowerFail_Z2 == 1) ||
				(Appl_diagActiveFault.PowerFail_Z3 == 1))
		SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ1;
		else if(Appl_diagActiveFault.BatteryReversal 		== 1) SM_ApplParam.Pausereason = PauseReasonEnum_BatteryReversal;
		else if(Appl_diagActiveFault.EEPLogMemoryFull 		== 1) SM_ApplParam.Pausereason = PauseReasonEnum_EEPLogMemoryFull;
		else if(Appl_diagActiveFault.UnderVoltage 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_UnderVoltage;
		else if(Appl_diagActiveFault.FiltercktOpen 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_FilterCircuitOpen;
		else if(Appl_diagActiveFault.HighTempperatureSCR 	== 1) SM_ApplParam.Pausereason = PauseReasonEnum_HighTemp;
		else if(Appl_diagActiveFault.OverVoltage 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverVoltage;
		else if(Appl_diagActiveFault.PhaseReversal 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_PhaseReversal;
		else if(Appl_diagActiveFault.OverCurrent == 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverCurrent;
		SM_enumECUState = SM_enum_ECU_CVPauseMode;
		StopVoltageProvidingLoop(SM_ApplParam.Pausereason);
	}


	if(Appl_EEPParam.stm32.EEP_Run_Hour == Appl_EEPParam.stm32.EEP_TimerSetHours){
		SM_ApplParam.Pausereason = PauseReasonEnum_TimerComplete;
		SM_enumECUState = SM_enum_ECU_CVPauseMode;
		Appl_EEPParam.stm32.EEP_Run_Hour = 0;
		Appl_EEPParam.stm32.EEP_Run_Min = 0;
		Appl_EEPParam.stm32.EEP_Run_Sec = 0;
		Appl_EEPParam.stm32.EEP_PowerFailedLastTime = 0;
		StopVoltageProvidingLoop(SM_ApplParam.Pausereason);
	}

}
void StartCurrentProvidingLoop(void){
	if(Appl_diagActiveFault.PowerFail_Z1 				!= 1 &&
			Appl_diagActiveFault.PowerFail_Z2 			!= 1 &&
			Appl_diagActiveFault.PowerFail_Z3 			!= 1 &&
			Appl_diagActiveFault.BatteryReversal 		!= 1 &&
			Appl_diagActiveFault.EEPLogMemoryFull 		!= 1 &&
			Appl_diagActiveFault.HighTempperatureSCR 	!= 1 &&
			Appl_diagActiveFault.OverVoltage 			!= 1 &&
			Appl_diagActiveFault.PhaseReversal 			!= 1 &&
			Appl_diagActiveFault.FiltercktOpen 			!= 1 &&
			Appl_diagActiveFault.UnderVoltage 			!= 1 &&
			Appl_diagActiveFault.OverCurrent 			!= 1){
		SM_enumECUState = SM_enum_ECU_CCRun_Slow;
		DC_CONTACTOR_ON();
		HAL_GPIO_WritePin(uC_LedOnBoard_GPIO_Port, uC_LedOnBoard_Pin,GPIO_PIN_SET);
		WriteEEPData();
		BUZZER_ON();
		SM_ApplParam.Buzzertoggle = 0;
		SM_ApplParam.BuzzerCounter = 0;
	}
	else{
		if((Appl_diagActiveFault.PowerFail_Z1 == 1) ||
				(Appl_diagActiveFault.PowerFail_Z2 == 1) ||
				(Appl_diagActiveFault.PowerFail_Z3 == 1))
		SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ1;
		else if(Appl_diagActiveFault.BatteryReversal 		== 1) SM_ApplParam.Pausereason = PauseReasonEnum_BatteryReversal;
		else if(Appl_diagActiveFault.EEPLogMemoryFull 		== 1) SM_ApplParam.Pausereason = PauseReasonEnum_EEPLogMemoryFull;
		else if(Appl_diagActiveFault.UnderVoltage 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_UnderVoltage;
		else if(Appl_diagActiveFault.FiltercktOpen 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_FilterCircuitOpen;
		else if(Appl_diagActiveFault.HighTempperatureSCR 	== 1) SM_ApplParam.Pausereason = PauseReasonEnum_HighTemp;
		else if(Appl_diagActiveFault.OverVoltage 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverVoltage;
		else if(Appl_diagActiveFault.PhaseReversal 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_PhaseReversal;
		else if(Appl_diagActiveFault.OverCurrent 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverCurrent;
		SM_enumECUState = SM_enum_ECU_CVPauseMode;
	}
}

void StartVoltageProvidingLoop(void){
	if(Appl_diagActiveFault.PowerFail_Z1 				!= 1 &&
			Appl_diagActiveFault.PowerFail_Z2 			!= 1 &&
			Appl_diagActiveFault.PowerFail_Z3 			!= 1 &&
			Appl_diagActiveFault.BatteryReversal 		!= 1 &&
			Appl_diagActiveFault.EEPLogMemoryFull 		!= 1 &&
			Appl_diagActiveFault.HighTempperatureSCR 	!= 1 &&
			Appl_diagActiveFault.OverVoltage 			!= 1 &&
			Appl_diagActiveFault.PhaseReversal 			!= 1 &&
			Appl_diagActiveFault.FiltercktOpen 			!= 1 &&
			Appl_diagActiveFault.UnderVoltage 			!= 1 &&
			Appl_diagActiveFault.OverCurrent 			!= 1){
		SM_enumECUState = SM_enum_ECU_CVRun_Slow;
		DC_CONTACTOR_ON();
		HAL_GPIO_WritePin(uC_LedOnBoard_GPIO_Port, uC_LedOnBoard_Pin,GPIO_PIN_SET);
		WriteEEPData();
		BUZZER_ON();
		SM_ApplParam.Buzzertoggle = 0;
		SM_ApplParam.BuzzerCounter = 0;
	}
	else{
		if((Appl_diagActiveFault.PowerFail_Z1 == 1) ||
				(Appl_diagActiveFault.PowerFail_Z2 == 1) ||
				(Appl_diagActiveFault.PowerFail_Z3 == 1))
		SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ1;
		else if(Appl_diagActiveFault.BatteryReversal 		== 1) SM_ApplParam.Pausereason = PauseReasonEnum_BatteryReversal;
		else if(Appl_diagActiveFault.EEPLogMemoryFull 		== 1) SM_ApplParam.Pausereason = PauseReasonEnum_EEPLogMemoryFull;
		else if(Appl_diagActiveFault.UnderVoltage 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_UnderVoltage;
		else if(Appl_diagActiveFault.FiltercktOpen 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_FilterCircuitOpen;
		else if(Appl_diagActiveFault.HighTempperatureSCR 	== 1) SM_ApplParam.Pausereason = PauseReasonEnum_HighTemp;
		else if(Appl_diagActiveFault.OverVoltage 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverVoltage;
		else if(Appl_diagActiveFault.PhaseReversal 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_PhaseReversal;
		else if(Appl_diagActiveFault.OverCurrent 			== 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverCurrent;
		SM_enumECUState = SM_enum_ECU_CVPauseMode;
	}
}

void StopCurrentProvidingLoop(PauseReason reason){
	REDLEDON();
	GREENLEDOFF();
	HAL_GPIO_WritePin(uC_LedOnBoard_GPIO_Port, uC_LedOnBoard_Pin,GPIO_PIN_RESET);
	CaptureExtEEPSnapshot_SingleShot(reason);
	WriteEEPData();
	BUZZER_ON();
	SM_ApplParam.Buzzertoggle = 0;
	SM_ApplParam.BuzzerCounter = 0;
}

void StopVoltageProvidingLoop(PauseReason reason){
	REDLEDON();
	GREENLEDOFF();
//	DC_CONTACTOR_OFF(); /*Purposefully removed from here because, the capacitor used to hold a lot of voltage*/
	HAL_GPIO_WritePin(uC_LedOnBoard_GPIO_Port, uC_LedOnBoard_Pin,GPIO_PIN_RESET);
	CaptureExtEEPSnapshot_SingleShot(reason);
	WriteEEPData();
	BUZZER_ON();
	SM_ApplParam.Buzzertoggle = 0;
	SM_ApplParam.BuzzerCounter = 0;
}

void switchOnBuzzer(void){
	static uint8_t offtime = 125;
	static uint8_t ontime = 75;
	ALL_INDICATORS_ON();
	HAL_Delay(ontime);
	ALL_INDICATORS_OFF();
	HAL_Delay(offtime);
	ALL_INDICATORS_ON();
	HAL_Delay(ontime);
	ALL_INDICATORS_OFF();
	HAL_Delay(offtime);
	ALL_INDICATORS_ON();
	HAL_Delay(ontime);
	ALL_INDICATORS_OFF();
	HAL_Delay(offtime);

	ALL_INDICATORS_ON();
	HAL_Delay(500);
	ALL_INDICATORS_OFF();
	HAL_Delay(250);
	ALL_INDICATORS_ON();
	HAL_Delay(500);
	ALL_INDICATORS_OFF();
	HAL_Delay(250);

	ALL_INDICATORS_ON();
	HAL_Delay(ontime);
	ALL_INDICATORS_OFF();
	HAL_Delay(offtime);
	ALL_INDICATORS_ON();
	HAL_Delay(ontime);
	ALL_INDICATORS_OFF();
	HAL_Delay(offtime);
	ALL_INDICATORS_ON();
	HAL_Delay(ontime);
	ALL_INDICATORS_OFF();
	HAL_Delay(offtime);
}


void StartSelftestCVPrepare(uint8_t loopms){
	/*Vishwesh:
	 * Stage 1 : Turn DC Contactor ON.
	 * Stage 2 : Wait 5-second to settle battery voltage.
	 * Stage 3 : Check for under or over voltage errors, Pause if fault active
	 * Stage 4 : Note down the battery voltage.
	 * Stage 5 : Disconnect the DC Contactor
	 * Stage 6 : Connect the Filter bank
	 * Stage 7 : slowly charge it till 90% of Noted battery voltage
	 * Stage 8 : Turn DC contactor on and start loop
	 */
	SM_ApplParam.ap_PrepareCounter0++;
	if(SM_ApplParam.ap_PrepareCounter1 == 0){
		DC_CONTACTOR_ON();
	}
	if(SM_ApplParam.ap_PrepareCounter0 >= (2500/loopms)){
		if(SM_ApplParam.ap_PrepareCounter1 == 0){
			SM_ApplParam.InitialBatteryVoltage = SM_ApplParam.RealtimeVoltage;
//			SM_ApplParam.InitialBatteryVoltage = (float)Appl_EEPParam.stm32.EEP_CV_SetVoltage;
			DC_CONTACTOR_OFF();
		}
		SM_ApplParam.ap_PrepareCounter1++;
		if(SM_ApplParam.ap_PrepareCounter1 >= (2500/loopms)){
			DC_CONTACTOR_OFF();
			CAPCONTACTOR_ON();
			/*Vishwesh:
			 * I use SM_ApplParam.ap_PrepareMode flag to handle
			 * SCR trigger to happen only when
			 * Capacitor contactor is on. Else do not trigger SCR.
			 */
			SM_ApplParam.ap_PrepareMode = 1;
			SM_ApplParam.PreparePercentage = (SM_ApplParam.RealtimeVoltage/SM_ApplParam.InitialBatteryVoltage*0.7)*100;
			if(SM_ApplParam.RealtimeVoltage > (SM_ApplParam.InitialBatteryVoltage * 0.7)){
				StartVoltageProvidingLoop();
				SM_ApplParam.ap_PrepareCounter1 = 0;
				SM_ApplParam.ap_PrepareCounter0 = 0;
				SM_ApplParam.ap_PrepareMode = 0;
				SM_ApplParam.PreparePercentage = 0;
			}
		}
	}
}

void StartSelftestCCPrepare(uint8_t loopms){
	/*Vishwesh:
	 * Stage 1 : Turn DC Contactor ON.
	 * Stage 2 : Wait 5-second to settle battery voltage.
	 * Stage 3 : Check for under or over voltage errors, Pause if fault active
	 * Stage 4 : Note down the battery voltage.
	 * Stage 5 : Disconnect the DC Contactor
	 * Stage 6 : Connect the Filter bank
	 * Stage 7 : slowly charge it till 90% of Noted battery voltage
	 * Stage 8 : Turn DC contactor on and start loop
	 */
	SM_ApplParam.ap_PrepareCounter0++;
	if(SM_ApplParam.ap_PrepareCounter1 == 0){
		DC_CONTACTOR_ON();
	}
	if(SM_ApplParam.ap_PrepareCounter0 >= (2500/loopms)){
		if(SM_ApplParam.ap_PrepareCounter1 == 0){
			SM_ApplParam.InitialBatteryVoltage = SM_ApplParam.RealtimeVoltage;
//			SM_ApplParam.InitialBatteryVoltage = (float)Appl_EEPParam.stm32.EEP_CC_CutOffVoltage;
			DC_CONTACTOR_OFF();
		}
		SM_ApplParam.ap_PrepareCounter1++;
		if(SM_ApplParam.ap_PrepareCounter1 >= (2500/loopms)){
			DC_CONTACTOR_OFF();
			CAPCONTACTOR_ON();
			SM_ApplParam.ap_PrepareMode = 1;
			SM_ApplParam.PreparePercentage = (SM_ApplParam.RealtimeVoltage/(SM_ApplParam.InitialBatteryVoltage*0.7))*100;
			if(SM_ApplParam.RealtimeVoltage > (SM_ApplParam.InitialBatteryVoltage*0.7)){
				StartCurrentProvidingLoop();
				SM_ApplParam.ap_PrepareCounter1 = 0;
				SM_ApplParam.ap_PrepareCounter0 = 0;
				SM_ApplParam.ap_PrepareMode = 0;
				SM_ApplParam.PreparePercentage = 0;

			}
		}
	}
}

#ifdef __cplusplus
}
#endif

#endif
