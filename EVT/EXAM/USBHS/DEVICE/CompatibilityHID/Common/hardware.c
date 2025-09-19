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
 * Compatibility HID Example:
 * This program provides examples of the pass-through of USB-HID data and serial port
 *  data based on compatibility HID device. And the data returned by Get_Report request is
 *  the data sent by the last Set_Report request.Speed of UART1/2 is 115200bps.
 *
 * Interrupt Transfers:
 *   UART4_RX   ---> Endpoint2
 *   Endpoint1  ---> UART4_TX
 *
 *   Note that the first byte is the valid data length and the remaining bytes are
 *   the transmission data for interrupt Transfers.
 *
 * Control Transfers:
 *   Set_Report ---> Debug UART TX
 *   Get_Report <--- last Set_Report packet
 *

 * If the USB is set to high-speed, an external crystal oscillator is recommended for the clock source.
 *  */
#include "hardware.h"
#include "ch32h417_usbhs_device.h"
#include "usbd_compatibility_hid.h"

/*********************************************************************
 * @fn      Var_Init
 *
 * @brief   Software parameter initialization
 *
 * @return  none
 */
void Var_Init(void)
{
    uint16_t i;
    RingBuffer_Comm.LoadPtr = 0;
    RingBuffer_Comm.StopFlag = 0;
    RingBuffer_Comm.DealPtr = 0;
    RingBuffer_Comm.RemainPack = 0;
    for(i=0; i<DEF_Ring_Buffer_Max_Blks; i++)
    {
        RingBuffer_Comm.PackLen[i] = 0;
    }
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
    printf("CompatibilityHID Running On USBHS Controller\n");
    
    /* Variables init */
    Var_Init( );

    TIM2_Init( );

    /* UART init */
    UART_Init( );
    UART_DMA_Init( );

    /* Disable SWD */
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    USBHS_Device_Init(ENABLE);

    while(1)
    {
        /* Determine if enumeration is complete, perform data transfer if completed */
        if (USBHS_DevEnumStatus)
        {
            UART_Rx_Service();
            UART_Tx_Service();
            HID_Set_Report_Deal();
        }
    }
}
