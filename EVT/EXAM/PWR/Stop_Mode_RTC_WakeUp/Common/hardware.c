/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/10/14
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

vu32 tmp = 0,cfg = 0;

/* definition of execute with WFI or WFE */
#define  Entry_WFI   0
#define  Entry_WFE   1
#define  Entry_Method  Entry_WFI
// #define  Entry_Method  Entry_WFE


/*********************************************************************
 * @fn      RTC_Alarm_WakeUp
 *
 * @brief  Initializes RTC alarm wake-up function.
 *
 * @return  none
 */
void RTC_Alarm_WakeUp(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR | RCC_HB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_ClearITPendingBit(RTC_IT_ALR);
    RTC_ClearITPendingBit(RTC_IT_SEC);
    /* Enable LSI oscillator */
    RCC_LSICmd(ENABLE);
    
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    __asm("fence");
    /* Enable the RTC clock */
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForLastTask();
    RTC_WaitForSynchro();
    /* Enable alarm interrupt */
    RTC_ITConfig( RTC_IT_ALR, ENABLE );
    RTC_WaitForLastTask();
    RTC_EnterConfigMode();
    /* Set the RTC prescaler value */
    RTC_SetPrescaler(32767);
    RTC_WaitForLastTask();
    RTC_SetCounter(0);
    RTC_WaitForLastTask();
    RTC_ExitConfigMode();

    /* Configure EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
#if(Entry_Method == Entry_WFI)
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#else
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
#endif
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

#if(Entry_Method == Entry_WFI)
    /* Enable the RTC alarm interrupt */
    NVIC_SetPriority(RTCAlarm_IRQn, 1);
    NVIC_EnableIRQ(RTCAlarm_IRQn);
#endif
}

/*********************************************************************
 * @fn      HSIAsSystemSource
 *
 * @brief   Select HSI as the clock source
 *
 * @return  none
 */
void HSIAsSystemSource(void)
{
	RCC->CTLR |= (uint32_t)0x00000001;
	RCC->CFGR0 &= (uint32_t)0x305C0000;
	while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x00);
	RCC->CFGR0 &= (uint32_t)0xFFBFFFFF;

	RCC->PLLCFGR &= (uint32_t)0x7FFFFFFF;

	RCC->CTLR &= (uint32_t)0x6AA6FFFF;
	RCC->CTLR &= (uint32_t)0xFFFBFFFF;

	RCC->PLLCFGR &= (uint32_t)0x0FFC0000;
	RCC->PLLCFGR |= (uint32_t)0x00000004;

	RCC->INTR = 0x00FF0000;
	RCC->CFGR2 &= 0x0C600000;
	RCC->PLLCFGR2 &= 0xFFF0E080;
	RCC->PLLCFGR2 |= 0x00080020;
}

/*********************************************************************
 * @fn      PWR_STOPMode_LP_Cmd
 *
 * @brief   Reduce the VDDK voltage in stop mode with the voltage regulator 
 *          in low-power mode.
 *
 * @return  none
 */
void PWR_STOPMode_LP_Cmd(FunctionalState NewState)
{
    uint32_t id = 0;
    id = (((*(uint32_t *)0x1FFFF704) & (0x000000F0)) >> 4);
    if(NewState != DISABLE)
    {
        switch(id)
        {
            case 0:
            {
                cfg &= ~((0x7 << 17));
                cfg |= ((0x1 << 17));
                break;
            }
            case 1:
            {
                cfg &= ~((0x7 << 17) | (0x7 << 4));
                cfg |= ((0x1 << 4));
                break;
            }
            case 2:
            {
                cfg &= ~((0x7 << 17) | (0x7 << 4));
                break;
            }
            default:
            {
                break;
            }
        }
        *(vu32*)SYS_CFGR0_BASE = cfg;
    }
    else
    {
        *(vu32*)SYS_CFGR0_BASE = tmp;
        Delay_Us(200);
    }
}

/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
    tmp = *(vu32*)SYS_CFGR0_BASE;
    cfg = *(vu32*)SYS_CFGR0_BASE;

    RTC_Alarm_WakeUp();

#ifdef Core_V3F
    /* wait V5F sleep */
    while((NVIC->CSTAR[1] & 0xC000) == RESET);
#else
    /* wait V3F sleep */
    while((NVIC->CSTAR[0] & 0xC000) == RESET);
#endif

    while(1)
    {
        printf(">>>enter stop mode\r\n");
        Delay_Ms(10);

        /* Set the RTC alarm value */
        RTC_SetAlarm(RTC_GetCounter()+5);
        RTC_WaitForLastTask();
        /* The frequency needs to be reduced before lowering the VDDK */
        HSIAsSystemSource();
        /* In stop mode with the voltage regulator in low-power mode,to reduce power consumption, 
        VDDK needs to be lowered and it is recommended to turn off all peripherals before enter stop mode. */
        PWR_STOPMode_LP_Cmd(ENABLE);
        #if(Entry_Method == Entry_WFI)
            PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
        #else
            PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);
            /* After waking up, the VDDK needs to be restored first and stabilized before restoring the clock */
            PWR_STOPMode_LP_Cmd(DISABLE);
            SystemInit();
        #endif
        printf("wake up\r\n\r\n");
        Delay_Ms(2000);
    }
}

#if(Entry_Method == Entry_WFI)

#if Func_Run_V3F/* or Func_Run_V5F if V5F run */
/*********************************************************************
 * @fn      RTCAlarm_IRQHandler
 *
 * @brief   This function handles RTCAlarm Handler.
 *
 * @return  none
 */
void RTCAlarm_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void RTCAlarm_IRQHandler(void)
{
    if(RTC_GetFlagStatus(RTC_FLAG_ALR) != RESET)
    {
        /* After waking up, the VDDK needs to be restored first and stabilized before restoring the clock */
        PWR_STOPMode_LP_Cmd(DISABLE);
        SystemInit();
        /* Clears EXTI line and alarm clock flag */
        EXTI_ClearITPendingBit(EXTI_Line17);
        RTC_ClearITPendingBit(RTC_IT_ALR); 
        RTC_WaitForLastTask();
    }
}
#endif

#endif