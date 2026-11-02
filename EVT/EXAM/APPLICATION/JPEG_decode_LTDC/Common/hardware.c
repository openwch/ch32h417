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
#include <setjmp.h>
#include <stdlib.h>
#include "debug.h"
#include "jpeglib.h"

extern const unsigned char JPEG_IMAGE[];
extern const unsigned int  JPEG_IMAGE_SIZE;

#define HBP               (43)
#define VBP               (12)
#define HSW               (4)
#define VSW               (4)
#define HFP               (8)
#define VFP               (8)

#define LCD_Width         (480)
#define LCD_Height        (272)

#define layer1_w          (320)
#define layer1_h          (240)

#define layer2_color_mode LTDC_Pixelformat_ARGB4444

#define layer1_color_mode LTDC_Pixelformat_RGB565
#define layer1_pixel_size 2

const uint32_t layerCm = GPHA_RGB565;

uint8_t __attribute__((section(".sram2"))) layer1[(layer1_w * layer1_h * layer1_pixel_size)];

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
 *        baseImg - A pointer to the destination image.
 *        fgImg - A pointer to the image.
 *
 * @return  none
 */
void GPHA_MixImg(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t origin_width,
                 uint32_t origin_height, uint8_t* baseImg, uint8_t* fgImg)
{
    GPHA_InitTypeDef    GPHA_InitStruct    = {0};
    GPHA_FG_InitTypeDef GPHA_FG_InitStruct = {0};

    uint32_t offset  = 2 * (origin_width * y + x);
    uint32_t address = (uint32_t)baseImg + offset;

    GPHA_InitStruct.GPHA_Mode            = GPHA_M2M_PFC;
    GPHA_InitStruct.GPHA_CMode           = GPHA_RGB565;
    GPHA_InitStruct.GPHA_OutputGreen     = 0;
    GPHA_InitStruct.GPHA_OutputBlue      = 0;
    GPHA_InitStruct.GPHA_OutputRed       = 0;
    GPHA_InitStruct.GPHA_OutputAlpha     = 0;
    GPHA_InitStruct.GPHA_OutputMemoryAdd = (uint32_t)address;
    GPHA_InitStruct.GPHA_OutputOffset    = origin_width - width;
    GPHA_InitStruct.GPHA_NumberOfLine    = height;
    GPHA_InitStruct.GPHA_PixelPerLine    = width;
    GPHA_Init(&GPHA_InitStruct);

    GPHA_FG_InitStruct.GPHA_FGMA              = (uint32_t)fgImg;
    GPHA_FG_InitStruct.GPHA_FGO               = 0;
    GPHA_FG_InitStruct.GPHA_FGCM              = CM_RGB565;
    GPHA_FG_InitStruct.GPHA_FG_CLUT_CM        = CLUT_CM_ARGB8888;
    GPHA_FG_InitStruct.GPHA_FG_CLUT_SIZE      = 0;
    GPHA_FG_InitStruct.GPHA_FGPFC_ALPHA_VALUE = 0xff;
    GPHA_FG_InitStruct.GPHA_FGPFC_ALPHA_MODE  = NO_MODIF_ALPHA_VALUE;
    GPHA_FG_InitStruct.GPHA_FGC_BLUE          = 0xff;
    GPHA_FG_InitStruct.GPHA_FGC_GREEN         = 0xff;
    GPHA_FG_InitStruct.GPHA_FGC_RED           = 0xff;
    GPHA_FG_InitStruct.GPHA_FGCMAR            = (uint32_t)NULL;

    GPHA_FGConfig(&GPHA_FG_InitStruct);

    GPHA_StartTransfer();

    while (GPHA_GetFlagStatus(GPHA_FLAG_TC) == RESET)
        ;
}

// 5ms
void TIM4_init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM4, ENABLE);
    TIM_Cmd(TIM4, DISABLE);

    TIM_TimeBaseStructure.TIM_Period        = 500 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler     = SystemCoreClock / 100000 / 4 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_EnableIRQ(TIM4_IRQn);
    TIM_Cmd(TIM4, ENABLE);
}

/* Custom libjpeg error manager: the standard manager plus a jmp_buf, so a
   fatal decode error can jump back to the caller of the decoder. */
typedef struct
{
    struct jpeg_error_mgr pub;
    jmp_buf               setjmp_buffer;
} my_error_mgr_t;
typedef my_error_mgr_t* my_error_ptr_t;

/*********************************************************************
 * @fn      my_output_message
 *
 * @brief   Discard libjpeg messages so they do not pollute the output.
 *
 * @param   cinfo - pointer to the libjpeg common structure (unused)
 *
 * @return  none
 */
METHODDEF(void)
my_output_message(j_common_ptr cinfo)
{
    (void)cinfo;
}

/*********************************************************************
 * @fn      my_error_exit
 *
 * @brief   libjpeg fatal error handler, reports the error and returns control
 *          to the caller of jpeg_decode_to_sram().
 *
 * @param   cinfo - pointer to the libjpeg common structure
 *
 * @return  none (control returns to the setjmp point via longjmp)
 */
METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr_t myerr = (my_error_ptr_t)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

/*********************************************************************
 * @fn      rgb_565
 *
 * @brief   Pack three 8-bit colour components into one RGB565 pixel value.
 *
 * @param   r - red component   (8 bits, the high 5 bits are kept)
 *           g - green component (8 bits, the high 6 bits are kept)
 *           b - blue component  (8 bits, the high 5 bits are kept)
 *
 * @return  16-bit RGB565 pixel value
 */
static inline uint16_t rgb_565(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

/*********************************************************************
 * @fn      jpeg_decode_to_sram
 *
 * @brief   Decode the embedded JPEG image and write it into sram_frame
 *          (internal SRAM framebuffer) as RGB565 pixels.
 *
 * @return  0 on success, 1 if the decoder reports a fatal error
 */
static int jpeg_decode_to_sram(void)
{
    struct jpeg_decompress_struct cinfo;

    my_error_mgr_t jerr;
    uint32_t       w, h, x, y;
    JSAMPARRAY     row_buf;

    cinfo.err               = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit     = my_error_exit;
    jerr.pub.output_message = my_output_message;

    if (setjmp(jerr.setjmp_buffer))
    {
        printf("JPG decode ERROR\r\n");
        jpeg_destroy_decompress(&cinfo);
        return 1;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (unsigned char*)JPEG_IMAGE, JPEG_IMAGE_SIZE); /* pic src */
    jpeg_read_header(&cinfo, TRUE);

    w = cinfo.image_width;
    h = cinfo.image_height;

    cinfo.out_color_space     = JCS_RGB;
    cinfo.dct_method          = JDCT_IFAST;
    cinfo.do_fancy_upsampling = FALSE; /* 4:2:0 */
    jpeg_start_decompress(&cinfo);

#define JPG_STRIP 32 /* scanlines decoded per strip */

    row_buf = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo,
                                         JPOOL_IMAGE,
                                         cinfo.output_width * (size_t)cinfo.output_components,
                                         JPG_STRIP);

    while (cinfo.output_scanline < cinfo.output_height)
    {

        unsigned int got = jpeg_read_scanlines(&cinfo, row_buf, JPG_STRIP);

        for (unsigned int i = 0; i < got; i++)
        {

            y = (unsigned int)(cinfo.output_scanline - got) + i;
            if (y >= h)
                continue;

            const unsigned char* row    = row_buf[i];
            uint16_t*            screen = (void*)layer1;
            uint16_t*            d      = &screen[(size_t)y * layer1_w];

            for (x = 0; x + 3 < w; x += 4)
            {
                const unsigned char* rp = &row[x * 3];
                d[x + 0]                = rgb_565(rp[0], rp[1], rp[2]);
                d[x + 1]                = rgb_565(rp[3], rp[4], rp[5]);
                d[x + 2]                = rgb_565(rp[6], rp[7], rp[8]);
                d[x + 3]                = rgb_565(rp[9], rp[10], rp[11]);
            }

            for (; x < w; x++)
            {
                const unsigned char* rp = &row[x * 3];
                d[x]                    = rgb_565(rp[0], rp[1], rp[2]);
            }
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return 0;
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

    /* Enable the GPHA Clock */
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPHA, ENABLE);

    GPIO_Config();

    // LTDC¡¡CLK Settings
    // In this routine, the CLK required for the screen is 10M and the PLL is 400M, so divide by 40
    RCC_LTDCCLKConfig(RCC_LTDCClockSource_PLL);
    RCC_LTDCClockSourceDivConfig(RCC_LTDCClockSource_Div40);
    LCD_Config();

    memset(layer1, 0, sizeof(layer1));

    jpeg_decode_to_sram();

    while (1)
        ;
}
