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
 * APP routine: this routine support USB and UART mode,
 * and you can choose the command method to jump to the IAP .
 * Key  parameters: CalAddr - address in flash(same in IAP), note that this address needs to be unused.
 *                  CheckNum - The value of 'CalAddr' that needs to be modified.
 * Tips :the routine need IAP software version 1.50.
 */

#include "hardware.h"
#include "debug.h"
#include "ch32h417_usbfs_device.h"
#include "ch32h417_usbhs_device.h"
#include "ch32h417_gpio.h"
#include "iap.h"

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
    while(1){
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
/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
	printf("Run in APP\r\n");
    //enable usart and usb device
    USART1_CFG(460800);
    /* USB20 device init */
    USBHS_RCC_Init(ENABLE );
    USBHS_Device_Init( ENABLE );
    
    USBFS_Init( );
    USART1_IT_CFG();

	while (1)
  {
#if 0
        if( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET){

                UART_Rx_Deal();
            }
#endif

        if(*(uint32_t*)CalAddr == CheckNum)
        {
             Delay_Ms(10);
             APP_2_IAP();
             while(1);
        }
  }
}
