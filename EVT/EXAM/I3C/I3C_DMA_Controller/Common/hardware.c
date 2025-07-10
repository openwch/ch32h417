/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides some functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

#define target_daa1 (0x32)

#define I3C_TX_mux  (83)
#define I3X_RX_mux  (84)

uint8_t trasmit_buf[10] = {};
uint8_t recive_buf[10]  = {};

/*********************************************************************
 * @fn      GPIO_Config
 *
 * @brief   Configures the GPIO ports.
 *
 * @return  none
 */
static void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE, ENABLE);

    // SCL PE14(AF3)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF3);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // SDA PE15(AF3)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF3);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      I3C_Config
 *
 * @brief   Configures the I3C.
 *
 * @return  none
 */
void I3C_Config()
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_I3C, ENABLE);

    I3C_Ctrl_BusTypeDef I3C_Ctrl_BusInit = {0};
    I3C_CtrlConfTypeDef I3C_CtrlConfInit = {0};
    I3C_FifoConfTypeDef I3C_FifoConfInit = {0};

    I3C_Ctrl_BusInit.SDAHoldTime        = I3C_SDAHoldTime_1_5;
    I3C_Ctrl_BusInit.WaitTime           = I3C_WaitTime_State_2;
    I3C_Ctrl_BusInit.SCLPPLowDuration   = 0x89;
    I3C_Ctrl_BusInit.SCLI3CHighDuration = 0x88;
    I3C_Ctrl_BusInit.SCLODLowDuration   = 0x88;
    I3C_Ctrl_BusInit.SCLI2CHighDuration = 0x88;
    I3C_Ctrl_BusInit.BusFreeDuration    = 0x77;
    I3C_Ctrl_BusInit.BusIdleDuration    = 0x8e;

    I3C_CtrlConfInit.DynamicAddr       = 0x22;
    I3C_CtrlConfInit.StallTime         = 0x10;
    I3C_CtrlConfInit.HotJoinAllowed_EN = DISABLE;
    I3C_CtrlConfInit.ACKStallState     = DISABLE;
    I3C_CtrlConfInit.CCCStallState     = DISABLE;
    I3C_CtrlConfInit.TxStallState      = DISABLE;
    I3C_CtrlConfInit.RxStallState      = DISABLE;

    I3C_FifoConfInit.RxFifoThreshold = I3C_RXFIFO_THRESHOLD_1_4;
    I3C_FifoConfInit.TxFifoThreshold = I3C_TXFIFO_THRESHOLD_1_4;
    I3C_FifoConfInit.ControlFifo     = I3C_CONTROLFIFO_DISABLE;
    I3C_FifoConfInit.StatusFifo      = I3C_STATUSFIFO_DISABLE;

    I3C_Ctrl_Init(&I3C_Ctrl_BusInit);

    I3C_Ctrl_Config(&I3C_CtrlConfInit);

    I3C_SetConfigFifo(&I3C_FifoConfInit);

    I3C_Cmd(ENABLE);
}

/*********************************************************************
 * @fn      I3C_RSTDAA
 *
 * @brief   Reset Dynamic Address Assignment.
 *
 * @return  1 - Error, 0 - Success
 */
uint32_t I3C_RSTDAA()
{
    uint32_t res = 0;
    I3C_ControllerHandleCCC(0x06, 0, I3C_GENERATE_STOP);

    while ((I3C_GetFlagStatus(I3C_EVR_FCF) == RESET) && (I3C_GetFlagStatus(I3C_EVR_ERRF) == RESET))
    {
    }

    if (I3C_GetFlagStatus(I3C_EVR_FCF))
    {
        I3C_ClearFlag(I3C_FLAG_FCF);
    }

    if (I3C_GetFlagStatus(I3C_EVR_ERRF))
    {
        I3C_ClearFlag(I3C_CEVR_CERRF);
        res = 1;
    }

    return res;
}

/*********************************************************************
 * @fn      I3C_ENTDAA
 *
 * @brief   Enter Dynamic Address Assignment.
 * 
 * @param   tgt_daa - Target Dynamic Address.
 *
 * @return  1 - Error, 0 - Success
 */
uint32_t I3C_ENTDAA(uint8_t tgt_daa)
{
    uint32_t tgt_cnt = 0;
    uint64_t payload = 0;
    uint32_t res     = 0;
    uint64_t target_payload[4];

    I3C_ControllerHandleCCC(0x07, 0, I3C_GENERATE_STOP);

    do
    {
        if (I3C_GetFlagStatus(I3C_EVR_TXFNFF))
        {
            for (uint32_t index = 0; index < 8; index++)
            {
                while (I3C_GetFlagStatus((I3C_EVR_RXFNEF)) == RESET)
                {
                }
                payload |= (uint64_t)((uint64_t)I3C_ReadByte() << (index * 8));
            }

            I3C_WriteByte(tgt_daa++);
            target_payload[tgt_cnt] = payload;
            tgt_cnt++;
        }

    } while ((I3C_GetFlagStatus(I3C_EVR_FCF) == RESET) &&
             (I3C_GetFlagStatus(I3C_EVR_ERRF) == RESET));

    if (I3C_GetFlagStatus(I3C_EVR_FCF))
    {
        I3C_ClearFlag(I3C_FLAG_FCF);
    }

    if (I3C_GetFlagStatus(I3C_EVR_ERRF))
    {
        I3C_ClearFlag(I3C_CEVR_CERRF);
        res = 1;
    }

    for (size_t i = 0; i < tgt_cnt; i++)
    {
        uint32_t payload_hidata = (uint32_t)(target_payload[i] & 0xFFFFFFFF);
        uint32_t payload_lodata = (uint32_t)(target_payload[i] >> 32);

        printf("payload hidata: %x\n", payload_hidata);
        printf("payload lodata: %x\n", payload_lodata);
    }

    return res;
}

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMA for transmit.
 *
 * @param   DMA_CHx - DMA channel.
 *          ppadr - Peripheral address.
 *          memadr - Memory address.
 *          bufsize - Buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_Memory0BaseAddr    = memadr;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = bufsize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      DMA_Rx_Init
 * 
 * @brief   Initializes the DMA for recive.
 *
 * @param   DMA_CHx - DMA channel.
 *          ppadr - Peripheral address.
 *          memadr - Memory address.
 *          bufsize - Buffer size.
 *
 * @return  none
 */
void DMA_Rx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_Memory0BaseAddr    = memadr;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = bufsize;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
}

/*********************************************************************
 * @fn      I3C_DMA_Transmit
 *
 * @brief   Transmits data using DMA.
 *
 * @param   addr - Target address.
 *          send_buf - Data to be transmitted.
 *          send_len - Length of data to be transmitted.
 *
 * @return  1 - Error, 0 - Success
 */
uint32_t I3C_DMA_Transmit(uint8_t addr, uint8_t *send_buf, uint16_t send_len)
{
    uint32_t count = 0;
    uint32_t res   = 0;

    DMA_Tx_Init(DMA1_Channel4, (uint32_t)(&(I3C->TDBR)), (uint32_t)send_buf, send_len);

    DMA_MuxChannelConfig(DMA_MuxChannel4, I3C_TX_mux);

    I3C_DMAReq_TXCmd(ENABLE);

    I3C_ControllerHandleMessage(addr, send_len, I3C_Direction_WR, I3C_CONTROLLER_MTYPE_PRIVATE,
                                I3C_GENERATE_STOP);
    DMA_Cmd(DMA1_Channel4, ENABLE);

    while (DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC4) == RESET)
    {
    }

    count = 0;
    while ((I3C_GetFlagStatus(I3C_EVR_FCF) == RESET) && (I3C_GetFlagStatus(I3C_EVR_ERRF) == RESET))
    {
        count++;
        __NOP();
        if (count > 5000)
        {
            res = 1;
            break;
        }
    }

    if (I3C_GetFlagStatus(I3C_EVR_FCF))
    {
        I3C_ClearFlag(I3C_FLAG_FCF);
    }

    if (I3C_GetFlagStatus(I3C_EVR_ERRF))
    {
        I3C_ClearFlag(I3C_CEVR_CERRF);
    }

    return res;
}

/*********************************************************************
 * @fn      I3C_DMA_Recive
 *
 * @brief   Recives data using DMA.
 *
 * @param   addr - Target address.
 *          recv_buf - Data to be recived.
 *          recv_len - Length of data to be recived.
 *
 * @return  1 - Error, 0 - Success
 */
uint32_t I3C_DMA_Recive(uint8_t addr, uint8_t *recv_buf, uint16_t recv_len)
{
    uint32_t count = 0;
    uint32_t res   = 0;

    DMA_Rx_Init(DMA1_Channel5, (uint32_t)(&(I3C->RDBR)), (uint32_t)recv_buf, recv_len);

    DMA_MuxChannelConfig(DMA_MuxChannel5, I3X_RX_mux);

    I3C_DMAReq_RXCmd(ENABLE);

    I3C_ControllerHandleMessage(addr, recv_len, I3C_Direction_RD, I3C_CONTROLLER_MTYPE_PRIVATE,
                                I3C_GENERATE_STOP);
    DMA_Cmd(DMA1_Channel5, ENABLE);

    while (DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC5) == RESET)
    {
    }

    count = 0;
    while ((I3C_GetFlagStatus(I3C_EVR_FCF) == RESET) && (I3C_GetFlagStatus(I3C_EVR_ERRF) == RESET))
    {
        count++;
        __NOP();
        if (count > 5000)
        {
            res = 1;
            break;
        }
    }

    if (I3C_GetFlagStatus(I3C_EVR_FCF))
    {
        I3C_ClearFlag(I3C_FLAG_FCF);
    }

    if (I3C_GetFlagStatus(I3C_EVR_ERRF))
    {
        I3C_ClearFlag(I3C_CEVR_CERRF);
    }

    return res;
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   main function.
 *
 * @return  none
 */
void Hardware(void)
{
    uint32_t res = 0;
    printf("I3C TEST\r\n");
    GPIO_Config();
    I3C_Config();

    for (size_t i = 0; i < 10; i++)
    {
        trasmit_buf[i] = i | 0xa0;
    }

    Delay_Ms(50);

    res = I3C_RSTDAA();
    if (res == 0)
    {
        printf("I3C RSTDAA Success\r\n");
    }
    else
    {
        printf("I3C RSTDAA Fail\r\n");
    }

    res = I3C_ENTDAA(target_daa1);
    if (res == 0)
    {
        printf("I3C ENTDAA Success\r\n");
    }
    else
    {
        printf("I3C ENTDAA Fail\r\n");
    }

    Delay_Ms(10);

    res = I3C_DMA_Transmit(target_daa1, trasmit_buf, 10);
    if (res == 0)
    {
        printf("I3C DMA Transmit Success\r\n");
    }
    else
    {
        printf("I3C DMA Transmit Fail\r\n");
    }

    Delay_Ms(10);

    res = I3C_DMA_Recive(target_daa1, recive_buf, 10);
    if (res == 0)
    {
        printf("I3C DMA Recive Success\r\n");
    }
    else
    {
        printf("I3C DMA Recive Fail\r\n");
    }

    for (size_t i = 0; i < 10; i++)
    {
        printf("%x ", recive_buf[i]);
    }
    printf("\n");

    while (1)
        ;
}
