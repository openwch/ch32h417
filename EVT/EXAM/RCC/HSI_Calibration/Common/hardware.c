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
#define ADJUST_STEP 17000
/* Global Variable */

/* Exported_Functions */
volatile uint8_t SampleFlag = 0;
volatile uint32_t rtc_cnt = 0;

void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler()
{
    rtc_cnt = RTC_GetCounter();
    SampleFlag = 1;
    RTC_SetCounter(0);
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

/*********************************************************************
 * @fn      RTC_Init
 *
 * @brief   Initializes RTC collection.
 *
 * @return  1 - Init Fail
 *          0 - Init Success
 */
u8 RTC_Init(void)
{
    u8 temp = 0;
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR | RCC_HB1Periph_BKP, ENABLE);   
    PWR_BackupAccessCmd(ENABLE);
    /* Is it the first configuration */

    RCC_HSEConfig(RCC_HSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET && temp < 250)
    {
        temp++;
        Delay_Ms(20);

    }
    if (temp >= 250)
        return 1;
    RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div512);

    #if (Run_Core != Run_Core_V3F)

    for(int temp1=0;temp1<10;temp1++)
    {
        __NOP();

    }

    #endif
     RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForLastTask();
    RTC_WaitForSynchro();
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    RTC_WaitForLastTask();

    RTC_EnterConfigMode();
    RTC_SetPrescaler(0);
    RTC_WaitForLastTask();
    RTC_ExitConfigMode();

    return 0;
}

/*********************************************************************
 * @fn      TIM1_Base_Init
 *
 * @brief   Initializes TIM1 output compare.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_Base_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

	NVIC_SetPriority(TIM1_UP_IRQn, 0);
    NVIC_EnableIRQ(TIM1_UP_IRQn);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    RTC_SetCounter(0);

    TIM_Cmd(TIM1, ENABLE);
}


/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
   printf("RTC Test\r\n");

    /* The above code is configuring the GPIO pin PB0 as an alternate function push-pull output with a
    maximum speed of 50MHz. It also enables the clock for GPIOA and configures the MCO
    (Microcontroller Clock Output) to use the HSI (High-Speed Internal) clock as the source. */
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB|RCC_HB2Periph_AFIO, ENABLE);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF0);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_MCOConfig(RCC_MCO_HSI);

    /* The above code is initializing the TIM1 timer and the RTC (Real-Time Clock) module. It then
    waits for a calibration flag to be set. Once the flag is set, it prints the calibration value. */
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);
    printf("TIM1 Freq: %d\r\n", RCC_Clocks.HCLK_Frequency);
    RTC_Init();

    Delay_Ms(100);
    TIM1_Base_Init(10000, RCC_Clocks.HCLK_Frequency / 100000 - 1);
    /* The code is performing a calibration of the HSI (High-Speed Internal) clock frequency. */
    static int last_Adjust_val = 16, Adjust_val = 16;
    static uint32_t Meas_Freq = 0, last_Meas_Freq = 0;
    printf("Primary adjust val: %d\r\n", last_Adjust_val);
    /* The code block is a do-while loop that performs a calibration of the HSI (High-Speed Internal)
    clock frequency. */
    do
    {
        if (SampleFlag)
        {
            SampleFlag = 0;
            Meas_Freq = (uint32_t)((float)HSI_VALUE * (float)(HSE_VALUE / 512 / 10) / (float)rtc_cnt);
            printf("HSI Freq: %d\r\n", Meas_Freq);
            Adjust_val = last_Adjust_val;
            last_Adjust_val += (int)(HSI_VALUE - (int)(Meas_Freq)) / ADJUST_STEP;
            last_Adjust_val &= 0x0000001f;
            printf("Adjust val: %d\r\n", last_Adjust_val);
            RCC_AdjustHSICalibrationValue(last_Adjust_val);
            Delay_Ms(100);
            last_Meas_Freq = (uint32_t)((float)HSI_VALUE * (float)(HSE_VALUE / 512 / 10) / (float)rtc_cnt);
            printf("Adjusted HSI Freq: %d\r\n", last_Meas_Freq);
        }
    } while ((int)(HSI_VALUE - (int)(last_Meas_Freq)) / ADJUST_STEP);


    if ((HSI_VALUE - (int)last_Meas_Freq) * (HSI_VALUE - (int)Meas_Freq) > 0)
    {

        while ((HSI_VALUE - (int)last_Meas_Freq) * (HSI_VALUE - (int)Meas_Freq) > 0)
        {
            SampleFlag = 0;
            while (!SampleFlag);
            Adjust_val = last_Adjust_val;
            Meas_Freq = last_Meas_Freq;
            last_Adjust_val - Adjust_val < 0 ? (last_Adjust_val -= 1) : (last_Adjust_val += 1);
            RCC_AdjustHSICalibrationValue(last_Adjust_val);
            Delay_Ms(100);
            last_Meas_Freq = (uint32_t)((float)HSI_VALUE * (float)(HSE_VALUE / 512 / 10) / (float)rtc_cnt);
        }
    }

    /* This code block is comparing the difference between the measured frequency (`last_Meas_Freq`)
    and the target frequency (`Meas_Freq`). It calculates the absolute difference between the two
    frequencies and checks if the difference of `last_Meas_Freq` is greater than the difference of
    `Meas_Freq`. */
    if (((HSI_VALUE - last_Meas_Freq) > 0 ? HSI_VALUE - last_Meas_Freq : last_Meas_Freq - HSI_VALUE) > ((HSI_VALUE - Meas_Freq) > 0 ? HSI_VALUE - Meas_Freq : Meas_Freq - HSI_VALUE))
    {
        last_Adjust_val = Adjust_val;
        RCC_AdjustHSICalibrationValue(last_Adjust_val);
    }

    while (1)
    {
        Delay_Ms(1000);
    }

}
