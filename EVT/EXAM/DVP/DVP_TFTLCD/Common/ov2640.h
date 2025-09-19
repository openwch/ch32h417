/********************************** (C) COPYRIGHT *******************************
* File Name          : ov2640.h
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/15
* Description        : This file contains the headers of the OV2640.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __OV2640_H
#define __OV2640_H

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

/* RGB565 PIXEL 320*240 */
#define OV2640_RGB565_HEIGHT   320
#define OV2640_RGB565_WIDTH	   240

/* JPEG PIXEL 1024*768 */
#define OV2640_JPEG_HEIGHT	768
#define OV2640_JPEG_WIDTH	1024

/* OV2640 ID */
#define SCCB_ID   			    0X60
#define OV2640_MID				0X7FA2
#define OV2640_PID				0X2642

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

void OV2640_DVP_GPIO_Init(void);
void OV2640_SCCB_GPIO_Init(void);
void OV2640_SCCB_Start(void);
void OV2640_SCCB_Stop(void);
void OV2640_SCCB_No_Ack(void);
UINT8 OV2640_SCCB_WR_Byte(UINT8 data);
UINT8 OV2640_SCCB_RD_Byte(void);
UINT8 OV2640_SCCB_WR_Reg(UINT8 Reg_Adr,UINT8 Reg_Val);
UINT8 OV2640_SCCB_RD_Reg(UINT8 Reg_Adr);
void OV2640_RGB565_Mode_Init(void);
void OV2640_JPEG_Mode_Init(void);
UINT8 OV2640_Init(void);
void OV2640_JPEG_Mode(void);
void OV2640_RGB565_Mode(void);
UINT8 OV2640_OutSize_Set(UINT16 Image_width,UINT16 Image_height);
void OV2640_Speed_Set(UINT8 Pclk_Div, UINT8 Xclk_Div);

#endif

