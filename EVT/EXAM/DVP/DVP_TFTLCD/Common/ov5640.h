/********************************** (C) COPYRIGHT *******************************
* File Name          : ov5640.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/09/15
* Description        : This file contains the headers of the OV5640.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __OV5640_H
#define __OV5640_H

#include "debug.h"

#ifndef NULL
#define NULL                    0
#endif

#ifndef VOID
#define VOID                    void
#endif
#ifndef CONST
#define CONST                   const
#endif
#ifndef BOOL
typedef unsigned char           BOOL;
#endif
#ifndef BOOLEAN
typedef unsigned char           BOOLEAN;
#endif
#ifndef CHAR
typedef char                    CHAR;
#endif
#ifndef INT8
typedef char                    INT8;
#endif
#ifndef INT16
typedef short                   INT16;
#endif
#ifndef INT32
typedef long                    INT32;
#endif
#ifndef UINT8
typedef unsigned char           UINT8;
#endif
#ifndef UINT16
typedef unsigned short          UINT16;
#endif
#ifndef UINT32
typedef unsigned long           UINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile  UINT8V;
#endif
#ifndef UINT16V
typedef unsigned short volatile UINT16V;
#endif
#ifndef UINT32V
typedef unsigned long volatile  UINT32V;
#endif

#ifndef PVOID
typedef void                    *PVOID;
#endif
#ifndef PCHAR
typedef char                    *PCHAR;
#endif
#ifndef PCHAR
typedef const char              *PCCHAR;
#endif
#ifndef PINT8
typedef char                    *PINT8;
#endif
#ifndef PINT16
typedef short                   *PINT16;
#endif
#ifndef PINT32
typedef long                    *PINT32;
#endif
#ifndef PUINT8
typedef unsigned char           *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short          *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long           *PUINT32;
#endif
#ifndef PUINT8V
typedef volatile unsigned char  *PUINT8V;
#endif
#ifndef PUINT16V
typedef volatile unsigned short *PUINT16V;
#endif
#ifndef PUINT32V
typedef volatile unsigned long  *PUINT32V;
#endif


/* RGB565 pixel 320*240 */
#define OV5640_RGB565_WIDTH  320
#define OV5640_RGB565_HEIGHT 240

#define SCCB_ID   		0x3C
#define OV5640_CHIP_ID  0x5640

#define OV5640_SUCCESS  0
#define OV5640_ERROR    1
#define OV5640_TIMEOUT  5000

#define SCCB_WRITE   	0x00
#define SCCB_READ   	0x01

/* PD2-RESET  PB0-PWDN */
#define OV_RESET_SET    {GPIOD->BSHR = GPIO_Pin_2;}
#define OV_RESET_CLR    {GPIOD->BCR = GPIO_Pin_2;}
#define OV_PWDN_SET     {GPIOB->BSHR = GPIO_Pin_0;}
#define OV_PWDN_CLR     {GPIOB->BCR = GPIO_Pin_0;}

/* PC0-SCL  PC1-SDA */
#define IIC_SCL_IN      {GPIOC->CFGLR&=0XFFFFFFF0;GPIOC->CFGLR|=8<<0;}
#define IIC_SCL_OUT     {GPIOC->CFGLR&=0XFFFFFFF0;GPIOC->CFGLR|=3<<0;}
#define IIC_SDA_IN      {GPIOC->CFGLR&=0XFFFFFF0F;GPIOC->CFGLR|=8<<4;}
#define IIC_SDA_OUT     {GPIOC->CFGLR&=0XFFFFFF0F;GPIOC->CFGLR|=3<<4;}

#define IIC_SDA_SET     {GPIOC->BSHR = GPIO_Pin_1;}
#define IIC_SDA_CLR     {GPIOC->BCR = GPIO_Pin_1;}
#define IIC_SCL_SET     {GPIOC->BSHR = GPIO_Pin_0;}
#define IIC_SCL_CLR     {GPIOC->BCR = GPIO_Pin_0;}

#define SDA_IN_R        (GPIOC->INDR & GPIO_Pin_1)

#define OV5640_ISP_INPUT_WIDTH_MAX    0x0A3F
#define OV5640_ISP_INPUT_HEIGHT_MAX   0x06A9
#define OV5640_FW_DOWNLOAD_ADDR       0x8000


/* ov5640 register configuration structure definition */
typedef struct
{
    uint16_t reg;
    uint8_t  data;
} OV5640_RegCfg_t;

/* ov5640 color contrast enumeration definition */
typedef enum
{
    OV5640_Contrast_0 = 0, /* contrast +4 */
    OV5640_Contrast_1,     /* contrast +3 */
    OV5640_Contrast_2,     /* contrast +2 */
    OV5640_Contrast_3,     /* contrast +1 */
    OV5640_Contrast_4,     /* contrast 0 */
    OV5640_Contrast_5,     /* contrast -1 */
    OV5640_Contrast_6,     /* contrast -2 */
    OV5640_Contrast_7,     /* contrast -3 */
    OV5640_Contrast_8,     /* contrast -4 */
} OV5640_Contrast_t;

/* ov5640 color brightness enumeration definition */
typedef enum
{
    OV5640_Brightness_0 = 0, /* brightness +4 */
    OV5640_Brightness_1,     /* brightness +3 */
    OV5640_Brightness_2,     /* brightness +2 */
    OV5640_Brightness_3,     /* brightness +1 */
    OV5640_Brightness_4,     /* brightness 0 */
    OV5640_Brightness_5,     /* brightness -1 */
    OV5640_Brightness_6,     /* brightness -2 */
    OV5640_Brightness_7,     /* brightness -3 */
    OV5640_Brightness_8,     /* brightness -4 */
} OV5640_Brightness_t;

/* ov5640 sharpness enumeration definition */
typedef enum
{
    OV5640_Sharpness_0 = 0, /* sharpness 0 */
    OV5640_Sharpness_1,     /* sharpness 1 */
    OV5640_Sharpness_2,     /* sharpness 2 */
    OV5640_Sharpness_3,     /* sharpness 3 */
    OV5640_Sharpness_4,     /* sharpness 4 */
    OV5640_Sharpness_5,     /* sharpness 5 */
    OV5640_Sharpness_6,     /* sharpness 6 */
    OV5640_Sharpness_7,     /* sharpness 7 */
    OV5640_Sharpness_8,     /* sharpness 8 */
    OV5640_Sharpness_9,     /* sharpness auto */
} OV5640_Sharpness_t;

/* ov5640 color hue enumeration definition */
typedef enum
{
    OV5640_Hue_0 = 0, /* hue -180бу */
    OV5640_Hue_1,     /* hue -150бу */
    OV5640_Hue_2,     /* hue -120бу */
    OV5640_Hue_3,     /* hue -90бу */
    OV5640_Hue_4,     /* hue -60бу */
    OV5640_Hue_5,     /* hue -30бу */
    OV5640_Hue_6,     /* hue 0бу */
    OV5640_Hue_7,     /* hue +30бу */
    OV5640_Hue_8,     /* hue +60бу */
    OV5640_Hue_9,     /* hue +90бу */
    OV5640_Hue_10,    /* hue +120бу */
    OV5640_Hue_11,    /* hue +150бу */
} OV5640_Hue_t;

/* ov5640 exposure enumeration definition */
typedef enum
{
    OV5640_Exposure_0 = 0, /* exposure EV -1.7 */
    OV5640_Exposure_1,     /* exposure EV -1.3 */
    OV5640_Exposure_2,     /* exposure EV -1.0 */
    OV5640_Exposure_3,     /* exposure EV -0.7 */
    OV5640_Exposure_4,     /* exposure EV -0.3 */
    OV5640_Exposure_5,     /* exposure default */
    OV5640_Exposure_6,     /* exposure EV 0.3 */
    OV5640_Exposure_7,     /* exposure EV 0.7 */
    OV5640_Exposure_8,     /* exposure EV 1.0 */
    OV5640_Exposure_9,     /* exposure EV 1.3 */
    OV5640_Exposure_10,    /* exposure EV 1.7 */
} OV5640_Exposure_t;

/* ov5640 color saturation enumeration definition */
typedef enum
{
    OV5640_ColorSaturation_0 = 0, /* color saturation +4 */
    OV5640_ColorSaturation_1,     /* color saturation +3 */
    OV5640_ColorSaturation_2,     /* color saturation +2 */
    OV5640_ColorSaturation_3,     /* color saturation +1 */
    OV5640_ColorSaturation_4,     /* color saturation 0 */
    OV5640_ColorSaturation_5,     /* color saturation -1 */
    OV5640_ColorSaturation_6,     /* color saturation -2 */
    OV5640_ColorSaturation_7,     /* color saturation -3 */
    OV5640_ColorSaturation_8,     /* color saturation -4 */
} OV5640_ColorSaturation_t;

/* ov5640 Lighting mode enumeration definition */
typedef enum
{
    OV5640_LightMode_AdvancedAWB = 0, /* advanced white balance mode */
    OV5640_LightMode_SimpleAWB,       /* simple white balance mode */
    OV5640_LightMode_Sunny,           /* sunny mode */
    OV5640_LightMode_Auto,            /* auto mode */
    OV5640_LightMode_CWF,             /* cwf mode */
    OV5640_LightMode_Cloudy           /* cloudy mode */
} OV5640_LightMode_t;


/* ov5640 special effect enumeration definition */
typedef enum
{
    OV5640_SpecialEffect_Normal = 0, /* Normal */
    OV5640_SpecialEffect_BW,         /* B&W */
    OV5640_SpecialEffect_Bluish,     /* Bluish */
    OV5640_SpecialEffect_Sepia,      /* Sepia */
    OV5640_SpecialEffect_Reddish,    /* Reddish */
    OV5640_SpecialEffect_Greenish,   /* Greenish */
    OV5640_SpecialEffect_Negative,   /* Negative */
} OV5640_SpecialEffect_t;


/* ov5640 mirror\inversion enumeration definition */
typedef enum
{
    OV5640_MirrorInversion_0 = 0, /* mirror */
    OV5640_MirrorInversion_1,     /* inversion */
    OV5640_MirrorInversion_2,     /* mirror and inversion */
    OV5640_MirrorInversion_3,     /* normal */
} OV5640_MirrorInversion_t;

/* ov5640 test pattern enumeration definition */
typedef enum
{
    OV5640_TestPattern_Off = 0,     /* off */
    OV5640_TestPattern_ColorBar,    /* color bar */
    OV5640_TestPattern_ColorSquare, /* color square */
} OV5640_TestPattern_t;

/* ov5640 output image format enumeration definition */
typedef enum
{
    OV5640_OutputFormat_RGB565 = 0, /* RGB565 */
    OV5640_OutputFormat_JPEG,       /* JPEG */
} OV5640_OutputFormat_t;


void OV5640_SCCB_GPIO_Init(void);
void OV5640_SCCB_Start(void);
void OV5640_SCCB_Stop(void);
void OV5640_SCCB_No_Ack(void);
UINT8 OV5640_SCCB_Write_Byte(UINT8 data);
UINT8 OV5640_SCCB_Read_Byte(void);
UINT8 OV5640_SCCB_Write_Reg(UINT16 Reg_Adr,UINT8 Reg_Val);
UINT8 OV5640_SCCB_Read_Reg(UINT16 Reg_Adr);
void OV5640_DVP_GPIO_Init(void);
void OV5640_ISP_InputSize_Get(void);
void OV5640_PreScalingSize_Get(void);
void OV5640_OutputSize_Get(void);
void OV5640_Exit_PowerDownMode(void);
void OV5640_HardwareRST(void);
void OV5640_SoftwareRST(void);
UINT16 OV5640_Read_ChipID(void);
void OV5640_Reg_Init(void);
UINT8 OV5640_Init(void);
UINT8 OV5640_AutoFocus_Init(void);
UINT8 OV5640_AutoFocus_Single(void);
UINT8 OV5640_AutoFocus_Continuous(void);
void OV5640_FlashLamp_On(void);
void OV5640_FlashLamp_Off(void);
UINT8 OV5640_LightMode_Config(OV5640_LightMode_t mode);
UINT8 OV5640_ColorSaturation_Config(OV5640_ColorSaturation_t saturation);
UINT8 OV5640_Brightness_Config(OV5640_Brightness_t brightness);
UINT8 OV5640_Contrast_Config(OV5640_Contrast_t contrast);
UINT8 OV5640_Hue_Config(OV5640_Hue_t hue);
UINT8 OV5640_SpecialEffect_Config(OV5640_SpecialEffect_t effect);
UINT8 OV5640_Exposure_Config(OV5640_Exposure_t level);
UINT8 OV5640_Sharpness_Config(OV5640_Sharpness_t sharpness);
UINT8 OV5640_MirrorInversion_Config(OV5640_MirrorInversion_t mirror_flip);
UINT8 OV5640_TestPattern_Config(OV5640_TestPattern_t pattern);
UINT8 OV5640_OutputFormat_Config(OV5640_OutputFormat_t format);
UINT8 OV5640_InputWindowSize_Config(UINT16 x, UINT16 y, UINT16 width, UINT16 height);
UINT8 OV5640_PreScalingWindow_Config(UINT16 x_offset, UINT16 y_offset);
UINT8 OV5640_OutputSize_Config(UINT16 width, UINT16 height);
void OV5640_RGB565_Mode_Init(void);

#endif