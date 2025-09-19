/********************************** (C) COPYRIGHT *******************************
 * File Name          : Internal_Flash.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/01/19
 * Description        : header file for Internal_Flash.c
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef USER_INTERNAL__FLASH_C_
#define USER_INTERNAL__FLASH_C_

#ifdef __cplusplus
 extern "C" {
#endif 

#define IFLASH_UDISK_START_ADDR   (1024 * (64 + 128))                           // V3F-64K  V5F-128K

#define IFLASH_UDISK_END_ADDR     (480 * 1024 - 1)

#define IFLASH_UDISK_SIZE         (IFLASH_UDISK_END_ADDR - IFLASH_UDISK_START_ADDR + 1 )

extern void IFlash_Prog_4K(uint32_t address,uint32_t *pbuf);
extern void IFlash_Read(uint32_t address, uint32_t len, uint8_t *pbuf);

#ifdef __cplusplus
}
#endif

#endif /* USER_INTERNAL__FLASH_C_ */
