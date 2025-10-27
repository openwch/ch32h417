/********************************** (C) COPYRIGHT  *******************************
 * File Name          : hardware.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/03/01
 * Description        : This file contains all the functions prototypes for the hardware.c file.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "hardware.h"
#include "stdlib.h"
#include "time.h"

__attribute__((aligned(4))) typedef struct
{
    uint32_t : 3;
    uint32_t Tx_output_swing : 2;
    uint32_t Tx_de_emphasis : 2;
    uint32_t : 2;
    uint32_t Rx_fbres : 2;
    uint32_t Rx_fbcap : 3;
    uint32_t : 18;
} ANALOGPARAM_t;

#define SDS1_TX__SDS2_RX
// #define SDS2_TX__SDS1_RX

// #define RX_PORT
#define TX_PORT

#define INPORT GPIOA
#define OUTPORT GPIOA
#define INPIN 13
#define OUTPIN 14

#define OUT_HIGH() GPIO_SetBits(OUTPORT, 1 << OUTPIN);
#define OUT_LOW() GPIO_ResetBits(OUTPORT, 1 << OUTPIN);
#define OUT_LOWPUL(x)                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        GPIO_ResetBits(OUTPORT, 1 << OUTPIN);                                                                          \
        Delay_Ms(x);                                                                                                   \
        GPIO_SetBits(OUTPORT, 1 << OUTPIN);                                                                            \
    } while (0)
#define IN_STAT() GPIO_ReadInputDataBit(INPORT, 1 << INPIN)

#define TX_RX_BUFFER_LEN 1024
#define TEST_TIME 1000

#if defined(RX_PORT)
typedef enum
{
    waitfor_commit,
    waitfor_data
} CopSta_e;
#endif

#if defined(TX_PORT)
typedef enum
{
    send_commit,
    waitfor_commit_callback,
    send_data,
    waitfor_send_data_callback
} CopSta_e;
#endif

volatile uint32_t vTick = 0;

__attribute__((aligned(16))) volatile uint8_t Buffer[TX_RX_BUFFER_LEN] = {0};

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
 * @brief   The function `sds_initial` initializes a SERDES peripheral, configures its settings based on the input
 * parameter `direc`, and waits for PLL lock before returning.
 *
 * @param   SDSx The `SDSx` parameter in the `sds_initial` function is a pointer to an `SDS_TypeDef` structure. This
 * structure likely contains configuration settings and data related to a SerDes (Serializer/Deserializer) peripheral in
 * a microcontroller or embedded system. The function initializes the SerDes peripheral direc The `direc` parameter in
 * the `sds_initial` function is used to determine the direction of operation. If `direc` is non-zero (true), certain
 * configurations are set for transmission (Tx), and if `direc` is zero (false), different configurations are set for
 * reception
 *
 * @return  The function `sds_initial` returns an unsigned 32-bit integer value. If everything executes successfully, it
 *        returns 0. If there are any errors during the initialization process, it returns the line number where the
 * error occurred.
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
        RCC_SERDESPLLMulConfig(RCC_SERDESPLL_MUL56);
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
        SDS_DMA_Rx_CFG(RxSDS, (uint32_t)Buffer, (uint32_t)Buffer);
        SDS_Config(SDSx, &sds_cfg);
        SDS_ConfigIT(SDSx, SDSIT_EN_RxInt | SDSIT_EN_Cominit, ENABLE);
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
 * @brief   The TIM1_INT_Init function initializes the TIM1 timer interrupt with the specified period and prescaler
 * values.
 *
 * @param   arr The `arr` parameter in the `TIM1_INT_Init` function represents the auto-reload value for the timer. It
 *        determines the period at which the timer will generate an update event.
 *          psc The parameter `psc` in the `TIM1_INT_Init` function stands for the prescaler value. The prescaler is
 * used to divide the timer input clock frequency before it is fed into the timer's counter. This division helps in
 * adjusting the timer resolution and the time intervals between timer ticks.
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
 * @brief   SerDes interface autoconnfig hardware initialization function.
 *
 * @return  none
 */
void Hardware(void)
{
    printf("This is the SerDes interface transmission test program in full duplex mode\r\n");
    TIM1_INT_Init(1000 - 1, SystemCoreClock / 1000000 - 1);

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_Very_High,
        .GPIO_Pin = 1 << OUTPIN,
    };

    GPIO_Init(OUTPORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = 1 << INPIN;
    GPIO_Init(INPORT, &GPIO_InitStructure);

    GPIO_ResetBits(OUTPORT, 1 << OUTPIN);

    uint32_t err_code = 0;
    ANALOGPARAM_t analogparam = {0};

    // *(ANALOGPARAM_t *)SYS_CFGR4_BASE = analogparam;
    *(uint32_t *)SYS_CFGR4_BASE = 0x543E2B;
#if defined(TX_PORT)
    err_code = sds_initial(TxSDS, 1);
    printf("Tx Return:%d\r\n", err_code);
    while (IN_STAT() == Bit_RESET)
    {
        printf("a %#x\n", IN_STAT());
    }
    printf("Rx device initialized\n");
    GPIO_SetBits(OUTPORT, 1 << OUTPIN);

    uint32_t TxCfg_CNT = 0, RxCfg_CNT = 0, Send_CNT = 0;
    CopSta_e stat = send_commit;
#endif
#if defined(RX_PORT)
    err_code = sds_initial(RxSDS, 0);
    printf("Rx Return:%d\r\n", err_code);
    GPIO_SetBits(OUTPORT, 1 << OUTPIN);
    uint32_t RxCfg_CNT = 0, Rec_CNT = 0;
    CopSta_e stat = waitfor_commit;
    while (IN_STAT() == 1 << Bit_RESET)
        ;
    preSer = 0;
#endif

    uint32_t TiTmp;

    while (1)
    {
        switch (stat)
        {
#if defined(RX_PORT)
        case waitfor_commit:
            TiTmp = vTick;
            while (IN_STAT() == Bit_SET)
            {
                if (vTick - TiTmp > 10 + 50)
                {
                    stat = waitfor_commit;
                    printf("Wait for commit timeout\n");
                    break;
                }
            }
            if (vTick - TiTmp > 10 + 50)
            {
                break;
            }
            stat = waitfor_data;
            printf("Commit received\n");
            while (IN_STAT() == Bit_RESET)
                ;
            OUT_LOWPUL(2);
            break;

        case waitfor_data:
            TiTmp = vTick;
            while (IN_STAT() == Bit_SET)
            {
                if (vTick - TiTmp > 1000)
                {
                    stat = waitfor_commit;
                    Rec_CNT = 0;
                    RxCfg_CNT = (RxCfg_CNT + 1) & 0b11111;
                    analogparam.Rx_fbcap = RxCfg_CNT >> 2;
                    analogparam.Rx_fbres = RxCfg_CNT & 0b11;
                    *(ANALOGPARAM_t *)SYS_CFGR4_BASE = analogparam;
                    printf("Wait for data timeout\n");
                    break;
                }
            }
            if (vTick - TiTmp > 1000)
            {
                break;
            }
            if ((SDS_ReadIT(RxSDS) & SDSIT_FLAG_RxInt))
            {
                SDS_ClearIT(RxSDS, SDSIT_FLAG_RxInt);
                Rec_CNT++;
                uint8_t temp = 0;
                for (uint32_t i = 0; i < TX_RX_BUFFER_LEN - 1; i++)
                {
                    temp += Buffer[i];
                }
                temp++;
                if ((temp != Buffer[TX_RX_BUFFER_LEN - 1]))
                {
                    printf("Data error");
                    stat = waitfor_commit;
                    break;
                }
                printf("Data received, total:%d\n", Rec_CNT);
                OUT_LOWPUL(2);
            }
            else
            {
                Rec_CNT = 0;
                stat = waitfor_commit;
                Rec_CNT = 0;
                RxCfg_CNT = (RxCfg_CNT + 1) & 0b11111;
                analogparam.Rx_fbcap = RxCfg_CNT >> 2;
                analogparam.Rx_fbres = RxCfg_CNT & 0b11;
                *(ANALOGPARAM_t *)SYS_CFGR4_BASE = analogparam;
                printf("No data received\n");
            }

            if (Rec_CNT > TEST_TIME)
            {
                OUT_LOW();
                printf("Test finished\n");
                printf("Proper config: Rx Fbcap:%d Fbres:%d\n",
                       analogparam.Rx_fbcap,
                       analogparam.Rx_fbres);
                while (1)
                    ;
            }
            while (IN_STAT() == Bit_RESET)
                ;
            break;
#endif

#if defined(TX_PORT)
        case send_commit:
            printf("Sending commit\n");
            SDSRTXCtrl_TypeDef sds_rtx_ctrl = {
                .SDSRTXCtrl_BuffMode = 0, .SDSRTXCtrl_LinkInit = 1, .SDSRTXCtrl_TxValiad = 0};
            SDS_RTX_Ctrl(TxSDS, &sds_rtx_ctrl);
            Delay_Ms(50);
            sds_rtx_ctrl.SDSRTXCtrl_LinkInit = 0;
            SDS_RTX_Ctrl(TxSDS, &sds_rtx_ctrl);
            printf("Commit sent\n");
            OUT_LOWPUL(2);
            stat = waitfor_commit_callback;
            break;

        case waitfor_commit_callback:
            TiTmp = vTick;
            while (IN_STAT() == Bit_SET)
            {
                if (vTick - TiTmp > 10)
                {
                    stat = send_commit;
                    printf("Wait for commit callback timeout %d %d\n", RxCfg_CNT, 0b11111);
                    Send_CNT = 0;
                    if (RxCfg_CNT == 0b11111)
                    {
                        if (TxCfg_CNT == 0b1111)
                        {
                            printf("At the current frequency and cable, no suitable\
							analog parameters were found. Please try to reduce the \
							frequency of SerDes or change the material and length \
							of the data transmission line \n");
                            while (1)
                                ;
                        }
                        TxCfg_CNT = (TxCfg_CNT + 1) & 0b1111;
                        analogparam.Tx_output_swing = TxCfg_CNT & 0b11;
                        analogparam.Tx_de_emphasis = TxCfg_CNT >> 2;
                        printf("CHa\n");
                        *(ANALOGPARAM_t *)SYS_CFGR4_BASE = analogparam;
                    }
                    RxCfg_CNT = (RxCfg_CNT + 1) & 0b11111;
                    break;
                }
            }
            if (vTick - TiTmp > 10)
            {
                break;
            }
            stat = send_data;
            printf("Commit callback received\n");
            while (IN_STAT() == Bit_RESET)
                ;
            break;

        case send_data:
            Buffer[TX_RX_BUFFER_LEN - 1] = 0;
            for (int i = 0; i < TX_RX_BUFFER_LEN - 1; i++)
            {
                srand(TIM1->CNT);
                Buffer[i] = rand() % 256;
                Buffer[TX_RX_BUFFER_LEN - 1] += Buffer[i];
            }
            Buffer[TX_RX_BUFFER_LEN - 1]++;

            SDS_DMA_Tx_CFG(TxSDS, (uint32_t)Buffer, sizeof(Buffer) / sizeof(*Buffer), 0xaa5544aa);
            while (!(SDS_ReadIT(TxSDS) & SDS_TRAN_DONE_IF))
                ;
            TxSDS->RTX_CTRL &= ~SDS_TX_VLD;
            while (TxSDS->RTX_CTRL & SDS_TX_VLD)
                ;
            SDS_ClearIT(TxSDS, SDSIT_FLAG_TxInt);
            OUT_LOWPUL(2);
            printf("Data sent\n");
            stat = waitfor_send_data_callback;
            break;

        case waitfor_send_data_callback:
            TiTmp = vTick;
            while (IN_STAT() == Bit_SET)
            {
                if (vTick - TiTmp > 10)
                {
                    stat = send_commit;
                    Send_CNT = 0;
                    printf("Wait for send data callback timeout\n");
                    if (RxCfg_CNT == 0b11111)
                    {
                        TxCfg_CNT = (TxCfg_CNT + 1) & 0b1111;
                        analogparam.Tx_output_swing = 0b1111 & 0b11;
                        analogparam.Tx_de_emphasis = 0b1111 >> 2;
                        *(ANALOGPARAM_t *)SYS_CFGR4_BASE = analogparam;
                    }
                    RxCfg_CNT = (RxCfg_CNT + 1) & 0b11111;

                    break;
                }
            }
            if (vTick - TiTmp > 10)
            {
                break;
            }
            if (Send_CNT < TEST_TIME)
            {
                Send_CNT++;
                stat = send_data;
                printf("Send data callback received\n");
            }
            else
            {
                OUT_LOW();
                printf("Finished\n");
                printf("Proper config: Tx Swing:%d De-emphasis:%d\n",
                       analogparam.Tx_output_swing,
                       analogparam.Tx_de_emphasis);
                while (1)
                    ;
            }
            while (IN_STAT() == Bit_RESET)
                ;
            break;

        default:
            stat = send_commit;
            break;
#endif
        }
    }
}

void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM1_UP_IRQHandler(void)
{
    vTick++;
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}
