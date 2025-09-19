/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/* @Note
 * UDisk Example:
 * This program provides examples of UDisk.Supports external SPI Flash and internal
 * Flash, selected by STORAGE_MEDIUM at SW_UDISK.h.
 *  */

#include "hardware.h"
#include "ch32h417_usbfs_device.h"
#include "SW_UDISK.h"
#include "SPI_FLASH.h"
#include "Internal_Flash.h"
/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
    printf("Build Time: %s %s\n", __DATE__, __TIME__);
    printf("GCC Version: %d.%d.%d\n",__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);

#if (STORAGE_MEDIUM == MEDIUM_SPI_FLASH)
    printf("USBD UDisk Demo\r\nStorage Medium: SPI FLASH \r\n");
    /* SPI flash init */
    FLASH_Port_Init( );
    /* FLASH ID check */
    FLASH_IC_Check( );
#elif (STORAGE_MEDIUM == MEDIUM_INTERAL_FLASH)
    printf("USBD UDisk Demo\r\nStorage Medium: INTERNAL FLASH \r\n");
    Flash_Sector_Count = IFLASH_UDISK_SIZE  / DEF_UDISK_SECTOR_SIZE;
    Flash_Sector_Size = DEF_UDISK_SECTOR_SIZE;
#endif

    /* Enable Udisk */
    Udisk_Capability = Flash_Sector_Count;
    Udisk_Status |= DEF_UDISK_EN_FLAG;

    /* Usb Init */
    USBFS_RCC_Init( );
    USBFS_Device_Init( ENABLE );
    NVIC_EnableIRQ(USBFS_IRQn);

	while(1)
	{
	    ;
	}
}
