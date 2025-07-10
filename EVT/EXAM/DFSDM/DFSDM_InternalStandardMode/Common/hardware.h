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

	 
void Hardware(void);
void Internal_Data(void);
void DMA_Rx_Init(DMA_Channel_TypeDef *DMA_CHx, int32_t ppadr, int32_t memadr, uint16_t bufsize);
void DFSDM_Function_Init(void);

#ifdef __cplusplus
}
#endif

#endif 





