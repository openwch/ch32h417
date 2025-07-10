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
#include "lcd.h"
#include "ov.h"

/* DVP Work Mode RGB565_MODE */

UINT32  JPEG_DVPDMAaddr0 = 0x2011A000;
UINT32  JPEG_DVPDMAaddr1 = 0x2011A000 + OV2640_JPEG_WIDTH;

UINT32  RGB565_DVPDMAaddr0 = 0x2011A000;
UINT32  RGB565_DVPDMAaddr1 = 0x2011A000 + RGB565_COL_NUM;

volatile UINT32 frame_cnt = 0;
volatile UINT32 addr_cnt = 0;
volatile UINT32 href_cnt = 0;


void DVP_IRQHandler (void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      LCD_Reset_GPIO_Init
 *
 * @brief   Init LCD reset GPIO.
 *
 * @return  none
 */
void LCD_Reset_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_15);
}

/*********************************************************************
 * @fn      DMA_SRAMLCD_Init
 *
 * @brief   Init SRAMLCD DMA
 *
 * @param   ddr: DVP data memory base addr.
 *
 * @return  none
 */
void DMA_SRAMLCD_Init(u32 ddr)
{
    DMA_InitTypeDef DMA_InitStructure={0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel3);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ddr;
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&LCD->LCD_RAM;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_ClearFlag(DMA1,DMA1_FLAG_TC3);
}
/*********************************************************************
 * @fn      DMA_SRAMLCD_Enable
 *
 * @brief   Enable SRAMLCD DMA transmission
 *
 * @return  none
 */
void DMA_SRAMLCD_Enable(void)
{
    DMA_Cmd(DMA1_Channel3, DISABLE );
    DMA_SetCurrDataCounter(DMA1_Channel3,lcddev.width);
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

/*********************************************************************
 * @fn      DVP_Function_Init
 *
 * @brief   Init DVP
 *
 * @return  none
 */
void DVP_Function_Init(void)
{
    DVP_InitTypeDef DVP_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DVP, ENABLE);

    DVP_InitStructure.DVP_DataSize = DVP_DataSize_8b;
    DVP_InitStructure.DVP_COL_NUM = RGB565_COL_NUM;
    DVP_InitStructure.DVP_ROW_NUM = RGB565_ROW_NUM;
    DVP_InitStructure.DVP_HCLK_P = DVP_Hclk_P_Rising;
    DVP_InitStructure.DVP_HSYNC_P = DVP_Hsync_P_High;
    DVP_InitStructure.DVP_VSYNC_P = DVP_Vsync_P_High;
    DVP_InitStructure.DVP_DMA_BUF0_Addr = RGB565_DVPDMAaddr0;
    DVP_InitStructure.DVP_DMA_BUF1_Addr = RGB565_DVPDMAaddr1;
    DVP_InitStructure.DVP_FrameCapRate = DVP_FrameCapRate_100P;
    DVP_InitStructure.DVP_JPEGMode = DISABLE;
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

        if (addr_cnt%2)     //buf1 done
        {
            addr_cnt++;
            //Send DVP data to LCD
            DMA_Cmd(DMA1_Channel3, DISABLE );
            DMA_SetCurrDataCounter(DMA1_Channel3,lcddev.width);
            DMA1_Channel3->PADDR = RGB565_DVPDMAaddr0;
            DMA_Cmd(DMA1_Channel3, ENABLE);
        }
        else                //buf0 done
        {
            addr_cnt++;
            //Send DVP data to LCD
            DMA_Cmd(DMA1_Channel3, DISABLE );
            DMA_SetCurrDataCounter(DMA1_Channel3,lcddev.width);
            DMA1_Channel3->PADDR = RGB565_DVPDMAaddr1;
            DMA_Cmd(DMA1_Channel3, ENABLE);
        }

        href_cnt++;
    }

    if (DVP_GetITStatus(DVP_IT_FRM_DONE) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_FRM_DONE);
        addr_cnt = 0;
        href_cnt = 0;
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
    /* LCD reset */
    LCD_Reset_GPIO_Init();
    GPIO_ResetBits(GPIOA,GPIO_Pin_15);
    Delay_Ms(100);
    GPIO_SetBits(GPIOA,GPIO_Pin_15);
    LCD_Init();

    while(OV2640_Init())
    {
        printf("Camera Err\r\n");
        Delay_Ms(1000);
    }
    printf("Camera Suc\r\n");


    Delay_Ms(1000);

    RGB565_Mode_Init();
    Delay_Ms(1000);   

    #if (DVP_Work_Mode == RGB565_MODE)
    printf("RGB565_MODE\r\n");

#endif

    DMA_SRAMLCD_Init((u32)RGB565_DVPDMAaddr0);  //DMA2
    DVP_Function_Init();

    while(1);
}
