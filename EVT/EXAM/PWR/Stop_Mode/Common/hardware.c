/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the CRC firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

vu32 tmp = 0,cfg = 0;

/* Execute with WFI or WFE Definition */
#define  Enter_WFI   0
#define  Enter_WFE   1

#define  Enter_MODE  Enter_WFI
// #define  Enter_MODE  Enter_WFE

/*********************************************************************
 * @fn      EXTI_INT_INIT
 *
 * @brief   Initializes EXTI line collection.
 *
 * @return  none
 */
void EXTI_INT_INIT(void)
{
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

#ifdef Core_V3F
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* GPIOB ----> EXTI_Line1 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
#if(Enter_MODE == Enter_WFI)
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#else
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
#endif
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
  
#if(Enter_MODE == Enter_WFI)
    NVIC_EnableIRQ(EXTI7_0_IRQn);
    NVIC_SetPriority(EXTI7_0_IRQn, 2);
#else
    /* Before executing WFE, it's necessary to set up the corresponding event line */
    __WFE_IDSET(EXTI_Line1);
#endif

#else
  	RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* GPIOA ----> EXTI_Line11 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
#if(Enter_MODE == Enter_WFI)
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#else
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
#endif
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

#if(Enter_MODE == Enter_WFI)
    NVIC_EnableIRQ(EXTI15_8_IRQn);
    NVIC_SetPriority(EXTI15_8_IRQn, 3);
#else
    /* Before executing WFE, it's necessary to set up the corresponding event line */
    __WFE_IDSET(EXTI_Line11);
#endif

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
 * @brief   Reduce the VDDK voltage in the stop lowpower mode.
 *
 * @return  none
 */
void PWR_STOPMode_LP_Cmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
      cfg &= ~((0x7 << 17) | (0x7 << 4));
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

    EXTI_INT_INIT();

    printf("sleep...\r\n");
    Delay_Ms(10);
#if ((Run_Core == Run_Core_V3FandV5F) || (Run_Core == Run_Core_V5F))
#ifdef Core_V5F 
        __asm("fence");
        /* wait V3 sleep */
        while((NVIC->CSTAR[0] & 0xC000) == RESET);
        /* The frequency needs to be reduced before lowering the VDDK */
        HSIAsSystemSource();
        /* In stop lowpower mode,to reduce power consumption, VDDK needs to be lowered 
		to 0.9V before stop, And it is recommended to turn off all peripherals. */
        PWR_STOPMode_LP_Cmd(ENABLE);
#endif
#else
        /* The frequency needs to be reduced before lowering the VDDK */
        HSIAsSystemSource();
        /* In stop lowpower mode,to reduce power consumption, VDDK needs to be lowered 
		to 0.9V before stop, And it is recommended to turn off all peripherals. */
        PWR_STOPMode_LP_Cmd(ENABLE);
#endif

#if(Enter_MODE == Enter_WFI)
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
#else
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);
    if((NVIC->CSTAR[0] & 0xC000) != RESET || (NVIC->CSTAR[1] & 0xC000) != RESET)
    {
        PWR_STOPMode_LP_Cmd(DISABLE);
        SystemInit();
    }
    printf("wake up!!!\r\n");
#endif
    
}

#if(Enter_MODE == Enter_WFI)
void EXTI15_8_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      EXTI15_8_IRQHandler
 *
 * @brief   This function handles EXTI15_8 exception.
 *
 * @return  none
 */
void EXTI15_8_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line11)!=RESET)
  {
    if((NVIC->CSTAR[0] & 0xC000) != RESET) /* the state of V3F */
    {
      /* After waking up, the VDDK needs to be restored first and stabilized before restoring the clock */
      PWR_STOPMode_LP_Cmd(DISABLE);
      SystemInit();
    }
	  printf("V5F EXTI11 Wake_up\r\n");
    EXTI_ClearITPendingBit(EXTI_Line11);
  }
} 

void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      EXTI7_0_IRQHandler
 *
 * @brief   This function handles EXTI7_0 exception.
 *
 * @return  none
 */
void EXTI7_0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1)!=RESET)
  {
    if((NVIC->CSTAR[1] & 0xC000) != RESET) /* the state of V5F */
    {
      /* After waking up, the VDDK needs to be restored first and stabilized before restoring the clock */
      PWR_STOPMode_LP_Cmd(DISABLE);
      SystemInit(); 
    }
    printf("V3F EXTI0 Wake_up\r\n");
    EXTI_ClearITPendingBit(EXTI_Line1);
  }
} 
#endif