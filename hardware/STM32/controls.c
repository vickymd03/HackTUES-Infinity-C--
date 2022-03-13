/**
  ******************************************************************************
  *                                 GreenBo
  *                           The Embedded Experts
  ******************************************************************************
  * @file    controls.c
  * @brief   This file provides code for the configuration
  *          of all the requested RAW and controls of the machine.
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
#include "controls.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
#define CHANNELS 10;
uint32_t _TICK_ = 0, _channels[CHANNELS];

// Initial loading of the structure
void Controls_Init(){
    //nop;
}

// Load the buffer from the server into the structure
void Prepare_ControlsBuffer(){
    //nop;
}

// Initial loading of the structure
void Info_Init(){
    //nop;
}

// Load the structure into a buffer to be sent to the server
void Prepare_InfoBuffer(){
    //nop;
}

void Control_Wait(uint8_t channel, uint32_t millsec){
    uint8_t return_status = 0;
    return return_status;
}
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/


/************************ (C) COPYRIGHT GreenBo *****END OF FILE****/
