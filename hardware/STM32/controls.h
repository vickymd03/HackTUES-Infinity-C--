/* USER CODE BEGIN Header /
/*
  ******************************************************************************
  *                                 GreenBo
  *                           The Embedded Experts
  ******************************************************************************
  * @file           : controls.h
  * @brief          : Header for controls.c file.
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
  /
/ USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONTROLS_H
#define __CONTROLS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private defines -----------------------------------------------------------*/
typedef struct controls_t{
    uint8_t power_mode;
}controls_t;

typedef struct info_t{
    uint8_t tempeture;
}info_t;

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------/
/ USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------/
/ USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */
void Controls_Init();
void Prepare_ControlsBuffer();
void Info_Init();
void Prepare_InfoBuffer();
void Control_Wait(uint8_t channel, uint32_t millsec);
/* USER CODE END EFP */


#ifdef __cplusplus
}
#endif

#endif /* __CONTROLS_H */

/************************ (C) COPYRIGHT GreenBo ************END OF FILE****/
