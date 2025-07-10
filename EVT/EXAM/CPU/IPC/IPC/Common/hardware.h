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
void IPC_Config(IPC_Channel_TypeDef IPC_CHx, IPC_TxCID_TypeDef IPC_TxCIDx, IPC_RxCID_TypeDef IPC_RxCIDx);
void IPC_Tx_IT_Config(void);
void IPC_Rx_IT_Config(void);

#ifdef __cplusplus
}
#endif

#endif 





