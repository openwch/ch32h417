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

#define I3C_TX_mux (83)
#define I3X_RX_mux (84)

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

void I3C_Target_Init()
{
    I3C_TgtConfTypeDef  I3C_TgtConfInit  = {0};
    I3C_FifoConfTypeDef I3C_FifoConfInit = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_I3C, ENABLE);

    I3C_TgtConfInit.Identifier             = 0xC6;
    I3C_TgtConfInit.MIPIVendorID           = 0x0;
    I3C_TgtConfInit.MIPIIdentifier         = 0xa;
    I3C_TgtConfInit.CtrlRoleRequest        = DISABLE;
    I3C_TgtConfInit.HotJoinRequest         = DISABLE;
    I3C_TgtConfInit.IBIRequest             = DISABLE;
    I3C_TgtConfInit.IBIPayload_EN          = DISABLE;
    I3C_TgtConfInit.IBIPayloadSize         = I3C_IBIPayloadSize_None;
    I3C_TgtConfInit.MaxReadDataSize        = 0xFF;
    I3C_TgtConfInit.MaxWriteDataSize       = 0xFF;
    I3C_TgtConfInit.CtrlCapability         = DISABLE;
    I3C_TgtConfInit.GroupAddrCapability    = DISABLE;
    I3C_TgtConfInit.DataTurnAroundDuration = I3C_DataTurnAroundDuration_Mode0;
    I3C_TgtConfInit.MaxReadTurnAround      = 0;
    I3C_TgtConfInit.MaxDataSpeed           = I3C_MaxDataSpeed_Format_Mode1;
    I3C_TgtConfInit.MaxSpeedLimitation     = DISABLE;
    I3C_TgtConfInit.HandOffActivityState   = I3C_HandOffActivityState_0;
    I3C_TgtConfInit.HandOffDelay           = DISABLE;
    I3C_TgtConfInit.PendingReadMDB         = DISABLE;

    I3C_FifoConfInit.RxFifoThreshold = I3C_RXFIFO_THRESHOLD_1_4;
    I3C_FifoConfInit.TxFifoThreshold = I3C_TXFIFO_THRESHOLD_1_4;
    I3C_FifoConfInit.ControlFifo     = I3C_CONTROLFIFO_DISABLE;
    I3C_FifoConfInit.StatusFifo      = I3C_STATUSFIFO_DISABLE;

    I3C_Tgt_Init(0xfe);
    I3C_Tgt_Config(&I3C_TgtConfInit);
    I3C_SetConfigFifo(&I3C_FifoConfInit);

    I3C_Cmd(ENABLE);
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
 * @return  none
 */
uint32_t I3C_DMA_Transmit(uint8_t *send_buf, uint16_t send_len)
{

    DMA_Tx_Init(DMA1_Channel4, (uint32_t)(&(I3C->TDBR)), (uint32_t)send_buf, send_len);

    DMA_MuxChannelConfig(DMA_MuxChannel4, I3C_TX_mux);

    I3C_DMAReq_TXCmd(ENABLE);

    DMA_Cmd(DMA1_Channel4, ENABLE);

    return 0;
}

/*********************************************************************
 * @fn      I3C_DMA_Recive
 *
 * @brief   Recives data using DMA.
 *
 * @param   addr - Target address.
 *          recv_buf - Buffer to store recived data.
 *          recv_len - Length of data to be recived.
 *
 * @return  none
 */
uint32_t I3C_DMA_Recive(uint8_t *recv_buf, uint16_t recv_len)
{
    DMA_Rx_Init(DMA1_Channel5, (uint32_t)(&(I3C->RDBR)), (uint32_t)recv_buf, recv_len);

    DMA_MuxChannelConfig(DMA_MuxChannel5, I3X_RX_mux);

    I3C_DMAReq_RXCmd(ENABLE);

    DMA_Cmd(DMA1_Channel5, ENABLE);

    return 0;
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
    printf("I3C Target TEST\r\n");
    GPIO_Config();
    I3C_Target_Init();

    for (size_t i = 0; i < 10; i++)
    {
        trasmit_buf[i] = i | 0xc0;
    }

    I3C_TxPreloadConfig(10);
    I3C_DMA_Transmit(trasmit_buf, 10);

    I3C_DMA_Recive(recive_buf, 10);

    while (DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC5) == RESET)
    {
    }

    for (size_t i = 0; i < 10; i++)
    {
        printf("%x ", recive_buf[i]);
    }
    printf("\n");

    while (1)
        ;
}
