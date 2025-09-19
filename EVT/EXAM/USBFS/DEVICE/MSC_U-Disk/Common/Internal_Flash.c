/********************************** (C) COPYRIGHT *******************************
 * File Name          : Internal_Flash.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/09/05
 * Description        : Internal Flash program
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32h417_conf.h"
#include "Internal_Flash.h"
#include "string.h"

void IFlash_Prog_4K(uint32_t address, uint32_t *pbuf)
{
    uint32_t addr = 0;
    uint32_t sector = 0;
    if(address > (IFLASH_UDISK_SIZE - 4096))
    {
        printf("Error Address %x\n", address);
        return;
    }
    
    if(FLASH_GetCapacity() == FLASHCapacity_480K)
    {
        addr = IFLASH_UDISK_START_ADDR + address;
        sector = 4096;
    }
    else
    {
        addr = IFLASH_UDISK_START_ADDR + (address / 0x1000) * 0x2000 + (address % 0x1000);
        sector = 8192;
    }
    addr += FLASH_BASE;
    FLASH_ROM_ERASE(addr, sector);
    FLASH_ROM_WRITE(addr, pbuf, sector);
}

void IFlash_Read(uint32_t address, uint32_t len, uint8_t *pbuf)
{
    uint32_t addr = 0;
    if(FLASH_GetCapacity() == FLASHCapacity_480K)
    {
        addr = IFLASH_UDISK_START_ADDR + address;
    }
    else
    {
        addr = IFLASH_UDISK_START_ADDR + (address / 0x1000) * 0x2000 + (address % 0x1000);
    }
    addr += FLASH_BASE;
    memcpy(pbuf,(UINT8*)addr,len);
}