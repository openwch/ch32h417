/********************************** (C) COPYRIGHT  *******************************
 * File Name          : hardware.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/10/21
 * Description        : This file provides all the CRC firmware functions.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "hardware.h"
#include "stdlib.h"
#include "time.h"

#define SDS1_TX__SDS2_RX
// #define SDS2_TX__SDS1_RX

#define USE_RX
#define USE_TX

#define TX_RX_BUFFER_LEN 1024
#define COMMIT_TIME      1000

volatile uint32_t vTick = 0;
uint32_t tempTi[2];

uint8_t commitFlag = 0, echocommitSendcmd = 0;

__attribute__((aligned(16))) volatile uint8_t vRxData[TX_RX_BUFFER_LEN] = {0};
__attribute__((aligned(16))) volatile uint8_t vTxData[TX_RX_BUFFER_LEN] = {0};

#if defined(SDS1_TX__SDS2_RX) && defined(SDS2_TX__SDS1_RX)
#error "SDS1_TX__SDS2_RX and SDS2_TX__SDS1_RX can't be used at the same time"
#endif

#ifdef SDS1_TX__SDS2_RX
SDS_TypeDef *TxSDS = SDS1;
SDS_TypeDef *RxSDS = SDS2;
#endif

#ifdef SDS2_TX__SDS1_RX
SDS_TypeDef *TxSDS = SDS2;
SDS_TypeDef *RxSDS = SDS1;
#endif

/*********************************************************************
 * @fn      SDS_Rx_Pwrp
 * 
 * @brief   The function `sds_initial` initializes a SERDES peripheral, configures its settings based on the input parameter
 *        `direc`, and waits for PLL lock before returning.
 * 
 * @param   SDSx The `SDSx` parameter in the `sds_initial` function is a pointer to an `SDS_TypeDef` structure. This structure
 *        likely contains configuration settings and data related to a SerDes (Serializer/Deserializer) peripheral in a
 *        microcontroller or embedded system. The function initializes the SerDes peripheral
 *          direc The `direc` parameter in the `sds_initial` function is used to determine the direction of operation. If
 *        `direc` is non-zero (true), certain configurations are set for transmission (Tx), and if `direc` is zero (false),
 *        different configurations are set for reception
 * 
 * @return  The function `sds_initial` returns an unsigned 32-bit integer value. If everything executes successfully, it
 *        returns 0. If there are any errors during the initialization process, it returns the line number where the error
 *        occurred.
 */
uint32_t sds_initial(SDS_TypeDef *SDSx, char direc)
{
    RCC_HBPeriphClockCmd(RCC_HBPeriph_SERDES, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE | RCC_HB2Periph_AFIO, ENABLE);
    uint32_t time_out = 0;

    if ((RCC->CTLR & (uint32_t)RCC_SERDES_PLLRDY) != (uint32_t)RCC_SERDES_PLLRDY)
    {
        if (!(RCC->CTLR & RCC_HSERDY))
        {
            RCC->CTLR |= ((uint32_t)RCC_HSEON);
            do
            {
                time_out++;
            } while (((RCC->CTLR & RCC_HSERDY) == 0) && (time_out != HSE_STARTUP_TIMEOUT));
            if (time_out == HSE_STARTUP_TIMEOUT)
                return __LINE__;
        }
        SDS_Rx_Pwrp(SDSx);
        RCC_SERDESPLLMulConfig(RCC_SERDESPLL_MUL50);
        RCC_SERDES_PLLCmd(ENABLE);

        do
        {
            time_out++;
        } while (((RCC->CTLR & RCC_SERDES_PLLRDY) == 0) && (time_out != 0x10000));
        if (time_out == 0x10000)
            return __LINE__;
    }
    if (direc)
    {
        SDS_CFG_TypeDef sds_cfg = {.ClearALL = 0,
                                   .DMA_Enable = 1,
                                   .INIBusy_En = 1,
                                   .InsertAlign = 1,
                                   .RxPowerUp = 0,
                                   .TxPowerUp = 1,
                                   .ReplaceSYNCwithCONT = 1,
                                   .RxEn = 0,
                                   .TxEn = 1,
                                   .SwitchRxPolarity = 0,
                                   .ResetLink = 0};
        SDS_Config(SDSx, &sds_cfg);
    }
    else
    {
        SDS_CFG_TypeDef sds_cfg = {.ClearALL = 0,
                                   .DMA_Enable = 1,
                                   .INIBusy_En = 1,
                                   .InsertAlign = 1,
                                   .RxPowerUp = 1,
                                   .TxPowerUp = 1,
                                   .ReplaceSYNCwithCONT = 1,
                                   .RxEn = 1,
                                   .TxEn = 0,
                                   .SwitchRxPolarity = 0,
                                   .ResetLink = 0};
        SDS_DMA_Rx_CFG(RxSDS, (uint32_t)vRxData, (uint32_t)vRxData);
        SDS_Config(SDSx, &sds_cfg);
        SDS_ConfigIT(SDSx, SDSIT_EN_RxInt | SDSIT_EN_Cominit, ENABLE);
        NVIC_EnableIRQ(SERDES_IRQn);
    }

    time_out = 0;
    while (!(SDS_ReadCOMMAFlagBit(SDSx) & SDSIT_ST_PLLLock))
    {
        if (time_out++ > 0x10000)
        {
            if (time_out == 0x10000)
                return __LINE__;
        }
    }

    return 0;
}

/*********************************************************************
 * @fn      TIM1_INT_Init
 * 
 * @brief   The TIM1_INT_Init function initializes the TIM1 timer interrupt with the specified period and prescaler values.
 * 
 * @param   arr The `arr` parameter in the `TIM1_INT_Init` function represents the auto-reload value for the timer. It
 *        determines the period at which the timer will generate an update event.
 *          psc The parameter `psc` in the `TIM1_INT_Init` function stands for the prescaler value. The prescaler is used to
 *        divide the timer input clock frequency before it is fed into the timer's counter. This division helps in adjusting the
 *        timer resolution and the time intervals between timer ticks.
 */
void TIM1_INT_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

    NVIC_SetPriority(TIM1_UP_IRQn, 0);
    NVIC_EnableIRQ(TIM1_UP_IRQn);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM1, ENABLE);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   SerDes interface transmission test program in full duplex mode
 *
 * @return  none
 */
void Hardware(void)
{
    printf("This is the SerDes interface transmission test program in full duplex mode\r\n");
    TIM1_INT_Init(1000 - 1, SystemCoreClock / 1000000 - 1);

    uint32_t err_code = 0;
#if defined(USE_TX)
    err_code = sds_initial(TxSDS, 1);
    printf("Tx Return:%d\r\n", err_code);
#endif
#if defined(USE_RX)
    err_code = sds_initial(RxSDS, 0);
    printf("Rx Return:%d\r\n", err_code);
#endif

#if defined(USE_TX)
    SDSRTXCtrl_TypeDef sds_rtx_ctrl = {.SDSRTXCtrl_BuffMode = 0, .SDSRTXCtrl_LinkInit = 1, .SDSRTXCtrl_TxValiad = 0};
    SDS_RTX_Ctrl(TxSDS, &sds_rtx_ctrl);
    Delay_Ms(120);
    sds_rtx_ctrl.SDSRTXCtrl_LinkInit = 0;
    SDS_RTX_Ctrl(TxSDS, &sds_rtx_ctrl);
#endif

#if defined(USE_RX)
    tempTi[0] = vTick;
    while (!(SDS_ReadIT(RxSDS) & (SDSIT_FLAG_Cominit)))
    {
        if (vTick - tempTi[0] > 5000)
        {
            printf("Wait for comintt signal failed, keep waiting\r\n");
#if defined(USE_RX) && defined(USE_TX)
            sds_rtx_ctrl.SDSRTXCtrl_LinkInit = 1;
            SDS_RTX_Ctrl(TxSDS, &sds_rtx_ctrl);
            Delay_Ms(120);
            sds_rtx_ctrl.SDSRTXCtrl_LinkInit = 0;
            SDS_RTX_Ctrl(TxSDS, &sds_rtx_ctrl);
#endif
            tempTi[0] = vTick;
        }
    }
    printf("Comintt signal captured\r\n");
#endif

    while (1)
    {
#if defined(USE_RX) && defined(USE_TX)
        if(vTick - tempTi[1] > COMMIT_TIME){
            SDS_ClearIT(RxSDS, SDSIT_FLAG_Cominit);
            SDS_ConfigIT(RxSDS, SDSIT_EN_Cominit, ENABLE);
        }
        if (echocommitSendcmd)
        {
            SDSRTXCtrl_TypeDef sds_rtx_ctrl = {
                .SDSRTXCtrl_BuffMode = 0, .SDSRTXCtrl_LinkInit = 1, .SDSRTXCtrl_TxValiad = 0};
            SDS_RTX_Ctrl(TxSDS, &sds_rtx_ctrl);
            Delay_Ms(120);
            sds_rtx_ctrl.SDSRTXCtrl_LinkInit = 0;
            SDS_RTX_Ctrl(TxSDS, &sds_rtx_ctrl);
            echocommitSendcmd = 0;
        }
#endif

#if defined(USE_TX)
        vTxData[TX_RX_BUFFER_LEN - 1] = 0;
        for (int i = 0; i < TX_RX_BUFFER_LEN - 1; i++)
        {
            srand(TIM1->CNT);
            vTxData[i] = rand() % 256;
            vTxData[TX_RX_BUFFER_LEN - 1] += vTxData[i];
        }
        vTxData[TX_RX_BUFFER_LEN - 1]++;
        printf("%d\n", vTxData[TX_RX_BUFFER_LEN - 1]);

        tempTi[0] = vTick;
        while (!(SDS_ReadCOMMAFlagBit(TxSDS) & SDSIT_ST_LinkFree))
        {
            if (vTick - tempTi[0] > 1000)
            {
                printf("TxSDS LinkFree timeout");
                break;
            }
        }
        SDS_DMA_Tx_CFG(TxSDS, (uint32_t)vTxData, sizeof(vTxData) / sizeof(*vTxData), 0xaa5544aa);

        tempTi[0] = vTick;
        while (!(SDS_ReadIT(TxSDS) & SDS_TRAN_DONE_IF))
        {
            if (vTick - tempTi[0] > 1000)
            {
                printf("TxSDS TRAN_DONE_IF timeout.\n");
                break;
            }
        }
        TxSDS->RTX_CTRL &= ~SDS_TX_VLD;
        while (TxSDS->RTX_CTRL & SDS_TX_VLD)
            ;
        SDS_ClearIT(TxSDS, SDSIT_FLAG_TxInt);
        printf("Data sent\r\n");

        Delay_Ms(rand() % 500 + 500);
        // while(1);
#endif
    }
}

void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void)
{
    vTick++;
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

uint32_t cnt = 0, errcnt = 0;
void SERDES_IRQHandler() __attribute__((interrupt("WCH-Interrupt-fast")));
void SERDES_IRQHandler()
{
    if ((SDS_ReadIT(RxSDS) & SDSIT_FLAG_RxInt))
    {
        SDS_ClearIT(RxSDS, SDSIT_FLAG_RxInt);
        SDS_DMA_Rx_CFG(RxSDS, (uint32_t)vRxData, (uint32_t)vRxData);

        unsigned char temp = 0;
        for (int i = 0; i < TX_RX_BUFFER_LEN - 1; i++)
        {
            temp += vRxData[i];
        }
        cnt++;
        printf("Err rate: %f in %d\n", (float)(errcnt) / (float)(cnt), cnt);
        if ((temp + 1) % 256 == vRxData[TX_RX_BUFFER_LEN - 1])
        {
            printf("Rx success - ");
            printf("%#x\n", vRxData[TX_RX_BUFFER_LEN - 1]);
        }
        else
        {
            printf("Rx failed - %d %d\n", (temp + 1) % 256, vRxData[TX_RX_BUFFER_LEN - 1]);
            printf("%#x\n", vRxData[TX_RX_BUFFER_LEN - 1]);
            for (int i = 0; i < TX_RX_BUFFER_LEN; i++)
            {
                printf("%X", vRxData[i]);
            }
            printf("\r");
            errcnt++;
        }
    }
    else if ((SDS_ReadIT(RxSDS) & SDSIT_FLAG_Cominit))
    {
        SDS_ClearIT(RxSDS, SDSIT_FLAG_Cominit);
        echocommitSendcmd = commitFlag;
        commitFlag = 1;

        tempTi[1] = vTick;
        SDS_ConfigIT(RxSDS, SDSIT_EN_Cominit, DISABLE);
    }
}
