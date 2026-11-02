/********************************** (C) COPYRIGHT *******************************
 * File Name          : sd_card.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/09/23
 * Description        : SD card driver in SPI mode.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __SD_CARD_H
#define __SD_CARD_H

#include "ch32h417.h"

/* Function return status */
#define SD_OK              0 /* Success */
#define SD_ERR_TIMEOUT     1 /* Timeout */
#define SD_ERR_RESPONSE    2 /* Command response error */
#define SD_ERR_CRC         3 /* Data check / write response error */
#define SD_ERR_NOT_SUPPORT 4 /* Unsupported operation or parameter */

/* Card type */
#define SD_TYPE_UNKNOWN    0 /* Unknown */
#define SD_TYPE_SD1        1 /* SD V1.x standard capacity */
#define SD_TYPE_SD2        2 /* SD V2.0 standard capacity */
#define SD_TYPE_SDHC       3 /* SD V2.0 high capacity (SDHC/SDXC) */

/* Card type and total sector count (defined in sd_card.c) */
extern uint8_t  SD_Type;
extern uint32_t SD_SectorCount;

void     SD_Init(void);                                                    /* Initialize GPIO and SPI */
uint8_t  SD_InitCard(void);                                                /* Initialize SD card protocol */
uint8_t  SD_ReadBlock(uint32_t sector, uint8_t* buf, uint16_t len);        /* Read a single block */
uint8_t  SD_WriteBlock(uint32_t sector, const uint8_t* buf, uint16_t len); /* Write a single block */
uint32_t SD_GetCapacity(void);                                             /* Get total sector count */
uint8_t  SD_GetType(void);                                                 /* Get card type */

#endif
