/********************************** (C) COPYRIGHT *******************************
* File Name          : lcd.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/15
* Description        : This file contains the headers of the lcd.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "debug.h"
#include "lcd.h"
#include "font.h"
#define  BCR1     (*((volatile unsigned long  *)(0x40025400)))
#define  BTR1     (*((volatile unsigned long  *)(0x40025404)))
#define  BWTR1     (*((volatile unsigned long  *)(0x40025504)))
_lcd_dev lcddev;
/* LCD brush and background colors */
u16 POINT_COLOR=0x0000;
u16 BACK_COLOR=0xFFFF;

/*********************************************************************
 * @fn      FSMC_Init
 *
 * @brief   Init FMC
 *
 * @return  none
 */

void FMC_Init(void)
{
	FMC_NORSRAMInitTypeDef  FMC_NORSRAMInitStructure={0};
   	FMC_NORSRAMTimingInitTypeDef  readWriteTiming={0};
	FMC_NORSRAMTimingInitTypeDef  writeTiming={0};

    readWriteTiming.FMC_AddressSetupTime = 0x0F;
    readWriteTiming.FMC_AddressHoldTime = 0x03;
    readWriteTiming.FMC_DataSetupTime = 0x0f;
    readWriteTiming.FMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FMC_AccessMode = FMC_AccessMode_A;

    writeTiming.FMC_AddressSetupTime = 0x00;
    writeTiming.FMC_AddressHoldTime = 0x00;
    writeTiming.FMC_DataSetupTime = 0x03;
    writeTiming.FMC_BusTurnAroundDuration = 0x00;
    writeTiming.FMC_CLKDivision = 0x00;
    writeTiming.FMC_DataLatency = 0x00;
    writeTiming.FMC_AccessMode = FMC_AccessMode_A;

    FMC_NORSRAMInitStructure.FMC_Bank = FMC_Bank1_NORSRAM1;
    
    FMC_NORSRAMInitStructure.FMC_DataAddressMux = FMC_DataAddressMux_Disable;
    FMC_NORSRAMInitStructure.FMC_MemoryType =FMC_MemoryType_SRAM;
    FMC_NORSRAMInitStructure.FMC_MemoryDataWidth = FMC_MemoryDataWidth_16b;
    FMC_NORSRAMInitStructure.FMC_BurstAccessMode =FMC_BurstAccessMode_Disable;
    FMC_NORSRAMInitStructure.FMC_WaitSignalPolarity = FMC_WaitSignalPolarity_Low;
    FMC_NORSRAMInitStructure.FMC_AsynchronousWait=FMC_AsynchronousWait_Disable;
    FMC_NORSRAMInitStructure.FMC_WaitSignalActive = FMC_WaitSignalActive_BeforeWaitState;
    FMC_NORSRAMInitStructure.FMC_WriteOperation = FMC_WriteOperation_Enable;
    FMC_NORSRAMInitStructure.FMC_WaitSignal = FMC_WaitSignal_Disable;
    FMC_NORSRAMInitStructure.FMC_ExtendedMode = FMC_ExtendedMode_Enable;
    FMC_NORSRAMInitStructure.FMC_WriteBurst = FMC_WriteBurst_Disable;
    FMC_NORSRAMInitStructure.FMC_ReadWriteTimingStruct = &readWriteTiming;
    FMC_NORSRAMInitStructure.FMC_WriteTimingStruct = &writeTiming;

    FMC_NORSRAMInit(&FMC_NORSRAMInitStructure);

	FMC_NORSRAMCmd(FMC_Bank1_NORSRAM1, ENABLE);

}
/*********************************************************************
 * @fn      LCD_GPIO_Init
 *
 * @brief   Init LCD GPIO
 *
 * @return  none
 */
void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_FMC,ENABLE);
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB|RCC_HB2Periph_GPIOD|RCC_HB2Periph_GPIOE|RCC_HB2Periph_AFIO,ENABLE);

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
 	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF12 );


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
 	GPIO_Init(GPIOE, &GPIO_InitStructure); 
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF12 );
	
    /*   RS:PD12  */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF12 );

    /* CS: PD11*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD,GPIO_Pin_11);
}


/*********************************************************************
 * @fn      LCD_WR_REG
 *
 * @brief   Write register
 *
 * @param   regval - register value
 *
 * @return  none
 */
void LCD_WR_REG(vu16 regval)
{
        regval = regval;		
    LCD->LCD_REG = regval;
}

/*********************************************************************
 * @fn      LCD_WR_DATA
 *
 * @brief   Write data
 *
 * @param   data
 *
 * @return  none
 */
void LCD_WR_DATA(vu16 data)
{
    LCD->LCD_RAM = data;
}

/*********************************************************************
 * @fn      LCD_RD_DATA
 *
 * @brief   Read data
 *
 * @return  ram - read data
 */
u16 LCD_RD_DATA(void)
{
    vu16 ram;                  
    ram=LCD->LCD_RAM;
    return ram;	 
}

/*********************************************************************
 * @fn      LCD_WriteReg
 *
 * @brief   Write value to register
 *
 * @param   LCD_Reg - register addr
 *          LCD_RegValue - value
 *
 * @return  none
 */
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;
    LCD->LCD_RAM = LCD_RegValue;
}
/*********************************************************************
 * @fn      LCD_ReadReg
 *
 * @brief   Read value from register
 *
 * @param   LCD_Reg - register addr
 *
 * @return  register value
 */
u16 LCD_ReadReg(u16 LCD_Reg)
{ 
    LCD_WR_REG(LCD_Reg);
    Delay_Us(5);
	return LCD_RD_DATA();
}
/*********************************************************************
 * @fn      LCD_WriteRAM
 *
 * @brief   Write GRAM
 *
 * @param   RGB_Code - colour value
 *
 * @return  none
 */
void LCD_WriteRAM(u16 RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;
}

/*********************************************************************
 * @fn      LCD_WriteRAM_Prepare
 *
 * @brief   Write GRAM prepare
 *
 * @return  none
 */
void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}

/*********************************************************************
 * @fn      LCD_BGR2RGB
 *
 * @brief   The data read from ILI93xx is in GBR format, and we write it in RGB format.
 *
 * @param   c-Color value in GBR format
 *
 * @return  rgb-RGB format color value
 */

u16 LCD_BGR2RGB(u16 c)
{
    u16  r, g, b, rgb;
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return(rgb);
}

/*********************************************************************
 * @fn      LCD_ReadPoint
 *
 * @brief   Read the color value of a certain point.
 *
 * @param   x-x-axis coordinate
 *          y-y-axis coordinate
 *
 * @return  The color of this point
 */

u16 LCD_ReadPoint(u16 x,u16 y)
{
    u16 r = 0, g = 0, b = 0;
    if(x >= lcddev.width || y >= lcddev.height)
        return 0;   //Out of range, return directly
    LCD_SetCursor(x, y);
    if (lcddev.id == 0X5510)    //5510 Send readGRAM command
    {
        LCD_WR_REG(0X2E00);
    }
    else                        //Other ICs(9341/5310/1963/7789)Send readGRAM command
    {
        LCD_WR_REG(0X2E);
    }

    r = LCD_RD_DATA();          

    if (lcddev.id == 0X1963)    
    {
        return r;              
    }

    r = LCD_RD_DATA();          //Actual coordinate color

    //9341/5310/5510/7789   To be read out in 2 parts;
    b = LCD_RD_DATA();
    g = r & 0XFF;               // 9341/5310/5510/7789, The first read is the value of RG, R first, G last, each occupying 8 bits.
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));  // 9341/5310/5510/7789  Need to convert the formula.
}

/*********************************************************************
 * @fn      LCD_DisplayOn
 *
 * @brief   LCD on display.
 *
 * @param   None
 *         
 * @return  None
 */

void LCD_DisplayOn(void)
{
    if (lcddev.id == 0X5510)    //5510 Enable display command;
    {
        LCD_WR_REG(0X2900);     //Turn on display
    }
    else                        //9341/5310/1963/7789  Wait for the send start display command;
    {
        LCD_WR_REG(0X29);       //Turn on display
    }
}

/*********************************************************************
 * @fn      LCD_DisplayOff
 *
 * @brief   LCD off display.
 *
 * @param   None
 *         
 * @return  None
 */

void LCD_DisplayOff(void)
{

    if (lcddev.id == 0X5510)    //Disable display command
    {
        LCD_WR_REG(0X2800);     //Turn off display
    }
    else                        
    {
        LCD_WR_REG(0X28);       
    }
}
/*********************************************************************
 * @fn      LCD_SetCursor
 *
 * @brief   Set the cursor position.
 *
 * @param   Xpos-x-axis coordinate
 *          Ypos-y-axis coordinate
 *         
 * @return  None
 */

void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    if (lcddev.id == 0X1963)
    {
        if (lcddev.dir == 0)   
        {
            Xpos = lcddev.width - 1 - Xpos;
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(0);
            LCD_WR_DATA(0);
            LCD_WR_DATA(Xpos >> 8);
            LCD_WR_DATA(Xpos & 0XFF);
        }
        else
        {
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(Xpos >> 8);
            LCD_WR_DATA(Xpos & 0XFF);
            LCD_WR_DATA((lcddev.width - 1) >> 8);
            LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        }

        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);

    }
    else if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(Ypos & 0XFF);
    }
    else     
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
    }
}
/*********************************************************************
 * @fn      LCD_Scan_Dir
 *
 * @brief   Set the automatic scanning direction of the LCD.
 *
 * @param   None
 *         
 * @return  None
 */

void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;
    u16 temp;
    //In landscape mode, keep the scanning direction unchanged for 1963, change the scanning direction for other ICs! In portrait mode, change the direction for1963, keep the scanning direction unchanged for other ICs!
    if ((lcddev.dir == 1 && lcddev.id != 0X1963) || (lcddev.dir == 0 && lcddev.id == 0X1963))
    {
        switch (dir)  
        {
            case 0:
                dir = 6;
                break;

            case 1:
                dir = 7;
                break;

            case 2:
                dir = 4;
                break;

            case 3:
                dir = 5;
                break;

            case 4:
                dir = 1;
                break;

            case 5:
                dir = 0;
                break;

            case 6:
                dir = 3;
                break;

            case 7:
                dir = 2;
                break;
        }
    }

    switch (dir)
    {
        case L2R_U2D://From left to right, from top to bottom
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U://From left to right, from bottom to top
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D://From right to left, from top to bottom
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U://From right to left, from bottom to top
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R://Top to bottom, left to right
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L://Top to bottom, right to left
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R://From bottom to top, from left to right
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L://From bottom to top, from right to left
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    if (lcddev.id == 0X5510)dirreg = 0X3600;
    else dirreg = 0X36;

    if (lcddev.id == 0X9341 || lcddev.id == 0X7789)   
    {
        regval |= 0X08;
    }

    LCD_WriteReg(dirreg, regval);

    if (lcddev.id != 0X1963)   
    {
        if (regval & 0X20)
        {
            if (lcddev.width < lcddev.height)   
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)   
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
    }


    if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setxcmd + 2);
        LCD_WR_DATA((lcddev.width - 1) >> 8);
        LCD_WR_REG(lcddev.setxcmd + 3);
        LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(0);
        LCD_WR_REG(lcddev.setycmd + 2);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_REG(lcddev.setycmd + 3);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    }
    else
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(0);
        LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.width - 1) >> 8);
        LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(0);
        LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    }
}

/*********************************************************************
 * @fn      LCD_DrawPoint
 *
 * @brief   Draw a point at the designated location
 *
 * @param   x,y    - Draw the coordinates of the points
 *          color  - Points Color
 *
 * @return  none
 */
void LCD_DrawPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y);        
    LCD_WriteRAM_Prepare(); 
    LCD->LCD_RAM = POINT_COLOR;
}


/*********************************************************************
 * @fn      LCD_Fast_DrawPoint
 *
 * @brief   Quick dot drawing
 *
 * @param   x,y    - Draw the coordinates of the points
 *          color  - Points Color
 *
 * @return  none
 */

void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color)
{
    if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(y & 0XFF);
    }
    else if (lcddev.id == 0X1963)
    {
        if (lcddev.dir == 0)x = lcddev.width - 1 - x;

        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }
    else    
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }

    LCD->LCD_REG=lcddev.wramcmd;
    LCD->LCD_RAM=color;
}

/*********************************************************************
 * @fn      LCD_SSD_BackLightSet
 *
 * @brief   SSD1963 backlight setting.
 *
 * @param   pwm - Rating of backlight:0~100. The brighter the better
 *
 * @return  none
 */

void LCD_SSD_BackLightSet(u8 pwm)
{
    LCD_WR_REG(0xBE);   //Configure PWM output
    LCD_WR_DATA(0x05);  //Set PWM frequency
    LCD_WR_DATA(pwm * 2.55);//Set PWM duty cycle
    LCD_WR_DATA(0x01);  
    LCD_WR_DATA(0xFF);  
    LCD_WR_DATA(0x00);  
    LCD_WR_DATA(0x00);  
}
/*********************************************************************
 * @fn      LCD_Display_Dir
 *
 * @brief   Set LCD display orientation
 *
 * @param   dir - 0:Portrait; 1:landscape
 *
 * @return  none
 */

void LCD_Display_Dir(u8 dir)
{
    lcddev.dir = dir;      

    if (dir == 0)        
    {
        lcddev.width = 240;
        lcddev.height = 320;

        if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0X2C00;
            lcddev.setxcmd = 0X2A00;
            lcddev.setycmd = 0X2B00;
            lcddev.width = 480;
            lcddev.height = 800;
        }
        else if (lcddev.id == 0X1963)
        {
            lcddev.wramcmd = 0X2C;  
            lcddev.setxcmd = 0X2B;  
            lcddev.setycmd = 0X2A;  
            lcddev.width = 480;     
            lcddev.height = 800;    
        }
        else                      
        {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2A;
            lcddev.setycmd = 0X2B;
        }

        if (lcddev.id == 0X5310)   
        {
            lcddev.width = 320;
            lcddev.height = 480;
        }
    }
    else    
    {
        lcddev.width = 320;
        lcddev.height = 240;

        if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0X2C00;
            lcddev.setxcmd = 0X2A00;
            lcddev.setycmd = 0X2B00;
            lcddev.width = 800;
            lcddev.height = 480;
        }
        else if (lcddev.id == 0X1963)
        {
            lcddev.wramcmd = 0X2C;  
            lcddev.setxcmd = 0X2A;  
            lcddev.setycmd = 0X2B;  
            lcddev.width = 800;     
            lcddev.height = 480;   
        }
        else                        
        {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2A;
            lcddev.setycmd = 0X2B;
        }

        if (lcddev.id == 0X5310)    
        {
            lcddev.width = 480;
            lcddev.height = 320;
        }
    }

    LCD_Scan_Dir(DFT_SCAN_DIR);     
}

/*********************************************************************
 * @fn      LCD_Set_Window
 *
 * @brief   Set the window, and automatically set the coordinates of the dot to the upper left corner of the window (sx, sy).
 *
 * @param   
 *          sx - Window start coordinates X
 *          sy - Window start coordinates Y
 *          width  - Window width
 *          height - Window height
 * @return  none
 */

void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{
    // u8 hsareg, heareg, vsareg, veareg;
    // u16 hsaval, heaval, vsaval, veaval;
    u16 twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;
    if (lcddev.id == 0X1963 && lcddev.dir != 1)     //1963
    {
        sx = lcddev.width - width - sx;
        height = sy + height - 1;
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_DATA((sx + width - 1) >> 8);
        LCD_WR_DATA((sx + width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_DATA(height >> 8);
        LCD_WR_DATA(height & 0XFF);
    }
    else if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_REG(lcddev.setxcmd + 2);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_REG(lcddev.setxcmd + 3);
        LCD_WR_DATA(twidth & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_REG(lcddev.setycmd + 2);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_REG(lcddev.setycmd + 3);
        LCD_WR_DATA(theight & 0XFF);
    }
    else     //9341/5310/7789/1963
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_DATA(twidth & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_DATA(theight & 0XFF);
    }
}
/*********************************************************************
 * @fn      LCD_Init
 *
 * @brief   Init LCD
 *
 * @return  none
 */
void LCD_Init(void)
{
    LCD_GPIO_Init();
    FMC_Init(); 
    LCD_WR_REG(0XD3);
    __disable_irq();
    lcddev.id= *(vu16*)0x60040000;  //dummy read
    lcddev.id = *(vu16*)0x60040000;  
    lcddev.id = *(vu16*)0x60040000;     
    lcddev.id <<= 8;
    lcddev.id |= *(vu16*)0x60040000;  
    __enable_irq();
    if(lcddev.id != 0X9341)     
    {
        LCD_WR_REG(0X04);
        __disable_irq();
        lcddev.id = *(vu16*)0x60040000;      
        lcddev.id = *(vu16*)0x60040000;
        lcddev.id = *(vu16*)0x60040000;
        lcddev.id <<= 8;
        lcddev.id |= *(vu16*)0x60040000;  
        __enable_irq();
        if (lcddev.id == 0X8552)        
        {
            lcddev.id = 0x7789;
        }
        if (lcddev.id != 0x7789)       
        {
            LCD_WR_REG(0XD4);
            __disable_irq();
            lcddev.id = *(vu16*)0x60040000;      
            lcddev.id = *(vu16*)0x60040000;
            lcddev.id = *(vu16*)0x60040000;
            lcddev.id <<= 8;
            lcddev.id |= *(vu16*)0x60040000;  
            __enable_irq();
            if (lcddev.id != 0X5310)    
            {
                LCD_WriteReg(0xF000, 0x0055);
                LCD_WriteReg(0xF001, 0x00AA);
                LCD_WriteReg(0xF002, 0x0052);
                LCD_WriteReg(0xF003, 0x0008);
                LCD_WriteReg(0xF004, 0x0001);
                LCD_WR_REG(0xC500);            
                 __disable_irq();
                lcddev.id = *(vu16*)0x60040000;   
                lcddev.id <<= 8;
                __enable_irq();
                LCD_WR_REG(0xC501);             
                 __disable_irq();
                lcddev.id = *(vu16*)0x60040000;   
                __enable_irq();
                Delay_Ms(5);
                if (lcddev.id != 0X5510)        
                {
                    LCD_WR_REG(0XA1);
                    __disable_irq();
                    lcddev.id = *(vu16*)0x60040000;      
                    lcddev.id = *(vu16*)0x60040000;
                     lcddev.id = *(vu16*)0x60040000;
                     lcddev.id <<= 8;
                     lcddev.id |= *(vu16*)0x60040000;  
                     __enable_irq();
                     if (lcddev.id == 0X5761)
                     lcddev.id = 0X1963; 
                }
            }
        }
    }
    if(lcddev.id == 0X9341) //9341
    {
        LCD_WR_REG(0xCF);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC1);
        LCD_WR_DATA(0X30);
        LCD_WR_REG(0xED);
        LCD_WR_DATA(0x64);
        LCD_WR_DATA(0x03);
        LCD_WR_DATA(0X12);
        LCD_WR_DATA(0X81);
        LCD_WR_REG(0xE8);
        LCD_WR_DATA(0x85);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x7A);
        LCD_WR_REG(0xCB);
        LCD_WR_DATA(0x39);
        LCD_WR_DATA(0x2C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x34);
        LCD_WR_DATA(0x02);
        LCD_WR_REG(0xF7);
        LCD_WR_DATA(0x20);
        LCD_WR_REG(0xEA);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0xC0);    //Power control
        LCD_WR_DATA(0x1B);   //VRH[5:0]
        LCD_WR_REG(0xC1);    //Power control
        LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0]
        LCD_WR_REG(0xC5);    //VCM control
        LCD_WR_DATA(0x30);   //3F
        LCD_WR_DATA(0x30);   //3C
        LCD_WR_REG(0xC7);    //VCM control2
        LCD_WR_DATA(0XB7);
        LCD_WR_REG(0x36);    // Memory Access Control
        LCD_WR_DATA(0x48);
        LCD_WR_REG(0x3A);
        LCD_WR_DATA(0x55);
        LCD_WR_REG(0xB1);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x1A);
        LCD_WR_REG(0xB6);    // Display Function Control
        LCD_WR_DATA(0x0A);
        LCD_WR_DATA(0xA2);
        LCD_WR_REG(0xF2);    // 3Gamma Function Disable
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0x26);    //Gamma curve selected
        LCD_WR_DATA(0x01);
        LCD_WR_REG(0xE0);    //Set Gamma
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x2A);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x08);
        LCD_WR_DATA(0x0E);
        LCD_WR_DATA(0x08);
        LCD_WR_DATA(0x54);
        LCD_WR_DATA(0XA9);
        LCD_WR_DATA(0x43);
        LCD_WR_DATA(0x0A);
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0XE1);    //Set Gamma
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x15);
        LCD_WR_DATA(0x17);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x11);
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x2B);
        LCD_WR_DATA(0x56);
        LCD_WR_DATA(0x3C);
        LCD_WR_DATA(0x05);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA(0x3F);
        LCD_WR_DATA(0x3F);
        LCD_WR_DATA(0x0F);
        LCD_WR_REG(0x2B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0x3f);
        LCD_WR_REG(0x2A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xef);
        LCD_WR_REG(0x11); //Exit Sleep
        Delay_Ms(120);
        LCD_WR_REG(0x29); //display on
    }
    else if (lcddev.id == 0x7789)   //7789
    {
        LCD_WR_REG(0x11);

        Delay_Ms(120);

        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x00);


        LCD_WR_REG(0x3A);
        LCD_WR_DATA(0X05);

        LCD_WR_REG(0xB2);
        LCD_WR_DATA(0x0C);
        LCD_WR_DATA(0x0C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x33);
        LCD_WR_DATA(0x33);

        LCD_WR_REG(0xB7);
        LCD_WR_DATA(0x35);

        LCD_WR_REG(0xBB);       //vcom
        LCD_WR_DATA(0x32);      //30
        // LCD_WR_DATA(0x19);      //30

        LCD_WR_REG(0xC0);
        LCD_WR_DATA(0x0C);
        // LCD_WR_DATA(0x2C);

        LCD_WR_REG(0xC2);
        LCD_WR_DATA(0x01);

        LCD_WR_REG(0xC3);       //vrh
        LCD_WR_DATA(0x10);      //17 0D
        // LCD_WR_DATA(0x12);      //17 0D

        LCD_WR_REG(0xC4);       //vdv
        LCD_WR_DATA(0x20);      //20

        LCD_WR_REG(0xC6);
        LCD_WR_DATA(0x0f);

        LCD_WR_REG(0xD0);
        LCD_WR_DATA(0xA4);
        LCD_WR_DATA(0xA1);

        LCD_WR_REG(0xE0);       //Set Gamma
        LCD_WR_DATA(0xd0);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x02);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x0a);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x32);
        LCD_WR_DATA(0X44);
        LCD_WR_DATA(0x42);
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x0e);
        LCD_WR_DATA(0x12);
        LCD_WR_DATA(0x14);
        LCD_WR_DATA(0x17);


        LCD_WR_REG(0XE1);       //Set Gamma
        LCD_WR_DATA(0xd0);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x02);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x0a);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x31);
        LCD_WR_DATA(0x54);
        LCD_WR_DATA(0x47);
        LCD_WR_DATA(0x0e);
        LCD_WR_DATA(0x1c);
        LCD_WR_DATA(0x17);
        LCD_WR_DATA(0x1b);
        LCD_WR_DATA(0x1e);



        LCD_WR_REG(0x2A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xef);

        LCD_WR_REG(0x2B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0x3f);

        LCD_WR_REG(0x29);       //display on
    }
    else if(lcddev.id == 0x6804) 
    {
        LCD_WR_REG(0X11);
        Delay_Ms(20);
        LCD_WR_REG(0XD0);//VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
        LCD_WR_DATA(0X07);
        LCD_WR_DATA(0X42);
        LCD_WR_DATA(0X1D);
        LCD_WR_REG(0XD1);//VCOMH VCOM_AC amplitude setting
        LCD_WR_DATA(0X00);
        LCD_WR_DATA(0X1a);
        LCD_WR_DATA(0X09);
        LCD_WR_REG(0XD2);//Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting
        LCD_WR_DATA(0X01);
        LCD_WR_DATA(0X22);
        LCD_WR_REG(0XC0);//REV SM GS
        LCD_WR_DATA(0X10);
        LCD_WR_DATA(0X3B);
        LCD_WR_DATA(0X00);
        LCD_WR_DATA(0X02);
        LCD_WR_DATA(0X11);

        LCD_WR_REG(0XC5);// Frame rate setting = 72HZ  when setting 0x03
        LCD_WR_DATA(0X03);

        LCD_WR_REG(0XC8);//Gamma setting
        LCD_WR_DATA(0X00);
        LCD_WR_DATA(0X25);
        LCD_WR_DATA(0X21);
        LCD_WR_DATA(0X05);
        LCD_WR_DATA(0X00);
        LCD_WR_DATA(0X0a);
        LCD_WR_DATA(0X65);
        LCD_WR_DATA(0X25);
        LCD_WR_DATA(0X77);
        LCD_WR_DATA(0X50);
        LCD_WR_DATA(0X0f);
        LCD_WR_DATA(0X00);

        LCD_WR_REG(0XF8);
        LCD_WR_DATA(0X01);

        LCD_WR_REG(0XFE);
        LCD_WR_DATA(0X00);
        LCD_WR_DATA(0X02);

        LCD_WR_REG(0X20);//Exit invert mode

        LCD_WR_REG(0X36);
        LCD_WR_DATA(0X08);

        LCD_WR_REG(0X3A);
        LCD_WR_DATA(0X55);
        LCD_WR_REG(0X2B);
        LCD_WR_DATA(0X00);
        LCD_WR_DATA(0X00);
        LCD_WR_DATA(0X01);
        LCD_WR_DATA(0X3F);

        LCD_WR_REG(0X2A);
        LCD_WR_DATA(0X00);
        LCD_WR_DATA(0X00);
        LCD_WR_DATA(0X01);
        LCD_WR_DATA(0XDF);
        Delay_Ms(120);
        LCD_WR_REG(0X29);
    }
    else if(lcddev.id == 0x5310)
    {
        LCD_WR_REG(0xED);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0xFE);

        LCD_WR_REG(0xEE);
        LCD_WR_DATA(0xDE);
        LCD_WR_DATA(0x21);

        LCD_WR_REG(0xF1);
        LCD_WR_DATA(0x01);
        LCD_WR_REG(0xDF);
        LCD_WR_DATA(0x10);

        //VCOMvoltage//
        LCD_WR_REG(0xC4);
        LCD_WR_DATA(0x8F);    //5f

        LCD_WR_REG(0xC6);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xE2);
        LCD_WR_DATA(0xE2);
        LCD_WR_DATA(0xE2);
        LCD_WR_REG(0xBF);
        LCD_WR_DATA(0xAA);

        LCD_WR_REG(0xB0);
        LCD_WR_DATA(0x0D);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x0D);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x11);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x19);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x21);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x2D);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x3D);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x5D);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x5D);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xB1);
        LCD_WR_DATA(0x80);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x8B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x96);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xB2);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x02);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x03);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xB3);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xB4);
        LCD_WR_DATA(0x8B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x96);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xA1);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xB5);
        LCD_WR_DATA(0x02);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x03);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x04);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xB6);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xB7);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x3F);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x5E);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x64);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x8C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xAC);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xDC);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x70);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x90);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xEB);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xDC);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xB8);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xBA);
        LCD_WR_DATA(0x24);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xC1);
        LCD_WR_DATA(0x20);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x54);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xFF);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xC2);
        LCD_WR_DATA(0x0A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x04);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xC3);
        LCD_WR_DATA(0x3C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x3A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x39);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x37);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x3C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x36);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x32);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x2F);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x2C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x29);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x26);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x24);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x24);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x23);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x3C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x36);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x32);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x2F);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x2C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x29);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x26);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x24);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x24);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x23);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xC4);
        LCD_WR_DATA(0x62);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x05);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x84);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xF0);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x18);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xA4);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x18);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x50);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x0C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x17);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x95);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xF3);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xE6);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xC5);
        LCD_WR_DATA(0x32);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x44);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x65);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x76);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x88);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xC6);
        LCD_WR_DATA(0x20);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x17);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xC7);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xC8);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xC9);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xE0);
        LCD_WR_DATA(0x16);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x1C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x21);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x36);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x46);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x52);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x64);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x7A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x8B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x99);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xA8);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xB9);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC4);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xCA);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xD2);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xD9);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xE0);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xF3);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xE1);
        LCD_WR_DATA(0x16);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x1C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x22);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x36);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x45);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x52);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x64);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x7A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x8B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x99);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xA8);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xB9);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC4);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xCA);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xD2);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xD8);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xE0);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xF3);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xE2);
        LCD_WR_DATA(0x05);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x0B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x1B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x34);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x44);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x4F);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x61);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x79);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x88);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x97);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xA6);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xB7);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC2);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC7);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xD1);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xD6);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xDD);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xF3);
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0xE3);
        LCD_WR_DATA(0x05);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xA);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x1C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x33);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x44);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x50);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x62);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x78);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x88);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x97);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xA6);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xB7);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC2);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC7);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xD1);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xD5);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xDD);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xF3);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xE4);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x02);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x2A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x3C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x4B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x5D);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x74);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x84);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x93);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xA2);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xB3);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xBE);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC4);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xCD);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xD3);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xDD);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xF3);
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0xE5);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x02);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x29);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x3C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x4B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x5D);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x74);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x84);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x93);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xA2);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xB3);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xBE);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xC4);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xCD);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xD3);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xDC);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xF3);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xE6);
        LCD_WR_DATA(0x11);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x34);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x56);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x76);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x77);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x66);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x88);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x99);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xBB);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x99);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x66);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x55);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x55);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x45);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x43);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x44);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xE7);
        LCD_WR_DATA(0x32);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x55);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x76);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x66);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x67);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x67);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x87);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x99);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xBB);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x99);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x77);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x44);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x56);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x23);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x33);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x45);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xE8);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x99);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x87);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x88);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x77);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x66);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x88);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xAA);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xBB);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x99);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x66);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x55);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x55);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x44);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x44);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x55);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xE9);
        LCD_WR_DATA(0xAA);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0x00);
        LCD_WR_DATA(0xAA);

        LCD_WR_REG(0xCF);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xF0);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x50);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xF3);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xF9);
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x29);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0x3A);
        LCD_WR_DATA(0x55);  //66

        LCD_WR_REG(0x11);
        Delay_Ms(100);
        LCD_WR_REG(0x29);
        LCD_WR_REG(0x35);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0x51);
        LCD_WR_DATA(0xFF);
        LCD_WR_REG(0x53);
        LCD_WR_DATA(0x2C);
        LCD_WR_REG(0x55);
        LCD_WR_DATA(0x82);
        LCD_WR_REG(0x2c);
    }
    else if(lcddev.id == 0x5510)
    {
        LCD_WriteReg(0xF000, 0x55);
        LCD_WriteReg(0xF001, 0xAA);
        LCD_WriteReg(0xF002, 0x52);
        LCD_WriteReg(0xF003, 0x08);
        LCD_WriteReg(0xF004, 0x01);
        //AVDD Set AVDD 5.2V
        LCD_WriteReg(0xB000, 0x0D);
        LCD_WriteReg(0xB001, 0x0D);
        LCD_WriteReg(0xB002, 0x0D);
        //AVDD ratio
        LCD_WriteReg(0xB600, 0x34);
        LCD_WriteReg(0xB601, 0x34);
        LCD_WriteReg(0xB602, 0x34);
        //AVEE -5.2V
        LCD_WriteReg(0xB100, 0x0D);
        LCD_WriteReg(0xB101, 0x0D);
        LCD_WriteReg(0xB102, 0x0D);
        //AVEE ratio
        LCD_WriteReg(0xB700, 0x34);
        LCD_WriteReg(0xB701, 0x34);
        LCD_WriteReg(0xB702, 0x34);
        //VCL -2.5V
        LCD_WriteReg(0xB200, 0x00);
        LCD_WriteReg(0xB201, 0x00);
        LCD_WriteReg(0xB202, 0x00);
        //VCL ratio
        LCD_WriteReg(0xB800, 0x24);
        LCD_WriteReg(0xB801, 0x24);
        LCD_WriteReg(0xB802, 0x24);
        //VGH 15V (Free pump)
        LCD_WriteReg(0xBF00, 0x01);
        LCD_WriteReg(0xB300, 0x0F);
        LCD_WriteReg(0xB301, 0x0F);
        LCD_WriteReg(0xB302, 0x0F);
        //VGH ratio
        LCD_WriteReg(0xB900, 0x34);
        LCD_WriteReg(0xB901, 0x34);
        LCD_WriteReg(0xB902, 0x34);
        //VGL_REG -10V
        LCD_WriteReg(0xB500, 0x08);
        LCD_WriteReg(0xB501, 0x08);
        LCD_WriteReg(0xB502, 0x08);
        LCD_WriteReg(0xC200, 0x03);
        //VGLX ratio
        LCD_WriteReg(0xBA00, 0x24);
        LCD_WriteReg(0xBA01, 0x24);
        LCD_WriteReg(0xBA02, 0x24);
        //VGMP/VGSP 4.5V/0V
        LCD_WriteReg(0xBC00, 0x00);
        LCD_WriteReg(0xBC01, 0x78);
        LCD_WriteReg(0xBC02, 0x00);
        //VGMN/VGSN -4.5V/0V
        LCD_WriteReg(0xBD00, 0x00);
        LCD_WriteReg(0xBD01, 0x78);
        LCD_WriteReg(0xBD02, 0x00);
        //VCOM
        LCD_WriteReg(0xBE00, 0x00);
        LCD_WriteReg(0xBE01, 0x64);
        //Gamma Setting
        LCD_WriteReg(0xD100, 0x00);
        LCD_WriteReg(0xD101, 0x33);
        LCD_WriteReg(0xD102, 0x00);
        LCD_WriteReg(0xD103, 0x34);
        LCD_WriteReg(0xD104, 0x00);
        LCD_WriteReg(0xD105, 0x3A);
        LCD_WriteReg(0xD106, 0x00);
        LCD_WriteReg(0xD107, 0x4A);
        LCD_WriteReg(0xD108, 0x00);
        LCD_WriteReg(0xD109, 0x5C);
        LCD_WriteReg(0xD10A, 0x00);
        LCD_WriteReg(0xD10B, 0x81);
        LCD_WriteReg(0xD10C, 0x00);
        LCD_WriteReg(0xD10D, 0xA6);
        LCD_WriteReg(0xD10E, 0x00);
        LCD_WriteReg(0xD10F, 0xE5);
        LCD_WriteReg(0xD110, 0x01);
        LCD_WriteReg(0xD111, 0x13);
        LCD_WriteReg(0xD112, 0x01);
        LCD_WriteReg(0xD113, 0x54);
        LCD_WriteReg(0xD114, 0x01);
        LCD_WriteReg(0xD115, 0x82);
        LCD_WriteReg(0xD116, 0x01);
        LCD_WriteReg(0xD117, 0xCA);
        LCD_WriteReg(0xD118, 0x02);
        LCD_WriteReg(0xD119, 0x00);
        LCD_WriteReg(0xD11A, 0x02);
        LCD_WriteReg(0xD11B, 0x01);
        LCD_WriteReg(0xD11C, 0x02);
        LCD_WriteReg(0xD11D, 0x34);
        LCD_WriteReg(0xD11E, 0x02);
        LCD_WriteReg(0xD11F, 0x67);
        LCD_WriteReg(0xD120, 0x02);
        LCD_WriteReg(0xD121, 0x84);
        LCD_WriteReg(0xD122, 0x02);
        LCD_WriteReg(0xD123, 0xA4);
        LCD_WriteReg(0xD124, 0x02);
        LCD_WriteReg(0xD125, 0xB7);
        LCD_WriteReg(0xD126, 0x02);
        LCD_WriteReg(0xD127, 0xCF);
        LCD_WriteReg(0xD128, 0x02);
        LCD_WriteReg(0xD129, 0xDE);
        LCD_WriteReg(0xD12A, 0x02);
        LCD_WriteReg(0xD12B, 0xF2);
        LCD_WriteReg(0xD12C, 0x02);
        LCD_WriteReg(0xD12D, 0xFE);
        LCD_WriteReg(0xD12E, 0x03);
        LCD_WriteReg(0xD12F, 0x10);
        LCD_WriteReg(0xD130, 0x03);
        LCD_WriteReg(0xD131, 0x33);
        LCD_WriteReg(0xD132, 0x03);
        LCD_WriteReg(0xD133, 0x6D);
        LCD_WriteReg(0xD200, 0x00);
        LCD_WriteReg(0xD201, 0x33);
        LCD_WriteReg(0xD202, 0x00);
        LCD_WriteReg(0xD203, 0x34);
        LCD_WriteReg(0xD204, 0x00);
        LCD_WriteReg(0xD205, 0x3A);
        LCD_WriteReg(0xD206, 0x00);
        LCD_WriteReg(0xD207, 0x4A);
        LCD_WriteReg(0xD208, 0x00);
        LCD_WriteReg(0xD209, 0x5C);
        LCD_WriteReg(0xD20A, 0x00);

        LCD_WriteReg(0xD20B, 0x81);
        LCD_WriteReg(0xD20C, 0x00);
        LCD_WriteReg(0xD20D, 0xA6);
        LCD_WriteReg(0xD20E, 0x00);
        LCD_WriteReg(0xD20F, 0xE5);
        LCD_WriteReg(0xD210, 0x01);
        LCD_WriteReg(0xD211, 0x13);
        LCD_WriteReg(0xD212, 0x01);
        LCD_WriteReg(0xD213, 0x54);
        LCD_WriteReg(0xD214, 0x01);
        LCD_WriteReg(0xD215, 0x82);
        LCD_WriteReg(0xD216, 0x01);
        LCD_WriteReg(0xD217, 0xCA);
        LCD_WriteReg(0xD218, 0x02);
        LCD_WriteReg(0xD219, 0x00);
        LCD_WriteReg(0xD21A, 0x02);
        LCD_WriteReg(0xD21B, 0x01);
        LCD_WriteReg(0xD21C, 0x02);
        LCD_WriteReg(0xD21D, 0x34);
        LCD_WriteReg(0xD21E, 0x02);
        LCD_WriteReg(0xD21F, 0x67);
        LCD_WriteReg(0xD220, 0x02);
        LCD_WriteReg(0xD221, 0x84);
        LCD_WriteReg(0xD222, 0x02);
        LCD_WriteReg(0xD223, 0xA4);
        LCD_WriteReg(0xD224, 0x02);
        LCD_WriteReg(0xD225, 0xB7);
        LCD_WriteReg(0xD226, 0x02);
        LCD_WriteReg(0xD227, 0xCF);
        LCD_WriteReg(0xD228, 0x02);
        LCD_WriteReg(0xD229, 0xDE);
        LCD_WriteReg(0xD22A, 0x02);
        LCD_WriteReg(0xD22B, 0xF2);
        LCD_WriteReg(0xD22C, 0x02);
        LCD_WriteReg(0xD22D, 0xFE);
        LCD_WriteReg(0xD22E, 0x03);
        LCD_WriteReg(0xD22F, 0x10);
        LCD_WriteReg(0xD230, 0x03);
        LCD_WriteReg(0xD231, 0x33);
        LCD_WriteReg(0xD232, 0x03);
        LCD_WriteReg(0xD233, 0x6D);
        LCD_WriteReg(0xD300, 0x00);
        LCD_WriteReg(0xD301, 0x33);
        LCD_WriteReg(0xD302, 0x00);
        LCD_WriteReg(0xD303, 0x34);
        LCD_WriteReg(0xD304, 0x00);
        LCD_WriteReg(0xD305, 0x3A);
        LCD_WriteReg(0xD306, 0x00);
        LCD_WriteReg(0xD307, 0x4A);
        LCD_WriteReg(0xD308, 0x00);
        LCD_WriteReg(0xD309, 0x5C);
        LCD_WriteReg(0xD30A, 0x00);

        LCD_WriteReg(0xD30B, 0x81);
        LCD_WriteReg(0xD30C, 0x00);
        LCD_WriteReg(0xD30D, 0xA6);
        LCD_WriteReg(0xD30E, 0x00);
        LCD_WriteReg(0xD30F, 0xE5);
        LCD_WriteReg(0xD310, 0x01);
        LCD_WriteReg(0xD311, 0x13);
        LCD_WriteReg(0xD312, 0x01);
        LCD_WriteReg(0xD313, 0x54);
        LCD_WriteReg(0xD314, 0x01);
        LCD_WriteReg(0xD315, 0x82);
        LCD_WriteReg(0xD316, 0x01);
        LCD_WriteReg(0xD317, 0xCA);
        LCD_WriteReg(0xD318, 0x02);
        LCD_WriteReg(0xD319, 0x00);
        LCD_WriteReg(0xD31A, 0x02);
        LCD_WriteReg(0xD31B, 0x01);
        LCD_WriteReg(0xD31C, 0x02);
        LCD_WriteReg(0xD31D, 0x34);
        LCD_WriteReg(0xD31E, 0x02);
        LCD_WriteReg(0xD31F, 0x67);
        LCD_WriteReg(0xD320, 0x02);
        LCD_WriteReg(0xD321, 0x84);
        LCD_WriteReg(0xD322, 0x02);
        LCD_WriteReg(0xD323, 0xA4);
        LCD_WriteReg(0xD324, 0x02);
        LCD_WriteReg(0xD325, 0xB7);
        LCD_WriteReg(0xD326, 0x02);
        LCD_WriteReg(0xD327, 0xCF);
        LCD_WriteReg(0xD328, 0x02);
        LCD_WriteReg(0xD329, 0xDE);
        LCD_WriteReg(0xD32A, 0x02);
        LCD_WriteReg(0xD32B, 0xF2);
        LCD_WriteReg(0xD32C, 0x02);
        LCD_WriteReg(0xD32D, 0xFE);
        LCD_WriteReg(0xD32E, 0x03);
        LCD_WriteReg(0xD32F, 0x10);
        LCD_WriteReg(0xD330, 0x03);
        LCD_WriteReg(0xD331, 0x33);
        LCD_WriteReg(0xD332, 0x03);
        LCD_WriteReg(0xD333, 0x6D);
        LCD_WriteReg(0xD400, 0x00);
        LCD_WriteReg(0xD401, 0x33);
        LCD_WriteReg(0xD402, 0x00);
        LCD_WriteReg(0xD403, 0x34);
        LCD_WriteReg(0xD404, 0x00);
        LCD_WriteReg(0xD405, 0x3A);
        LCD_WriteReg(0xD406, 0x00);
        LCD_WriteReg(0xD407, 0x4A);
        LCD_WriteReg(0xD408, 0x00);
        LCD_WriteReg(0xD409, 0x5C);
        LCD_WriteReg(0xD40A, 0x00);
        LCD_WriteReg(0xD40B, 0x81);

        LCD_WriteReg(0xD40C, 0x00);
        LCD_WriteReg(0xD40D, 0xA6);
        LCD_WriteReg(0xD40E, 0x00);
        LCD_WriteReg(0xD40F, 0xE5);
        LCD_WriteReg(0xD410, 0x01);
        LCD_WriteReg(0xD411, 0x13);
        LCD_WriteReg(0xD412, 0x01);
        LCD_WriteReg(0xD413, 0x54);
        LCD_WriteReg(0xD414, 0x01);
        LCD_WriteReg(0xD415, 0x82);
        LCD_WriteReg(0xD416, 0x01);
        LCD_WriteReg(0xD417, 0xCA);
        LCD_WriteReg(0xD418, 0x02);
        LCD_WriteReg(0xD419, 0x00);
        LCD_WriteReg(0xD41A, 0x02);
        LCD_WriteReg(0xD41B, 0x01);
        LCD_WriteReg(0xD41C, 0x02);
        LCD_WriteReg(0xD41D, 0x34);
        LCD_WriteReg(0xD41E, 0x02);
        LCD_WriteReg(0xD41F, 0x67);
        LCD_WriteReg(0xD420, 0x02);
        LCD_WriteReg(0xD421, 0x84);
        LCD_WriteReg(0xD422, 0x02);
        LCD_WriteReg(0xD423, 0xA4);
        LCD_WriteReg(0xD424, 0x02);
        LCD_WriteReg(0xD425, 0xB7);
        LCD_WriteReg(0xD426, 0x02);
        LCD_WriteReg(0xD427, 0xCF);
        LCD_WriteReg(0xD428, 0x02);
        LCD_WriteReg(0xD429, 0xDE);
        LCD_WriteReg(0xD42A, 0x02);
        LCD_WriteReg(0xD42B, 0xF2);
        LCD_WriteReg(0xD42C, 0x02);
        LCD_WriteReg(0xD42D, 0xFE);
        LCD_WriteReg(0xD42E, 0x03);
        LCD_WriteReg(0xD42F, 0x10);
        LCD_WriteReg(0xD430, 0x03);
        LCD_WriteReg(0xD431, 0x33);
        LCD_WriteReg(0xD432, 0x03);
        LCD_WriteReg(0xD433, 0x6D);
        LCD_WriteReg(0xD500, 0x00);
        LCD_WriteReg(0xD501, 0x33);
        LCD_WriteReg(0xD502, 0x00);
        LCD_WriteReg(0xD503, 0x34);
        LCD_WriteReg(0xD504, 0x00);
        LCD_WriteReg(0xD505, 0x3A);
        LCD_WriteReg(0xD506, 0x00);
        LCD_WriteReg(0xD507, 0x4A);
        LCD_WriteReg(0xD508, 0x00);
        LCD_WriteReg(0xD509, 0x5C);
        LCD_WriteReg(0xD50A, 0x00);
        LCD_WriteReg(0xD50B, 0x81);

        LCD_WriteReg(0xD50C, 0x00);
        LCD_WriteReg(0xD50D, 0xA6);
        LCD_WriteReg(0xD50E, 0x00);
        LCD_WriteReg(0xD50F, 0xE5);
        LCD_WriteReg(0xD510, 0x01);
        LCD_WriteReg(0xD511, 0x13);
        LCD_WriteReg(0xD512, 0x01);
        LCD_WriteReg(0xD513, 0x54);
        LCD_WriteReg(0xD514, 0x01);
        LCD_WriteReg(0xD515, 0x82);
        LCD_WriteReg(0xD516, 0x01);
        LCD_WriteReg(0xD517, 0xCA);
        LCD_WriteReg(0xD518, 0x02);
        LCD_WriteReg(0xD519, 0x00);
        LCD_WriteReg(0xD51A, 0x02);
        LCD_WriteReg(0xD51B, 0x01);
        LCD_WriteReg(0xD51C, 0x02);
        LCD_WriteReg(0xD51D, 0x34);
        LCD_WriteReg(0xD51E, 0x02);
        LCD_WriteReg(0xD51F, 0x67);
        LCD_WriteReg(0xD520, 0x02);
        LCD_WriteReg(0xD521, 0x84);
        LCD_WriteReg(0xD522, 0x02);
        LCD_WriteReg(0xD523, 0xA4);
        LCD_WriteReg(0xD524, 0x02);
        LCD_WriteReg(0xD525, 0xB7);
        LCD_WriteReg(0xD526, 0x02);
        LCD_WriteReg(0xD527, 0xCF);
        LCD_WriteReg(0xD528, 0x02);
        LCD_WriteReg(0xD529, 0xDE);
        LCD_WriteReg(0xD52A, 0x02);
        LCD_WriteReg(0xD52B, 0xF2);
        LCD_WriteReg(0xD52C, 0x02);
        LCD_WriteReg(0xD52D, 0xFE);
        LCD_WriteReg(0xD52E, 0x03);
        LCD_WriteReg(0xD52F, 0x10);
        LCD_WriteReg(0xD530, 0x03);
        LCD_WriteReg(0xD531, 0x33);
        LCD_WriteReg(0xD532, 0x03);
        LCD_WriteReg(0xD533, 0x6D);
        LCD_WriteReg(0xD600, 0x00);
        LCD_WriteReg(0xD601, 0x33);
        LCD_WriteReg(0xD602, 0x00);
        LCD_WriteReg(0xD603, 0x34);
        LCD_WriteReg(0xD604, 0x00);
        LCD_WriteReg(0xD605, 0x3A);
        LCD_WriteReg(0xD606, 0x00);
        LCD_WriteReg(0xD607, 0x4A);
        LCD_WriteReg(0xD608, 0x00);
        LCD_WriteReg(0xD609, 0x5C);
        LCD_WriteReg(0xD60A, 0x00);
        LCD_WriteReg(0xD60B, 0x81);

        LCD_WriteReg(0xD60C, 0x00);
        LCD_WriteReg(0xD60D, 0xA6);
        LCD_WriteReg(0xD60E, 0x00);
        LCD_WriteReg(0xD60F, 0xE5);
        LCD_WriteReg(0xD610, 0x01);
        LCD_WriteReg(0xD611, 0x13);
        LCD_WriteReg(0xD612, 0x01);
        LCD_WriteReg(0xD613, 0x54);
        LCD_WriteReg(0xD614, 0x01);
        LCD_WriteReg(0xD615, 0x82);
        LCD_WriteReg(0xD616, 0x01);
        LCD_WriteReg(0xD617, 0xCA);
        LCD_WriteReg(0xD618, 0x02);
        LCD_WriteReg(0xD619, 0x00);
        LCD_WriteReg(0xD61A, 0x02);
        LCD_WriteReg(0xD61B, 0x01);
        LCD_WriteReg(0xD61C, 0x02);
        LCD_WriteReg(0xD61D, 0x34);
        LCD_WriteReg(0xD61E, 0x02);
        LCD_WriteReg(0xD61F, 0x67);
        LCD_WriteReg(0xD620, 0x02);
        LCD_WriteReg(0xD621, 0x84);
        LCD_WriteReg(0xD622, 0x02);
        LCD_WriteReg(0xD623, 0xA4);
        LCD_WriteReg(0xD624, 0x02);
        LCD_WriteReg(0xD625, 0xB7);
        LCD_WriteReg(0xD626, 0x02);
        LCD_WriteReg(0xD627, 0xCF);
        LCD_WriteReg(0xD628, 0x02);
        LCD_WriteReg(0xD629, 0xDE);
        LCD_WriteReg(0xD62A, 0x02);
        LCD_WriteReg(0xD62B, 0xF2);
        LCD_WriteReg(0xD62C, 0x02);
        LCD_WriteReg(0xD62D, 0xFE);
        LCD_WriteReg(0xD62E, 0x03);
        LCD_WriteReg(0xD62F, 0x10);
        LCD_WriteReg(0xD630, 0x03);
        LCD_WriteReg(0xD631, 0x33);
        LCD_WriteReg(0xD632, 0x03);
        LCD_WriteReg(0xD633, 0x6D);
        //LV2 Page 0 enable
        LCD_WriteReg(0xF000, 0x55);
        LCD_WriteReg(0xF001, 0xAA);
        LCD_WriteReg(0xF002, 0x52);
        LCD_WriteReg(0xF003, 0x08);
        LCD_WriteReg(0xF004, 0x00);
        //Display control
        LCD_WriteReg(0xB100, 0xCC);
        LCD_WriteReg(0xB101, 0x00);
        //Source hold time
        LCD_WriteReg(0xB600, 0x05);
        //Gate EQ control
        LCD_WriteReg(0xB700, 0x70);
        LCD_WriteReg(0xB701, 0x70);
        //Source EQ control (Mode 2)
        LCD_WriteReg(0xB800, 0x01);
        LCD_WriteReg(0xB801, 0x03);
        LCD_WriteReg(0xB802, 0x03);
        LCD_WriteReg(0xB803, 0x03);
        //Inversion mode (2-dot)
        LCD_WriteReg(0xBC00, 0x02);
        LCD_WriteReg(0xBC01, 0x00);
        LCD_WriteReg(0xBC02, 0x00);
        //Timing control 4H w/ 4-delay
        LCD_WriteReg(0xC900, 0xD0);
        LCD_WriteReg(0xC901, 0x02);
        LCD_WriteReg(0xC902, 0x50);
        LCD_WriteReg(0xC903, 0x50);
        LCD_WriteReg(0xC904, 0x50);
        LCD_WriteReg(0x3500, 0x00);
        LCD_WriteReg(0x3A00, 0x55);  //16-bit/pixel
        LCD_WR_REG(0x1100);
        Delay_Us(120);
        LCD_WR_REG(0x2900);
    }
    else if(lcddev.id == 0x9325)//9325
    {
        LCD_WriteReg(0x00E5, 0x78F0);
        LCD_WriteReg(0x0001, 0x0100);
        LCD_WriteReg(0x0002, 0x0700);
        LCD_WriteReg(0x0003, 0x1030);
        LCD_WriteReg(0x0004, 0x0000);
        LCD_WriteReg(0x0008, 0x0202);
        LCD_WriteReg(0x0009, 0x0000);
        LCD_WriteReg(0x000A, 0x0000);
        LCD_WriteReg(0x000C, 0x0000);
        LCD_WriteReg(0x000D, 0x0000);
        LCD_WriteReg(0x000F, 0x0000);
        //power on sequence VGHVGL
        LCD_WriteReg(0x0010, 0x0000);
        LCD_WriteReg(0x0011, 0x0007);
        LCD_WriteReg(0x0012, 0x0000);
        LCD_WriteReg(0x0013, 0x0000);
        LCD_WriteReg(0x0007, 0x0000);
        //vgh
        LCD_WriteReg(0x0010, 0x1690);
        LCD_WriteReg(0x0011, 0x0227);
      
        //vregiout
        LCD_WriteReg(0x0012, 0x009D); //0x001b
      
        //vom amplitude
        LCD_WriteReg(0x0013, 0x1900);
  
        //vom H
        LCD_WriteReg(0x0029, 0x0025);
        LCD_WriteReg(0x002B, 0x000D);
        //gamma
        LCD_WriteReg(0x0030, 0x0007);
        LCD_WriteReg(0x0031, 0x0303);
        LCD_WriteReg(0x0032, 0x0003);// 0006
        LCD_WriteReg(0x0035, 0x0206);
        LCD_WriteReg(0x0036, 0x0008);
        LCD_WriteReg(0x0037, 0x0406);
        LCD_WriteReg(0x0038, 0x0304);//0200
        LCD_WriteReg(0x0039, 0x0007);
        LCD_WriteReg(0x003C, 0x0602);// 0504
        LCD_WriteReg(0x003D, 0x0008);
        //ram
        LCD_WriteReg(0x0050, 0x0000);
        LCD_WriteReg(0x0051, 0x00EF);
        LCD_WriteReg(0x0052, 0x0000);
        LCD_WriteReg(0x0053, 0x013F);
        LCD_WriteReg(0x0060, 0xA700);
        LCD_WriteReg(0x0061, 0x0001);
        LCD_WriteReg(0x006A, 0x0000);
        //
        LCD_WriteReg(0x0080, 0x0000);
        LCD_WriteReg(0x0081, 0x0000);
        LCD_WriteReg(0x0082, 0x0000);
        LCD_WriteReg(0x0083, 0x0000);
        LCD_WriteReg(0x0084, 0x0000);
        LCD_WriteReg(0x0085, 0x0000);
        //
        LCD_WriteReg(0x0090, 0x0010);
        LCD_WriteReg(0x0092, 0x0600);

        LCD_WriteReg(0x0007, 0x0133);
        LCD_WriteReg(0x00, 0x0022);//
    }
    else if(lcddev.id == 0x9328)//ILI9328   OK
    {
        LCD_WriteReg(0x00EC, 0x108F);// internal timeing
        LCD_WriteReg(0x00EF, 0x1234);// ADD

        LCD_WriteReg(0x0001, 0x0100);
        LCD_WriteReg(0x0002, 0x0700);

        //AM ID0 ID1   FUNCATION
        // 0  0   0    R->L D->U
        // 1  0   0    D->U R->L
        // 0  1   0    L->R D->U
        // 1  1   0    D->U L->R
        // 0  0   1    R->L U->D
        // 1  0   1    U->D R->L
        // 0  1   1    L->R U->D 
        // 1  1   1    U->D L->R
        LCD_WriteReg(0x0003, (1 << 12) | (3 << 4) | (0 << 3) );//65K
        LCD_WriteReg(0x0004, 0x0000);
        LCD_WriteReg(0x0008, 0x0202);
        LCD_WriteReg(0x0009, 0x0000);
        LCD_WriteReg(0x000a, 0x0000);//display setting
        LCD_WriteReg(0x000c, 0x0001);//display setting
        LCD_WriteReg(0x000d, 0x0000);//0f3c
        LCD_WriteReg(0x000f, 0x0000);
       
        LCD_WriteReg(0x0010, 0x0000);
        LCD_WriteReg(0x0011, 0x0007);
        LCD_WriteReg(0x0012, 0x0000);
        LCD_WriteReg(0x0013, 0x0000);
        LCD_WriteReg(0x0007, 0x0001);
        Delay_Ms(50);
        LCD_WriteReg(0x0010, 0x1490);
        LCD_WriteReg(0x0011, 0x0227);
        Delay_Ms(50);
        LCD_WriteReg(0x0012, 0x008A);
        Delay_Ms(50);
        LCD_WriteReg(0x0013, 0x1a00);
        LCD_WriteReg(0x0029, 0x0006);
        LCD_WriteReg(0x002b, 0x000d);
        Delay_Ms(50);
        LCD_WriteReg(0x0020, 0x0000);
        LCD_WriteReg(0x0021, 0x0000);
        Delay_Ms(50);
       
        LCD_WriteReg(0x0030, 0x0000);
        LCD_WriteReg(0x0031, 0x0604);
        LCD_WriteReg(0x0032, 0x0305);
        LCD_WriteReg(0x0035, 0x0000);
        LCD_WriteReg(0x0036, 0x0C09);
        LCD_WriteReg(0x0037, 0x0204);
        LCD_WriteReg(0x0038, 0x0301);
        LCD_WriteReg(0x0039, 0x0707);
        LCD_WriteReg(0x003c, 0x0000);
        LCD_WriteReg(0x003d, 0x0a0a);
        Delay_Ms(50);
        LCD_WriteReg(0x0050, 0x0000); 
        LCD_WriteReg(0x0051, 0x00ef);
        LCD_WriteReg(0x0052, 0x0000); 
        LCD_WriteReg(0x0053, 0x013f); 

        LCD_WriteReg(0x0060, 0xa700);
        LCD_WriteReg(0x0061, 0x0001);
        LCD_WriteReg(0x006a, 0x0000);
        LCD_WriteReg(0x0080, 0x0000);
        LCD_WriteReg(0x0081, 0x0000);
        LCD_WriteReg(0x0082, 0x0000);
        LCD_WriteReg(0x0083, 0x0000);
        LCD_WriteReg(0x0084, 0x0000);
        LCD_WriteReg(0x0085, 0x0000);

        LCD_WriteReg(0x0090, 0x0010);
        LCD_WriteReg(0x0092, 0x0600);
     
        LCD_WriteReg(0x0007, 0x0133);
    }
    else if(lcddev.id == 0x9320)//聆彸OK.
    {
        LCD_WriteReg(0x00, 0x0000);
        LCD_WriteReg(0x01, 0x0100); //Driver Output Contral.
        LCD_WriteReg(0x02, 0x0700); //LCD Driver Waveform Contral.
        LCD_WriteReg(0x03, 0x1030);//Entry Mode Set.
        //LCD_WriteReg(0x03, 0x1018);   //Entry Mode Set.

        LCD_WriteReg(0x04, 0x0000); //Scalling Contral.
        LCD_WriteReg(0x08, 0x0202); //Display Contral 2.(0x0207)
        LCD_WriteReg(0x09, 0x0000); //Display Contral 3.(0x0000)
        LCD_WriteReg(0x0a, 0x0000); //Frame Cycle Contal.(0x0000)
        LCD_WriteReg(0x0c, (1 << 0));   //Extern Display Interface Contral 1.(0x0000)
        LCD_WriteReg(0x0d, 0x0000); //Frame Maker Position.
        LCD_WriteReg(0x0f, 0x0000); //Extern Display Interface Contral 2.
        Delay_Ms(50);
        LCD_WriteReg(0x07, 0x0101); //Display Contral.
        Delay_Ms(50);
        LCD_WriteReg(0x10, (1 << 12) | (0 << 8) | (1 << 7) | (1 << 6) | (0 << 4));  //Power Control 1.(0x16b0)
        LCD_WriteReg(0x11, 0x0007);                             //Power Control 2.(0x0001)
        LCD_WriteReg(0x12,(1 << 8) | (1 << 4) | (0 << 0));              //Power Control 3.(0x0138)
        LCD_WriteReg(0x13, 0x0b00);                             //Power Control 4.
        LCD_WriteReg(0x29, 0x0000);                             //Power Control 7.

        LCD_WriteReg(0x2b, (1 << 14) | (1 << 4));
        LCD_WriteReg(0x50, 0);  //Set X Star
   
        LCD_WriteReg(0x51, 239);    //Set Y Star
        LCD_WriteReg(0x52, 0);  //Set Y End.t.
        LCD_WriteReg(0x53, 319);    //

        LCD_WriteReg(0x60, 0x2700); //Driver Output Control.
        LCD_WriteReg(0x61, 0x0001); //Driver Output Control.
        LCD_WriteReg(0x6a, 0x0000); //Vertical Srcoll Control.

        LCD_WriteReg(0x80, 0x0000); //Display Position? Partial Display 1.
        LCD_WriteReg(0x81, 0x0000); //RAM Address Start? Partial Display 1.
        LCD_WriteReg(0x82, 0x0000); //RAM Address End-Partial Display 1.
        LCD_WriteReg(0x83, 0x0000); //Displsy Position? Partial Display 2.
        LCD_WriteReg(0x84, 0x0000); //RAM Address Start? Partial Display 2.
        LCD_WriteReg(0x85, 0x0000); //RAM Address End? Partial Display 2.

        LCD_WriteReg(0x90, (0 << 7) | (16 << 0));   //Frame Cycle Contral.(0x0013)
        LCD_WriteReg(0x92, 0x0000); //Panel Interface Contral 2.(0x0000)
        LCD_WriteReg(0x93, 0x0001); //Panel Interface Contral 3.
        LCD_WriteReg(0x95, 0x0110); //Frame Cycle Contral.(0x0110)
        LCD_WriteReg(0x97, (0 << 8));   //
        LCD_WriteReg(0x98, 0x0000); //Frame Cycle Contral.
        LCD_WriteReg(0x07, 0x0173); //(0x0173)
    }
    else if(lcddev.id == 0X9331)//OK |/|/|
    {
        LCD_WriteReg(0x00E7, 0x1014);
        LCD_WriteReg(0x0001, 0x0100); // set SS and SM bit
        LCD_WriteReg(0x0002, 0x0200); // set 1 line inversion
        LCD_WriteReg(0x0003, (1 << 12) | (3 << 4) | (1 << 3));//65K
        //LCD_WriteReg(0x0003, 0x1030); // set GRAM write direction and BGR=1.
        LCD_WriteReg(0x0008, 0x0202); // set the back porch and front porch
        LCD_WriteReg(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
        LCD_WriteReg(0x000A, 0x0000); // FMARK function
        LCD_WriteReg(0x000C, 0x0000); // RGB interface setting
        LCD_WriteReg(0x000D, 0x0000); // Frame marker Position
        LCD_WriteReg(0x000F, 0x0000); // RGB interface polarity
        //*************Power On sequence ****************//
        LCD_WriteReg(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
        LCD_WriteReg(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
        LCD_WriteReg(0x0012, 0x0000); // VREG1OUT voltage
        LCD_WriteReg(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
        Delay_Ms(200); // Dis-charge capacitor power voltage
        LCD_WriteReg(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
        LCD_WriteReg(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
        Delay_Ms(50); // Delay 50ms
        LCD_WriteReg(0x0012, 0x000C); // Internal reference voltage= Vci;
        Delay_Ms(50); // Delay 50ms
        LCD_WriteReg(0x0013, 0x0800); // Set VDV[4:0] for VCOM amplitude
        LCD_WriteReg(0x0029, 0x0011); // Set VCM[5:0] for VCOMH
        LCD_WriteReg(0x002B, 0x000B); // Set Frame Rate
        Delay_Ms(50); // Delay 50ms
        LCD_WriteReg(0x0020, 0x0000); // GRAM horizontal Address
        LCD_WriteReg(0x0021, 0x013f); // GRAM Vertical Address
        // ----------- Adjust the Gamma Curve ----------//
        LCD_WriteReg(0x0030, 0x0000);
        LCD_WriteReg(0x0031, 0x0106);
        LCD_WriteReg(0x0032, 0x0000);
        LCD_WriteReg(0x0035, 0x0204);
        LCD_WriteReg(0x0036, 0x160A);
        LCD_WriteReg(0x0037, 0x0707);
        LCD_WriteReg(0x0038, 0x0106);
        LCD_WriteReg(0x0039, 0x0707);
        LCD_WriteReg(0x003C, 0x0402);
        LCD_WriteReg(0x003D, 0x0C0F);
        //------------------ Set GRAM area ---------------//
        LCD_WriteReg(0x0050, 0x0000); // Horizontal GRAM Start Address
        LCD_WriteReg(0x0051, 0x00EF); // Horizontal GRAM End Address
        LCD_WriteReg(0x0052, 0x0000); // Vertical GRAM Start Address
        LCD_WriteReg(0x0053, 0x013F); // Vertical GRAM Start Address
        LCD_WriteReg(0x0060, 0x2700); // Gate Scan Line
        LCD_WriteReg(0x0061, 0x0001); // NDL,VLE, REV
        LCD_WriteReg(0x006A, 0x0000); // set scrolling line
        //-------------- Partial Display Control ---------//
        LCD_WriteReg(0x0080, 0x0000);
        LCD_WriteReg(0x0081, 0x0000);
        LCD_WriteReg(0x0082, 0x0000);
        LCD_WriteReg(0x0083, 0x0000);
        LCD_WriteReg(0x0084, 0x0000);
        LCD_WriteReg(0x0085, 0x0000);
        //-------------- Panel Control -------------------//
        LCD_WriteReg(0x0090, 0x0010);
        LCD_WriteReg(0x0092, 0x0600);
        LCD_WriteReg(0x0007, 0x0133); // 262K color and display ON
    }
    else if(lcddev.id == 0x5408)
    {
        LCD_WriteReg(0x01, 0x0100);
        LCD_WriteReg(0x02, 0x0700);//LCD Driving Waveform Contral
        LCD_WriteReg(0x03, 0x1030);

        LCD_WriteReg(0x04, 0x0000); //Scalling Control register
        LCD_WriteReg(0x08, 0x0207); //Display Control 2
        LCD_WriteReg(0x09, 0x0000); //Display Control 3
        LCD_WriteReg(0x0A, 0x0000); //Frame Cycle Control
        LCD_WriteReg(0x0C, 0x0000); //External Display Interface Control 1
        LCD_WriteReg(0x0D, 0x0000); //Frame Maker Position
        LCD_WriteReg(0x0F, 0x0000); //External Display Interface Control 2
        Delay_Ms(20);
    
        LCD_WriteReg(0x10, 0x16B0); //0x14B0 //Power Control 1
        LCD_WriteReg(0x11, 0x0001); //0x0007 //Power Control 2
        LCD_WriteReg(0x17, 0x0001); //0x0000 //Power Control 3
        LCD_WriteReg(0x12, 0x0138); //0x013B //Power Control 4
        LCD_WriteReg(0x13, 0x0800); //0x0800 //Power Control 5
        LCD_WriteReg(0x29, 0x0009); //NVM read data 2
        LCD_WriteReg(0x2a, 0x0009); //NVM read data 3
        LCD_WriteReg(0xa4, 0x0000);
        LCD_WriteReg(0x50, 0x0000); 
        LCD_WriteReg(0x51, 0x00EF); 
        LCD_WriteReg(0x52, 0x0000); 
        LCD_WriteReg(0x53, 0x013F); 
        LCD_WriteReg(0x60, 0x2700); //Driver Output Control
        LCD_WriteReg(0x61, 0x0001); //Driver Output Control
        LCD_WriteReg(0x6A, 0x0000); //Vertical Scroll Control
        LCD_WriteReg(0x80, 0x0000); //Display Position 每 Partial Display 1
        LCD_WriteReg(0x81, 0x0000); //RAM Address Start 每 Partial Display 1
        LCD_WriteReg(0x82, 0x0000); //RAM address End - Partial Display 1
        LCD_WriteReg(0x83, 0x0000); //Display Position 每 Partial Display 2
        LCD_WriteReg(0x84, 0x0000); //RAM Address Start 每 Partial Display 2
        LCD_WriteReg(0x85, 0x0000); //RAM address End 每 Partail Display2
        LCD_WriteReg(0x90, 0x0013); //Frame Cycle Control
        LCD_WriteReg(0x92, 0x0000);  //Panel Interface Control 2
        LCD_WriteReg(0x93, 0x0003); //Panel Interface control 3
        LCD_WriteReg(0x95, 0x0110);  //Frame Cycle Control
        LCD_WriteReg(0x07, 0x0173);
        Delay_Ms(50);
    }
    else if(lcddev.id == 0x1505)//OK
    {
        // second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
        LCD_WriteReg(0x0007, 0x0000);
        Delay_Ms(50);
        LCD_WriteReg(0x0012, 0x011C);//0x011A   why need to set several times?
        LCD_WriteReg(0x00A4, 0x0001);//NVM
        LCD_WriteReg(0x0008, 0x000F);
        LCD_WriteReg(0x000A, 0x0008);
        LCD_WriteReg(0x000D, 0x0008);

        LCD_WriteReg(0x0030, 0x0707);
        LCD_WriteReg(0x0031, 0x0007); //0x0707
        LCD_WriteReg(0x0032, 0x0603);
        LCD_WriteReg(0x0033, 0x0700);
        LCD_WriteReg(0x0034, 0x0202);
        LCD_WriteReg(0x0035, 0x0002); //?0x0606
        LCD_WriteReg(0x0036, 0x1F0F);
        LCD_WriteReg(0x0037, 0x0707); //0x0f0f  0x0105
        LCD_WriteReg(0x0038, 0x0000);
        LCD_WriteReg(0x0039, 0x0000);
        LCD_WriteReg(0x003A, 0x0707);
        LCD_WriteReg(0x003B, 0x0000); //0x0303
        LCD_WriteReg(0x003C, 0x0007); //?0x0707
        LCD_WriteReg(0x003D, 0x0000); //0x1313//0x1f08
        Delay_Ms(50);
        LCD_WriteReg(0x0007, 0x0001);
        LCD_WriteReg(0x0017, 0x0001);
        Delay_Ms(50);
    
        LCD_WriteReg(0x0010, 0x17A0);
        LCD_WriteReg(0x0011, 0x0217);//reference voltage VC[2:0]   Vciout = 1.00*Vcivl
        LCD_WriteReg(0x0012, 0x011E);//0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?
        LCD_WriteReg(0x0013, 0x0F00);//VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
        LCD_WriteReg(0x002A, 0x0000);
        LCD_WriteReg(0x0029, 0x000A);//0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage??
        LCD_WriteReg(0x0012, 0x013E);// 0x013C  power supply on
        //Coordinates Control//
        LCD_WriteReg(0x0050, 0x0000);//0x0e00
        LCD_WriteReg(0x0051, 0x00EF);
        LCD_WriteReg(0x0052, 0x0000);
        LCD_WriteReg(0x0053, 0x013F);
        //Pannel Image Control//
        LCD_WriteReg(0x0060, 0x2700);
        LCD_WriteReg(0x0061, 0x0001);
        LCD_WriteReg(0x006A, 0x0000);
        LCD_WriteReg(0x0080, 0x0000);
        //Partial Image Control//
        LCD_WriteReg(0x0081, 0x0000);
        LCD_WriteReg(0x0082, 0x0000);
        LCD_WriteReg(0x0083, 0x0000);
        LCD_WriteReg(0x0084, 0x0000);
        LCD_WriteReg(0x0085, 0x0000);
        //Panel Interface Control//
        LCD_WriteReg(0x0090, 0x0013);//0x0010 frenqucy
        LCD_WriteReg(0x0092, 0x0300);
        LCD_WriteReg(0x0093, 0x0005);
        LCD_WriteReg(0x0095, 0x0000);
        LCD_WriteReg(0x0097, 0x0000);
        LCD_WriteReg(0x0098, 0x0000);

        LCD_WriteReg(0x0001, 0x0100);
        LCD_WriteReg(0x0002, 0x0700);
        LCD_WriteReg(0x0003, 0x1038);
        LCD_WriteReg(0x0004, 0x0000);
        LCD_WriteReg(0x000C, 0x0000);
        LCD_WriteReg(0x000F, 0x0000);
        LCD_WriteReg(0x0020, 0x0000);
        LCD_WriteReg(0x0021, 0x0000);
        LCD_WriteReg(0x0007, 0x0021);
        Delay_Ms(20);
        LCD_WriteReg(0x0007, 0x0061);
        Delay_Ms(20);
        LCD_WriteReg(0x0007, 0x0173);
        Delay_Ms(20);
    }
    else if(lcddev.id == 0xB505)
    {
        LCD_WriteReg(0x0000, 0x0000);
        LCD_WriteReg(0x0000, 0x0000);
        LCD_WriteReg(0x0000, 0x0000);
        LCD_WriteReg(0x0000, 0x0000);

        LCD_WriteReg(0x00a4, 0x0001);
        Delay_Ms(20);
        LCD_WriteReg(0x0060, 0x2700);
        LCD_WriteReg(0x0008, 0x0202);

        LCD_WriteReg(0x0030, 0x0214);
        LCD_WriteReg(0x0031, 0x3715);
        LCD_WriteReg(0x0032, 0x0604);
        LCD_WriteReg(0x0033, 0x0e16);
        LCD_WriteReg(0x0034, 0x2211);
        LCD_WriteReg(0x0035, 0x1500);
        LCD_WriteReg(0x0036, 0x8507);
        LCD_WriteReg(0x0037, 0x1407);
        LCD_WriteReg(0x0038, 0x1403);
        LCD_WriteReg(0x0039, 0x0020);

        LCD_WriteReg(0x0090, 0x001a);
        LCD_WriteReg(0x0010, 0x0000);
        LCD_WriteReg(0x0011, 0x0007);
        LCD_WriteReg(0x0012, 0x0000);
        LCD_WriteReg(0x0013, 0x0000);
        Delay_Ms(20);

        LCD_WriteReg(0x0010, 0x0730);
        LCD_WriteReg(0x0011, 0x0137);
        Delay_Ms(20);

        LCD_WriteReg(0x0012, 0x01b8);
        Delay_Ms(20);

        LCD_WriteReg(0x0013, 0x0f00);
        LCD_WriteReg(0x002a, 0x0080);
        LCD_WriteReg(0x0029, 0x0048);
        Delay_Ms(20);

        LCD_WriteReg(0x0001, 0x0100);
        LCD_WriteReg(0x0002, 0x0700);
        LCD_WriteReg(0x0003, 0x1038);
        LCD_WriteReg(0x0008, 0x0202);
        LCD_WriteReg(0x000a, 0x0000);
        LCD_WriteReg(0x000c, 0x0000);
        LCD_WriteReg(0x000d, 0x0000);
        LCD_WriteReg(0x000e, 0x0030);
        LCD_WriteReg(0x0050, 0x0000);
        LCD_WriteReg(0x0051, 0x00ef);
        LCD_WriteReg(0x0052, 0x0000);
        LCD_WriteReg(0x0053, 0x013f);
        LCD_WriteReg(0x0060, 0x2700);
        LCD_WriteReg(0x0061, 0x0001);
        LCD_WriteReg(0x006a, 0x0000);
        //LCD_WriteReg(0x0080, 0x0000);
        //LCD_WriteReg(0x0081, 0x0000);
        LCD_WriteReg(0x0090, 0X0011);
        LCD_WriteReg(0x0092, 0x0600);
        LCD_WriteReg(0x0093, 0x0402);
        LCD_WriteReg(0x0094, 0x0002);
        Delay_Ms(20);

        LCD_WriteReg(0x0007, 0x0001);
        Delay_Ms(20);
        LCD_WriteReg(0x0007, 0x0061);
        LCD_WriteReg(0x0007, 0x0173);

        LCD_WriteReg(0x0020, 0x0000);
        LCD_WriteReg(0x0021, 0x0000);
        LCD_WriteReg(0x00, 0x22);
    }
    else if(lcddev.id==0xC505)
    {
        LCD_WriteReg(0x0000, 0x0000);
        LCD_WriteReg(0x0000, 0x0000);
        Delay_Ms(20);
        LCD_WriteReg(0x0000, 0x0000);
        LCD_WriteReg(0x0000, 0x0000);
        LCD_WriteReg(0x0000, 0x0000);
        LCD_WriteReg(0x0000, 0x0000);
        LCD_WriteReg(0x00a4, 0x0001);
        Delay_Ms(20);
        LCD_WriteReg(0x0060, 0x2700);
        LCD_WriteReg(0x0008, 0x0806);

        LCD_WriteReg(0x0030, 0x0703);//gamma setting
        LCD_WriteReg(0x0031, 0x0001);
        LCD_WriteReg(0x0032, 0x0004);
        LCD_WriteReg(0x0033, 0x0102);
        LCD_WriteReg(0x0034, 0x0300);
        LCD_WriteReg(0x0035, 0x0103);
        LCD_WriteReg(0x0036, 0x001F);
        LCD_WriteReg(0x0037, 0x0703);
        LCD_WriteReg(0x0038, 0x0001);
        LCD_WriteReg(0x0039, 0x0004);

        LCD_WriteReg(0x0090, 0x0015);   //80Hz
        LCD_WriteReg(0x0010, 0X0410);   //BT,AP
        LCD_WriteReg(0x0011, 0x0247);   //DC1,DC0,VC
        LCD_WriteReg(0x0012, 0x01BC);
        LCD_WriteReg(0x0013, 0x0e00);
        Delay_Ms(120);
        LCD_WriteReg(0x0001, 0x0100);
        LCD_WriteReg(0x0002, 0x0200);
        LCD_WriteReg(0x0003, 0x1030);

        LCD_WriteReg(0x000A, 0x0008);
        LCD_WriteReg(0x000C, 0x0000);

        LCD_WriteReg(0x000E, 0x0020);
        LCD_WriteReg(0x000F, 0x0000);
        LCD_WriteReg(0x0020, 0x0000);   //H Start
        LCD_WriteReg(0x0021, 0x0000);   //V Start
        LCD_WriteReg(0x002A, 0x003D);   //vcom2
        Delay_Ms(20);
        LCD_WriteReg(0x0029, 0x002d);
        LCD_WriteReg(0x0050, 0x0000);
        LCD_WriteReg(0x0051, 0xD0EF);
        LCD_WriteReg(0x0052, 0x0000);
        LCD_WriteReg(0x0053, 0x013F);
        LCD_WriteReg(0x0061, 0x0000);
        LCD_WriteReg(0x006A, 0x0000);
        LCD_WriteReg(0x0092, 0x0300);

        LCD_WriteReg(0x0093, 0x0005);
        LCD_WriteReg(0x0007, 0x0100);
    }
    else if(lcddev.id==0x4531)//OK 
    {
        LCD_WriteReg(0X00, 0X0001);
        Delay_Ms(10);
        LCD_WriteReg(0X10, 0X1628);
        LCD_WriteReg(0X12, 0X000e);//0x0006
        LCD_WriteReg(0X13, 0X0A39);
        Delay_Ms(10);
        LCD_WriteReg(0X11, 0X0040);
        LCD_WriteReg(0X15, 0X0050);
        Delay_Ms(10);
        LCD_WriteReg(0X12, 0X001e);//16
        Delay_Ms(10);
        LCD_WriteReg(0X10, 0X1620);
        LCD_WriteReg(0X13, 0X2A39);
        Delay_Ms(10);
        LCD_WriteReg(0X01, 0X0100);
        LCD_WriteReg(0X02, 0X0300);
        LCD_WriteReg(0X03, 0X1038);
        LCD_WriteReg(0X08, 0X0202);
        LCD_WriteReg(0X0A, 0X0008);
        LCD_WriteReg(0X30, 0X0000);
        LCD_WriteReg(0X31, 0X0402);
        LCD_WriteReg(0X32, 0X0106);
        LCD_WriteReg(0X33, 0X0503);
        LCD_WriteReg(0X34, 0X0104);
        LCD_WriteReg(0X35, 0X0301);
        LCD_WriteReg(0X36, 0X0707);
        LCD_WriteReg(0X37, 0X0305);
        LCD_WriteReg(0X38, 0X0208);
        LCD_WriteReg(0X39, 0X0F0B);
        LCD_WriteReg(0X41, 0X0002);
        LCD_WriteReg(0X60, 0X2700);
        LCD_WriteReg(0X61, 0X0001);
        LCD_WriteReg(0X90, 0X0210);
        LCD_WriteReg(0X92, 0X010A);
        LCD_WriteReg(0X93, 0X0004);
        LCD_WriteReg(0XA0, 0X0100);
        LCD_WriteReg(0X07, 0X0001);
        LCD_WriteReg(0X07, 0X0021);
        LCD_WriteReg(0X07, 0X0023);
        LCD_WriteReg(0X07, 0X0033);
        LCD_WriteReg(0X07, 0X0133);
        LCD_WriteReg(0XA0, 0X0000);
    }
    else if(lcddev.id==0x4535)
    {
        LCD_WriteReg(0X15, 0X0030);
        LCD_WriteReg(0X9A, 0X0010);
        LCD_WriteReg(0X11, 0X0020);
        LCD_WriteReg(0X10, 0X3428);
        LCD_WriteReg(0X12, 0X0002);//16
        LCD_WriteReg(0X13, 0X1038);
        Delay_Ms(40);
        LCD_WriteReg(0X12, 0X0012);//16
        Delay_Ms(40);
        LCD_WriteReg(0X10, 0X3420);
        LCD_WriteReg(0X13, 0X3038);
        Delay_Ms(70);
        LCD_WriteReg(0X30, 0X0000);
        LCD_WriteReg(0X31, 0X0402);
        LCD_WriteReg(0X32, 0X0307);
        LCD_WriteReg(0X33, 0X0304);
        LCD_WriteReg(0X34, 0X0004);
        LCD_WriteReg(0X35, 0X0401);
        LCD_WriteReg(0X36, 0X0707);
        LCD_WriteReg(0X37, 0X0305);
        LCD_WriteReg(0X38, 0X0610);
        LCD_WriteReg(0X39, 0X0610);

        LCD_WriteReg(0X01, 0X0100);
        LCD_WriteReg(0X02, 0X0300);
        LCD_WriteReg(0X03, 0X1030);
        LCD_WriteReg(0X08, 0X0808);
        LCD_WriteReg(0X0A, 0X0008);
        LCD_WriteReg(0X60, 0X2700);
        LCD_WriteReg(0X61, 0X0001);
        LCD_WriteReg(0X90, 0X013E);
        LCD_WriteReg(0X92, 0X0100);
        LCD_WriteReg(0X93, 0X0100);
        LCD_WriteReg(0XA0, 0X3000);
        LCD_WriteReg(0XA3, 0X0010);
        LCD_WriteReg(0X07, 0X0001);
        LCD_WriteReg(0X07, 0X0021);
        LCD_WriteReg(0X07, 0X0023);
        LCD_WriteReg(0X07, 0X0033);
        LCD_WriteReg(0X07, 0X0133);
    }
    else if(lcddev.id==0X1963)
    {
        LCD_WR_REG(0xE2);       //Set PLL with OSC = 10MHz (hardware),  Multiplier N = 35, 250MHz < VCO < 800MHz = OSC*(N+1), VCO = 360MHz
        LCD_WR_DATA(0x23);      
        LCD_WR_DATA(0x02);      
        LCD_WR_DATA(0x04);     
        Delay_Us(100);
        LCD_WR_REG(0xE0);       // Start PLL command
        LCD_WR_DATA(0x01);      // enable PLL
        Delay_Ms(10);
        LCD_WR_REG(0xE0);       // Start PLL command again
        LCD_WR_DATA(0x03);      // now, use PLL output as system clock
        Delay_Ms(12);
        LCD_WR_REG(0x01);       
        Delay_Ms(10);

        LCD_WR_REG(0xE6);       
        LCD_WR_DATA(0x03);
        LCD_WR_DATA(0xFF);
        LCD_WR_DATA(0xFF);

        LCD_WR_REG(0xB0);       
        LCD_WR_DATA(0x20);      
        LCD_WR_DATA(0x00);      

        LCD_WR_DATA((SSD_HOR_RESOLUTION-1)>>8);
        LCD_WR_DATA(SSD_HOR_RESOLUTION-1);
        LCD_WR_DATA((SSD_VER_RESOLUTION-1)>>8);
        LCD_WR_DATA(SSD_VER_RESOLUTION-1);
        LCD_WR_DATA(0x00);      

        LCD_WR_REG(0xB4);       //Set horizontal period
        LCD_WR_DATA((SSD_HT-1)>>8);
        LCD_WR_DATA(SSD_HT-1);
        LCD_WR_DATA((SSD_HPS-1)>>8);
        LCD_WR_DATA(SSD_HPS-1);
        LCD_WR_DATA(SSD_HOR_PULSE_WIDTH-1);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0xB6);       //Set vertical period
        LCD_WR_DATA((SSD_VT-1)>>8);
        LCD_WR_DATA(SSD_VT-1);
        LCD_WR_DATA((SSD_VPS-1)>>8);
        LCD_WR_DATA(SSD_VPS-1);
        LCD_WR_DATA(SSD_VER_FRONT_PORCH-1);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xF0);   
        LCD_WR_DATA(0x03);  //16-bit(565 format) data for 16bpp

        LCD_WR_REG(0x29);   
       
        LCD_WR_REG(0xD0);   
        LCD_WR_DATA(0x00);  //disable

        LCD_WR_REG(0xBE);   
        LCD_WR_DATA(0x05);  
        LCD_WR_DATA(0xFE);  
        LCD_WR_DATA(0x01);  
        LCD_WR_DATA(0xFF);  
        LCD_WR_DATA(0x00); 

        LCD_WR_REG(0xB8);   
        LCD_WR_DATA(0x0F);  
        LCD_WR_DATA(0x01);  
        LCD_WR_REG(0xBA);
        LCD_WR_DATA(0X01);  

        LCD_SSD_BackLightSet(100);
    }
    LCD_Display_Dir(0);                  
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
    LCD_Clear(YELLOW);

}
/*********************************************************************
 * @fn      LCD_SetXY
 *
 * @brief   Set the start point of LCD display
 *
 * @param    x - X coordinate
 *           y - Y coordinate
 *
 * @return  none
 */

void LCD_SetXY(u16 x, u16 y)
{
    LCD_SetCursor(x, y);
}

/*********************************************************************
 * @fn      Gui_DrawPoint
 *
 * @brief   Draw a dot
 *
 * @param    x - X coordinate
 *           y - Y coordinate
 *           color - color
 * @return  none
 */

void Gui_DrawPoint(u16 x, u16 y, u16 color)
{

    if (lcddev.id == 0X1963)
    {
        if (lcddev.dir == 0)
            x = lcddev.width - 1 - x;
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }
    else
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }
    LCD->LCD_REG = lcddev.wramcmd;
    LCD->LCD_RAM = color;
}

/*********************************************************************
 * @fn      Gui_DrawPoint
 *
 * @brief   clear screen 
 *
 * @param   color - Filling color
 *           
 * @return  none
 */

void LCD_Clear(u16 color)
{
    u32 index = 0;
    u32 totalpoint = lcddev.width;
    totalpoint *= lcddev.height;    

    LCD_SetCursor(0x00, 0x0000);    
    LCD_WriteRAM_Prepare();         

    for (index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM=color;
    }
}

/*********************************************************************
 * @fn      LCD_Fill
 *
 * @brief   Fill color in designated area
 *
 * @param   xsta,ysta - Starting coordinates
 *          xend,yend - Termination coordinates
 *          color     - The color to be filled
 *
 * @return  none
 */

void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{

    u16 i, j;
    u16 xlen = 0;

    xlen = ex - sx + 1;

    for (i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);       
        LCD_WriteRAM_Prepare();     

        for (j = 0; j < xlen; j++)
        {
            LCD->LCD_RAM=color;     
        }
    }
}
/*********************************************************************
 * @fn      LCD_DrawLine
 *
 * @brief   Draw a line
 *
 * @param   x1,y1 - Starting coordinates
 *          x2,y2 - Termination coordinates
 *          color - Line color
 *
 * @return  none
 */
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;             
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0)incx = 1;      
    else if (delta_x == 0)incx = 0; 
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; 
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x; 
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )    
    {
        LCD_DrawPoint(uRow, uCol); 
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/*********************************************************************
 * @fn      LCD_DrawRectangle
 *
 * @brief   Draw a rectangle
 *
 * @param   x1,y1 - Starting coordinates
 *          x2,y2 - Termination coordinates
 *         
 *
 * @return  none
 */
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

/*********************************************************************
 * @fn      Draw_Circle
 *
 * @brief   Draw a circle
 *
 * @param   x0,y0 - Center coordinates
 *          r     - radius
 *          
 *
 * @return  none
 */
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);             
    while(a <= b)
    {
        LCD_DrawPoint(x0 + a, y0 - b);             //5
        LCD_DrawPoint(x0 + b, y0 - a);             //0
        LCD_DrawPoint(x0 + b, y0 + a);             //4
        LCD_DrawPoint(x0 + a, y0 + b);             //6
        LCD_DrawPoint(x0 - a, y0 + b);             //1
        LCD_DrawPoint(x0 - b, y0 + a);
        LCD_DrawPoint(x0 - a, y0 - b);             //2
        LCD_DrawPoint(x0 - b, y0 - a);             //7
        a++;

        if(di < 0)
            di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}
/*********************************************************************
 * @fn      LCD_ShowString
 *
 * @brief   Display string
 *
 * @param   x   - X coordinate
 *          y   - Y coordinate
 *          num -Characters to be displayed
 *          size- font size
 *          mode- Overlapping method
 *
 * @return  none
 */

void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
    u8 temp, t1, t;
    u16 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);      
    num = num - ' ';
    for(t = 0; t < csize; t++)
    {
        if(size == 12)
            temp = asc2_1206[num][t];       
        else if(size == 16)
            temp = asc2_1608[num][t];   
        else if(size == 24)
            temp = asc2_2412[num][t];   
        else
            return;                             
        for(t1 = 0; t1 < 8; t1++)
        {
            if(temp & 0x80)
                LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if(mode==0)
                LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            temp <<= 1;
            y++;
            if(y >= lcddev.height)
                return;     
            if((y - y0) == size)
            {
                y = y0;
                x++;
                if(x >= lcddev.width)
                    return; 
                break;
            }
        }
    }
}

/*********************************************************************
 * @fn      LCD_Pow
 *
 * @brief   m^n function
 *
 * @return  result - m^n
 */
u32 LCD_Pow(u8 m, u8 n)
{
    u32 result = 1;
    while(n--)
        result *= m;
    return result;
}

/*********************************************************************
 * @fn      LCD_ShowNum
 *
 * @brief   Display number
 *
 * @param   x - X coordinate
 *          y - Y coordinate
 *          len - number lenth
 *          size -font size
 *          color - font color
 *          num - value(0~4294967295)
 *
 * @return  none
 */
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;
    for(t = 0;t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                LCD_ShowChar(x + (size / 2) * t, y, ' ', size, 0);
                continue;
            }
            else
                enshow = 1;

        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, 0);
    }
}
/*********************************************************************
 * @fn      LCD_ShowNum
 *
 * @brief   Display number
 *
 * @param   x - X coordinate
 *          y - Y coordinate
 *          len - number lenth
 *          size -font size
 *          color - font color
 *          num - value(0~4294967295)
 *          mode[7]-0, no fill; 1, fill 0. [6:1]: keep, [0]: 0, no overlay display; , overlay display.
 *
 * @return  none
 */

void LCD_ShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;
    for(t = 0;t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                if(mode & 0X80)
                    LCD_ShowChar(x + (size / 2) * t, y, '0', size, mode & 0X01);
                else
                    LCD_ShowChar(x + (size / 2) * t, y, ' ', size, mode & 0X01);
                continue;
            }
            else
                enshow = 1;

        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0X01);
    }
}

/*********************************************************************
 * @fn      LCD_ShowString
 *
 * @brief   Display string
 *
 * @param   x - X coordinate
 *          y - Y coordinate
 *          width - String width
 *          height - String height
 *          size - font size
 *          *p - String start address
 *
 * @return  none
 */
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
    u8 x0 = x;
    width += x;
    height += y;
    while((*p <= '~') && (*p >= ' '))
    {
        if(x >= width)
        {
            x = x0;
            y += size;
        }
        if(y >= height)
            break;
        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}
