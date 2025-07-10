/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file contains all the functions prototypes for the 
*                      hardware.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __HARDWARE_H
#define __HARDWARE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32h417.h"
#include "debug.h"


extern uint32_t GetHclkClock; 
extern uint32_t GetSysClock; 
extern uint32_t GetCoreClock; 
extern uint8_t HSE_STATE;
	 
void Hardware(void);
void SYSRInit(void);
void SetClock(void);

void SYSCLK_400M_V5F_400M_V3F_100M_HSE(void);
void SYSCLK_480M_V5F_240M_V3F_120M_HSE(void);
void SYSCLK_400M_V5F_400M_V3F_100M_HSI(void);
void SYSCLK_480M_V5F_240M_V3F_120M_HSI(void);

#ifdef __cplusplus
}
#endif

#endif 





