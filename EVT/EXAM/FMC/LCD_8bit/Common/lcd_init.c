/********************************** (C) COPYRIGHT *******************************
* File Name          : lcd_init.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/04/03
* Description        : This file contains the headers of the LCD.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "lcd_init.h"
#include "debug.h"

/*********************************************************************
 * @fn      LCD_GPIO_Init
 *
 * @brief   Init LCD GPIO
 *
 * @param   none
 *
 * @return  none
 */

void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    FMC_NORSRAMInitTypeDef  FMC_NORSRAMInitStructure={0};
    FMC_NORSRAMTimingInitTypeDef  readWriteTiming={0};
    FMC_NORSRAMTimingInitTypeDef  writeTiming={0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_FMC,ENABLE);
	RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA|RCC_HB2Periph_GPIOB|RCC_HB2Periph_GPIOD|RCC_HB2Periph_GPIOE|RCC_HB2Periph_AFIO,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_12|GPIO_Pin_15);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_11|GPIO_Pin_14|GPIO_Pin_15;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
 	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF12 );
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF12 );
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF12 );
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF12 );

   


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
 	GPIO_Init(GPIOE, &GPIO_InitStructure); 
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF12 );
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF12 );


    readWriteTiming.FMC_AddressSetupTime = 0x00;
    readWriteTiming.FMC_AddressHoldTime = 0x01;
    readWriteTiming.FMC_DataSetupTime = 0x0f;
    readWriteTiming.FMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FMC_AccessMode = FMC_AccessMode_A;

    writeTiming.FMC_AddressSetupTime = 0x00;
    writeTiming.FMC_AddressHoldTime = 0x01;
    writeTiming.FMC_DataSetupTime = 0x03;
    writeTiming.FMC_BusTurnAroundDuration = 0x00;
    writeTiming.FMC_CLKDivision = 0x00;
    writeTiming.FMC_DataLatency = 0x00;
    writeTiming.FMC_AccessMode = FMC_AccessMode_A;

    FMC_NORSRAMInitStructure.FMC_Bank = FMC_Bank1_NORSRAM1;

    FMC_NORSRAMInitStructure.FMC_DataAddressMux = FMC_DataAddressMux_Disable;
    FMC_NORSRAMInitStructure.FMC_MemoryType =FMC_MemoryType_SRAM;
    FMC_NORSRAMInitStructure.FMC_MemoryDataWidth = FMC_MemoryDataWidth_8b;
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
 * @fn      LCD_RD_DATA8
 *
 * @brief   Read data
 *
 * @return  dat - read data
 */

u8 LCD_RD_DATA8(void)
{
    vu8 dat;
    dat=LCD->LCD_DAT;
    return dat;
}

/*********************************************************************
 * @fn      LCD_WR_DATA8
 *
 * @brief   Write data
 *
 * @param   dat
 *
 * @return  none
 */

void LCD_WR_DATA8(u8 dat)
{
    LCD->LCD_DAT=dat;
}

/*********************************************************************
 * @fn      LCD_WR_DATA
 *
 * @brief   Write 16 bit data
 *
 * @param   dat
 *
 * @return  none
 */

void LCD_WR_DATA(u16 dat)
{
    LCD->LCD_DAT=dat>>8;
    LCD->LCD_DAT=dat;
}

/*********************************************************************
 * @fn      LCD_WR_REG
 *
 * @brief   Write value to register
 *
 * @param   dat - value
 *
 * @return  none
 */

void LCD_WR_REG(vu8 dat)
{   
    dat=dat;
    LCD->LCD_REG=dat;
}

/*********************************************************************
 * @fn      LCD_Address_Set
 *
 * @brief   Set the start and end addresses
 *
 * @param   x1 - The starting address of the column
 *          x2 - The end address of the column
 *          y1 - The starting address of the line
 *          y2 - The end address of the line
 *
 * @return  none
 */

void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
    if(USE_HORIZONTAL==0)
	{
        LCD_WR_REG(0x2a);
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);
	}
	else
	{
		LCD_WR_REG(0x2a);
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);
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
    u16 ID;
    u8 id1,id2,id3;
    LCD_GPIO_Init();
	LCD_RES_Clr();
	Delay_Ms(100);
	LCD_RES_Set();
	Delay_Ms(100);
	LCD_BLK_Set();//Turn on the backlight
	Delay_Ms(100);
    LCD_WR_REG(0x04);
#ifdef Core_V3F
#elif defined(Core_V5F)
       __disable_irq();
#endif
    id1=*(vu8*)0x60010000;
    id1=*(vu8*)0x60010000;
    id2=*(vu8*)0x60010000;
    while(id1!=id2);
    id3 =*(vu8*)0x60010000;
    ID=(id1<<8)|id3;
#ifdef Core_V3F
#elif defined(Core_V5F)
     __enable_irq();
#endif  
   printf("LCDID:%X\r\n",ID);
   while(ID!=0x8552 ){};

    LCD_WR_REG(0x11);
    Delay_Ms(120);
    LCD_WR_REG(0x36);
    if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
    else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
    else LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x19);

    LCD_WR_REG(0xC0);
    LCD_WR_DATA8(0x2C);

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x12);

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20);

    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F);

    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2B);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x54);
    LCD_WR_DATA8(0x4C);
    LCD_WR_DATA8(0x18);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x0B);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x23);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2C);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x44);
    LCD_WR_DATA8(0x51);
    LCD_WR_DATA8(0x2F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x20);
    LCD_WR_DATA8(0x23);

    LCD_WR_REG(0x21);

    LCD_WR_REG(0x29);
}







