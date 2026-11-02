/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v3f.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/03/01
 * Description        : Main program body for V3F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 PB0 push-pull output.

*/

#include "debug.h"
#include "hardware.h"
#include "ch32h417.h"
#include <rtthread.h>
#include <rthw.h>
#include "drivers/pin.h"

/* Global typedef */

/* Global define */

/* LED0 is driven by the pin driver interface of rt  */
#define LED0_PIN  45   //PB0

/* Global Variable */

/* Test using the driver interface to operate the I/O port  */
int led(void)
{
    rt_uint8_t count;

    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    printf("led_SP:%08x\r\n",__get_SP());
    for(count = 0 ; count < 10 ;count++)
    {
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_kprintf("led on, count : %d\r\n", count);
        rt_thread_mdelay(500);

        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_kprintf("led off\r\n");
        rt_thread_mdelay(500);
    }
    return 0;
}
MSH_CMD_EXPORT(led,  led sample by using I/O drivers);
/* Global define */

/*********************************************************************
 * @fn      LED1_BLINK_INIT
 *
 * @brief   LED1 directly calls the underlying driver
 *
 * @return  none
 */
void LED1_BLINK_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
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
#if (Run_Core == Run_Core_V3FandV5F)
	NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
	HSEM_ITConfig(HSEM_ID0, ENABLE);
    NVIC->SCTLR |= 1<<4;
	RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR,ENABLE);
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
	HSEM_ClearFlag(HSEM_ID0);
	printf("V3F wake up\r\n");

    rt_kprintf("\r\n MCU: CH32H417\r\n");
	rt_kprintf(" SysClk: %dHz\r\n",SystemCoreClock);
    rt_kprintf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    rt_kprintf(" www.wch.cn\r\n");
	LED1_BLINK_INIT();
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);


	while(1)
	{
	    GPIO_SetBits(GPIOB,GPIO_Pin_1);
	    rt_thread_mdelay(500);
	    GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	    rt_thread_mdelay(500);
	}

#elif (Run_Core == Run_Core_V3F)
	
    rt_kprintf("\r\n MCU: CH32H417\r\n");
	rt_kprintf(" SysClk: %dHz\r\n",SystemCoreClock);
    rt_kprintf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    rt_kprintf(" www.wch.cn\r\n");
	LED1_BLINK_INIT();
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);


	while(1)
	{
	    GPIO_SetBits(GPIOB,GPIO_Pin_1);
	    rt_thread_mdelay(500);
	    GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	    rt_thread_mdelay(500);
	}


#elif (Run_Core == Run_Core_V5F)
	NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
	printf("V3F wake up\r\n");
#endif

	
}

