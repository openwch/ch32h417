/********************************** (C) COPYRIGHT *******************************
* File Name          : ov.c
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
#include "ov2640cfg.h"

/*********************************************************************
 * @fn      DVP_GPIO_Init
 *
 * @brief   Init DVP GPIO.
 *
 * @return  none
 */
void DVP_GPIO_Init(void)
{
	RCC_HB2PeriphClockCmd( RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC | RCC_HB2Periph_GPIOD, ENABLE ); 

	GPIO_InitTypeDef GPIO_InitStructure = {0};
    
	/* DPWDN-PB0  RESET-PB1 */
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* SCL-PB10  SDA-PB11 */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF15); 		// VSYNC
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
 
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF15); 		// DPCLK
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF8);  		// HERF
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
 
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF13);   		// D0
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF13);   		// D1
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF13);   		// D2
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF13);   		// D3
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF13);  		// D4
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF13);  		// D8
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF13);  		// D9
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF13); 		// D5
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF13);		// D6
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF13);		// D7
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF13); 		// D10
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF13); 		// D11
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_6 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* test */
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
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
		printf("MID:%d\r\n",reg);
		return 1;
	}

	reg=SCCB_RD_Reg(OV2640_SENSOR_PIDH);	//read ID high
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_PIDL);	//read ID low

	if(reg!=OV2640_PID)
	{
		printf("HID:%d\r\n",reg);
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
    uint16_t i = 0;

    SCCB_WR_Reg(0xff, 0x01);
    SCCB_WR_Reg(0x12, 0x80);
    Delay_Ms(5);
    

    switch( choice )
	{
        case FORMAT_MJPEG:
        	// printf("MJPEG\r\n");

            /*initialization OV2640,uxga resolution(1600*1200) 15fps */
            for(i=0;i<sizeof(ov2640_uxga_init_reg_tbl)/2;i++)
            {
                SCCB_WR_Reg(ov2640_uxga_init_reg_tbl[i][0],ov2640_uxga_init_reg_tbl[i][1]);
            }
            for(i=0;i<(sizeof(ov2640_jpeg_reg_tbl)/2);i++)
            {
                SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0],ov2640_jpeg_reg_tbl[i][1]);
            }
            break;
        case FORMAT_YUV2:
            for(i=0;i<sizeof(yuv_uxga)/2;i++)
            {
                SCCB_WR_Reg(yuv_uxga[i][0],yuv_uxga[i][1]);
            }
            break;
    }
}

/*******************************************************************************
 * @fn      OV2640_Speed_Mode
 *
 * @brief   switch clk speed
 *
 * @return  None
 */
void OV2640_Speed_Mode( uint8_t format )
{
    switch( format )
	{
        case FORMAT_MJPEG:
			ov2640_speed_ctrl(36,0);
            break;
        case FORMAT_YUV2:
            ov2640_speed_ctrl(1,0);
            break;
    }
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
        case FORMAT_MJPEG:
            if(( USB_Enum_Status == U2U3_SUCC ) && USBSS_DevEnumStatus )
            {
                Resolution_width  = ov2640_JPEGframe_resolution_USB30[frameindex-1][0];
                Resolution_height = ov2640_JPEGframe_resolution_USB30[frameindex-1][1];
            }
            else if(( USB_Enum_Status == U2U3_SUCC ) && USBHS_DevEnumStatus )
            {
                Resolution_width  = ov2640_JPEGframe_resolution_USB20[frameindex-1][0];
                Resolution_height = ov2640_JPEGframe_resolution_USB20[frameindex-1][1];
            }
            break;
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
    Dvp_Row_Col_Set(Resolution_width , Resolution_height);
    OV2640_OutSize_Set(Resolution_width,Resolution_height);
    OV2640_Speed_Mode(Formatchange_flag);
}

/* Start Camera list of initialization configuration registers */
const static uint8_t OV2640_AUTOEXPOSURE_LEVEL[5][8]=
{
	{
		0xFF,0x01,
		0x24,0x20,
		0x25,0x18,
		0x26,0x60,
	},
	{
		0xFF,0x01,
		0x24,0x34,
		0x25,0x1c,
		0x26,0x00,
	},
	{
		0xFF,0x01,
		0x24,0x3e,
		0x25,0x38,
		0x26,0x81,
	},
	{
		0xFF,0x01,
		0x24,0x48,
		0x25,0x40,
		0x26,0x81,
	},
	{
		0xFF,0x01,
		0x24,0x58,
		0x25,0x50,
		0x26,0x92,
	},
};

/*******************************************************************************
 * @fn       OV2640_Auto_Exposure
 *
 * @brief    Auto exposure level setting
 *
 * @param    level - 0~4.
 *
 * @return   None
 */
void OV2640_Auto_Exposure(uint8_t level)
{
	uint8_t i;
	uint8_t *p=(uint8_t*)OV2640_AUTOEXPOSURE_LEVEL[level];
	for(i=0;i<4;i++)
	{
		SCCB_WR_Reg(p[i*2],p[i*2+1]);
	}
}

/*******************************************************************************
 * @fn       OV2640_Light_Mode
 *
 * @brief    White balance settings
 *
 * @param    mode -
 *             0 - automatic
 *             1 - sunny
 *             2 - cloudy
 *             3 - office
 *             4 - home
 *
 * @return   None
 */
void OV2640_Light_Mode(uint8_t mode)
{
	uint8_t regccval=0X5E;//Sunny
	uint8_t regcdval=0X41;
	uint8_t regceval=0X54;
	switch(mode)
	{
		case 0://auto
			SCCB_WR_Reg(0XFF,0X00);
			SCCB_WR_Reg(0XC7,0X00);//AWB ON
			return;
		case 2://cloudy
			regccval=0X65;
			regcdval=0X41;
			regceval=0X4F;
			break;
		case 3://office
			regccval=0X52;
			regcdval=0X41;
			regceval=0X66;
			break;
		case 4://home
			regccval=0X42;
			regcdval=0X3F;
			regceval=0X71;
			break;
	}
	SCCB_WR_Reg(0XFF,0X00);
	SCCB_WR_Reg(0XC7,0X40);	//AWB OFF
	SCCB_WR_Reg(0XCC,regccval);
	SCCB_WR_Reg(0XCD,regcdval);
	SCCB_WR_Reg(0XCE,regceval);
}


/*******************************************************************************
 * @fn       OV2640_Color_Saturation
 *
 * @brief    Chroma settings
 *
 * @param    sat-
 *             0 - -2
 *             1 - -1
 *             2 - 0
 *             3 - +1
 *             4 - +2
 *
 * @return   None
 */
void OV2640_Color_Saturation(uint8_t sat)
{
	uint8_t reg7dval=((sat+2)<<4)|0X08;
	SCCB_WR_Reg(0XFF,0X00);
	SCCB_WR_Reg(0X7C,0X00);
	SCCB_WR_Reg(0X7D,0X02);
	SCCB_WR_Reg(0X7C,0X03);
	SCCB_WR_Reg(0X7D,reg7dval);
	SCCB_WR_Reg(0X7D,reg7dval);
}


/*******************************************************************************
 * @fn       OV2640_Brightness
 *
 * @brief    Brightness setting
 *
 * @param    bright -
 *             0 - (0X00)-2
 *             1 - (0X10)-1
 *             2 - (0X20) 0
 *             3 - (0X30)+1
 *             4 - (0X40)+2
 *
 * @return   None
 */
void OV2640_Brightness(uint8_t bright)
{
  SCCB_WR_Reg(0xff, 0x00);
  SCCB_WR_Reg(0x7c, 0x00);
  SCCB_WR_Reg(0x7d, 0x04);
  SCCB_WR_Reg(0x7c, 0x09);
  SCCB_WR_Reg(0x7d, bright<<4);
  SCCB_WR_Reg(0x7d, 0x00);
}


/*******************************************************************************
 * @fn       OV2640_Contrast
 *
 * @brief    Contrast setting
 *
 * @param    contrast - 
 *             0 - -2
 *             1 - -1
 *             2 - 0
 *             3 - +1
 *             4 - +2
 *
 * @return   None
 */
void OV2640_Contrast(uint8_t contrast)
{
	uint8_t reg7d0val=0X20;//The default is normal mode
	uint8_t reg7d1val=0X20;
  	switch(contrast)
	{
		case 0://-2
			reg7d0val=0X18;
			reg7d1val=0X34;
			break;
		case 1://-1
			reg7d0val=0X1C;
			reg7d1val=0X2A;
			break;
		case 3://1
			reg7d0val=0X24;
			reg7d1val=0X16;
			break;
		case 4://2
			reg7d0val=0X28;
			reg7d1val=0X0C;
			break;
	}
	SCCB_WR_Reg(0xff,0x00);
	SCCB_WR_Reg(0x7c,0x00);
	SCCB_WR_Reg(0x7d,0x04);
	SCCB_WR_Reg(0x7c,0x07);
	SCCB_WR_Reg(0x7d,0x20);
	SCCB_WR_Reg(0x7d,reg7d0val);
	SCCB_WR_Reg(0x7d,reg7d1val);
	SCCB_WR_Reg(0x7d,0x06);
}


/*******************************************************************************
 * @fn       OV2640_Special_Effects
 *
 * @brief    Special effect settings
 *
 * @param    eft -
 *             0 - Normal mode
 *             1 - negative film
 *             2 - black and white
 *             3 - Reddish
 *             4 - Greenish
 *             5 - Bluish
 *             6 - Retro
 *
 * @return   None
 */
void OV2640_Special_Effects(uint8_t eft)
{
	uint8_t reg7d0val=0X00;//The default is normal mode
	uint8_t reg7d1val=0X80;
	uint8_t reg7d2val=0X80;
	switch(eft)
	{
		case 1://negative film
			reg7d0val=0X40;
			break;
		case 2://black and white
			reg7d0val=0X18;
			break;
		case 3://Reddish
			reg7d0val=0X18;
			reg7d1val=0X40;
			reg7d2val=0XC0;
			break;
		case 4://Greenish
			reg7d0val=0X18;
			reg7d1val=0X40;
			reg7d2val=0X40;
			break;
		case 5://Bluish
			reg7d0val=0X18;
			reg7d1val=0XA0;
			reg7d2val=0X40;
			break;
		case 6://Retro
			reg7d0val=0X18;
			reg7d1val=0X40;
			reg7d2val=0XA6;
			break;
	}
	SCCB_WR_Reg(0xff,0x00);
	SCCB_WR_Reg(0x7c,0x00);
	SCCB_WR_Reg(0x7d,reg7d0val);
	SCCB_WR_Reg(0x7c,0x05);
	SCCB_WR_Reg(0x7d,reg7d1val);
	SCCB_WR_Reg(0x7d,reg7d2val);
}


/*******************************************************************************
 * @fn       OV2640_Color_Bar
 *
 * @brief    Color bar test
 *
 * @param    sw -
 *            0 - Turn off color bar
 *            1 - Open color bar
 *
 * @return   None
 */
void OV2640_Color_Bar(uint8_t sw)
{
	uint8_t reg;
	SCCB_WR_Reg(0XFF,0X01);
	reg=SCCB_RD_Reg(0X12);
	reg&=~(1<<1);
	if(sw)reg|=1<<1;
	SCCB_WR_Reg(0X12,reg);
}

/*******************************************************************************
 * @fn       OV2640_Window_Set
 *
 * @brief    Set sensor output window
 *
 * @param    sx,sy - Starting address
 *           width,height - horizontal,vertical
 *
 * @return   None
 */
void OV2640_Window_Set(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint16_t endx;
	uint16_t endy;
	uint8_t temp;
	endx=sx+width/2;	//V*2
 	endy=sy+height/2;

	SCCB_WR_Reg(0XFF,0X01);
	temp=SCCB_RD_Reg(0X03);
	temp&=0XF0;
	temp|=((endy&0X03)<<2)|(sy&0X03);
	SCCB_WR_Reg(0X03,temp);
	SCCB_WR_Reg(0X19,sy>>2);
	SCCB_WR_Reg(0X1A,endy>>2);

	temp=SCCB_RD_Reg(0X32);
	temp&=0XC0;
	temp|=((endx&0X07)<<3)|(sx&0X07);
	SCCB_WR_Reg(0X32,temp);
	SCCB_WR_Reg(0X17,sx>>3);
	SCCB_WR_Reg(0X18,endx>>3);
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
 * @fn       OV2640_ImageWin_Set
 *
 * @brief    Set image window size
 *
 * @param    offx,offy - start address
 *           width,height - horizontal,vertical
 *
 * @return   None
 */
uint8_t OV2640_ImageWin_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{
	uint16_t hsize;
	uint16_t vsize;
	uint8_t temp;
	if(width%4)return 1;
	if(height%4)return 2;
	hsize=width/4;
	vsize=height/4;
	SCCB_WR_Reg(0XFF,0X00);
	SCCB_WR_Reg(0XE0,0X04);
	SCCB_WR_Reg(0X51,hsize&0XFF);
	SCCB_WR_Reg(0X52,vsize&0XFF);
	SCCB_WR_Reg(0X53,offx&0XFF);
	SCCB_WR_Reg(0X54,offy&0XFF);
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07;
	SCCB_WR_Reg(0X55,temp);
	SCCB_WR_Reg(0X57,(hsize>>2)&0X80);
	SCCB_WR_Reg(0XE0,0X00);
	return 0;
}

/*******************************************************************************
 * @fn       OV2640_ImageSize_Set
 *
 * @brief    Image Resolution
 *
 * @param    Image_width
 *           Image_height
 *
 * @return   0 - Success
 *           1 - Error
 */
uint8_t OV2640_ImageSize_Set(uint16_t width,uint16_t height)
{
	uint8_t temp;
	SCCB_WR_Reg(0XFF,0X00);
	SCCB_WR_Reg(0XE0,0X04);
	SCCB_WR_Reg(0XC0,(width)>>3&0XFF);
	SCCB_WR_Reg(0XC1,(height)>>3&0XFF);
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80;
	SCCB_WR_Reg(0X8C,temp);
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

