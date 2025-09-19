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
#include "hardware.h"
/*
 *@Note
  USB host IAP demo code.
  USBFS HOST USB drive enumeration and operation, read the APP.BIN file in the root directory inside the USB drive, write to location 0x08005000, after a successful check
  Write the flag data, then jump to the user code. The code will run IAP first to check the flag data, if the preparation data is normal, it will jump to the user code, otherwise
  If the compiled data is OK, it will jump to the user code, if not, it will stay in IAP again.
    Support: FAT12/FAT16/FAT32
*/

#include "usb_host_iap.h"

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   
 *
 * @return  none
 */
void Hardware(void)
{
    printf("Build Time: %s %s\n", __DATE__, __TIME__);
    printf("GCC Version: %d.%d.%d\n",__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);

	printf("USBFS-OTG HOST,UDisk IAP.\r\n"); 
    printf("APP Address = 0x%08x\n",DEF_APP_CODE_START_ADDR);
    printf("Verify Code Address = 0x%08x\n",DEF_VERIFY_CODE_START_ADDR);
    IAP_Initialization( );

    while(1)
    {
        IAP_Main_Deal( );
    }
}

