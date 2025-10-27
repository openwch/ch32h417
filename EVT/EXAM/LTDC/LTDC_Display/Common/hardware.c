/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/10/21
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

#define layer1_w          (480)
#define layer1_h          (255)

#define layer2_w          (40)
#define layer2_h          (40)

#define layer1_color_mode LTDC_Pixelformat_RGB565

#define layer2_color_mode LTDC_Pixelformat_ARGB4444

#define layer1_pixel_size 2
#define layer2_pixel_size 2

__attribute__((aligned(32))) uint8_t layer1[(layer1_w * layer1_h * layer1_pixel_size)];
__attribute__((aligned(32))) uint8_t layer2[(layer2_w * layer2_h * layer2_pixel_size)];

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
 * @fn      rgb_to_rgb565
 * 
 * @brief   Convert 32-bit RGB color to 16-bit RGB565 color
 * 
 * @param   color - 32-bit RGB color
 * 
 * @return  16-bit RGB565 color
 */
static inline uint16_t rgb_to_rgb565(uint32_t color)
{
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

/*********************************************************************
 * @fn      Drawrect_rgb565
 * 
 * @brief   Draw a rectangle with RGB565 color on the image
 * 
 * @param   color - 32-bit RGB color
 *           x - x coordinate of the top-left point of the rectangle
 *           y - y coordinate of the top-left point of the rectangle
 *           w - width of the rectangle
 *           h - height of the rectangle
 *           img_w - width of the image
 *           img_h - height of the image
 *           img - pointer to the image buffer
 * 
 * @return  none
 */
void Drawrect_rgb565(uint32_t color, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t img_w,
                     uint32_t img_h, uint8_t* img)
{

    uint32_t end_x = (x + w > img_w) ? img_w : x + w;
    uint32_t end_y = (y + h > img_h) ? img_h : y + h;

    uint16_t rgb565_color = rgb_to_rgb565(color);
    uint8_t  rgb565_l     = (rgb565_color >> 8) & 0xFF;
    uint8_t  rgb565_h     = rgb565_color & 0xFF;

    for (uint32_t j = y; j < end_y; ++j)
    {
        for (uint32_t i = x; i < end_x; ++i)
        {
            uint32_t index = (j * img_w + i) * 2;

            img[index]     = rgb565_h;
            img[index + 1] = rgb565_l;
        }
    }
}

/*********************************************************************
 * @fn      argb_to_argb4444
 * 
 * @brief   Convert 32-bit ARGB color to 16-bit ARGB4444 color
 * 
 * @param   color - 32-bit ARGB color
 * 
 * @return  16-bit ARGB4444 color
 */
uint16_t argb_to_argb4444(uint32_t color)
{
    uint8_t a = (color >> 24) & 0xFF;
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    return ((a >> 4) << 12) | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4);
}

/*********************************************************************
 * @fn      Drawrect_argb4444
 * 
 * @brief   Draw a rectangle with ARGB4444 color on the image
 * 
 * @param   color - 32-bit ARGB color
 *           x - x coordinate of the top-left point of the rectangle
 *           y - y coordinate of the top-left point of the rectangle
 *           w - width of the rectangle
 *           h - height of the rectangle
 *           img_w - width of the image
 *           img_h - height of the image
 *           img - pointer to the image buffer
 * 
 * @return  none
 */
void Drawrect_argb4444(uint32_t color, uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                       uint32_t img_w, uint32_t img_h, uint8_t* img)
{

    uint32_t end_x = (x + w > img_w) ? img_w : x + w;
    uint32_t end_y = (y + h > img_h) ? img_h : y + h;

    uint16_t argb4444_color = argb_to_argb4444(color);

    uint8_t argb4444_h = (argb4444_color >> 8) & 0xFF;
    uint8_t argb4444_l = argb4444_color & 0xFF;

    for (uint32_t j = y; j < end_y; ++j)
    {
        for (uint32_t i = x; i < end_x; ++i)
        {
            uint32_t index = (j * img_w + i) * 2;

            img[index]     = argb4444_l;
            img[index + 1] = argb4444_h;
        }
    }
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
    LTDC_Layer_InitTypeDef LTDC_Layer_2InitStruct = {0};

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

    // layer2
    LTDC_Layer_2InitStruct.LTDC_HorizontalStart = 20;
    LTDC_Layer_2InitStruct.LTDC_HorizontalStop  = layer2_w + 20;
    LTDC_Layer_2InitStruct.LTDC_VerticalStart   = 20;
    LTDC_Layer_2InitStruct.LTDC_VerticalStop    = layer2_h + 20;

    LTDC_Layer_2InitStruct.LTDC_PixelFormat = layer2_color_mode;

    LTDC_Layer_2InitStruct.LTDC_ConstantAlpha = 0xff;

    LTDC_Layer_2InitStruct.LTDC_DefaultColorBlue  = 0;
    LTDC_Layer_2InitStruct.LTDC_DefaultColorGreen = 0;
    LTDC_Layer_2InitStruct.LTDC_DefaultColorRed   = 0;
    LTDC_Layer_2InitStruct.LTDC_DefaultColorAlpha = 0;

    LTDC_Layer_2InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;
    LTDC_Layer_2InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;

    LTDC_Layer_2InitStruct.LTDC_CFBStartAdress = (uint32_t)layer2;

    LTDC_Layer_2InitStruct.LTDC_CFBLineLength = ((layer2_w * layer2_pixel_size) + 31);

    LTDC_Layer_2InitStruct.LTDC_CFBPitch = (layer2_w * layer2_pixel_size);

    LTDC_Layer_2InitStruct.LTDC_CFBLineNumber = layer2_h;

    LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_1InitStruct);

    LTDC_LayerCmd(LTDC_Layer1, ENABLE);

    LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_2InitStruct);

    LTDC_LayerCmd(LTDC_Layer2, ENABLE);

    LTDC_ReloadConfig(LTDC_IMReload);

    LTDC_Cmd(ENABLE);
}

/*********************************************************************
 * @fn      Move_Layer2
 *
 * @brief   Move layer2.
 *
 * @param   x - x position.
 *          y - y position.
 *
 * @return  none
 */
void Move_Layer2(int16_t x, int16_t y)
{
    uint32_t startX = x;
    uint32_t startY = y;
    uint32_t endX   = layer2_w + x;
    uint32_t endY   = layer2_h + y;

    LTDC_Layer2->WVPCR = endY << 16 | startY;
    LTDC_Layer2->WHPCR = endX << 16 | startX;
    LTDC_ReloadConfig(LTDC_VBReload);
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
    uint32_t color_index  = 0;
    uint32_t color_change = 0;

    int32_t layer2_x = 20;
    int32_t layer2_y = 20;

    int32_t layer2_ax = 1;
    int32_t layer2_ay = 1;

    GPIO_Config();

    // LTDC CLK Settings
    // In this routine, the CLK required for the screen is 10M and the PLL is 400M, so divide by 40
    RCC_LTDCCLKConfig(RCC_LTDCClockSource_PLL);
    RCC_LTDCClockSourceDivConfig(RCC_LTDCClockSource_Div40);
    LCD_Config();

    Drawrect_rgb565(argb8888(0xff, 0xff, 0xff, 0xff), 0, 0, layer1_w, layer1_h, layer1_w, layer1_h,
                    layer1);

    const uint32_t mh = (layer1_h / 3);
    const uint32_t eh = layer1_h - mh * 2;

    uint32_t h = 0;

    for (size_t i = 0; i < 255; i++)
    {
        Drawrect_rgb565(i << 16, i, h, 1, mh, layer1_w, layer1_h, layer1);
    }
    h += mh;

    for (size_t i = 0; i < 255; i++)
    {
        Drawrect_rgb565(i << 8, i, h, 1, mh, layer1_w, layer1_h, layer1);
    }
    h += mh;
    for (size_t i = 0; i < 255; i++)
    {
        Drawrect_rgb565(i, i, h, 1, eh, layer1_w, layer1_h, layer1);
    }
    const uint32_t mw = layer1_w - 255;

    for (uint8_t i = 0; i < 255; i++)
    {
        if (i > layer1_h)
        {
            break;
        }
        Drawrect_rgb565(i | (i << 8) | (i << 16), 255, i, mw, 1, layer1_w, layer1_h, layer1);
    }

    Drawrect_argb4444(color_list[0], 0, 0, layer2_w, layer2_h, layer2_w, layer2_h, layer2);

    while (1)
    {
        Delay_Ms(10);
        layer2_x += layer2_ax;
        layer2_y += layer2_ay;

        if (layer2_x < 0)
        {
            layer2_x     = 0;
            layer2_ax    = -layer2_ax;
            color_change = 1;
        }
        if (layer2_x + layer2_w > LCD_Width)
        {
            layer2_x     = LCD_Width - layer2_w;
            layer2_ax    = -layer2_ax;
            color_change = 1;
        }
        if (layer2_y < 0)
        {
            layer2_y     = 0;
            layer2_ay    = -layer2_ay;
            color_change = 1;
        }
        if (layer2_y + layer2_h > LCD_Height)
        {
            layer2_y     = LCD_Height - layer2_h;
            layer2_ay    = -layer2_ay;
            color_change = 1;
        }
        if (color_change)
        {
            color_index = (color_index + 1) % 7;
            Drawrect_argb4444(color_list[color_index], 0, 0, layer2_w, layer2_h, layer2_w, layer2_h,
                              layer2);
            color_change = 0;
        }

        Move_Layer2(layer2_x, layer2_y);
    }

    while (1)
        ;
}
