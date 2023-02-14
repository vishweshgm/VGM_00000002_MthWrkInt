/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "stdarg.h"
#include "constants.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define uC_BattteryReverse_Pin GPIO_PIN_13
#define uC_BattteryReverse_GPIO_Port GPIOC
#define uC_RS485_RE_Pin GPIO_PIN_5
#define uC_RS485_RE_GPIO_Port GPIOA
#define uC_ZC1_Pin GPIO_PIN_6
#define uC_ZC1_GPIO_Port GPIOA
#define uC_ZC1_EXTI_IRQn EXTI9_5_IRQn
#define uC_ZC2_Pin GPIO_PIN_7
#define uC_ZC2_GPIO_Port GPIOA
#define uC_ZC2_EXTI_IRQn EXTI9_5_IRQn
#define uC_CapBank_Pin GPIO_PIN_4
#define uC_CapBank_GPIO_Port GPIOC
#define uC_TransistorSwitch6_Pin GPIO_PIN_1
#define uC_TransistorSwitch6_GPIO_Port GPIOB
#define uC_LedOnBoard_Pin GPIO_PIN_13
#define uC_LedOnBoard_GPIO_Port GPIOB
#define uC_Buzzer_Pin GPIO_PIN_6
#define uC_Buzzer_GPIO_Port GPIOC
#define uC_TransistorSwitch4_Pin GPIO_PIN_7
#define uC_TransistorSwitch4_GPIO_Port GPIOC
#define uC_Relay_Pin GPIO_PIN_9
#define uC_Relay_GPIO_Port GPIOC
#define uC_ZC3_Pin GPIO_PIN_8
#define uC_ZC3_GPIO_Port GPIOA
#define uC_ZC3_EXTI_IRQn EXTI9_5_IRQn
#define uC_G2T_Pin GPIO_PIN_11
#define uC_G2T_GPIO_Port GPIOA
#define uC_G3T_Pin GPIO_PIN_15
#define uC_G3T_GPIO_Port GPIOA
#define uC_G1T_Pin GPIO_PIN_5
#define uC_G1T_GPIO_Port GPIOB
#define uC_SCRTemp_Pin GPIO_PIN_6
#define uC_SCRTemp_GPIO_Port GPIOB
#define uC_RS485_DE_Pin GPIO_PIN_7
#define uC_RS485_DE_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
