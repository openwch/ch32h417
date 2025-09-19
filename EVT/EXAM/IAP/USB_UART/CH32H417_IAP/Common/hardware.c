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
 * IAP routine: this routine support USB and UART mode,
 * and you can choose the command method or the IO method to jump to the APP .
 * Key  parameters: CalAddr - address in flash (same in APP), note that this address needs to be unused.
 *                  CheckNum - The value of 'CalAddr' that needs to be modified.
 * Tips :the routine need IAP software version 1.50.
 */

#include "hardware.h"
#include "ch32h417_usbfs_device.h"
#include "ch32h417_usbhs_device.h"
#include "ch32h417_gpio.h"
#include "iap.h"
extern u8 End_Flag;

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
    USBHS_Device_Init( DISABLE );
    NVIC_DisableIRQ( USBHS_IRQn );
    USBFSD->BASE_CTRL=0x06;
    USBFSD->INT_EN=0x00;
    Delay_Ms(50);
    printf("jump APP\r\n");
    Delay_Ms(50);
    GPIO_DeInit(GPIOA);
    GPIO_DeInit( GPIOB);
    USART_DeInit(USART1);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, DISABLE);
    RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOB,DISABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_USART1,DISABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS, DISABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_OTG_FS, DISABLE);
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
    if(*(uint32_t*)FLASH_Base  != 0xe339e339 )
    {
        if(*(uint32_t*)CalAddr != CheckNum)
        {
            IAP_2_APP();
            while(1);
        }
    }
#elif UPGRADE_MODE == UPGRADE_MODE_IO
    if(PA0_Check() == 0)
    {
        IAP_2_APP();
        while(1);
    }
#endif

    USART1_CFG(460800);
    // /* USB20 device init */
    USBHS_RCC_Init(ENABLE );
    USBHS_Device_Init( ENABLE );

    USBFS_Init( );
	while(1)
	{
        if( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET){

            UART_Rx_Deal();
        }

#if UPGRADE_MODE == UPGRADE_MODE_COMMAND
        if (End_Flag)
         {
            IAP_2_APP();
            while(1);
         }
#endif
	}
}
