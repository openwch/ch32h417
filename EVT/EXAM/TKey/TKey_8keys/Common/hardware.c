/********************************** (C) COPYRIGHT  *******************************
 * File Name          : hardware.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/03/01
 * Description        : This file provides all the hardware firmware functions.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "hardware.h"
#include "wchtouch.h"

#define TKY_MEMHEAP_SIZE (TKY_MAX_QUEUE_NUM * TKY_BUFLEN)  // Externally defined data buffer length

#define TKY_PollForFilter() TKY_PollForFilterMode_3()

uint8_t volatile timerFlag = 0;
uint8_t volatile TKY_ScanFlag = 0;
uint8_t volatile print_time = 0;

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint32_t GPIO_Pin;
    uint32_t GPIO_CFG_MASK;
    uint32_t GPIO_CFG_OUT;
} TKY_CH_GPIO_S;

/*Sort gpio by touch channel hardware serial number*/
const TKY_CH_GPIO_S TKY_Pin[16] = {
    {.GPIOx = GPIOA, .GPIO_Pin = GPIO_Pin_0, .GPIO_CFG_MASK = 0xfffffff0, .GPIO_CFG_OUT = 0x00000001}, //  PA0,ADC0
    {.GPIOx = GPIOA, .GPIO_Pin = GPIO_Pin_1, .GPIO_CFG_MASK = 0xffffff0f, .GPIO_CFG_OUT = 0x00000010}, //  PA1,ADC1
    {.GPIOx = GPIOA, .GPIO_Pin = GPIO_Pin_2, .GPIO_CFG_MASK = 0xfffff0ff, .GPIO_CFG_OUT = 0x00000100}, //  PA2,ADC2
    {.GPIOx = GPIOA, .GPIO_Pin = GPIO_Pin_3, .GPIO_CFG_MASK = 0xffff0fff, .GPIO_CFG_OUT = 0x00001000}, //  PA3,ADC3
    {.GPIOx = GPIOA, .GPIO_Pin = GPIO_Pin_4, .GPIO_CFG_MASK = 0xfff0ffff, .GPIO_CFG_OUT = 0x00010000}, //  PA4,ADC4
    {.GPIOx = GPIOA, .GPIO_Pin = GPIO_Pin_5, .GPIO_CFG_MASK = 0xff0fffff, .GPIO_CFG_OUT = 0x00100000}, //  PA5,ADC5
    {.GPIOx = GPIOA, .GPIO_Pin = GPIO_Pin_6, .GPIO_CFG_MASK = 0xf0ffffff, .GPIO_CFG_OUT = 0x01000000}, //  PA6,ADC6
    {.GPIOx = GPIOA, .GPIO_Pin = GPIO_Pin_7, .GPIO_CFG_MASK = 0x0fffffff, .GPIO_CFG_OUT = 0x10000000}, //  PA7,ADC7

    {.GPIOx = GPIOB, .GPIO_Pin = GPIO_Pin_0, .GPIO_CFG_MASK = 0xfffffff0, .GPIO_CFG_OUT = 0x00000001}, //  PB0,ADC8
    {.GPIOx = GPIOB, .GPIO_Pin = GPIO_Pin_1, .GPIO_CFG_MASK = 0xffffff0f, .GPIO_CFG_OUT = 0x00000010}, //  PB1,ADC9

    {.GPIOx = GPIOC, .GPIO_Pin = GPIO_Pin_0, .GPIO_CFG_MASK = 0xfffffff0, .GPIO_CFG_OUT = 0x00000001}, //  PC0,ADC10
    {.GPIOx = GPIOC, .GPIO_Pin = GPIO_Pin_1, .GPIO_CFG_MASK = 0xffffff0f, .GPIO_CFG_OUT = 0x00000010}, //  PC1,ADC11
    {.GPIOx = GPIOC, .GPIO_Pin = GPIO_Pin_2, .GPIO_CFG_MASK = 0xfffff0ff, .GPIO_CFG_OUT = 0x00000100}, //  PC2,ADC12
    {.GPIOx = GPIOC, .GPIO_Pin = GPIO_Pin_3, .GPIO_CFG_MASK = 0xffff0fff, .GPIO_CFG_OUT = 0x00001000}, //  PC3,ADC13
    {.GPIOx = GPIOC, .GPIO_Pin = GPIO_Pin_4, .GPIO_CFG_MASK = 0xfff0ffff, .GPIO_CFG_OUT = 0x00010000}, //  PC4,ADC14
    {.GPIOx = GPIOC, .GPIO_Pin = GPIO_Pin_5, .GPIO_CFG_MASK = 0xff0fffff, .GPIO_CFG_OUT = 0x00100000}, //  PC5,ADC15
};

static const TKY_ChannelInitTypeDef my_tky_ch_init[TKY_QUEUE_END] = {TKY_CHS_INIT};
__attribute__ ((aligned (4))) uint32_t TKY_MEMBUF[(TKY_MEMHEAP_SIZE - 1) / 4 + 1] = {0};

/*********************************************************************
 * @fn      Timer_Init
 *
 * @brief   Timer Init function
 *
 * @param   none
 *
 * @return  none
 */
void Timer_Init (void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    RCC_HB1PeriphClockCmd (RCC_HB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig (TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd (TIM2, ENABLE);
    TIM_ClearITPendingBit (TIM2, TIM_IT_Update);
    NVIC_SetPriority (TIM2_IRQn, 0x80);
    NVIC_EnableIRQ (TIM2_IRQn);
}

/*********************************************************************
 * @fn      touch_GPIOModeCfg
 *
 * @brief   Touch Key Mode Configuration.
 *
 * @param   mode - key mode
 *          channel - touch channel
 *
 * @return  none
 */
void touch_GPIOModeCfg (GPIOMode_TypeDef mode, uint32_t channel) {
    switch (mode) {
    case GPIO_Mode_AIN:
        TKY_Pin[channel].GPIOx->CFGLR &= TKY_Pin[channel].GPIO_CFG_MASK;
        break;
    case GPIO_Mode_Out_PP:
        TKY_Pin[channel].GPIOx->CFGLR |= TKY_Pin[channel].GPIO_CFG_OUT;
        TKY_Pin[channel].GPIOx->BCR |= TKY_Pin[channel].GPIO_Pin;
        break;
    default:
        break;
    }
}

/*********************************************************************
 * @fn      touch_Regcfg
 *
 * @brief   touch configration.
 *
 * @param   none
 *
 * @return  none
 */
void touch_Regcfg (void) {
    uint8_t i = 0;
    ADC_InitTypeDef ADC_InitStructure = {0};

    RCC_HB2PeriphClockCmd (RCC_HB2Periph_ADC2 | RCC_HB2Periph_GPIOA | RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC, ENABLE);
    RCC_ADCCLKConfig (RCC_ADCCLKSource_HCLK);
    RCC_ADCHCLKCLKAsSourceConfig (RCC_PPRE2_DIV4, RCC_HCLK_ADCPRE_DIV8);

    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++) {
        touch_GPIOModeCfg (GPIO_Mode_AIN, my_tky_ch_init[i].channelNum);
    }

    ADC_DeInit (ADC2);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init (ADC2, &ADC_InitStructure);
    ADC_LowPowerModeCmd (ADC2, ENABLE);
    ADC_Cmd (ADC2, ENABLE);

#if TKY_SHIELD_EN
    TKey2->DRV |= (1 << 16);
    TKey2->DRV |= (0xffff);
#endif
    TKey2->CTLR1 |= (1 << 26) | (1 << 24);  // Enable TouchKey and Buffer
    TKY_SaveCfgReg();
}

/*********************************************************************
 * @fn      touch_Baseinit
 *
 * @brief   Touch base library initialization.
 *
 * @param   none
 *
 * @return  none
 */
static void touch_Baseinit (void) {
    uint8_t sta = 0xff;
    TKY_BaseInitTypeDef TKY_BaseInitStructure = {0};

    //----------Initialization of touch key base settings--------
    TKY_BaseInitStructure.filterMode = TKY_FILTER_MODE;
    TKY_BaseInitStructure.shieldEn = TKY_SHIELD_EN;
    TKY_BaseInitStructure.singlePressMod = TKY_SINGLE_PRESS_MODE;
    TKY_BaseInitStructure.filterGrade = TKY_FILTER_GRADE;
    TKY_BaseInitStructure.maxQueueNum = TKY_MAX_QUEUE_NUM;
    TKY_BaseInitStructure.baseRefreshOnPress = TKY_BASE_REFRESH_ON_PRESS;
    /*---Speed of baseline update£¬baseRefreshSampleNum and filterGrade£¬Inversely proportional to the baseline update speed,
    the baseline update speed is also related to the code structure, which can be observed through the function GetCurQueueBaseLine---*/
    TKY_BaseInitStructure.baseRefreshSampleNum = TKY_BASE_REFRESH_SAMPLE_NUM;
    TKY_BaseInitStructure.baseUpRefreshDouble = TKY_BASE_UP_REFRESH_DOUBLE;
    TKY_BaseInitStructure.baseDownRefreshSlow = TKY_BASE_DOWN_REFRESH_SLOW;
    TKY_BaseInitStructure.tkyBufP = TKY_MEMBUF;
    sta = TKY_BaseInit (TKY_BaseInitStructure);
    printf ("TKY_BaseInit:%02X\n", sta);
}

/*********************************************************************
 * @fn      touch_Channelinit
 *
 * @brief   Touch Channel Initialization.
 *
 * @param   none
 *
 * @return  none
 */
static void touch_Channelinit (void) {
    uint8_t error_flag = 0;
    uint16_t chx_mean = 0, chx_mean_last = 0;
    uint16_t baselineup, baselinedown;

    for (uint8_t i = 0; i < TKY_MAX_QUEUE_NUM; i++) {
        TKY_CHInit (my_tky_ch_init[i]);
    }

    for (uint8_t i = 0; i < TKY_MAX_QUEUE_NUM; i++) {

        chx_mean = TKY_GetCurChannelMean (my_tky_ch_init[i].channelNum, my_tky_ch_init[i].chargeTime,
                                          my_tky_ch_init[i].disChargeTime, 100);

        baselineup = 3500;
        baselinedown = 3200;

        if (chx_mean < baselinedown || chx_mean > baselineup) {
            error_flag = 1;
        } else {
            TKY_SetCurQueueBaseLine (i, chx_mean);
        }
    }

    // Charge/discharge baseline value abnormal, recalibrate baseline value
    if (error_flag != 0) {
        printf ("\r\nCharging parameters error, preparing for recalibration ...\r\n");
        uint16_t charge_time_CTransN;

        for (uint8_t i = 0; i < TKY_MAX_QUEUE_NUM; i++) {
            charge_time_CTransN = 1, chx_mean = 0;


            while (1) {

                chx_mean = TKY_GetCurChannelMean (my_tky_ch_init[i].channelNum, charge_time_CTransN, my_tky_ch_init[i].disChargeTime, 10);

                if ((charge_time_CTransN == 0) && ((chx_mean > baselineup))) {
                    printf ("Error, %u KEY%u Too small Cap,Please check the hardware !\r\n", chx_mean, i);
                    break;
                } else {
                    if ((chx_mean > baselinedown) && (chx_mean < baselineup)) {  // Charging parameters are normal
                        TKY_SetCurQueueBaseLine (i, chx_mean);
                        TKY_SetCurQueueChargeTime (i, charge_time_CTransN, my_tky_ch_init[i].disChargeTime);
                        printf ("channel:%u, chargetime:%u,BaseLine:%u\r\n",
                                i, charge_time_CTransN, chx_mean);
                        break;
                    } else if (chx_mean >= baselineup) {
                        TKY_SetCurQueueBaseLine (i, chx_mean_last);
                        TKY_SetCurQueueChargeTime (i, charge_time_CTransN - 1, my_tky_ch_init[i].disChargeTime);
                        printf ("Warning,channel:%u Too large Current, chargetime:%u,BaseLine:%u\r\n",
                                i, charge_time_CTransN, chx_mean_last);
                        break;
                    }
                    charge_time_CTransN++;
                    chx_mean_last = chx_mean;
                    if (charge_time_CTransN > 0xfff) {
                        printf ("Error, Chargetime Max,KEY%u Too large Cap,Please check the hardware !\r\n", i);
                        break;
                    }
                }
            }
        }
    }
}

/*********************************************************************
 * @fn      touch_InfoDebug
 *
 * @brief   Touch data print function.
 *
 * @param   none
 *
 * @return  none
 */
void touch_InfoDebug (void) {
    uint8_t i;
    uint16_t data_dispNum[TKY_MAX_QUEUE_NUM] = {0};

    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++) {
        data_dispNum[i] = TKY_GetCurQueueValue (i);
    }

    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++) {
        printf ("%04d,", data_dispNum[i]);
    }
    printf ("\r\n");

    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++) {
        data_dispNum[i] = TKY_GetCurQueueBaseLine (i);
    }

    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++) {
        printf ("%04d,", data_dispNum[i]);
    }
    printf ("\r\n");

    for (i = 0; i < TKY_MAX_QUEUE_NUM; i++) {
        printf ("%04d,", TKY_GetCurQueueRealVal (i));
    }
    printf ("\r\n");
    printf ("\r\n");
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware (void) {
    u16 keyData = 0;
    Timer_Init();
    touch_Regcfg();
    touch_Baseinit();
    touch_Channelinit();
    TKY_SaveAndStop();  

    while (1) {
        if (TKY_ScanFlag > 5) {
            TKY_ScanFlag = 0;
            TKY_LoadAndRun();

            keyData = TKY_PollForFilterMode_3();

            TKY_SaveAndStop();
        }

        if (timerFlag > 200) {
            timerFlag = 0;
            printf ("keyData: 0x%04x\r\n", keyData);
            touch_InfoDebug();
            
        }
    }
}

/********************************************
 * @fn      TIM3_UP_IRQHandler
 *
 * @brief   This function handles TIM1 interrupt.
 *
 * @return  none
 */
__attribute__ ((interrupt ("WCH-Interrupt-fast"))) void TIM2_IRQHandler (void) {
    TIM_ClearITPendingBit (TIM2, TIM_IT_Update);
    TKY_ScanFlag++;
    timerFlag++;
}