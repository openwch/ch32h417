/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/17
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "hardware.h"
#include "lcd_init.h"
#include "lcd.h"
#include "pic.h"
/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   LCD display.
 *
 * @return  none
 */
void Hardware(void)
{
    
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);
    
    LCD_Init();
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);

    while(1)
    {
        LCD_ShowChinese(45,40,"ÇßºãÎ¢µç×Ó",DARKBLUE,WHITE,32,0);
        LCD_ShowString(95,80,"WCH",DARKBLUE,WHITE,32,0);
        LCD_ShowString(65,120,"LCD DEMO",DARKBLUE,WHITE,32,0);
    }
}
