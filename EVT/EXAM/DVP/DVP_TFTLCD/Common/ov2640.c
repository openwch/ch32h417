/********************************** (C) COPYRIGHT *******************************
* File Name          : ov2640.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/15
* Description        : This file contains the headers of the OV2640.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ov2640.h"

/* Start Camera list of initialization configuration registers */
const UINT8 OV2640_InitRegTbl[][2]=
{
	0xff, 0x00, 0x2c, 0xff,	0x2e, 0xdf,	0xff, 0x01,	0x3c, 0x32,

	0x11, 0x00,	0x09, 0x02,	0x04, 0xD8,	0x13, 0xe5,	0x14, 0x48,	0x2c, 0x0c,	0x33, 0x78,	0x3a, 0x33,	0x3b, 0xfB,

	0x3e, 0x00,	0x43, 0x11,	0x16, 0x10,

	0x39, 0x92,

	0x35, 0xda,	0x22, 0x1a,	0x37, 0xc3,	0x23, 0x00,	0x34, 0xc0,	0x36, 0x1a,	0x06, 0x88,	0x07, 0xc0,	0x0d, 0x87,	0x0e, 0x41,	0x4c, 0x00,

	0x48, 0x00,	0x5B, 0x00,	0x42, 0x03,

	0x4a, 0x81,	0x21, 0x99,

	0x24, 0x40,	0x25, 0x38,	0x26, 0x82,	0x5c, 0x00,	0x63, 0x00,	0x46, 0x00,	0x0c, 0x3c,

	0x61, 0x70,	0x62, 0x80,	0x7c, 0x05,

	0x20, 0x80,	0x28, 0x30,	0x6c, 0x00,	0x6d, 0x80,	0x6e, 0x00,	0x70, 0x02,	0x71, 0x94,	0x73, 0xc1,	0x3d, 0x34,	0x5a, 0x57,

	0x12, 0x00,

	0x17, 0x11, 0x18, 0x75,	0x19, 0x01,	0x1a, 0x97,	0x32, 0x36,	0x03, 0x0f,	0x37, 0x40,

	0x4f, 0xca,	0x50, 0xa8,	0x5a, 0x23,	0x6d, 0x00,	0x6d, 0x38,

	0xff, 0x00,	0xe5, 0x7f,	0xf9, 0xc0,	0x41, 0x24,	0xe0, 0x14,	0x76, 0xff,	0x33, 0xa0,	0x42, 0x20,	0x43, 0x18,	0x4c, 0x00,	0x87, 0xd5,	0x88, 0x3f,	0xd7, 0x03,	0xd9, 0x10,	0xd3, 0x82,

	0xc8, 0x08,	0xc9, 0x80,

	0x7c, 0x00,	0x7d, 0x00,	0x7c, 0x03,	0x7d, 0x48,	0x7d, 0x48,	0x7c, 0x08,	0x7d, 0x20,	0x7d, 0x10,	0x7d, 0x0e,

	0x90, 0x00,	0x91, 0x0e,	0x91, 0x1a,	0x91, 0x31,	0x91, 0x5a,	0x91, 0x69,	0x91, 0x75,	0x91, 0x7e,	0x91, 0x88,	0x91, 0x8f,	0x91, 0x96,	0x91, 0xa3,	0x91, 0xaf,	0x91, 0xc4,	0x91, 0xd7,	0x91, 0xe8,	0x91, 0x20,

	0x92, 0x00,	0x93, 0x06,	0x93, 0xe3,	0x93, 0x05,	0x93, 0x05,	0x93, 0x00,	0x93, 0x04,	0x93, 0x00,	0x93, 0x00,	0x93, 0x00,	0x93, 0x00,	0x93, 0x00,	0x93, 0x00,	0x93, 0x00,

	0x96, 0x00,	0x97, 0x08,	0x97, 0x19,	0x97, 0x02,	0x97, 0x0c,	0x97, 0x24,	0x97, 0x30,	0x97, 0x28,	0x97, 0x26,	0x97, 0x02,	0x97, 0x98,	0x97, 0x80,	0x97, 0x00,	0x97, 0x00,

	0xc3, 0xef,

	0xa4, 0x00,	0xa8, 0x00,	0xc5, 0x11,	0xc6, 0x51,	0xbf, 0x80,	0xc7, 0x10,	0xb6, 0x66,	0xb8, 0xA5,	0xb7, 0x64,	0xb9, 0x7C,	0xb3, 0xaf,	0xb4, 0x97,	0xb5, 0xFF,	0xb0, 0xC5,	0xb1, 0x94,	0xb2, 0x0f,	0xc4, 0x5c,

	0xc0, 0xc8,	0xc1, 0x96,	0x8c, 0x00,	0x86, 0x3d,	0x50, 0x00,	0x51, 0x90,	0x52, 0x2c,	0x53, 0x00,	0x54, 0x00,	0x55, 0x88,

	0x5a, 0x90,	0x5b, 0x2C,	0x5c, 0x05,

	0xd3, 0x02,

	0xc3, 0xed,	0x7f, 0x00,

	0xda, 0x09,

	0xe5, 0x1f,	0xe1, 0x67,	0xe0, 0x00,	0xdd, 0x7f,	0x05, 0x00,
};

/* YUV422 */
const UINT8 OV2640_YUV422RegTbl[][2]=
{
	0xFF, 0x00,	0xDA, 0x10,	0xD7, 0x03,	0xDF, 0x00,	0x33, 0x80,	0x3C, 0x40,	0xe1, 0x77,	0x00, 0x00,
};

/* JPEG */
const UINT8 OV2640_JPEGRegTbl[][2]=
{
	0xff, 0x01,	0xe0, 0x14,	0xe1, 0x77,	0xe5, 0x1f,	0xd7, 0x03,	0xda, 0x10,	0xe0, 0x00,
};

/* RGB565 */
const UINT8 OV2640_RGB565RegTbl[][2]=
{
	0xFF, 0x00,	0xDA, 0x09,	0xD7, 0x03,	0xDF, 0x02,	0x33, 0xa0,	0x3C, 0x00,	0xe1, 0x67,
	0xff, 0x01,	0xe0, 0x00,	0xe1, 0x00,	0xe5, 0x00,	0xd7, 0x00,	0xda, 0x00,	0xe0, 0x00,
};

/*********************************************************************
 * @fn      OV2640_SCCB_GPIO_Init
 *
 * @brief   Init SCCB GPIO.
 *
 * @return  none
 */
void OV2640_SCCB_GPIO_Init(void)
{
	IIC_SCL_OUT;
	IIC_SDA_OUT;
	IIC_SCL_SET;
	IIC_SDA_SET;
}

/*********************************************************************
 * @fn      OV2640_SCCB_Start
 *
 * @brief   Start Signal
 *
 * @return  none
 */
void OV2640_SCCB_Start(void)
{
	IIC_SDA_SET;
	IIC_SCL_SET;
	Delay_Us(50);
	IIC_SDA_CLR;
	Delay_Us(50);
	IIC_SCL_CLR;
}

/*********************************************************************
 * @fn      OV2640_SCCB_Stop
 *
 * @brief   Stop Signal
 *
 * @return  none
 */
void OV2640_SCCB_Stop(void)
{
	IIC_SDA_CLR;
	Delay_Us(50);
	IIC_SCL_SET;
	Delay_Us(50);
	IIC_SDA_SET;
	Delay_Us(50);
}

/*********************************************************************
 * @fn      OV2640_SCCB_No_Ack
 *
 * @brief   NAK Signal
 *
 * @return  none
 */
void OV2640_SCCB_No_Ack(void)
{
	Delay_Us(50);
	IIC_SDA_SET;
	IIC_SCL_SET;
	Delay_Us(50);
	IIC_SCL_CLR;
	Delay_Us(50);
	IIC_SDA_CLR;
	Delay_Us(50);
}

/*********************************************************************
 * @fn      OV2640_SCCB_WR_Byte
 *
 * @brief   Write One Byte
 *
 * @param   data
 *
 * @return  0 - Success
 *          other - Err
 */
UINT8 OV2640_SCCB_WR_Byte(UINT8 data)
{
	UINT8 i,t;

	for(i=0; i<8; i++){
		if(data & 0x80){
			IIC_SDA_SET;
		}
		else{
		    IIC_SDA_CLR;
		}

		data <<= 1;
		Delay_Us(50);
		IIC_SCL_SET;
		Delay_Us(50);
		IIC_SCL_CLR;
	}

	IIC_SDA_IN;
	Delay_Us(50);
	IIC_SCL_SET;
	Delay_Us(50);

	if(SDA_IN_R)t=1;
	else t=0;

	IIC_SCL_CLR;
	IIC_SDA_OUT;

	return t;
}

/*********************************************************************
 * @fn      OV2640_SCCB_RD_Byte
 *
 * @brief   Read One Byte
 *
 * @return  Read one byte data
 */
UINT8 OV2640_SCCB_RD_Byte(void)
{
	UINT8 t=0,i;

	IIC_SDA_IN;

	for(i=8; i>0; i--){
		Delay_Us(50);
		IIC_SCL_SET;
		t=t<<1;

		if(SDA_IN_R)t++;

		Delay_Us(50);
		IIC_SCL_CLR;
	}

	IIC_SDA_OUT;

	return t;
}

/*********************************************************************
 * @fn      OV2640_SCCB_WR_Reg
 *
 * @brief   Write camera Register
 *
 * @param   Reg_Adr - Register address
 *          Reg_Val - Register value
 *
 * @return  0 - Success
 *          other - Err
 */
UINT8 OV2640_SCCB_WR_Reg(UINT8 Reg_Adr,UINT8 Reg_Val)
{
	UINT8 res=0;

	OV2640_SCCB_Start();
	if(OV2640_SCCB_WR_Byte(SCCB_ID))res=1;
	Delay_Us(100);
  	if(OV2640_SCCB_WR_Byte(Reg_Adr))res=1;
  	Delay_Us(100);
  	if(OV2640_SCCB_WR_Byte(Reg_Val))res=1;
  	OV2640_SCCB_Stop();
  	return	res;
}

/*********************************************************************
 * @fn      OV2640_SCCB_RD_Reg
 *
 * @brief   Read camera Register
 *
 * @return  Camera Register value
 */
UINT8 OV2640_SCCB_RD_Reg(UINT8 Reg_Adr)
{
	UINT8 val=0;

	OV2640_SCCB_Start();
	OV2640_SCCB_WR_Byte(SCCB_ID);
	Delay_Us(100);
  	OV2640_SCCB_WR_Byte(Reg_Adr);
  	Delay_Us(100);
	OV2640_SCCB_Stop();
	Delay_Us(100);

	OV2640_SCCB_Start();
	OV2640_SCCB_WR_Byte(SCCB_ID|0X01);
	Delay_Us(100);
  	val=OV2640_SCCB_RD_Byte();
  	OV2640_SCCB_No_Ack();
  	OV2640_SCCB_Stop();

  	return val;
}

/*********************************************************************
 * @fn      OV2640_DVP_GPIO_Init
 *
 * @brief   Init DVP GPIO.
 *
 * @return  none
 */
void OV2640_DVP_GPIO_Init(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOE | RCC_HB2Periph_GPIOB | 
                          RCC_HB2Periph_GPIOC | RCC_HB2Periph_GPIOD | RCC_HB2Periph_GPIOF, ENABLE);

	/* DPWDN-PB0  RESET-PD2 */
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
	/* SCL-PC0  SDA-PC1 */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF13);   /* VSYNC */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOF, GPIO_PinSource13, GPIO_AF11);  /* DPCLK */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF8);   /* HERF */ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF11);   /* D0 */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF11);   /* D1 */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource2, GPIO_AF13);   /* D2 */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource3, GPIO_AF13);   /* D3 */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF13);   /* D4 */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF13);   /* D6 */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF13);   /* D7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOF, GPIO_PinSource14, GPIO_AF11);  /* D5 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF13);  /* D8 */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF13);  /* D9 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF13);  /* D10 */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF13);  /* D11 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      OV2640_Init
 *
 * @brief   Init OV2640
 *
 * @return  0 - Success
 *          1 - Err
 */
UINT8 OV2640_Init(void)
{
	UINT16 i=0;
	UINT16 reg;

	OV2640_DVP_GPIO_Init();

	OV_PWDN_CLR;                /* POWER ON */
 	Delay_Ms(10);
 	OV_RESET_CLR;				/* Reset OV2640 */
	Delay_Ms(10);
	OV_RESET_SET;				/* Reset End */

	OV2640_SCCB_GPIO_Init();
	OV2640_SCCB_WR_Reg(0xFF, 0x01);	/* Register Bank Select Sensor address */

 	OV2640_SCCB_WR_Reg(0x12, 0x80);	/* Reset All Register */

 	Delay_Ms(50);

	/* Read MID */
	reg=OV2640_SCCB_RD_Reg(0x1C);
	reg<<=8;
	reg|=OV2640_SCCB_RD_Reg(0x1D);

	if(reg!=OV2640_MID)
	{
		printf("MID:%08x\r\n",reg);
		return 1;
	}

	/* Read PID */
	reg=OV2640_SCCB_RD_Reg(0x0A);
	reg<<=8;
	reg|=OV2640_SCCB_RD_Reg(0x0B);

	if(reg!=OV2640_PID)
	{
		printf("PID:%08x\r\n",reg);
		return 2;
	}

	for(i=0;i<sizeof(OV2640_InitRegTbl)/2;i++)
	{
	   	OV2640_SCCB_WR_Reg(OV2640_InitRegTbl[i][0],OV2640_InitRegTbl[i][1]);
 	}

  	return 0;
}

/*********************************************************************
 * @fn      OV2640_RGB565_Mode_Init
 *
 * @brief   Init RGB565 mode
 *
 * @return  none
 */
void OV2640_RGB565_Mode_Init(void)
{
	OV2640_RGB565_Mode();
	OV2640_OutSize_Set(OV2640_RGB565_WIDTH,OV2640_RGB565_HEIGHT);
	OV2640_Speed_Set(1,1);
}
/*********************************************************************
 * @fn      OV2640_JPEG_Mode_Init
 *
 * @brief   Init JPEG mode
 *
 * @return  none
 */
void OV2640_JPEG_Mode_Init(void)
{
	OV2640_JPEG_Mode();
  	OV2640_OutSize_Set(OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT);
  	OV2640_Speed_Set(1,1);
}
/*********************************************************************
 * @fn      OV2640_JPEG_Mode
 *
 * @brief   JPEG Mode
 *
 * @return  none
 */
void OV2640_JPEG_Mode(void)
{
	UINT16 i=0;

	/* YUV422 */
	for(i=0;i<(sizeof(OV2640_YUV422RegTbl)/2);i++)
	{
		OV2640_SCCB_WR_Reg(OV2640_YUV422RegTbl[i][0],OV2640_YUV422RegTbl[i][1]);
	}
	/* JPEG */
	for(i=0;i<(sizeof(OV2640_JPEGRegTbl)/2);i++)
	{
		OV2640_SCCB_WR_Reg(OV2640_JPEGRegTbl[i][0],OV2640_JPEGRegTbl[i][1]);
	}
}

/*********************************************************************
 * @fn      OV2640_RGB565_Mode
 *
 * @brief   RGB565 Mode
 *
 * @return  none
 */
void OV2640_RGB565_Mode(void)
{
	UINT16 i=0;

	/* RGB565 */
	for(i=0;i<(sizeof(OV2640_RGB565RegTbl)/2);i++)
	{
		OV2640_SCCB_WR_Reg(OV2640_RGB565RegTbl[i][0],OV2640_RGB565RegTbl[i][1]);
	}
}

/*********************************************************************
 * @fn      OV2640_OutSize_Set
 *
 * @brief   Set Image Resolution
 *
 * @param   width - The width of the actual output image
 *          height - The height of the actual output image
 *
 * @return  none
 */
UINT8 OV2640_OutSize_Set(UINT16 Image_width,UINT16 Image_height)
{
	UINT16 Out_Size_Width;
	UINT16 Out_Size_Height;
	UINT8 t;

	if((Image_width % 4) || (Image_height % 4)) return 0x01;

	Out_Size_Width = Image_width/4;
	Out_Size_Height = Image_height/4;

	OV2640_SCCB_WR_Reg(0XFF, 0X00);
	OV2640_SCCB_WR_Reg(0XE0, 0X04);
	OV2640_SCCB_WR_Reg(0X5A, Out_Size_Width & 0XFF);
	OV2640_SCCB_WR_Reg(0X5B, Out_Size_Height & 0XFF);
	t = (Out_Size_Width >> 8) & 0X03;
	t |= (Out_Size_Height >> 6) & 0X04;
	OV2640_SCCB_WR_Reg(0X5C, t);
	OV2640_SCCB_WR_Reg(0XE0, 0X00);

	return 0;
}

/*********************************************************************
 * @fn      OV2640_Speed_Set
 *
 * @brief   Set DVP PCLK
 *
 * @param   Pclk_Div - DVP output speed ctrl
 *          Xclk_Div - Crystal oscillator input frequency division
 *
 * @return  0 - Success
 *          other - Err
 */
void OV2640_Speed_Set(UINT8 Pclk_Div, UINT8 Xclk_Div)
{
	OV2640_SCCB_WR_Reg(0XFF,0X00);
	OV2640_SCCB_WR_Reg(0XD3,Pclk_Div);

	OV2640_SCCB_WR_Reg(0XFF,0X01);
	OV2640_SCCB_WR_Reg(0X11,Xclk_Div);
}



