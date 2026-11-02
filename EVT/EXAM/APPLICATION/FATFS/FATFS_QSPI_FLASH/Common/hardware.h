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
#include "ch32h417_qspi.h"
	 
void Hardware(void);
uint32_t QSPI_GetManufacturerID(void);
void QSPI_GetData(uint32_t addr, uint8_t *data, uint32_t len);
void QSPI_EraseSector(uint32_t addr);
void QSPI_ProgramPage(uint32_t addr, uint8_t *data, uint16_t len);
void QSPI_Flash_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif 





