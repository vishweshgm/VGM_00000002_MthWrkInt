/*
 * constants.h
 *
 *  Created on: Aug 20, 2022
 *      Author: vishweshgm
 */

#ifndef APPLICATION_DRIVER_STATEMACHINE_INC_CONSTANTS_H_
#define APPLICATION_DRIVER_STATEMACHINE_INC_CONSTANTS_H_

/*DONOT FORGET TO ERASE EEP ONCE AFTER FLASH*/


#define OVERCURRENT_ADMIN_VALUE							70.0
#define UNDERVOLTAGE_ADMIN_VALUE						48.0
#define INC(x,max,inc) 									x = (x<=(max - inc))?(x+inc):max
#define DEC(x,min,dec) 									x = (x>=(min + dec))?(x-dec):min


#define REDLEDON() 										HAL_GPIO_WritePin(uC_TransistorSwitch3_GPIO_Port,uC_TransistorSwitch3_Pin,GPIO_PIN_SET)
#define GREENLEDON() 									HAL_GPIO_WritePin(uC_TransistorSwitch2_GPIO_Port,uC_TransistorSwitch2_Pin,GPIO_PIN_SET)
#define ORANGELEDON() 									HAL_GPIO_WritePin(uC_TransistorSwitch5_GPIO_Port,uC_TransistorSwitch5_Pin,GPIO_PIN_SET)
#define BLUELEDON() 									HAL_GPIO_WritePin(uC_TransistorSwitch1_GPIO_Port,uC_TransistorSwitch1_Pin,GPIO_PIN_SET)

#define REDLEDOFF() 									HAL_GPIO_WritePin(uC_TransistorSwitch3_GPIO_Port,uC_TransistorSwitch3_Pin,GPIO_PIN_RESET)
#define GREENLEDOFF() 									HAL_GPIO_WritePin(uC_TransistorSwitch2_GPIO_Port,uC_TransistorSwitch2_Pin,GPIO_PIN_RESET)
#define ORANGELEDOFF() 									HAL_GPIO_WritePin(uC_TransistorSwitch5_GPIO_Port,uC_TransistorSwitch5_Pin,GPIO_PIN_RESET)
#define BLUELEDOFF() 									HAL_GPIO_WritePin(uC_TransistorSwitch1_GPIO_Port,uC_TransistorSwitch1_Pin,GPIO_PIN_RESET)

#define DC_CONTACTOR_STATE()							HAL_GPIO_ReadPin(uC_TransistorSwitch6_GPIO_Port, uC_TransistorSwitch6_Pin)

#define BATTERY_RELAY_ON()								HAL_GPIO_WritePin(uC_Relay_GPIO_Port,uC_Relay_Pin,GPIO_PIN_SET)
#define BATTERY_RELAY_OFF()								HAL_GPIO_WritePin(uC_Relay_GPIO_Port,uC_Relay_Pin,GPIO_PIN_RESET)

#define DC_CONTACTOR_ON() 								HAL_GPIO_WritePin(uC_TransistorSwitch6_GPIO_Port,uC_TransistorSwitch6_Pin,GPIO_PIN_RESET)
#define BUZZER_ON() 									HAL_GPIO_WritePin(uC_TransistorSwitch4_GPIO_Port,uC_TransistorSwitch4_Pin,GPIO_PIN_RESET)

#define DC_CONTACTOR_OFF() 								HAL_GPIO_WritePin(uC_TransistorSwitch6_GPIO_Port,uC_TransistorSwitch6_Pin,GPIO_PIN_SET)
#define BUZZER_OFF() 									HAL_GPIO_WritePin(uC_TransistorSwitch4_GPIO_Port,uC_TransistorSwitch4_Pin,GPIO_PIN_SET)


#define G1T_ON()										HAL_GPIO_WritePin(uC_G1T_GPIO_Port,uC_G1T_Pin,GPIO_PIN_RESET);
#define G2T_ON()										HAL_GPIO_WritePin(uC_G2T_GPIO_Port,uC_G2T_Pin,GPIO_PIN_RESET);
#define G3T_ON()										HAL_GPIO_WritePin(uC_G3T_GPIO_Port,uC_G3T_Pin,GPIO_PIN_RESET);

#define G1T_OFF()										HAL_GPIO_WritePin(uC_G1T_GPIO_Port,uC_G1T_Pin,GPIO_PIN_SET);
#define G2T_OFF()										HAL_GPIO_WritePin(uC_G2T_GPIO_Port,uC_G2T_Pin,GPIO_PIN_SET);
#define G3T_OFF()										HAL_GPIO_WritePin(uC_G3T_GPIO_Port,uC_G3T_Pin,GPIO_PIN_SET);

#define ONBOARDLED_TOGGLE()								HAL_GPIO_TogglePin(uC_LedOnBoard_GPIO_Port,uC_LedOnBoard_Pin)

#define ALL_INDICATORS_ON()								do{\
														BUZZER_ON();\
														REDLEDON();\
														GREENLEDON();\
														ORANGELEDON();\
														BLUELEDON();\
														}while(0)

#define ALL_INDICATORS_OFF()							do{\
														BUZZER_OFF();\
														REDLEDOFF();\
														GREENLEDOFF();\
														ORANGELEDOFF();\
														BLUELEDOFF();\
														}while(0)

#define MAP_SM(idx,mode) 								do{\
														if(Appl_Display.SelectedIndexIntheList == idx){ SM_enumECUState = mode; }\
														}while(0);

#define TRIGGER_ANGLE_MIN									(150)
#define TRIGGER_ANGLE_MAX									(180)
#define SOFT_START_ANGLE									(140)



#define MOVING_AVERAGE_SIZE_VOLTAGE							100
#define MOVING_AVERAGE_SIZE_HallCURRENT						10
#define MOVING_AVERAGE_SIZE_ShuntCURRENT					10

#define SCR_ANGLE_TRIGGER_MIN 								50.0
#define SCR_ANGLE_TRIGGER_MAX 								200.0

#define PID_CURRENT_MIN										0
#define PID_CURRENT_MAX										150


#define PID_CURRENT_HYSTERISIS_DELTA 						2


#define KP_VOLTAGE											0.8
#define KI_VOLTAGE											0.8
#define KD_VOLTAGE											0.5
#define KP_CURRENT											0.4
#define KI_CURRENT											0.8

#define X3 													(0)
#define X2 													(0)
#define X1 													(7.632725E-02)
#define X0 													(-8.227556E-01)

#define SHUNT_X3 											(0)
#define SHUNT_X2 											(0)
#define SHUNT_X1 											(4.696731E-02)
#define SHUNT_X0 											(2.490229E-01)

#define U32_MAX												(4294967295)

#define SCR_TIMER_TICKS_FOR_1US								1
#define SCR_TIMER3_TICKS_FOR_1US							1
#define TIMER3_TICKSTO_TIMER5TICKS							1


#define TASK_SM_REPEAT_MS        							10
#define COUNT_1SEC_TASK_SM    								(1000/TASK_SM_REPEAT_MS)
#define COUNT_5SEC_TASK_SM    								(5000/TASK_SM_REPEAT_MS)

#define MINIMUM_ANGLE										90
#define MAXIMUM_ANGLE										170


#define NUM_OF_BYTES_IN_A_EXTEEP_RECORD						16
#define NUM_OF_ATMEL_RECORDS_IN_A_PAGE						(ATMEL_EEP_CHIP_PAGE_SIZE/NUM_OF_BYTES_IN_A_EXTEEP_RECORD)
#define START_RECORD_FLAG									1
#define STOP_RECORD_FLAG									0
#define ATMEL_EEP_CHIP_PAGE_SIZE							256


#endif /* APPLICATION_DRIVER_STATEMACHINE_INC_CONSTANTS_H_ */
