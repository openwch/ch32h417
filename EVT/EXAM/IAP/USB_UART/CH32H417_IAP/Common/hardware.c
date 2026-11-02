/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.3
* Date               : 2026/10/16
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 * IAP routine: this routine support USB and UART mode,
 * and you can choose the command method or the IO method to jump to the APP .
 * Key  parameters: CalAddr - address in flash (same in APP), note that this address needs to be unused.
 *                  CheckNum - The value of 'CalAddr' that needs to be modified.
 * Tips :the routine need IAP software version 1.60 or later.
 *       DEF_USB_APP_MODE - USB Vendor or HID mode
 */

#include "hardware.h"
#include "ch32h417_usbfs_device.h"
#include "ch32h417_usbhs_device.h"
#include "ch32h417_gpio.h"
#include "iap.h"
#include "usb_inf.h"
extern vu8 End_Flag;

#define UPGRADE_MODE_COMMAND   0
#define UPGRADE_MODE_IO        1

#define UPGRADE_MODE   UPGRADE_MODE_COMMAND
/*********************************************************************
 * @fn      IAP_2_APP
 *
 * @brief   IAP_2_APP program.
 *
 * @return  none
 */
void IAP_2_APP(void)
{
    USB_Init(DISABLE);
    Delay_Ms(50);
    printf("jump APP\r\n");
    Delay_Ms(50);
    GPIO_DeInit(GPIOA);
    GPIO_DeInit( GPIOB);
    USART_DeInit(USART1);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, DISABLE);
    RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOB,DISABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_USART1,DISABLE);
    Delay_Ms(10);

    RCC_DeInit();
    NVIC_DisableIRQ(USBFS_IRQn);
    NVIC_EnableIRQ(Software_IRQn);
    NVIC_SetPendingIRQ(Software_IRQn);
}


/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
#if UPGRADE_MODE == UPGRADE_MODE_COMMAND
    if(*(vu32*)FLASH_Base  != 0xe339e339 )
    {
        if(*(vu32*)CalAddr != CheckNum)
        {
            IAP_2_APP();
            while(1)
            {
            }
        }
    }
#elif UPGRADE_MODE == UPGRADE_MODE_IO
    if(PA0_Check() == 0)
    {
        IAP_2_APP();
        while(1)
        {
        }
    }
#endif

    USART1_CFG(460800);
    USB_Init(ENABLE);

	while(1)
	{
        if( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET){

            UART_Rx_Deal();
        }

#if UPGRADE_MODE == UPGRADE_MODE_COMMAND
        if (End_Flag)
        {
            IAP_2_APP();
            while(1)
            {
            }
        }
#endif
	}
}
