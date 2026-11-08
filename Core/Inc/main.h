/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32g4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define POWER_CYCLE_Pin GPIO_PIN_3
#define POWER_CYCLE_GPIO_Port GPIOA
#define LIMIT_SWITCH_1_Pin GPIO_PIN_4
#define LIMIT_SWITCH_1_GPIO_Port GPIOC
#define LIMIT_SWITCH_2_Pin GPIO_PIN_5
#define LIMIT_SWITCH_2_GPIO_Port GPIOC
#define LED_0_Pin GPIO_PIN_0
#define LED_0_GPIO_Port GPIOB
#define LED_1_Pin GPIO_PIN_1
#define LED_1_GPIO_Port GPIOB
#define CAN_LED_Pin GPIO_PIN_11
#define CAN_LED_GPIO_Port GPIOB
#define BRAKE_ELBOW_Pin GPIO_PIN_14
#define BRAKE_ELBOW_GPIO_Port GPIOB
#define BRAKE_SHOULDER_Pin GPIO_PIN_15
#define BRAKE_SHOULDER_GPIO_Port GPIOB
#define BRAKE_BASE_Pin GPIO_PIN_6
#define BRAKE_BASE_GPIO_Port GPIOC
#define DBG_LED_Pin GPIO_PIN_4
#define DBG_LED_GPIO_Port GPIOB
#define ERR_LED_Pin GPIO_PIN_5
#define ERR_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
