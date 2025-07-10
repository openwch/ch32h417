/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32h417_crc.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file contains all the functions prototypes for the 
*                      CRC firmware library.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32H417_CRC_H
#define __CH32H417_CRC_H

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

void SYSCLK_384M_V5F_384M_V3F_96M_HSE(void);
void SYSCLK_288M_V5F_288M_V3F_144M_HSE(void);
void SYSCLK_384M_V5F_384M_V3F_96M_HSI(void);
void SYSCLK_288M_V5F_288M_V3F_144M_HSI(void);

#ifdef __cplusplus
}
#endif

#endif 





