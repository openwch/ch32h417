/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/03/01
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 PEC error check, master/slave mode transceiver routine:
 I2C2_SCL(PC0)\I2C2_SDA(PC1).
 This example demonstrates that the Master sends with PEC error checking, and the Slave receives.
 If a transmission error occurs, an error interrupt is triggered.
 Note: The two boards download the Master and Slave programs respectively, and power on at the same time.
     Hardware connection:
	 				PC0 -- PC0
					PC1 -- PC1

*/

#include "debug.h"
#include "hardware.h"

void GPIO_Init1()
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void delay_nop(uint32_t t)
{
	while(t--)
	{
		__NOP();
	}
}
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	SystemAndCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);
	printf("V5F SystemCoreClk:%d\r\n", SystemCoreClock);
GPIO_Init1();
	Delay_Ms(500);

#if (Run_Core == Run_Core_V3FandV5F)
	HSEM_FastTake(HSEM_ID0);
	HSEM_ReleaseOneSem(HSEM_ID0, 0);

#elif (Run_Core == Run_Core_V3F)

#elif (Run_Core == Run_Core_V5F)
	Hardware();
#endif


	while(1)
	{
		printf("V5F running...\r\n");
		Delay_Ms(1000);

		GPIO_ResetBits(GPIOB,GPIO_Pin_1);
		delay_nop(10000);
		GPIO_SetBits(GPIOB,GPIO_Pin_1);
		delay_nop(10000);
	}
}
