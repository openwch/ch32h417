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

/* Mode Definition */
#define MODE1   0
#define MODE2   1

/* Mode Selection */
#define MODE MODE1
//#define MODE MODE2

/*********************************************************************
 * @fn      TIM1_UP_IRQHandler
 *
 * @brief   This function handles TIM1 UP exception.
 *
 *
 * @return  none
 */
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void)
{

    if(TIM_GetITStatus(TIM1, TIM_IT_Update)==SET)
    {   
        if( U3_UP_Flag == 0 )
        {
            U3_Time_Out_Count++;
            if(U3_Time_Out_Count > 1)
            {
                U3_Time_Out_Count = 0;
                U3_UP_Flag = 1;

                USBHS_Device_Init(ENABLE); 

            }
        }

    }
    TIM_ClearITPendingBit( TIM1, TIM_IT_Update );
}

/*********************************************************************
 * @fn      TIM1_INT_Init
 *
 * @brief   Initializes TIM1 output compare.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *
 * @return  none
 */
void TIM1_INT_Init( u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1, ENABLE );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 50;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    TIM_ClearITPendingBit( TIM1, TIM_IT_Update );

	NVIC_SetPriority(TIM1_UP_IRQn,0);
	NVIC_EnableIRQ(TIM1_UP_IRQn);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

}

/*********************************************************************
 * @fn      Hardware
 *
 * TIM1_INT
 *
 * @return  none
 */
void Hardware(void)
{
    uint8_t ret;

    /* Disable SWD */
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    USBSS_PLL_Init( ENABLE );
    RCC_HBPeriphClockCmd( RCC_HBPeriph_USBSS | RCC_HBPeriph_USBHS, ENABLE );
    RCC_PIPECmd( ENABLE );
    RCC_UTMIcmd(ENABLE);
    USBSS_Device_Init( ENABLE );

    NVIC_EnableIRQ( USBSS_IRQn );
    NVIC_EnableIRQ( USBSS_LINK_IRQn );
    USBSS_CFG_MOD();    

	printf("TIM_INT\r\n");
    TIM1_INT_Init( 200-1, 14400-1);
    #if(MODE==MODE1)

    TIM_Cmd( TIM1, ENABLE );

    #elif(MODE==MODE2)

    TIM_GenerateEvent(TIM1, TIM_IT_Update);

    #endif
	while (1)
    {
        /* Determine if enumeration is complete, perform data transfer if completed */
        if( USBHS_DevEnumStatus )
        {
            /* Data Transfer */
            if( RingBuffer_Comm.RemainPack )
            {
                ret = USBHS_Endp_DataUp(DEF_UEP1, &Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_USBD_HS_PACK_SIZE], RingBuffer_Comm.PackLen[RingBuffer_Comm.DealPtr], DEF_UEP_DMA_LOAD);
                if(ret == 0)
                {
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
