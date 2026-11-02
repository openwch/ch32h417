/********************************** (C) COPYRIGHT *******************************
* File Name          : eeprom.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/09/17
* Description        : eeprom functions definetion.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef  __EEPROM_H
#define  __EEPROM_H

#include "stdio.h"

#define EEPROM_PAGE_SIZE               (4096*2)

#define EEPROM_START_ADDRESS           ((uint32_t)(0x08028000))  
#define EEPROM_PART0_BASE_ADDRESS      (EEPROM_START_ADDRESS)
#define EEPROM_PART0_END_ADDRESS       (EEPROM_START_ADDRESS+EEPROM_PAGE_SIZE-1)

#define EEPROM_PART1_BASE_ADDRESS      (EEPROM_START_ADDRESS + EEPROM_PAGE_SIZE)
#define EEPROM_PART1_END_ADDRESS       (EEPROM_PART1_BASE_ADDRESS + EEPROM_PAGE_SIZE -1)

#define PAGE0      (0u)
#define PAGE1      (1u)

#define NO_VALID_PAGE         ((uint16_t)0xEEEE)

#define PAGE_STATE_EMPTY      ((uint16_t)0xe339)
#define PAGE_STATE_RECEIVE    ((uint16_t)0x6221)
#define PAGE_STATE_VALID      ((uint16_t)0x0000)

#define READ_VALID  (0u)
#define WRITE_VALID (1u)

#define PAGE_FULL             ((uint16_t)0xAAAA)
#define NB_VAR                (3u)

#define GAP_STEP              (2u)

uint16_t EEPROM_Init(void);
uint16_t EEPROM_Read(uint16_t VirtAddress, uint16_t* Data);
uint16_t EEPROM_Write(uint16_t VirtAddress, uint16_t Data);

#endif