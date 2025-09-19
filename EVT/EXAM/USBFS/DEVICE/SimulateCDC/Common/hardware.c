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
 *@Note
 *Example routine to emulate a simulate USB-CDC Device, USE USART4(PF3[RX]/PF4[TX]);
 *Please note: This code uses the default serial port 1 for debugging,
 *if you need to modify the debugging serial port, please do not use USART4
*/
#include "hardware.h"
#include "ch32h417_usbfs_device.h"

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
    printf("GCC Version: %d.%d.%d\n",__GNUC__, __GNUC_MINOR__,__GNUC_PATCHLEVEL__);
    printf("SimulateCDC Running On USBFS Controller\n");
	RCC_Configuration( );

    /* Tim2 init */
    TIM2_Init( );

    /* Usart1 init */
    UART_Init( 1, DEF_UARTx_BAUDRATE, DEF_UARTx_STOPBIT, DEF_UARTx_PARITY );

    /* USB20 device init */
    USBFS_RCC_Init( );
    USBFS_Device_Init( ENABLE );
    
    while(1)
    {
        UART_DataRx_Deal( );
        UART_DataTx_Deal( );
    }
}
