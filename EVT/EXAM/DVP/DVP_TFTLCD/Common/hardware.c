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
#include "ov5640.h"
#include "ov2640.h"
#include "lcd.h"

/* Camera module selection */
#define OV2640_Module  0
#define OV5640_Module  1
#define CAMERA_SELECT  OV5640_Module

#if(CAMERA_SELECT == OV2640_Module)
__attribute__ ((aligned(32))) uint16_t ImageDataOv2640[2 * OV2640_RGB565_WIDTH] = {0};
#elif(CAMERA_SELECT == OV5640_Module)
__attribute__ ((aligned(32))) uint16_t ImageDataOv5640[2 * OV5640_RGB565_WIDTH] = {0};
#endif

volatile UINT32 frame_cnt = 0;
volatile UINT32 href_cnt = 0;

void DVP_IRQHandler(void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      DMA_SRAMLCD_Enable
 *
 * @brief   Enable SRAMLCD DMA transmission
 *
 * @return  none
 */
void DMA_SRAMLCD_Enable(void)
{
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel3, lcddev.width);
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

/*********************************************************************
 * @fn      DVP_Init
 *
 * @brief   Init DVP
 *
 * @return  none
 */
void DVP_Function_Init(void) 
{
    DVP_InitTypeDef DVP_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DVP, ENABLE);

    DVP_DeInit();
    DVP_InitStructure.DVP_DataSize = DVP_DataSize_8b;
#if(CAMERA_SELECT == OV2640_Module)
    DVP_InitStructure.DVP_COL_NUM = OV2640_RGB565_WIDTH * 2;
    DVP_InitStructure.DVP_ROW_NUM = OV2640_RGB565_HEIGHT;
#elif(CAMERA_SELECT == OV5640_Module)
    DVP_InitStructure.DVP_COL_NUM = OV5640_RGB565_WIDTH * 2;
    DVP_InitStructure.DVP_ROW_NUM = OV5640_RGB565_HEIGHT;
#endif
    DVP_InitStructure.DVP_HCLK_P = DVP_Hclk_P_Rising;
    DVP_InitStructure.DVP_HSYNC_P = DVP_Hsync_P_High;
    DVP_InitStructure.DVP_VSYNC_P = DVP_Vsync_P_High;
#if(CAMERA_SELECT == OV2640_Module)
    DVP_InitStructure.DVP_DMA_BUF0_Addr = (uint32_t)(ImageDataOv2640);
    DVP_InitStructure.DVP_DMA_BUF1_Addr = (uint32_t)(ImageDataOv2640 + OV2640_RGB565_WIDTH);
#elif(CAMERA_SELECT == OV5640_Module)
    DVP_InitStructure.DVP_DMA_BUF0_Addr = (uint32_t)(ImageDataOv5640);
    DVP_InitStructure.DVP_DMA_BUF1_Addr = (uint32_t)(ImageDataOv5640 + OV5640_RGB565_WIDTH);
#endif
    DVP_InitStructure.DVP_FrameCapRate = DVP_FrameCapRate_100P;
    DVP_InitStructure.DVP_JPEGMode = DISABLE;
    DVP_Init(&DVP_InitStructure);

    DVP_ReceiveCircuitResetCmd(DISABLE);
    DVP_FIFO_ResetCmd(DISABLE);

    NVIC_EnableIRQ(DVP_IRQn);
    NVIC_SetPriority(DVP_IRQn, 0);

    DVP_ITConfig(DVP_IT_STR_FRM | DVP_IT_ROW_DONE | DVP_IT_FRM_DONE | DVP_IT_FIFO_OV | DVP_IT_STP_FRM, ENABLE);

    DVP_DMACmd(ENABLE);
    DVP_Cmd(ENABLE);
}

/*********************************************************************
 * @fn      LCD_Reset_GPIO_Init
 *
 * @brief   Init LCD reset GPIO.
 *
 * @return  none
 */
void LCD_Reset_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_ResetBits (GPIOD, GPIO_Pin_3);
    Delay_Ms(100);
    GPIO_SetBits (GPIOD, GPIO_Pin_3);
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
void DMA_SRAMLCD_Init(u32 addr)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = addr;
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&LCD->LCD_RAM;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_ClearFlag(DMA1, DMA1_FLAG_TC3);

    DMA_Cmd(DMA1_Channel3, ENABLE);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   dvp function
 *
 * @return  none
 */
void Hardware(void)
{  
#if(CAMERA_SELECT == OV2640_Module)
    LCD_Reset_GPIO_Init();
    LCD_Init();   
   while(OV2640_Init())
    {
        printf("Camera Initialize failed.\r\n");
        Delay_Ms(1000);
    }
    printf("Camera Initialize Success.\r\n");

    Delay_Ms(1000);

    OV2640_RGB565_Mode_Init();
    Delay_Ms(1000);   

    printf("RGB565 Mode...\r\n");

    DMA_SRAMLCD_Init((u32)ImageDataOv2640);
    DVP_Function_Init();
#endif

#if(CAMERA_SELECT == OV5640_Module)
    uint8_t tempstr[50]; 
    LCD_Reset_GPIO_Init();
    LCD_Init();   
    LCD_Display_Dir(1);
    
    sprintf ((char*)tempstr,"height: %d  width: %d  id: %#x\n", lcddev.height, lcddev.width, lcddev.id);
    LCD_ShowString(0,120-12,320,48,16,tempstr);

    LCD_SetCursor (0, 0);
    LCD_WriteRAM_Prepare();

    while(OV5640_Init()) {
        printf("Camera Initialize failed.\r\n");
        Delay_Ms(1000);
    }
    printf("Camera Initialize Success.\r\n");

    OV5640_RGB565_Mode_Init();
    Delay_Ms(500);
    printf("RGB565 Mode...\r\n");
    DMA_SRAMLCD_Init((uint32_t)ImageDataOv5640);
    DVP_Function_Init();
#endif
    
    while(1);
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
    if(DVP_GetITStatus(DVP_IT_ROW_DONE) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_ROW_DONE);

        if(href_cnt % 2)
        {
            /* Send DVP data to LCD */
            DMA_Cmd(DMA1_Channel3, DISABLE);    
#if(CAMERA_SELECT == OV2640_Module)
            DMA_SetCurrDataCounter(DMA1_Channel3, OV2640_RGB565_WIDTH * 2);
            DMA1_Channel3->PADDR = (uint32_t)(ImageDataOv2640);
#elif(CAMERA_SELECT == OV5640_Module)
            DMA_SetCurrDataCounter(DMA1_Channel3, OV5640_RGB565_WIDTH * 2);
            DMA1_Channel3->PADDR = (uint32_t)(ImageDataOv5640);
#endif     
            DMA_Cmd(DMA1_Channel3, ENABLE);
        }
        href_cnt++;
    }

    if(DVP_GetITStatus(DVP_IT_FRM_DONE) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_FRM_DONE);
        href_cnt = 0;
    }

    if(DVP_GetITStatus(DVP_IT_STR_FRM) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_STR_FRM);
        frame_cnt++;
    }

    if(DVP_GetITStatus(DVP_IT_STP_FRM) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_STP_FRM);
    }

    if(DVP_GetITStatus(DVP_IT_FIFO_OV) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_FIFO_OV);
    }
}
