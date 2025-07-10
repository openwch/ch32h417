/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/05/07
 * Description        : Main program body for V5F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
/*
 *@Note
   1,Exam 1 :
   USBHS host controller enumeration routines for byte manipulation of USB flash drives,
 including file creation, modification, reading and deletion.
   2,Exam 6 :
   USBHS host controller enumeration routines for sector manipulation of USB flash drives,
 including file creation, modification, reading and deletion
   3,Exam 9 :
   The USBHS host controller enumerates the USB flash drive to create folders on the drive,
 including the primary and secondary directories, and the demo files in the corresponding directories.
   4,Exam 11 :
   USBHS host controller enumerates USB flash drives, enabling the enumeration of
 the first 1000 files on the drive.
   5,Exam 13 :USBHS host controller enumerates USB drives to create long filename files,
 or to get long filename files.
  Important: Only FAT12/FAT16/FAT32 formats are supported:
*/


#include "debug.h"
#include "hardware.h"
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    SystemAndCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(921600);
    printf("V5F SystemCoreClk:%d\r\n", SystemCoreClock);

#if (Run_Core == Run_Core_V3FandV5F)
    HSEM_FastTake(HSEM_ID0);
    HSEM_ReleaseOneSem(HSEM_ID0, 0);

#elif (Run_Core == Run_Core_V3F)

#elif (Run_Core == Run_Core_V5F)
    Hardware();
#endif

    while(1)
    {
        ;
    }
}
