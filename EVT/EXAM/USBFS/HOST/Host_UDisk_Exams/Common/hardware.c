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
#include "debug.h"
#include "stdio.h"
#include "string.h"
#include "Udisk_Operation.h"

/* Switches for code demonstration, 1:Enable 0:Disable */
#define  DEF_TEST_FUNC_NUM                DEF_TEST_FUNC_1
#define  DEF_TEST_FUNC_1                  1                          /* Test 1 , Operation for Open/Read/Modify/Delete a file by bytes */
#define  DEF_TEST_FUNC_6                  6                          /* Test 6 , Operation for Open/Read/Modify/Delete a file by sectors */
#define  DEF_TEST_FUNC_9                  9                          /* Test 9 , Operation for creating folders */
#define  DEF_TEST_FUNC_11                 11                         /* Test 11 , Operation for enumeration of the first 1000 files */
#define  DEF_TEST_FUNC_13                 13                         /* Test 13 , Operation for creating long filename files */

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
    printf( "USBFS HOST Test EXAM %d\r\n", DEF_TEST_FUNC_NUM ); 

    /* General USB Host UDisk Operation Initialization */
    Udisk_USBH_Initialization( );
    UDisk_Opeation_Flag = 1;

    while(1)
    {
#if DEF_TEST_FUNC_NUM == DEF_TEST_FUNC_1
        UDisk_USBH_ByteOperation( );
#elif DEF_TEST_FUNC_NUM == DEF_TEST_FUNC_6
        UDisk_USBH_SectorOperation( );
#elif DEF_TEST_FUNC_NUM == DEF_TEST_FUNC_9
        UDisk_USBH_CreatDirectory( );
#elif DEF_TEST_FUNC_NUM == DEF_TEST_FUNC_11
        UDisk_USBH_EnumFiles( );
#elif DEF_TEST_FUNC_NUM == DEF_TEST_FUNC_13
        UDisk_USBH_Longname( );
#endif
    }
}
