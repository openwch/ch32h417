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
 * This example demonstrates the process of enumerating the keyboard and mouse 
 * by a USB host and obtaining data based on the polling time of the input endpoints 
 * of the keyboard and mouse. 
 * The USBFS port also supports enumeration of keyboard and mouse attached at tier
 * level 2(Hub 1).
*/
/*******************************************************************************/
#include "hardware.h"

/* Header Files */
#include "usb_host_config.h"


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
    DUG_PRINTF( "USBFS HOST KM Test\r\n" );

    /* Initialize TIM3 */
    TIM3_Init();
    DUG_PRINTF( "TIM3 Init OK!\r\n" );

    /* Initialize USBFS host */
#if DEF_USBFS_PORT_EN
    DUG_PRINTF( "USBFS Host Init\r\n" );
    USBFS_RCC_Init( );
    USBFS_Host_Init( ENABLE );
    memset( &RootHubDev.bStatus, 0, sizeof( ROOT_HUB_DEVICE ) );
    memset( &HostCtl[ DEF_USBFS_PORT_INDEX * DEF_ONE_USB_SUP_DEV_TOTAL ].InterfaceNum, 0, DEF_ONE_USB_SUP_DEV_TOTAL * sizeof( HOST_CTL ) );
#endif

    while( 1 )
    {
        USBH_MainDeal( );
    }
}

