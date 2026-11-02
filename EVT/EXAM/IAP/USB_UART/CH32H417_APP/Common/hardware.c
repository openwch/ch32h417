/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.3
* Date               : 2026/04/03
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 * APP routine: this routine support USB and UART mode,
 * and you can choose the command method to jump to the IAP .
 * Key  parameters: CalAddr - address in flash(same in IAP), note that this address needs to be unused.
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

extern vu32 Flash_Erase_Page_Size;
#define Size_256B                  0x100
#define Size_4KB                   0x1000
#define Size_8KB                   0x2000
/*********************************************************************
 * @fn      APP_2_IAP
 *
 * @brief   APP_2_IAP program.
 *
 * @return  none
 */
void APP_2_IAP(void)
{
    NVIC_SystemReset();
    while(1)
    {
    }
}

/*********************************************************************
 * @fn      USART1_IT_CFG
 *
 * @brief   USART1 IT configuration.
 *
 * @return  none
 */
void USART1_IT_CFG(void)
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_USART1, ENABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_SetPriority(USART1_IRQn,0);
    NVIC_EnableIRQ(USART1_IRQn);
    USART_Cmd(USART1, ENABLE);
}

#if Func_Run_V3F

void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      USART3_IRQHandler
 *
 * @brief   This function handles USART3 global interrupt request.
 *
 * @return  none
 */
void USART1_IRQHandler(void)
{
    if( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET){

        UART_Rx_Deal();
    }
}

#endif
/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
	printf("Run in APP\r\n");
    if(((*(vu32*)FLASH_CFGR0_BASE) & (1<<28)) != 0)
    {
        Flash_Erase_Page_Size = Size_8KB;
    }
    else 
    {
        Flash_Erase_Page_Size = Size_4KB;
    }
    //enable usart and usb device
    USART1_CFG(460800);
    /* USB20 device init */
    USB_Init(ENABLE);
    USART1_IT_CFG();

	while (1)
  {
#if 0
        if( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET){

                UART_Rx_Deal();
            }
#endif

        if(*(vu32*)CalAddr == CheckNum)
        {
             Delay_Ms(10);
             APP_2_IAP();
             while(1)
             {
             }
        }
  }
}
