/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  *                                 GreenBo         
  *                           The Embedded Experts
  ******************************************************************************
  * @file           : BMP.h
  * @brief          : Header for BMP.c file.
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
#ifndef __BMP_H
#define __BMP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
extern uint8_t BitMapFileHeader[80];

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void BitMapFileHeader_init(void);
uint8_t Calculate_CRC(void);
void Prepare_RGBdata(void);

#ifdef __cplusplus
}
#endif

#endif /* __BMP_H */

/************************ (C) COPYRIGHT GreenBo ****************END OF FILE****/
