/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides some functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "string.h"

#if (Run_Core == Run_Core_V3FandV5F)

#define TIM_PCK (10000ul)
#elif (Run_Core == Run_Core_V3F)

#define TIM_PCK (10000ul)
#elif (Run_Core == Run_Core_V5F)
#define TIM_PCK (4 * 10000ul)
#endif

#define test_width  (200)
#define test_height (200)

uint32_t test_rect[test_width * test_height];

#define StartTiming()           \
    {                           \
        TIM5->CNT = 0;          \
        TIM5->CTLR1 |= TIM_CEN; \
    }
#define GetTiming(tag)                                   \
    {                                                    \
        TIM5->CTLR1 &= (uint16_t)(~((uint16_t)TIM_CEN)); \
        uint32_t time = TIM5->CNT;                       \
        printf("%s tim %u \n", tag, time);               \
    }

/*********************************************************************
 * @fn      TIM5_Init
 * 
 * @brief   Initializes the TIM5
 * 
 * @return  none
 */
void TIM5_Init()
{
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM5, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    TIM_Cmd(TIM5, DISABLE);

    TIM_TimeBaseInitStructure.TIM_Period        = 0xffff;
    TIM_TimeBaseInitStructure.TIM_Prescaler     = ((SystemCoreClock / 4) / TIM_PCK) - 1;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStructure);

    TIM_ARRPreloadConfig(TIM5, DISABLE);
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
}

/*********************************************************************
 * @fn      GPHA_DrawRect
 * 
 * @brief   Draws a rectangle on a given image buffer.
 * 
 * @param x - The X coordinate of the top-left corner of the rectangle.
 *        y - The Y coordinate of the top-left corner of the rectangle.
 *        width - The width of the rectangle.
 *        height - The height of the rectangle.
 *        color - The color of the rectangle.
 *        origin_width - The width of the destination image buffer.
 *        origin_height - The height of the destination image buffer.
 *        image - A pointer to the destination image buffer where the rectangle will be drawn.
 *
 * @return  none
 */
void GPHA_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color,
                   uint32_t origin_width, uint32_t origin_height, uint8_t *image)
{
    GPHA_InitTypeDef GPHA_InitStruct = {0};

    uint32_t alpha = (color >> 24) & 0xff;
    uint32_t red   = (color >> 16) & 0xff;
    uint32_t green = (color >> 8) & 0xff;
    uint32_t blue  = color & 0xff;

    uint32_t offset  = 4 * (origin_width * y + x);
    uint32_t address = (uint32_t)image + offset;

    GPHA_InitStruct.GPHA_Mode            = GPHA_R2M;
    GPHA_InitStruct.GPHA_CMode           = GPHA_ARGB8888;
    GPHA_InitStruct.GPHA_OutputGreen     = green;
    GPHA_InitStruct.GPHA_OutputBlue      = blue;
    GPHA_InitStruct.GPHA_OutputRed       = red;
    GPHA_InitStruct.GPHA_OutputAlpha     = alpha;
    GPHA_InitStruct.GPHA_OutputMemoryAdd = address;
    GPHA_InitStruct.GPHA_OutputOffset    = origin_width - width;
    GPHA_InitStruct.GPHA_NumberOfLine    = height;
    GPHA_InitStruct.GPHA_PixelPerLine    = width;

    GPHA_Init(&GPHA_InitStruct);
    GPHA_StartTransfer();

    while (GPHA_GetFlagStatus(GPHA_FLAG_TC) == RESET)
        ;

    GPHA_ClearFlag(GPHA_FLAG_TC);
}

/*********************************************************************
 * @fn      Hardware
 * 
 * @brief   Main function
 * 
 * @return  none
 */
void Hardware(void)
{
    printf("GPHA R2M \n");
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPHA, ENABLE);

    TIM5_Init();

    memset(test_rect, 0, sizeof(test_rect));

    StartTiming();

    for (size_t i = 0; i < test_height; i++)
    {
        for (size_t j = 0; j < test_width; j++)
        {
            test_rect[i * test_width + j] = 0xff00ff00;
        }
    }

    GetTiming("soft Fill");

    memset(test_rect, 0, sizeof(test_rect));
    StartTiming();
    GPHA_DrawRect(0, 0, test_width, test_width, 0xff00ff00, test_width, test_width, (void *)test_rect);

    GetTiming("GPHA Fill");

    while (1)
    {
    }
}
