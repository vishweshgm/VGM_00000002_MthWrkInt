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
#include "keypress.h"
#include "appl_diag.h"
#include "stateMachine.h"
#include "string.h"
#include "math.h"
#include "eeprom_external.h"
#include "cmsis_os.h"
#include "vishwaAdc.h"
/*extern variables*/
extern EEP_DataStructure Appl_EEPParam;
extern DiagFaultTable Appl_diagActiveFault;
extern SM_DisplayParam Appl_Display;
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
float Appl_fltPhasePowerDelivery;
uint8_t Appl_u8ManualPhaseSetting = 1;

volatile uint32_t Appl_u32AngleInTime;
static uint32_t ExtEEP_ErrorCounter;

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

//SM_DisplayState_Enum SM_enumDisplayState;
volatile SM_ECUState_Enum SM_enumECUState;
SM_MainApplParameters SM_ApplParam;
float output;
float error;
//keypress_EnumButton SM_KeyPressed;
uint8_t Appl_u8Toggle = 0;
static float kgain = 0.0005;
extern keypress_EnumButton SM_KeyPressed;
External_EEP_DataStructure sendBuffer[NUM_OF_ATMEL_RECORDS_IN_A_PAGE];
uint8_t uartSendBytesBuffer[NUM_OF_BYTES_IN_A_EXTEEP_RECORD*3+1];/*+1 signifies delimiter '|'*/

#if 1
/*sujit office*/
float INITIAL_START_GAIN		=	(0.0000050);
float STABLE_GAIN			=		(1.0e-4);
#endif
#if 0
//float INITIAL_START_GAIN		=	(0.0000250);
//float STABLE_GAIN			=		(0.0000500);


//float INITIAL_START_GAIN		=	(1e-6);
//float STABLE_GAIN			=		(1e-4);

#endif
#ifdef __cplusplus
extern "C"{
#endif


void SM_ECUInit(void){
	/*Vishwesh : At the start I shut off DC Contactor, All LEDs,
	 * Gates must be off - very important else transistor becomes short circuit path for current
	 * Buzzer theme is played for 2-seconds and then Splash screens displayed.
	 */
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
	HAL_TIM_Base_Start(&htim5);
	HAL_TIM_Base_Start(&htim8);
	vishwaEEp_Init();
	ReadEEPData();

	/*Vishwesh:
	 * While Testing use following values
	 */

//	SM_ApplParam.OverVoltageThreshold = 350;
	SM_ApplParam.UnderVoltageThreshold = UNDERVOLTAGE_ADMIN_VALUE;
	SM_ApplParam.OverCurrentThreshold = OVERCURRENT_ADMIN_VALUE;

	SM_ApplParam.Appl_u16PulseSCRWidth_us = 200;
	SM_enumECUState = SM_enum_ECU_Init;
	Appl_Display.PowerOffCounter = 0;

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
	/* Vishwesh :
	 * If Modbus to be supported use following library. THank me later
	 */
//	vishwaModbusInit();
}



void SM_StateMachineHandler(uint8_t loopms){

	static uint32_t Fnclocal_u32local;
	static uint32_t localruntimrctr;
	uint8_t delimiter;
	static uint8_t Fnclocal_uartBuffer[4];
	static uint8_t Fnclocal_char;

	ExtEEP_Error EEP_Error;
	VishFuncError VishwaErr;

	if(((Appl_diagActiveFault.PowerFail_Z1 == 1) || \
			(Appl_diagActiveFault.PowerFail_Z2 == 1) || \
			(Appl_diagActiveFault.PowerFail_Z3 == 1))&&\
			((SM_enumECUState != SM_enum_ECU_VldAC_RunCurrentLoop_increaseVoltageMode_Slow) && \
			(SM_enumECUState != SM_enum_ECU_VldAC_RunCurrentLoop_StableVoltagefloat))){

		BUZZER_OFF();
		SM_ApplParam.BuzzerCounter++;
		if(SM_ApplParam.BuzzerCounter>(1000/loopms)){
			BUZZER_ON();
			SM_ApplParam.BuzzerCounter = 0;
			Appl_Display.PowerOffCounter++;
			if(Appl_Display.PowerOffCounter >= 5){
				Appl_Display.PowerOffCounter = 0;
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
		Appl_Display.PowerOffCounter = 0;


		if(SM_enumECUState == SM_enum_ECU_Init){
			Fnclocal_u32local++;
			if(Fnclocal_u32local > (5000/loopms)){
				if(Appl_EEPParam.stm32.EEP_PowerFailedLastTime == 0){
					SM_enumECUState = SM_enum_ECU_StandByMode;
					Appl_Display.SelectedIndexIntheList = 1;
				}
				else{
					SM_enumECUState = SM_enum_ECU_ContinueFromLastPowerOff;
					Appl_Display.PowerOnCounter = 0;
					output = 0;
				}
			}
		}

		else if(SM_enumECUState == SM_enum_ECU_StandByMode){
			/*Vishwesh:
			 * Basic Behaviour of this Mode*/
			WriteEEPData();
			DC_CONTACTOR_OFF();
			BUZZER_OFF();
			REDLEDON();
			GREENLEDOFF();
			ORANGELEDOFF();
			BLUELEDOFF();
			/*Vishwesh:
			 * Button Handling of this mode*/
			if(SM_KeyPressed == CMD_Enum_B4) DEC(Appl_Display.SelectedIndexIntheList,1,1);
			if(SM_KeyPressed == CMD_Enum_B3) INC(Appl_Display.SelectedIndexIntheList,Appl_Display.MaximumIndexInTheList,1);
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				MAP_SM(1,SM_enum_ECU_RejuvenationMenu);
				MAP_SM(2,SM_enum_ECU_SettingMenu);
				MAP_SM(3,SM_enum_ECU_SelfTest);
				MAP_SM(4,SM_enum_ECU_Info);
				MAP_SM(5,SM_enum_ECU_VishwatestMode);
				Appl_Display.SelectedIndexIntheList = 1;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_Info;
			}
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

			if(SM_KeyPressed == CMD_Enum_B4) DEC(Appl_Display.SelectedIndexIntheList,1,1);
			if(SM_KeyPressed == CMD_Enum_B3) INC(Appl_Display.SelectedIndexIntheList,Appl_Display.MaximumIndexInTheList,1);
			if(SM_KeyPressed == CMD_Enum_B2){
				MAP_SM(1,SM_enum_ECU_SettingMenu_Transfer);
				MAP_SM(2,SM_enum_ECU_SettingMenu_Erase);
				MAP_SM(3,SM_enum_ECU_SettingMenu_MaxVoltage);
				Appl_Display.SelectedSubIndex = 1;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				Appl_Display.SelectedIndexIntheList = 1;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
		}

		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Transfer){
			if(SM_KeyPressed == CMD_Enum_B4){}
			if(SM_KeyPressed == CMD_Enum_B3){}
			if(SM_KeyPressed == CMD_Enum_B2){
				Appl_Display.SelectedSubIndex = 1;
				SM_ApplParam.Transfer_PageloopCount = 1;
				SM_ApplParam.Transfer_OffsetCounter = 0;
				SM_ApplParam.ExtEepTotalRecordsTotransferred = (uint16_t)((uint16_t)Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberH<<8 | Appl_EEPParam.stm32.EEP_ExternalEEPRecordSerialNumberL);
				SM_enumECUState = SM_enum_ECU_SettingMenu_Completed;
				delimiter = '$';
				HAL_UART_Transmit(&huart1, &delimiter, 1, loopms);
			}
			if(SM_KeyPressed == CMD_Enum_B1){SM_enumECUState = SM_enum_ECU_SettingMenu;}
		}

		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Erase){
			if(SM_KeyPressed == CMD_Enum_B4){}
			if(SM_KeyPressed == CMD_Enum_B3){}
			if(SM_KeyPressed == CMD_Enum_B2){
				Appl_Display.SelectedSubIndex = 2;
				SM_ApplParam.Transfer_PageloopCount = 1;
				SM_ApplParam.Transfer_OffsetCounter = 0;
				SM_enumECUState = SM_enum_ECU_SettingMenu_Completed;
			}
			if(SM_KeyPressed == CMD_Enum_B1){SM_enumECUState = SM_enum_ECU_SettingMenu;}
		}

		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Completed){
			if(SM_KeyPressed == CMD_Enum_B4){}
			if(SM_KeyPressed == CMD_Enum_B3){}
			if(SM_KeyPressed == CMD_Enum_B2){}
			if(SM_KeyPressed == CMD_Enum_B1){}
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
					EEP_Error = EEPROM_Read(SM_ApplParam.Transfer_PageloopCount,SM_ApplParam.Transfer_OffsetCounter,&sendBuffer[0].bytes[0],1*NUM_OF_BYTES_IN_A_EXTEEP_RECORD,Appl_EEPParam.stm32.ExtEepAddress);
					if(EEP_Error == EXTEEP_OK){
						/*Step 2: Increment offset to next record*/
						SM_ApplParam.Transfer_OffsetCounter += NUM_OF_BYTES_IN_A_EXTEEP_RECORD;
						/*Step 3: Decrement offset to next record*/
						SM_ApplParam.ExtEepTotalRecordsTotransferred-=1;
						/*step 4: there are 16bytes of hex numbers. convert all these numbers into
						 * ascii format. Since each number is 1 byte, maximum of 3-bytes required
						 * for each hex-byte. (i.e 0x255 = '2''5''5')
						 * last byte added '|' is wall delimiter
						 * So total 16*3+1 bytes are sent per record through RS-232
						 * If you know number of records avaialble, you can predict how much bytes are sent*/
						for(uint8_t i = 0, j=0; i<(NUM_OF_BYTES_IN_A_EXTEEP_RECORD);i++,j+=3){
							Fnclocal_uartBuffer[0] = (sendBuffer[0].bytes[i]/100)+0x30;
							Fnclocal_char = (sendBuffer[0].bytes[i])%100;
							Fnclocal_uartBuffer[1] = (Fnclocal_char/10)+0x30;
							Fnclocal_uartBuffer[2] = (Fnclocal_char%10)+0x30;
							uartSendBytesBuffer[j+0] = Fnclocal_uartBuffer[0];
							uartSendBytesBuffer[j+1] = Fnclocal_uartBuffer[1];
							uartSendBytesBuffer[j+2] = Fnclocal_uartBuffer[2];
						}
						uartSendBytesBuffer[NUM_OF_BYTES_IN_A_EXTEEP_RECORD*3] = '|';
						/*step 5: Transmit via UART*/
						HAL_UART_Transmit(&huart1, &uartSendBytesBuffer[0], 3*NUM_OF_BYTES_IN_A_EXTEEP_RECORD+1, loopms);
						/*step 6 : */
						if(SM_ApplParam.Transfer_OffsetCounter >= 256){
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
					delimiter = '#';
					HAL_UART_Transmit(&huart1, &delimiter, 1, loopms);
				}
			}
			else if(Appl_Display.SelectedSubIndex == 2){
				ResetRecords();
				WriteEEPData();
				Appl_EEPParam.stm32.ExtEepAddress = 0xA0;
				EEP_Error = EEPROM_PageErase(SM_ApplParam.Transfer_PageloopCount,Appl_EEPParam.stm32.ExtEepAddress);
				if(EEP_Error == EXTEEP_OK){
					SM_ApplParam.Transfer_PageloopCount++;

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

		else if(SM_enumECUState == SM_enum_ECU_RejuvenationMenu){
			if(SM_KeyPressed == CMD_Enum_B4){
				if(Appl_Display.SelectedIndexIntheList == 1) INC(Appl_EEPParam.stm32.EEP_CustomerNumber,100,1);
				if(Appl_Display.SelectedIndexIntheList == 2) DEC(Appl_EEPParam.stm32.EEP_RejuvenationCycleID,1,1);
				if(Appl_Display.SelectedIndexIntheList == 3) INC(SM_ApplParam.UserSetCurrent,60,1);
				if(Appl_Display.SelectedIndexIntheList == 4) INC(Appl_EEPParam.stm32.EEP_TimerSetHours,60,1);
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				if(Appl_Display.SelectedIndexIntheList == 1) DEC(Appl_EEPParam.stm32.EEP_CustomerNumber,0,1);
				if(Appl_Display.SelectedIndexIntheList == 2) INC(Appl_EEPParam.stm32.EEP_RejuvenationCycleID,6,1);
				if(Appl_Display.SelectedIndexIntheList == 3) DEC(SM_ApplParam.UserSetCurrent,4,1);
				if(Appl_Display.SelectedIndexIntheList == 4) DEC(Appl_EEPParam.stm32.EEP_TimerSetHours,0,1);
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				INC(Appl_Display.SelectedIndexIntheList,Appl_Display.MaximumIndexInTheList+1,1);
				if(Appl_Display.SelectedIndexIntheList >Appl_Display.MaximumIndexInTheList){
					Appl_Display.SelectedIndexIntheList = 1;
					Appl_EEPParam.stm32.EEP_Run_Hour = 0;
					Appl_EEPParam.stm32.EEP_Run_Min = 0;
					Appl_EEPParam.stm32.EEP_Run_Sec = 0;
					StartCurrentProvidingLoop();
				}
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				DEC(Appl_Display.SelectedIndexIntheList,0,1);
				MAP_SM(0,SM_enum_ECU_StandByMode);
			}

		}

		else if((SM_enumECUState == SM_enum_ECU_VldAC_RunCurrentLoop_increaseVoltageMode_Slow)){
			localruntimrctr++;
			SM_ApplParam.BuzzerCounter++;
			if(SM_ApplParam.BuzzerCounter > (1000/loopms) && SM_ApplParam.Buzzertoggle == 0){
				BUZZER_OFF();
				SM_ApplParam.Buzzertoggle = 1;
				SM_ApplParam.BuzzerCounter = 0;
			}
			if(localruntimrctr >= (1000/loopms)){
				localruntimrctr = 0;
				MonitorTemperature();
				Update_Usage_hour_eep();
				SafeRunMonitoring();
				Update_RunningTime_eep();

			}

			if(SM_KeyPressed == CMD_Enum_B4){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_PauseMode;
				SM_ApplParam.Pausereason = PauseReasonEnum_UserPaused;
				StopCurrentProvidingLoop(SM_ApplParam.Pausereason);
			}
		}

		else if(SM_enumECUState == SM_enum_ECU_VldAC_RunCurrentLoop_StableVoltagefloat){
			localruntimrctr++;
			SM_ApplParam.BuzzerCounter++;
			if(SM_ApplParam.BuzzerCounter > (1000/loopms) && SM_ApplParam.Buzzertoggle == 0){
				BUZZER_OFF();
				SM_ApplParam.Buzzertoggle = 1;
				SM_ApplParam.BuzzerCounter = 0;
			}
			if(localruntimrctr >= (1000/loopms)){
				localruntimrctr = 0;
				MonitorTemperature();
				Update_Usage_hour_eep();
				SafeRunMonitoring();
				Update_RunningTime_eep();
			}
			if(SM_KeyPressed == CMD_Enum_B4){}
			if(SM_KeyPressed == CMD_Enum_B3){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_PauseMode;
				SM_ApplParam.Pausereason = PauseReasonEnum_UserPaused;
				StopCurrentProvidingLoop(SM_ApplParam.Pausereason);
			}
		}

		else if(SM_enumECUState == SM_enum_ECU_SelfTest){
			if(SM_KeyPressed == CMD_Enum_B4){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
			if(Appl_diagActiveFault.BatteryReversal == 0){
				DC_CONTACTOR_ON();
			}
			localruntimrctr++;
			if(localruntimrctr >= (500/loopms)){
				GREENLEDON();
				if(localruntimrctr >= (600/loopms)){
					REDLEDON();
					if(localruntimrctr >= (700/loopms)){
						ORANGELEDON();
						if(localruntimrctr >= (800/loopms)){
							BLUELEDON();
							if(localruntimrctr >= (1000/loopms)){
								localruntimrctr = 0;
								REDLEDOFF();
								ORANGELEDOFF();
								GREENLEDOFF();
								BLUELEDOFF();
							}
						}
					}
				}
			}
		}

		else if(SM_enumECUState == SM_enum_ECU_VishwatestMode){
			if(SM_KeyPressed == CMD_Enum_B4){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;

			}
			if(Appl_diagActiveFault.BatteryReversal == 0){
				DC_CONTACTOR_ON();
			}

		}

		else if(SM_enumECUState == SM_enum_ECU_PauseMode){


			if(SM_KeyPressed == CMD_Enum_B4){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_VldAC_RunCurrentLoop_increaseVoltageMode_Slow;
				StartCurrentProvidingLoop();
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
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

		else if(SM_enumECUState == SM_enum_ECU_Info){
			if(SM_KeyPressed == CMD_Enum_B4){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}
		}

		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			if(SM_KeyPressed == CMD_Enum_B4){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_VldAC_RunCurrentLoop_increaseVoltageMode_Slow;
				StartCurrentProvidingLoop();
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_StandByMode;
			}

			BUZZER_OFF();
			SM_ApplParam.BuzzerCounter++;
			if(SM_ApplParam.BuzzerCounter>(1000/loopms)){
				BUZZER_ON();
				SM_ApplParam.BuzzerCounter =0;
				Appl_Display.PowerOnCounter++;
				if(Appl_Display.PowerOnCounter >= 20){
					SM_enumECUState = SM_enum_ECU_VldAC_RunCurrentLoop_increaseVoltageMode_Slow;
					StartCurrentProvidingLoop();
				}
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_MaxVoltage){
			if(SM_KeyPressed == CMD_Enum_B4){
				INC(SM_ApplParam.OverVoltageThreshold,270,1);
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				DEC(SM_ApplParam.OverVoltageThreshold,48,1);
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_SettingMenu;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
				SM_enumECUState = SM_enum_ECU_SettingMenu;
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			if(SM_KeyPressed == CMD_Enum_B4){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			if(SM_KeyPressed == CMD_Enum_B4){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			if(SM_KeyPressed == CMD_Enum_B4){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			if(SM_KeyPressed == CMD_Enum_B4){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B3){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B2){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
			if(SM_KeyPressed == CMD_Enum_B1){
				SM_KeyPressed = CMD_Enum_DEFAULT;
			}
		}
	}
	DisplayWindowSelection();


}

void SM_10msTask(void){
	Run_Diagnostic_Actions();

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	/*CONFIDENTIAL*/

}


static inline void ScheduleTrigger(uint8_t Fnclocal_u8SelectedGate){/*CONFIDENTIAL*/}


void TIM5_Callback(void){/*CONFIDENTIAL*/}

void TIM6_Callback(void){/*CONFIDENTIAL*/}

void TIM7_Callback(void){/*CONFIDENTIAL*/}

void Appl_AdcPhaseAngleDetermination(void){
/*CONFIDENTIAL*/
	}

float PID_Current(float gain){
	error = SM_ApplParam.targetPIDCurrent - SM_ApplParam.RealtimeCurrent;
	output = output + error * gain;
	if(output >= PID_CURRENT_MAX) output = PID_CURRENT_MAX;
	if(output <= PID_CURRENT_MIN) output = PID_CURRENT_MIN;
	return output;
}

#if 0
void TIM3_Callback(){
	static uint32_t Appl_u32TimerTicks;
	Appl_u32TimerTicks++;
}
#endif

void delay_us (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim4,0);  // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(&htim4) < us);  // wait for the counter to reach the us input in the parameter
	return;
}



void MonitorTemperature(void){
	if(Appl_diagActiveFault.HighTempperatureSCR == 1){
		SM_ApplParam.targetPIDCurrent = SM_ApplParam.HighTemperatureCurrent;
		SM_enumECUState = SM_enum_ECU_VldAC_RunCurrentLoop_increaseVoltageMode_Slow;
	}
	else
		SM_ApplParam.targetPIDCurrent = SM_ApplParam.UserSetCurrent;
}

void SafeRunMonitoring(void){
	Appl_EEPParam.stm32.EEP_PowerFailedLastTime = 1;

	if(Appl_diagActiveFault.PowerFail_Z1 != 1 &&
				Appl_diagActiveFault.PowerFail_Z2 != 1 &&
				Appl_diagActiveFault.PowerFail_Z3 != 1 &&
				Appl_diagActiveFault.BatteryReversal != 1 &&
				Appl_diagActiveFault.EEPLogMemoryFull != 1 &&
				Appl_diagActiveFault.OverVoltage != 1 &&
				Appl_diagActiveFault.UnderVoltage != 1 &&
				Appl_diagActiveFault.OverCurrent != 1){

		}
	else{
		if(Appl_diagActiveFault.PowerFail_Z1 == 1) SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ1;
		else if(Appl_diagActiveFault.PowerFail_Z2 == 1) SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ2;
		else if(Appl_diagActiveFault.PowerFail_Z3 == 1) SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ3;
		else if(Appl_diagActiveFault.OverVoltage == 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverVoltage;
		else if(Appl_diagActiveFault.BatteryReversal == 1) SM_ApplParam.Pausereason = PauseReasonEnum_BatteryReversal;
		else if(Appl_diagActiveFault.EEPLogMemoryFull == 1) SM_ApplParam.Pausereason = PauseReasonEnum_EEPLogMemoryFull;
		else if(Appl_diagActiveFault.UnderVoltage == 1) SM_ApplParam.Pausereason = PauseReasonEnum_UnderVoltage;
		else if(Appl_diagActiveFault.OverCurrent == 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverCurrent;
		StopCurrentProvidingLoop(SM_ApplParam.Pausereason);
		SM_enumECUState = SM_enum_ECU_PauseMode;
	}


	if(Appl_EEPParam.stm32.EEP_Run_Hour == Appl_EEPParam.stm32.EEP_TimerSetHours){
		SM_ApplParam.Pausereason = PauseReasonEnum_TimerComplete;
		SM_enumECUState = SM_enum_ECU_PauseMode;
		Appl_EEPParam.stm32.EEP_Run_Hour = 0;
		Appl_EEPParam.stm32.EEP_Run_Min = 0;
		Appl_EEPParam.stm32.EEP_Run_Sec = 0;
		Appl_EEPParam.stm32.EEP_PowerFailedLastTime = 0;
		StopCurrentProvidingLoop(SM_ApplParam.Pausereason);
	}

}

void StartCurrentProvidingLoop(void){
	Appl_diagActiveFault.OverVoltage = 0;
	if(Appl_diagActiveFault.PowerFail_Z1 != 1 &&
			Appl_diagActiveFault.PowerFail_Z2 != 1 &&
			Appl_diagActiveFault.PowerFail_Z3 != 1 &&
			Appl_diagActiveFault.BatteryReversal != 1 &&
			Appl_diagActiveFault.EEPLogMemoryFull != 1 &&
			Appl_diagActiveFault.OverVoltage != 1 &&
			Appl_diagActiveFault.UnderVoltage != 1){
		SM_enumECUState = SM_enum_ECU_VldAC_RunCurrentLoop_increaseVoltageMode_Slow;
		SM_ApplParam.targetPIDCurrent = SM_ApplParam.UserSetCurrent;
		SM_ApplParam.HighTemperatureCurrent = SM_ApplParam.UserSetCurrent/2.0;

		CaptureExtEEPSnapshot_SingleShot(START_RECORD_FLAG,0);
		REDLEDOFF();
		GREENLEDON();
		DC_CONTACTOR_ON();
		HAL_GPIO_WritePin(uC_LedOnBoard_GPIO_Port, uC_LedOnBoard_Pin,GPIO_PIN_SET);
		WriteEEPData();

		BUZZER_ON();
		SM_ApplParam.Buzzertoggle = 0;
		SM_ApplParam.BuzzerCounter = 0;
	}
	else{
		if(Appl_diagActiveFault.PowerFail_Z1 == 1) SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ1;
		else if(Appl_diagActiveFault.PowerFail_Z2 == 1) SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ2;
		else if(Appl_diagActiveFault.PowerFail_Z3 == 1) SM_ApplParam.Pausereason = PauseReasonEnum_PowerFailZ3;
		else if(Appl_diagActiveFault.OverVoltage == 1) SM_ApplParam.Pausereason = PauseReasonEnum_OverVoltage;
		else if(Appl_diagActiveFault.BatteryReversal == 1) SM_ApplParam.Pausereason = PauseReasonEnum_BatteryReversal;
		else if(Appl_diagActiveFault.EEPLogMemoryFull == 1) SM_ApplParam.Pausereason = PauseReasonEnum_EEPLogMemoryFull;
		else if(Appl_diagActiveFault.UnderVoltage == 1) SM_ApplParam.Pausereason = PauseReasonEnum_UnderVoltage;
		SM_enumECUState = SM_enum_ECU_PauseMode;
	}
}

void StopCurrentProvidingLoop(PauseReason reason){
	REDLEDON();
	GREENLEDOFF();
	DC_CONTACTOR_OFF();
	HAL_GPIO_WritePin(uC_LedOnBoard_GPIO_Port, uC_LedOnBoard_Pin,GPIO_PIN_RESET);
	CaptureExtEEPSnapshot_SingleShot(STOP_RECORD_FLAG,reason);
	WriteEEPData();
	BUZZER_ON();
	SM_ApplParam.Buzzertoggle = 0;
	SM_ApplParam.BuzzerCounter = 0;
}

static inline void DisplayWindowSelection(void){
	/* DISPLAY WINDOW SELECTION */

	if((Appl_diagActiveFault.PowerFail_Z1 == 1) || \
			(Appl_diagActiveFault.PowerFail_Z3 == 1)){
		SM_enumDisplayState = SM_enum_Display_PoweringOffWindow;
	}
	else
		if(SM_enumECUState == SM_enum_ECU_Init){
			SM_enumDisplayState = SM_enum_Display_POWERON_LOGO;
		}
		else if(SM_enumECUState == SM_enum_ECU_StandByMode){
			SM_enumDisplayState = SM_enum_Display_MAIN_MENU;
		}
		else if((SM_enumECUState == SM_enum_ECU_RejuvenationMenu)){
			SM_enumDisplayState = SM_enum_Display_RejuvenationMenu;
		}
		else if(SM_enumECUState == SM_enum_ECU_VldAC_RunCurrentLoop_StableVoltagefloat){
			SM_enumDisplayState = SM_enum_Display_MM_CurrentloopRunning_Stable;
		}
		else if(SM_enumECUState == SM_enum_ECU_VldAC_RunCurrentLoop_increaseVoltageMode_Slow){
			SM_enumDisplayState = SM_enum_Display_MM_CurrentloopRunning_Slow;
		}
		else if(SM_enumECUState == SM_enum_ECU_SelfTest){
			SM_enumDisplayState = SM_enum_Display_SelfTest;
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu){
			SM_enumDisplayState = SM_enum_Display_Settings_Menu;
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu){
			SM_enumDisplayState = SM_enum_Display_Settings_Menu;
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Transfer){
			SM_enumDisplayState = SM_enum_Display_Settings_Menu_Transfer;
		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Erase){
			SM_enumDisplayState = SM_enum_Display_Settings_Menu_Erase;

		}
		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_Completed){
			SM_enumDisplayState = SM_enum_Display_Settings_Menu_Completed;
		}
		else if(SM_enumECUState == SM_enum_ECU_VishwatestMode){
			SM_enumDisplayState = SM_enum_Display_VishwaTest;
		}
		else if(SM_enumECUState == SM_enum_ECU_AutoModeUserInput){
			SM_enumDisplayState = SM_enum_Display_AutoModeUserInput;
		}
		else if(SM_enumECUState == SM_enum_ECU_StatusDisplay){
			SM_enumDisplayState = SM_enum_Display_StatusDisplay;
		}
		else if(SM_enumECUState == SM_enum_ECU_PauseMode){
			SM_enumDisplayState = SM_enum_Display_Pause;
		}
		else if(SM_enumECUState == SM_enum_ECU_Info){
			SM_enumDisplayState = SM_enum_Display_Info;
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			SM_enumDisplayState = SM_enum_Display_StoppedWhileRunning;
		}

		else if(SM_enumECUState == SM_enum_ECU_SettingMenu_MaxVoltage){
			SM_enumDisplayState = SM_enum_Display_Settings_Menu_MaxVoltage;
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			SM_enumDisplayState = SM_enum_Display_StoppedWhileRunning;
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			SM_enumDisplayState = SM_enum_Display_StoppedWhileRunning;
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			SM_enumDisplayState = SM_enum_Display_StoppedWhileRunning;
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			SM_enumDisplayState = SM_enum_Display_StoppedWhileRunning;
		}
		else if(SM_enumECUState == SM_enum_ECU_ContinueFromLastPowerOff){
			SM_enumDisplayState = SM_enum_Display_StoppedWhileRunning;
		}

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
#ifdef __cplusplus
}
#endif

#endif
