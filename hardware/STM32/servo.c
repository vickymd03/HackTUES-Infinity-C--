/**
  ******************************************************************************
  *                                 GreenBo
  *                           The Embedded Experts
  ******************************************************************************
  * @file    servo.c
  * @brief   This file provides code for the configuration
  *          of all the requested RAW to servo functions.
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
  **/

/* Includes ------------------------------------------------------------------*/
#include "servo.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef tim2 = {0};
TIM_OC_InitTypeDef PWM_Config2 = {0};


/* Private function prototypes -----------------------------------------------*/
void Init_TIM2(){
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    GPIO_InitTypeDef tim2_pin;

    /** TIM2 PWM PIN **/
    tim2_pin.Pin = GPIO_PIN_11;
    tim2_pin.Mode = GPIO_MODE_AF_PP;
    tim2_pin.Pull = GPIO_PULLUP;
    tim2_pin.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    tim2_pin.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOB, &tim2_pin);

    __HAL_RCC_TIM2_CLK_ENABLE();
    tim2.Instance = TIM2;
    tim2.Init.Prescaler = 640;  //355
    tim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim2.Init.Period = 3700; //3600
    tim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;       /** SYSTEM FREQUENCY / 1 = 64MHz **/
    tim2.Init.RepetitionCounter = 0;
    tim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if(HAL_TIM_PWM_Init(&tim2) != HAL_OK)    Error_Handler();

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if(HAL_TIM_ConfigClockSource(&tim2, &sClockSourceConfig) != HAL_OK)    Error_Handler();

    PWM_Config2.OCMode = TIM_OCMODE_PWM2;
    PWM_Config2.Pulse = 110;
    PWM_Config2.OCPolarity = TIM_OCPOLARITY_LOW;
    PWM_Config2.OCFastMode = TIM_OCFAST_DISABLE;
    PWM_Config2.OCIdleState = TIM_OCIDLESTATE_RESET;
    PWM_Config2.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if(HAL_TIM_PWM_ConfigChannel(&tim2, &PWM_Config2, TIM_CHANNEL_4) != HAL_OK)    Error_Handler();

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&tim2, &sMasterConfig) != HAL_OK)    Error_Handler();
}

/* Private user code ---------------------------------------------------------*/


/************************ (C) COPYRIGHT GreenBo *****END OF FILE****/
  /*///////////////////////////////////////////////////////////// Servo /////////////
  __HAL_RCC_GPIOB_CLK_ENABLE();
  Init_TIM2();
  //HAL_TIM_PWM_Start(&tim2, TIM_CHANNEL_4);
  TIM2->CCR4 = 3400;
  ///////////////////////////////////////////////////////////// Servo /////////////*/
