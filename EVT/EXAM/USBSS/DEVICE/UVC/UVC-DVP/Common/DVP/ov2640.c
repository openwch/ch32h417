/********************************** (C) COPYRIGHT *******************************
* File Name          : ov2640.c
* Author             : WCH
* Version            : V1.2
* Date               : 2025/07/28
* Description        :
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "uvclib.h"
#include "dvp.h"
#include "ov2640.h"

/* Register tables taken from the CH9346X reference DVP implementation. */
static const uint8_t ov2640_evt_tbl[][2] =
{
    {0xFF,0x00}, {0x2C,0xFF}, {0x2E,0xDF}, {0xFF,0x01}, {0x3C,0x32}, {0x11,0x00},
    {0x09,0x02}, {0x04,0xD8}, {0x13,0xE5}, {0x14,0x48}, {0x2C,0x0C}, {0x33,0x78},
    {0x3A,0x33}, {0x3B,0xFB}, {0x3E,0x00}, {0x43,0x11}, {0x16,0x10}, {0x39,0x92},
    {0x35,0xDA}, {0x22,0x1A}, {0x37,0xC3}, {0x23,0x00}, {0x34,0xC0}, {0x36,0x1A},
    {0x06,0x88}, {0x07,0xC0}, {0x0D,0x87}, {0x0E,0x41}, {0x4C,0x00}, {0x48,0x00},
    {0x5B,0x00}, {0x42,0x03}, {0x4A,0x81}, {0x21,0x99}, {0x24,0x40}, {0x25,0x38},
    {0x26,0x82}, {0x5C,0x00}, {0x63,0x00}, {0x46,0x00}, {0x0C,0x3C}, {0x61,0x70},
    {0x62,0x80}, {0x7C,0x05}, {0x20,0x80}, {0x28,0x30}, {0x6C,0x00}, {0x6D,0x80},
    {0x6E,0x00}, {0x70,0x02}, {0x71,0x94}, {0x73,0xC1}, {0x3D,0x34}, {0x5A,0x57},
    {0x12,0x00}, {0x17,0x11}, {0x18,0x75}, {0x19,0x01}, {0x1A,0x97}, {0x32,0x36},
    {0x03,0x0F}, {0x37,0x40}, {0x4F,0xCA}, {0x50,0xA8}, {0x5A,0x23}, {0x6D,0x00},
    {0x6D,0x38}, {0xFF,0x00}, {0xE5,0x7F}, {0xF9,0xC0}, {0x41,0x24}, {0xE0,0x14},
    {0x76,0xFF}, {0x33,0xA0}, {0x42,0x20}, {0x43,0x18}, {0x4C,0x00}, {0x87,0xD5},
    {0x88,0x3F}, {0xD7,0x03}, {0xD9,0x10}, {0xD3,0x82}, {0xC8,0x08}, {0xC9,0x80},
    {0x7C,0x00}, {0x7D,0x00}, {0x7C,0x03}, {0x7D,0x48}, {0x7D,0x48}, {0x7C,0x08},
    {0x7D,0x20}, {0x7D,0x10}, {0x7D,0x0E}, {0x90,0x00}, {0x91,0x0E}, {0x91,0x1A},
    {0x91,0x31}, {0x91,0x5A}, {0x91,0x69}, {0x91,0x75}, {0x91,0x7E}, {0x91,0x88},
    {0x91,0x8F}, {0x91,0x96}, {0x91,0xA3}, {0x91,0xAF}, {0x91,0xC4}, {0x91,0xD7},
    {0x91,0xE8}, {0x91,0x20}, {0x92,0x00}, {0x93,0x06}, {0x93,0xE3}, {0x93,0x05},
    {0x93,0x05}, {0x93,0x00}, {0x93,0x04}, {0x93,0x00}, {0x93,0x00}, {0x93,0x00},
    {0x93,0x00}, {0x93,0x00}, {0x93,0x00}, {0x93,0x00}, {0x96,0x00}, {0x97,0x08},
    {0x97,0x19}, {0x97,0x02}, {0x97,0x0C}, {0x97,0x24}, {0x97,0x30}, {0x97,0x28},
    {0x97,0x26}, {0x97,0x02}, {0x97,0x98}, {0x97,0x80}, {0x97,0x00}, {0x97,0x00},
    {0xC3,0xEF}, {0xA4,0x00}, {0xA8,0x00}, {0xC5,0x11}, {0xC6,0x51}, {0xBF,0x80},
    {0xC7,0x10}, {0xB6,0x66}, {0xB8,0xA5}, {0xB7,0x64}, {0xB9,0x7C}, {0xB3,0xAF},
    {0xB4,0x97}, {0xB5,0xFF}, {0xB0,0xC5}, {0xB1,0x94}, {0xB2,0x0F}, {0xC4,0x5C},
    {0xC0,0xC8}, {0xC1,0x96}, {0x8C,0x00}, {0x86,0x3D}, {0x50,0x00}, {0x51,0x90},
    {0x52,0x2C}, {0x53,0x00}, {0x54,0x00}, {0x55,0x88}, {0x5A,0x90}, {0x5B,0x2C},
    {0x5C,0x05}, {0xD3,0x02}, {0xC3,0xED}, {0x7F,0x00}, {0xDA,0x09}, {0xE5,0x1F},
    {0xE1,0x67}, {0xE0,0x00}, {0xDD,0x7F}, {0x05,0x00},
};

static const uint8_t ov2640_yuv422_tbl[][2] =
{
    {0xFF,0x00}, {0xE0,0x04}, {0xDA,0x00}, {0xD7,0x01}, {0xE1,0x67}, {0xE0,0x00},
    {0x00,0x00},
};

static const uint8_t ov2640_win_svga[][2] =
{
    {0xFF,0x01}, {0x12,0x40}, {0x03,0x0A}, {0x32,0x09}, {0x17,0x11}, {0x18,0x43},
    {0x19,0x00}, {0x1A,0x4B}, {0x37,0xC0}, {0x4F,0xCA}, {0x50,0xA8}, {0x5A,0x23},
    {0x6D,0x00}, {0x3D,0x38}, {0x39,0x92}, {0x35,0xDA}, {0x22,0x1A}, {0x37,0xC3},
    {0x23,0x00}, {0x34,0xC0}, {0x06,0x88}, {0x07,0xC0}, {0x0D,0x87}, {0x0E,0x41},
    {0x42,0x03}, {0x4C,0x00}, {0xFF,0x00}, {0xE0,0x04}, {0xC0,0x64}, {0xC1,0x4B},
    {0x8C,0x00}, {0x51,0xC8}, {0x52,0x96}, {0x53,0x00}, {0x54,0x00}, {0x55,0x00},
    {0x57,0x00}, {0x50,0x80}, {0xE0,0x00},
};

static const uint8_t ov2640_uxga_imgwin[][2] =
{
    {0xFF,0x00}, {0xE0,0x04}, {0x51,0x90}, {0x52,0x2C}, {0x53,0x00}, {0x54,0x00},
    {0x55,0x88}, {0x57,0x00}, {0xE0,0x00},
};

static uint8_t s_ov2640_svga = 0;

/*******************************************************************************
 * @fn      OV2640_Write_Tbl
 *
 * @brief   Write a register table
 *
 * @return  none
 */
static void OV2640_Write_Tbl(const uint8_t (*tbl)[2], uint16_t n)
{
    uint16_t i;

    for(i = 0; i < n; i++)
    {
        SCCB_WR_Reg(tbl[i][0], tbl[i][1]);
    }
}

/*******************************************************************************
 * @fn      OV2640_YUV422_Relock
 *
 * @brief   Re-lock the YUV422 output format
 *
 * @return  none
 */
static void OV2640_YUV422_Relock(void)
{
    OV2640_Write_Tbl(ov2640_yuv422_tbl, sizeof(ov2640_yuv422_tbl) / 2);
}

/*******************************************************************************
 * @fn      OV2640_Window_Select
 *
 * @brief   Select the SVGA/UXGA readout window
 *
 * @return  none
 */
static void OV2640_Window_Select(uint16_t w, uint16_t h)
{
    uint8_t svga = (uint8_t)((w <= 800) && (h <= 600));

    if(svga == s_ov2640_svga) return;

    if(svga)
    {
        OV2640_Write_Tbl(ov2640_win_svga, sizeof(ov2640_win_svga) / 2);
    }
    else
    {
        OV2640_Write_Tbl(ov2640_evt_tbl, sizeof(ov2640_evt_tbl) / 2);
        OV2640_Write_Tbl(ov2640_uxga_imgwin, sizeof(ov2640_uxga_imgwin) / 2);
    }
    s_ov2640_svga = svga;
}

/*********************************************************************
 * @fn      DVP_GPIO_Init
 *
 * @brief   Init DVP GPIO.
 *
 * @return  none
 */
void DVP_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC |
	                       RCC_HB2Periph_GPIOD | RCC_HB2Periph_AFIO, ENABLE );

	/* PWDN-PB0, RESET-PB1 */
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* VSYNC-PB14 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF15);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* PCLK-PB12, HREF-PB13 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF15);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF8);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* D0-D4: PC6, PC7, PC8, PC9, PC11 */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6,  GPIO_AF13);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7,  GPIO_AF13);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8,  GPIO_AF13);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9,  GPIO_AF13);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF13);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 |
	                               GPIO_Pin_9 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* D5-D7: PD3, PD14, PD15 */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource3,  GPIO_AF13);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF13);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF13);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
/*********************************************************************
 * @fn      OV2640_Detect
 *
 * @brief   Read-only probe (no configuration registers are written).
 *
 * @return  1 - OV2640 detected, 0 - not present
 */
uint8_t OV2640_Detect(void)
{
	uint16_t mid, pid;

	SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01);          // sensor bank
	mid = ((uint16_t)SCCB_RD_Reg(OV2640_SENSOR_MIDH) << 8) | SCCB_RD_Reg(OV2640_SENSOR_MIDL);
	pid = ((uint16_t)SCCB_RD_Reg(OV2640_SENSOR_PIDH) << 8) | SCCB_RD_Reg(OV2640_SENSOR_PIDL);

	return (uint8_t)((mid == OV2640_MID) && (pid == OV2640_PID));
}

/*********************************************************************
 * @fn      OV2640_Init
 *
 * @brief   Init OV2640
 *
 * @return  0 - Success
 *          1 - Err
 */
uint8_t OV2640_Init(void)
{
	uint16_t reg;

	DVP_GPIO_Init();

	OV_PWDN_CLR;                            //POWER ON
	Delay_Ms(10);
 	OV_RESET_CLR;				            //Reset OV2640
 	Delay_Ms(10);
	OV_RESET_SET;				            //End reset

  	SCCB_Init();        		            //initialization SCCB
	SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01);	//Operation sensor register
 	SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80);	//Soft reset OV2640

 	Delay_Ms(50);
	reg=SCCB_RD_Reg(OV2640_SENSOR_MIDH);	//read ID high
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_MIDL);	//read ID low

	if(reg!=OV2640_MID)
	{
		return 1;
	}

  	return 0x00;
}

/*******************************************************************************
 * @fn      OV2640_Format_Mode
 * @brief   Ov2640 Select frame format
 * @param   None
 * @return  None
 */
void OV2640_Format_Mode( uint8_t choice )
{
    if( choice != FORMAT_YUV2 )
    {
        return;
    }

    SCCB_WR_Reg(0xFF, 0x01);
    SCCB_WR_Reg(0x12, 0x80);
    Delay_Ms(50);
    OV2640_Write_Tbl(ov2640_evt_tbl, sizeof(ov2640_evt_tbl) / 2);
    OV2640_OutSize_Set(640, 480);
    SCCB_WR_Reg(0xFF, 0x00);
    SCCB_WR_Reg(0xD3, 0x01);
    SCCB_WR_Reg(0xFF, 0x01);
    SCCB_WR_Reg(0x11, 0x00);
    s_ov2640_svga = 0;
    OV2640_Window_Select(640, 480);
    OV2640_YUV422_Relock();
}

/*******************************************************************************
 * @fn      OV2640_Speed_Mode
 *
 * @brief   switch clk speed
 *
 * @return  None
 */
void OV2640_Speed_Mode( void )
{
    ov2640_speed_ctrl(1, 0);
}

/*******************************************************************************
 * @fn     OV2640_Change_Resolution
 *
 * @brief   Ov2640 Select resolution and frame rate
 *
 * @return  None
 */
void OV2640_Change_Resolution( uint8_t formatindex,uint8_t frameindex )
{
    switch( formatindex )
	{
        case FORMAT_YUV2:
            if(( USB_Enum_Status == U2U3_SUCC ) && USBSS_DevEnumStatus )
            {
                Resolution_width  = ov2640_YUVframe_resolution_USB30[frameindex-1][0];
                Resolution_height = ov2640_YUVframe_resolution_USB30[frameindex-1][1];
            }
            else if(( USB_Enum_Status == U2U3_SUCC ) && USBHS_DevEnumStatus )
            {
                Resolution_width  = ov2640_YUVframe_resolution_USB20[frameindex-1][0];
                Resolution_height = ov2640_YUVframe_resolution_USB20[frameindex-1][1];
            }
            break;
    }
    if( formatindex == FORMAT_YUV2 )
    {
        OV2640_Window_Select(Resolution_width, Resolution_height);
    }
    OV2640_OutSize_Set(Resolution_width,Resolution_height);
    OV2640_Speed_Mode();

    if( formatindex == FORMAT_YUV2 )
    {
        OV2640_YUV422_Relock();
    }
}

/*******************************************************************************
 * @fn       OV2640_OutSize_Set
 *
 * @brief    OV2640 Size (resolution) of output image
 *
 * @param    width,height - horizontal,vertical ,Width and height must be multiples of 4
 *
 * @return   0 - Success
 *           1 - Error
 */
uint8_t OV2640_OutSize_Set(uint16_t width,uint16_t height)
{
	uint16_t outh;
	uint16_t outw;
	uint8_t temp;
	if(width%4)return 1;
	if(height%4)return 2;
	outw=width/4;
	outh=height/4;
	SCCB_WR_Reg(0XFF,0X00);
	SCCB_WR_Reg(0XE0,0X04);
	SCCB_WR_Reg(0X5A,outw&0XFF);
	SCCB_WR_Reg(0X5B,outh&0XFF);
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	SCCB_WR_Reg(0X5C,temp);
	SCCB_WR_Reg(0XE0,0X00);
	return 0;
}

/*******************************************************************************
 * @fn        ov2640_speed_ctrl
 *
 * @brief     Set DVP PCLK
 *
 * @param     Pclk_Div: DVP output speed ctrl
 *            Xclk_Div: Crystal oscillator input frequency division
 *
 * @return    none
 */

void ov2640_speed_ctrl(uint8_t pclkdiv, uint8_t clkdiv)
{
	SCCB_WR_Reg(0XFF,0X00);
	SCCB_WR_Reg(0XD3,pclkdiv);	//Set PCLK frequency division
	SCCB_WR_Reg(0XFF,0X01);
	SCCB_WR_Reg(0X11,clkdiv);	//Set CLK frequency division
}

