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

#define HBP               (43)
#define VBP               (12)
#define HSW               (4)
#define VSW               (4)
#define HFP               (8)
#define VFP               (8)

#define LCD_Width         (480)
#define LCD_Height        (272)

#define layer1_w          (350)
#define layer1_h          (200)

#define layer2_w          (240)  // logo size
#define layer2_h          (60)

#define layer1_color_mode LTDC_Pixelformat_RGB565
#define layer1_pixel_size 2

const uint32_t layerCm = GPHA_RGB565;

__attribute__((aligned(32))) uint8_t layer1[(layer1_w * layer1_h * layer1_pixel_size)];
__attribute__((aligned(32))) uint8_t layer2[(layer2_w * layer2_h * 4)];

extern const uint8_t img_logo_color[];
extern const uint8_t img_logo_map[];

extern const uint32_t img_logo_map_size;
extern const uint32_t img_logo_color_size;
extern const uint32_t img_logo_w;
extern const uint32_t img_logo_h;

/*********************************************************************
 * @fn      argb8888
 *
 * @brief   Combine the four color  into a 32-bit
 * 
 * @param   a - alpha value
 *           r - red value  
 *           g - green value
 *           b - blue value
 * 
 * @return  32-bit color value
 */
static inline uint32_t argb8888(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

/*********************************************************************
 * @fn      GPIO_Config
 * 
 * @brief   Configure the GPIO pins
 * 
 * @param   none
 */
static void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOD, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOF, ENABLE);

    // CLK PF1(AF14)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // HSYNC PC6(AF14)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // VSYNC PA7(AF14)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // DE PC5(AF14)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // R[0] PE1(AF14)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // R[1] PF0(AF11)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF11);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // R[2] PD13(AF3)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF3);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // R[3] PD12(AF11)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF11);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // R[4] PD11(AF11)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF11);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // R[5] PA9(AF14)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // R[6] PA8(AF14)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // R[7] PC4(AF14)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // G[0] PE5(AF14)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // G[1] PE6(AF14)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // G[2] PA6(AF14)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // G[3] PC9(AF10)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // G[4] PC8(AF14)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // G[5] PC1(AF14)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // G[6] PC7(AF14)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // G[7] PD3(AF14)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // B[0] PE4(AF14)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // B[1] PE0(AF14)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // B[2] PD6(AF14)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // B[3] PD7(AF14)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // B[4] PD4(AF14)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // B[5] PD5(AF14)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // B[6] PA14(AF14)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // B[7] PD2(AF9)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF9);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // BL
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

/*********************************************************************
 * @fn      LCD_Config
 * 
 * @brief   LCD configuration.
 *
 * @return  none
 */
static void LCD_Config(void)
{

    LTDC_InitTypeDef       LTDC_InitStruct        = {0};
    LTDC_Layer_InitTypeDef LTDC_Layer_1InitStruct = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_LTDC, ENABLE);

    LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AH;
    LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;
    LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AH;
    LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IIPC;

    LTDC_InitStruct.LTDC_HorizontalSync     = HSW - 1;
    LTDC_InitStruct.LTDC_VerticalSync       = VSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedHBP     = HBP + HSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedVBP     = VBP + VSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedActiveW = LCD_Width + HSW + HBP - 1;
    LTDC_InitStruct.LTDC_AccumulatedActiveH = LCD_Height + VSW + VBP - 1;
    LTDC_InitStruct.LTDC_TotalWidth         = LCD_Width + HSW + HBP + HFP - 1;
    LTDC_InitStruct.LTDC_TotalHeigh         = LCD_Height + VSW + VBP + VFP - 1;

    LTDC_InitStruct.LTDC_BackgroundRedValue   = 0xff;
    LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
    LTDC_InitStruct.LTDC_BackgroundBlueValue  = 0;

    LTDC_Init(&LTDC_InitStruct);

    // layer1
    LTDC_Layer_1InitStruct.LTDC_HorizontalStart = 0;
    LTDC_Layer_1InitStruct.LTDC_HorizontalStop  = layer1_w;
    LTDC_Layer_1InitStruct.LTDC_VerticalStart   = 0;
    LTDC_Layer_1InitStruct.LTDC_VerticalStop    = layer1_h;

    LTDC_Layer_1InitStruct.LTDC_PixelFormat = layer1_color_mode;

    LTDC_Layer_1InitStruct.LTDC_ConstantAlpha = 0xff;

    LTDC_Layer_1InitStruct.LTDC_DefaultColorBlue  = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorGreen = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorRed   = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorAlpha = 0;

    LTDC_Layer_1InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
    LTDC_Layer_1InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

    LTDC_Layer_1InitStruct.LTDC_CFBStartAdress = (uint32_t)layer1;

    LTDC_Layer_1InitStruct.LTDC_CFBLineLength = ((layer1_w * layer1_pixel_size) + 31);

    LTDC_Layer_1InitStruct.LTDC_CFBPitch = (layer1_w * layer1_pixel_size);

    LTDC_Layer_1InitStruct.LTDC_CFBLineNumber = layer1_h;

    LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_1InitStruct);

    LTDC_LayerCmd(LTDC_Layer1, ENABLE);

    LTDC_ReloadConfig(LTDC_IMReload);

    LTDC_Cmd(ENABLE);
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
 *        origin_width - The width of the destination image.
 *        origin_height - The height of the destination image.
 *        cmode - The color mode that specifies how the color should be interpreted.
 *        image - A pointer to the destination image buffer where the rectangle will be drawn.
 *
 * @return  none
 */
void GPHA_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color,
                   uint32_t origin_width, uint32_t origin_height, uint32_t cmode, uint8_t* image)
{
    GPHA_InitTypeDef GPHA_InitStruct = {0};

    uint32_t ps = 2;
    if (cmode == GPHA_ARGB8888)
    {
        ps = 4;
    }
    else if (cmode == GPHA_RGB565 || cmode == GPHA_ARGB1555 || cmode == GPHA_ARGB4444)
    {
        ps = 2;
    }
    else if (cmode == GPHA_RGB888)
    {
        ps = 3;
    }

    uint32_t alpha = (color >> 24) & 0xff;
    uint32_t red   = (color >> 16) & 0xff;
    uint32_t green = (color >> 8) & 0xff;
    uint32_t blue  = color & 0xff;

    uint32_t offset  = ps * (origin_width * y + x);
    uint32_t address = (uint32_t)image + offset;

    GPHA_InitStruct.GPHA_Mode            = GPHA_R2M;
    GPHA_InitStruct.GPHA_CMode           = cmode;
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
 * @fn      GPHA_MixImg
 * 
 * @brief   Mix img
 * 
 * @param x - The X coordinate.
 *        y - The Y coordinate.
 *        width - The width.
 *        height - The height.
 *        origin_width - The width of the destination image.
 *        origin_height - The height of the destination image.
 *        cmode - The color mode that specifies how the color should be interpreted.
 *        image - A pointer to the destination image.
 *
 * @return  none
 */
void GPHA_MixImg(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t origin_width,
                 uint32_t origin_height, uint32_t cmode, uint8_t* baseImg)
{
    GPHA_InitTypeDef    GPHA_InitStruct    = {0};
    GPHA_FG_InitTypeDef GPHA_FG_InitStruct = {0};
    GPHA_BG_InitTypeDef GPHA_BG_InitStruct = {0};

    uint32_t ps = 2;
    if (cmode == GPHA_ARGB8888)
    {
        ps = 4;
    }
    else if (cmode == GPHA_RGB565 || cmode == GPHA_ARGB1555 || cmode == GPHA_ARGB4444)
    {
        ps = 2;
    }
    else if (cmode == GPHA_RGB888)
    {
        ps = 3;
    }
    uint32_t offset  = ps * (origin_width * y + x);
    uint32_t address = (uint32_t)baseImg + offset;

    GPHA_InitStruct.GPHA_Mode            = GPHA_M2M_BLEND;
    GPHA_InitStruct.GPHA_CMode           = cmode;
    GPHA_InitStruct.GPHA_OutputGreen     = 0;
    GPHA_InitStruct.GPHA_OutputBlue      = 0;
    GPHA_InitStruct.GPHA_OutputRed       = 0;
    GPHA_InitStruct.GPHA_OutputAlpha     = 0;
    GPHA_InitStruct.GPHA_OutputMemoryAdd = (uint32_t)address;
    GPHA_InitStruct.GPHA_OutputOffset    = origin_width - width;
    GPHA_InitStruct.GPHA_NumberOfLine    = height;
    GPHA_InitStruct.GPHA_PixelPerLine    = width;

    GPHA_Init(&GPHA_InitStruct);

    GPHA_FG_InitStruct.GPHA_FGMA              = (uint32_t)img_logo_map;
    GPHA_FG_InitStruct.GPHA_FGO               = 0;
    GPHA_FG_InitStruct.GPHA_FGCM              = CM_L8;
    GPHA_FG_InitStruct.GPHA_FG_CLUT_CM        = CLUT_CM_ARGB8888;
    GPHA_FG_InitStruct.GPHA_FG_CLUT_SIZE      = (img_logo_color_size / 4) - 1;
    GPHA_FG_InitStruct.GPHA_FGPFC_ALPHA_VALUE = 0x80;
    GPHA_FG_InitStruct.GPHA_FGPFC_ALPHA_MODE  = NO_MODIF_ALPHA_VALUE;
    GPHA_FG_InitStruct.GPHA_FGC_BLUE          = 0xff;
    GPHA_FG_InitStruct.GPHA_FGC_GREEN         = 0xff;
    GPHA_FG_InitStruct.GPHA_FGC_RED           = 0xff;
    GPHA_FG_InitStruct.GPHA_FGCMAR            = (uint32_t)img_logo_color;

    GPHA_FGConfig(&GPHA_FG_InitStruct);

    // enalbe lut
    uint32_t cmd = (1 << 8);
    GPHA->FGCWRS = cmd;

    GPHA_FGStart(ENABLE);
    Delay_Ms(100);

    GPHA_BG_InitStruct.GPHA_BGMA              = (uint32_t)layer2;
    GPHA_BG_InitStruct.GPHA_BGO               = 0;
    GPHA_BG_InitStruct.GPHA_BGCM              = CM_ARGB8888;
    GPHA_BG_InitStruct.GPHA_BG_CLUT_CM        = CLUT_CM_ARGB8888;
    GPHA_BG_InitStruct.GPHA_BG_CLUT_SIZE      = 0;
    GPHA_BG_InitStruct.GPHA_BGPFC_ALPHA_VALUE = 0x80;
    GPHA_BG_InitStruct.GPHA_BGPFC_ALPHA_MODE  = NO_MODIF_ALPHA_VALUE;
    GPHA_BG_InitStruct.GPHA_BGC_BLUE          = 0xff;
    GPHA_BG_InitStruct.GPHA_BGC_GREEN         = 0xff;
    GPHA_BG_InitStruct.GPHA_BGC_RED           = 0xff;
    GPHA_BG_InitStruct.GPHA_BGCMAR            = (uint32_t)NULL;

    GPHA_BGConfig(&GPHA_BG_InitStruct);

    GPHA_StartTransfer();

    while (GPHA_GetFlagStatus(GPHA_FLAG_TC) == RESET)
        ;

    GPHA_ClearFlag(GPHA_FLAG_TC);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   main.
 *
 * @return  none
 */
void Hardware(void)
{
    printf("LTDC_Display\n");

    uint32_t color_list[] = {
        argb8888(0x9f, 0xff, 0xff, 0xff),
        argb8888(0x9f, 0x0, 0xff, 0xff),
        argb8888(0x9f, 0xff, 0x0, 0xff),
        argb8888(0x9f, 0xff, 0xff, 0x0),
        argb8888(0x9f, 0x0, 0x0, 0xff),
        argb8888(0x9f, 0x0, 0xff, 0x0),
        argb8888(0x9f, 0xff, 0x0, 0x0),
    };
    uint32_t color_index = 0;

    /* Enable the GPHA Clock */
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPHA, ENABLE);

    GPIO_Config();

    // LTDC¡¡CLK Settings
    // In this routine, the CLK required for the screen is 10M and the PLL is 400M, so divide by 40
    RCC_LTDCCLKConfig(RCC_LTDCClockSource_PLL);
    RCC_LTDCClockSourceDivConfig(RCC_LTDCClockSource_Div40);
    LCD_Config();

    GPHA_DrawRect(0, 0, layer2_w, layer2_h, argb8888(0xff, 0xff, 0xff, 0xff), layer2_w, layer2_h, GPHA_ARGB8888, layer2);

    GPHA_DrawRect(0, 0, layer1_w, layer1_h, argb8888(0xff, 0xff, 0xff, 0xff), layer1_w, layer1_h, layerCm, layer1);

    GPHA_MixImg(20, 20, layer2_w, layer2_h, layer1_w, layer1_h, layerCm, layer1);

    while (1)
    {
        Delay_Ms(500);
        color_index = (color_index + 1) % 7;

        GPHA_DrawRect(0, 0, layer2_w, layer2_h, color_list[color_index], layer2_w, layer2_h, GPHA_ARGB8888, layer2);
        GPHA_MixImg(20, 20, layer2_w, layer2_h, layer1_w, layer1_h, layerCm, layer1);
    }

    while (1)
        ;
}
