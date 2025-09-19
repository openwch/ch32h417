/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32h417_crc.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the CRC firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "ch32h417_usbss_device.h"
#include "ch32h417_usbhs_device.h"

 /*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @param   none
 *
 * @return  none
 */
void Hardware(void)
{
    printf("Build Time: %s %s\n", __DATE__, __TIME__);
    printf("GCC Version: %d.%d.%d\n",__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
    printf("CH372Device Running On USBHS Controller\n");

    /* Disable SWD */
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    USB_Timer_Init( );
    USBSS_Device_Init( ENABLE );

    while(1)
    {
        /* Determine if enumeration is complete, perform data transfer if completed */
        if(USBHS_DevEnumStatus)
        {
            /* Data Transfer */
            if(RingBuffer_Comm.RemainPack)
            {
                if((USBHSD->UEP1_TX_CTRL & USBHS_UEP_T_RES_MASK) == USBHS_UEP_T_RES_NAK)
                {
                    USBHSD->UEP1_TX_DMA = (uint32_t)&Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_USBD_HS_PACK_SIZE];
                    USBHSD->UEP1_TX_LEN = RingBuffer_Comm.PackLen[RingBuffer_Comm.DealPtr];
                    USBHSD->UEP1_TX_CTRL = (USBHSD->UEP1_TX_CTRL &= ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_ACK;

                    NVIC_DisableIRQ( USBHS_IRQn );
                    RingBuffer_Comm.RemainPack--;
                    RingBuffer_Comm.DealPtr++;
                    if(RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks)
                    {
                        RingBuffer_Comm.DealPtr = 0;
                    }
                    NVIC_EnableIRQ( USBHS_IRQn );
                }
            }

            /* Monitor whether the remaining space is available for further downloads */
            if( RingBuffer_Comm.RemainPack < (DEF_Ring_Buffer_Max_Blks - DEF_RING_BUFFER_RESTART ))
            {
                if( RingBuffer_Comm.StopFlag )
                {
                    RingBuffer_Comm.StopFlag = 0;
                    USBHSD->UEP1_RX_CTRL = (USBHSD->UEP1_RX_CTRL & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
                }
            }
        }
    }
}
