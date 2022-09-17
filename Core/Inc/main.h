/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
#define APPS2_Pin GPIO_PIN_0
#define APPS2_GPIO_Port GPIOC
#define BRAKE_PRESSURE_Pin GPIO_PIN_1
#define BRAKE_PRESSURE_GPIO_Port GPIOC
#define WSFL_Pin GPIO_PIN_0
#define WSFL_GPIO_Port GPIOA
#define WSFR_Pin GPIO_PIN_1
#define WSFR_GPIO_Port GPIOA
#define NTRL_SW_Pin GPIO_PIN_4
#define NTRL_SW_GPIO_Port GPIOA
#define UPSHIFT_BTN_Pin GPIO_PIN_15
#define UPSHIFT_BTN_GPIO_Port GPIOF
#define UPSHIFT_BTN_EXTI_IRQn EXTI15_10_IRQn
#define RTD_BTN_Pin GPIO_PIN_7
#define RTD_BTN_GPIO_Port GPIOE
#define RTD_BTN_EXTI_IRQn EXTI9_5_IRQn
#define DOWNSHIFT_BTN_Pin GPIO_PIN_8
#define DOWNSHIFT_BTN_GPIO_Port GPIOE
#define DOWNSHIFT_BTN_EXTI_IRQn EXTI9_5_IRQn
#define GPIO_IN_Pin GPIO_PIN_9
#define GPIO_IN_GPIO_Port GPIOE
#define GPIO_IN_EXTI_IRQn EXTI9_5_IRQn
#define AERO_REAR_BTN_Pin GPIO_PIN_10
#define AERO_REAR_BTN_GPIO_Port GPIOE
#define AERO_REAR_BTN_EXTI_IRQn EXTI15_10_IRQn
#define CLUTCH_SLOW_BTN_Pin GPIO_PIN_11
#define CLUTCH_SLOW_BTN_GPIO_Port GPIOE
#define CLUTCH_SLOW_BTN_EXTI_IRQn EXTI15_10_IRQn
#define PUMP_CTRL_Pin GPIO_PIN_14
#define PUMP_CTRL_GPIO_Port GPIOD
#define FAN_CTRL_Pin GPIO_PIN_15
#define FAN_CTRL_GPIO_Port GPIOD
#define HEARTBEAT_Pin GPIO_PIN_10
#define HEARTBEAT_GPIO_Port GPIOC
#define DAM_LED_Pin GPIO_PIN_11
#define DAM_LED_GPIO_Port GPIOC
#define HARDFAULT_Pin GPIO_PIN_12
#define HARDFAULT_GPIO_Port GPIOC
#define RTD_BZZR_Pin GPIO_PIN_0
#define RTD_BZZR_GPIO_Port GPIOD
#define LSW2_Pin GPIO_PIN_1
#define LSW2_GPIO_Port GPIOD
#define BRAKE_LIGHT_Pin GPIO_PIN_2
#define BRAKE_LIGHT_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
