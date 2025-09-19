/********************************** (C) COPYRIGHT  *******************************
* File Name          : shared.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "stdio.h"
#include "shared.h"
/* Global define */

/* Global Variable */


__attribute__((section(".shared_data"))) 
volatile uint8_t Buffer_Sharing[4] ;
__attribute__((section(".shared_data"))) 
volatile uint32_t Data_Sharing ;