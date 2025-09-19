/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/05/24
* Description        : This file provides all the CRC firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
   FMC routine to operate TFTLCD-:
  LCD--PIN:
    PD11--FMC_A16
    PD12--FMC_A17
    PD5 --FMC_NEW
    PD4 --FMC_NOE
    PA15--LCDRST#
    PD14--FMC_D0
    PD15--FMC_D1
    PD0 --FMC_D2
    PD1--FMC_D3
    PE7--FMC_D4
    PE8 --FMC_D5
    PE9 --FMC_D6
    PE10--FMC_D7
    PE11--FMC_D8
    PE12--FMC_D9
    PE13--FMC_D10
    PE14--FMC_D11
    PE15--FMC_D12
    PD8 --FMC_D13
    PD9--FMC_D14
    PD10--FMC_D15
    PB14--IO_BLCTR
*/


#include "hardware.h"
#include "lcd.h"
/* Global define */


/* Global Variable */



/*********************************************************************
 * @fn      LCD_Reset_GPIO_Init
 *
 * @brief   Init LCD reset GPIO.
 *
 * @return  none
 */
void LCD_Reset_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_SetBits(GPIOD,GPIO_Pin_3);
}


/*********************************************************************
 * @fn      Hardware
 *
 * @brief   LCD display.
 *
 * @return  none
 */
void Hardware(void)
{
	u8 x=0;
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);
	LCD_Reset_GPIO_Init();
	//LCD reset
	GPIO_ResetBits(GPIOD,GPIO_Pin_3);
	Delay_Ms(100);
	GPIO_SetBits(GPIOD,GPIO_Pin_3);

    LCD_Init();
	POINT_COLOR=RED;		
	 
    while(1) 
	{		 
		switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break;

			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		POINT_COLOR=RED;	  
		LCD_ShowString(30,40,210,24,24,"CH32H417");
		LCD_ShowString(30,70,200,16,16,"TFTLCD TEST");
		LCD_ShowString(30,90,200,16,16,"WCH");
	    x++;
		if(x==12)x=0;			   		 
		Delay_Ms(1000);	

	} 
}
