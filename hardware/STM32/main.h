/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  *                                 GreenBo
  *                           The Embedded Experts
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 GreenBo.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by GreenBo under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "stm32h7xx_ll_dma.h"

/* Private includes ----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#ifdef TFT96
// QQVGA
#define FrameWidth 160
#define FrameHeight 120
#elif TFT18
// QQVGA2
#define FrameWidth 128
#define FrameHeight 160
#endif

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
extern uint16_t pic[FrameWidth][FrameHeight];
extern uint8_t sendBuf[80 + (FrameWidth * FrameHeight * 2)];
extern uint8_t sendInfoBuff[1024];
extern uint8_t recieveControlsBuff[1024];
/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
/* Private defines -----------------------------------------------------------*/
#define PE3_Pin GPIO_PIN_3
#define PE3_GPIO_Port GPIOE
#define KEY_Pin GPIO_PIN_13
#define KEY_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOE
#define LCD_WR_RS_Pin GPIO_PIN_13
#define LCD_WR_RS_GPIO_Port GPIOE

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
