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
u16 SPI_Flash_ReadID(void);
void SPI_Flash_Erase_Sector(u32 Dst_Addr);
void SPI_Flash_Read(u8 *pBuffer, u32 ReadAddr, u16 size);
void SPI_Flash_Write(u8 *pBuffer, u32 WriteAddr, u16 size);
#ifdef __cplusplus
}
#endif

#endif 





