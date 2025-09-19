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
/*
 * @Note
 * Composite Keyboard and Mouse Example:
 * This example uses PB11-PB14 and PA4-PA7 to simulate keyboard key pressing and mouse
 * movement respectively, active low.
 * At the same time, it also uses USART4(PF3) to receive the specified data sent from
 * the host to simulate the pressing and releasing of the following specific keyboard
 * keys. Data is sent in hexadecimal format and 1 byte at a time.
 * 'W' -> 0x1A
 * 'A' -> 0x04
 * 'S' -> 0x16
 * 'D' -> 0x07
 */

#include "hardware.h"
#include "ch32h417_usbfs_device.h"
#include "usbd_composite_km.h"
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
    printf("USBFS Composite KM Device Test\r\n");

    /* Initialize USART2 for receiving the specified keyboard data */
    USART4_Init( 115200 );
    printf( "USART2 Init OK!\r\n" );

    /* Initialize GPIO for keyboard scan */
    KB_Scan_Init( );
    printf( "KB Scan Init OK!\r\n" );

    /* Initialize GPIO for mouse scan */
    MS_Scan_Init( );
    printf( "MS Scan Init OK!\r\n" );

    /* Initialize timer for Keyboard and mouse scan timing */
    TIM3_Init( 1, SystemCoreClock / 10000 - 1 );
    printf( "TIM3 Init OK!\r\n" );

    /* Initialize USBFS interface to communicate with the host  */
    USBFS_RCC_Init( );
    USBFS_Device_Init( ENABLE );
    USB_Sleep_Wakeup_CFG( );


    while( 1 )
    {
        if( USBFS_DevEnumStatus )
        {
            /* Handle keyboard scan data */
            KB_Scan_Handle(  );

            /* Handle keyboard lighting */
            KB_LED_Handle( );

            /* Handle mouse scan data */
            MS_Scan_Handle( );

            /* Handle USART2 receiving data */
            USART4_Receive_Handle( );
        }
    }
}
