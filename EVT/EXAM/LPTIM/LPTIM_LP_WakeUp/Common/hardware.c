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

/* Global define */
#define Sleep             1
#define Stop              2

#define EXWAKEUP          1
#define INWAKEUP          2

#define MODE            Sleep
//#define MODE          Stop

//#define WKMODE       EXWAKEUP
#define WKMODE        INWAKEUP

/* Global Variable */


void LPTIM1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void LPTIM1_IRQHandler(void)
{
   USART_Printf_Init(115200);
    if((LPTIM_GetFlagStatus(LPTIM1,LPTIM_FLAG_ARRM)==SET))
    {
         printf("----11wake up\r\n");
 
#if(MODE==Stop)
        SystemAndCoreClockUpdate();
        USART_Printf_Init(115200);
#endif
     }
     LPTIM_ClearFlag(LPTIM1, LPTIM_FLAG_ARRM);
}

/*********************************************************************
 * @fn      LPTIM_Init
 *
 * @brief   LPTIM_Init.
 *
 * @return  none
 */
void LPTIM_Init(u16 arr)
{

	EXTI_InitTypeDef EXTI_InitStructure = {0};
    LPTIM_TimeBaseInitTypeDef   LPTIM_TimeBaseInitStruct = {0};
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR | RCC_HB1Periph_BKP|RCC_HB1Periph_LPTIM1, ENABLE);    
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE|RCC_HB2Periph_GPIOD|RCC_HB2Periph_GPIOF|RCC_HB2Periph_AFIO, ENABLE);

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF1);

	EXTI_InitStructure.EXTI_Line = EXTI_Line23;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_SetPriority(LPTIM1_IRQn, 0);
    NVIC_EnableIRQ(LPTIM1_IRQn);

    LPTIM_Cmd(LPTIM1,ENABLE);

#if(WKMODE==INWAKEUP)
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)!=SET);
#endif

#if(WKMODE==EXWAKEUP)
    LPTIM_TimeBaseInitStruct.LPTIM_ClockSource = LPTIM_ClockSource_Ex;
    LPTIM_TimeBaseInitStruct.LPTIM_CountSource = LPTIM_CountSource_External;
    LPTIM_TimeBaseInitStruct.LPTIM_ClockPrescaler = LPTIM_TClockPrescaler_DIV8;
    LPTIM_TimeBaseInitStruct.LPTIM_InClockSource = LPTIM_InClockSource_PCLK1;
#elif(WKMODE==INWAKEUP)
    LPTIM_TimeBaseInitStruct.LPTIM_ClockSource = LPTIM_ClockSource_In;
    LPTIM_TimeBaseInitStruct.LPTIM_CountSource = LPTIM_CountSource_Internal;
    LPTIM_TimeBaseInitStruct.LPTIM_ClockPrescaler = LPTIM_TClockPrescaler_DIV128;
    LPTIM_TimeBaseInitStruct.LPTIM_InClockSource = LPTIM_InClockSource_LSI;
#endif
    LPTIM_TimeBaseInitStruct.LPTIM_ClockPolarity = LPTIM_ClockPolarity_Falling;
    LPTIM_TimeBaseInitStruct.LPTIM_ClockSampleTime = LPTIM_ClockSampleTime_0T;
    LPTIM_TimeBaseInitStruct.LPTIM_TriggerSampleTime = LPTIM_TriggerSampleTime_0T;
    LPTIM_TimeBaseInitStruct.LPTIM_ExTriggerPolarity = LPTIM_ExTriggerPolarity_Disable;
    LPTIM_TimeBaseInitStruct.LPTIM_TimeOut = ENABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_OutputPolarity = LPTIM_OutputPolarity_High;
    LPTIM_TimeBaseInitStruct.LPTIM_UpdateMode = LPTIM_UpdateMode0;
    LPTIM_TimeBaseInitStruct.LPTIM_Encoder = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_ForceOutHigh = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_SingleMode = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_ContinuousMode = ENABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_PWMOut = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_CounterDirIndicat = DISABLE;
    LPTIM_TimeBaseInitStruct.LPTIM_Pulse = 0;
    LPTIM_TimeBaseInitStruct.LPTIM_Period = arr;

    LPTIM_TimeBaseInit(LPTIM1, & LPTIM_TimeBaseInitStruct);
    LPTIM_ITConfig(LPTIM1,LPTIM_IT_ARRM, ENABLE);
}


/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
	printf("LPTIM\r\n");
#if(WKMODE==EXWAKEUP)
    LPTIM_Init(20);
#elif(WKMODE==INWAKEUP)
       LPTIM_Init(2000);

#endif

#if(MODE==Sleep)
    printf("Welcome to WCH\r\n");
         __WFI();
#elif(MODE==Stop)
   printf("Entry stop\r\n");
   PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
#endif
    
    printf("wake up\r\n");  
    while(1)
    {
        #if(WKMODE==EXWAKEUP)
        printf("Welcome to WCH\r\n");
        Delay_Ms(1000);
       #endif
    }

}
