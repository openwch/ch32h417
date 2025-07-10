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


/* External Clock Mode Definition */
#define ETR_CLK_MODE1    0
#define ETR_CLK_MODE2    1

/*  External Clock Mode Selection */
#define ETR_CLK          ETR_CLK_MODE1
//#define ETR_CLK ETR_CLK_MODE2

/*********************************************************************
 * @fn      TIM1_ETRClockMode1_Init
 *
 * @brief   Configures the External clock Mode1.
 *
 * @return  none
 */
void TIM1_ETRClockMode1_Init(void)
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1|RCC_HB2Periph_AFIO, ENABLE);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF1);

    TIM_CounterModeConfig(TIM1, TIM_CounterMode_Up);
    TIM_SetAutoreload(TIM1, 0x1B);
    /* GPIOE11 Input as TIM Clock Source */
    TIM_TIxExternalClockConfig(TIM1, TIM_TIxExternalCLK1Source_TI2, TIM_ICPolarity_Rising, 0x00);
    TIM_Cmd(TIM1, ENABLE);
}

/*********************************************************************
 * @fn      TIM1_ETRClockMode2_Init
 *
 * @brief   Configures the External clock Mode2.
 *
 * @return  none
 */
void TIM1_ETRClockMode2_Init(void)
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1|RCC_HB2Periph_AFIO, ENABLE);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF1);

    TIM_CounterModeConfig(TIM1, TIM_CounterMode_Up);
    TIM_SetAutoreload(TIM1, 0x3B);
    /* GPIOA12 Input as TIM Clock Source */
    TIM_ETRClockMode2Config(TIM1, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_Inverted, 0x0);
    TIM_Cmd(TIM1, ENABLE);
}



/*********************************************************************
 * @fn      Hardware
 *
 * @return  none
 */
void Hardware(void)
{
	printf("Clock Select\r\n");
#if(ETR_CLK == ETR_CLK_MODE1)
    TIM1_ETRClockMode1_Init();

#elif(ETR_CLK == ETR_CLK_MODE2)
    TIM1_ETRClockMode2_Init();

#endif

    while(1)
    {
        if(TIM_GetFlagStatus(TIM1, TIM_FLAG_CC2) != RESET)
        {
            TIM_ClearFlag(TIM1, TIM_FLAG_CC2);
        }

        if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Update) != RESET)
        {
            printf("Count Update\r\n");

            TIM_ClearFlag(TIM1, TIM_FLAG_Update);
        }
    }

}
