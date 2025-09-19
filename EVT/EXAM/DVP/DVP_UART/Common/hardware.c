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
#include "hardware.h"
#include "ov2640.h"

/* DVP Work Mode JPEG_MODE */

__attribute__ ((aligned(32))) UINT32  JPEG_DVPDMAaddr0 = 0x2011A000;
__attribute__ ((aligned(32))) UINT32  JPEG_DVPDMAaddr1 = 0x2011A000 + OV2640_JPEG_WIDTH;

volatile UINT32 frame_cnt = 0;
volatile UINT32 addr_cnt = 0;
volatile UINT32 href_cnt = 0;

void DVP_IRQHandler (void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      USART_Send_Byte
 *
 * @brief   USART send one byte data.
 *
 * @param   t - UART send Data.
 *
 * @return  none
 */
void USART_Send_Byte(u8 t)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, t);
}

/*********************************************************************
 * @fn      DVP_Init
 *
 * @brief   dvp fuunction
 *
 * @return  none
 */
void DVP_Function_Init(void)
{
    DVP_InitTypeDef DVP_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DVP, ENABLE);

    DVP_DeInit();
    DVP_InitStructure.DVP_DataSize = DVP_DataSize_8b;
    DVP_InitStructure.DVP_COL_NUM = OV2640_JPEG_WIDTH;
    DVP_InitStructure.DVP_HCLK_P = DVP_Hclk_P_Rising;
    DVP_InitStructure.DVP_HSYNC_P = DVP_Hsync_P_High;
    DVP_InitStructure.DVP_VSYNC_P = DVP_Vsync_P_High;
    DVP_InitStructure.DVP_DMA_BUF0_Addr = JPEG_DVPDMAaddr0;
    DVP_InitStructure.DVP_DMA_BUF1_Addr = JPEG_DVPDMAaddr1;
    DVP_InitStructure.DVP_FrameCapRate = DVP_FrameCapRate_25P;
    DVP_InitStructure.DVP_JPEGMode = ENABLE;
    DVP_Init(&DVP_InitStructure);

    DVP_ReceiveCircuitResetCmd(DISABLE);
    DVP_FIFO_ResetCmd(DISABLE);

    NVIC_EnableIRQ(DVP_IRQn);
    NVIC_SetPriority(DVP_IRQn, 0);

    DVP_ITConfig(DVP_IT_STR_FRM|DVP_IT_ROW_DONE|DVP_IT_FRM_DONE|DVP_IT_FIFO_OV|DVP_IT_STP_FRM, ENABLE);
    
    DVP_DMACmd(ENABLE);
    DVP_Cmd(ENABLE);
}

/*********************************************************************
 * @fn      DVP_IRQHandler
 *
 * @brief   This function handles DVP exception.
 *
 * @return  none
 */
void DVP_IRQHandler(void)
{
    if (DVP_GetITStatus(DVP_IT_ROW_DONE) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_ROW_DONE);

        href_cnt++;
        if (addr_cnt%2)     /* buf1 done */
        {
            addr_cnt++;
            DVP->DMA_BUF1 += OV2640_JPEG_WIDTH *2;
        }
        else                /* buf0 done */
        {
            addr_cnt++;
            DVP->DMA_BUF0 += OV2640_JPEG_WIDTH *2;
        }
    }

    if (DVP_GetITStatus(DVP_IT_FRM_DONE) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_FRM_DONE);

        DVP_Cmd(DISABLE);
        /* Use uart2 send JPEG data */
        {
            UINT32 i;
            UINT8 val;

            href_cnt = href_cnt*OV2640_JPEG_WIDTH;

            for(i=0; i<href_cnt; i++){
                val = *(UINT8*)(0x2011A000+i);
                USART_Send_Byte(val);
            }
        }
        DVP_Cmd(ENABLE);

        DVP->DMA_BUF0 = JPEG_DVPDMAaddr0;  /* DMA addr0 */
        DVP->DMA_BUF1 = JPEG_DVPDMAaddr1;  /* DMA addr1 */
        href_cnt = 0;

        addr_cnt =0;       
    }

    if (DVP_GetITStatus(DVP_IT_STR_FRM) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_STR_FRM);
        frame_cnt++;
    }

    if (DVP_GetITStatus(DVP_IT_STP_FRM) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_STP_FRM);
    }

    if (DVP_GetITStatus(DVP_IT_FIFO_OV) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_FIFO_OV);
    }
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   dvp fuunction
 *
 * @return  none
 */
void Hardware(void)
{
  while(OV2640_Init())
  {
      printf("Camera Model Error\r\n");
      Delay_Ms(1000);
  }
    Delay_Ms(1000);
    RGB565_Mode_Init();
    Delay_Ms(1000);

#if (DVP_Work_Mode == JPEG_MODE)
    printf("JPEG Mode\r\n");
    JPEG_Mode_Init();
    Delay_Ms(1000);

#endif
    DVP_Function_Init();
    while(1);
}
