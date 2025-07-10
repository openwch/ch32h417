/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32h417_crc.c
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


#define FlT_Sinc_Order    DFSDM_FLT_Sinc3
#define FLT_Over_Sample   32

#define RxBuffSize        2
#define TxbuffSize        25*2

int32_t Rxdata[RxBuffSize] = {0};
uint8_t SPI_Tx_Data[TxbuffSize] = {0};

int32_t high_threshold_val = 32768;
int32_t low_threshold_val = -32768;
uint8_t ErrorFlag = 0;

/*********************************************************************
 * @fn      SPI_Function_Init
 *
 * @brief   Initializes SPI collection.
 *
 * @return  none
 */
void SPI_Function_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	SPI_InitTypeDef SPI_InitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC, ENABLE );
	RCC_HB1PeriphClockCmd( RCC_HB1Periph_SPI2, ENABLE );

	/* SCK PB13(AF5) */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* MOSI PC3(AF5) */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode5;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init( SPI2, &SPI_InitStructure );

    SPI_Cmd(SPI2, ENABLE); 
}

/*********************************************************************
 * @fn      SPI_Data
 *
 * @brief   Initializes the data.
 *
 * @return  none
 */
void SPI_Data(void)
{
    uint16_t i = 0;
    uint8_t h = 0xff;

    for(i=0; i < TxbuffSize; i++)
    {
        SPI_Tx_Data[i] = h;
        if((i+1)%(TxbuffSize/RxBuffSize) == 0) h=~h;
    }

}

/*********************************************************************
 * @fn      DFSDM_Function_Init
 *
 * @brief   Initializes DFSDM collection.
 *
 * @return  none
 */
void DFSDM_Function_Init(void)
{
    DFSDM_ChannelInitTypeDef DFSDM_ChannelInitStructure = {0};
    DFSDM_FilterInitTypeDef DFSDM_FilterInitStructure = {0};
    DFSDM_RcInitTypeDef DFSDM_RcInitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

	RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_DFSDM | RCC_HB2Periph_GPIOC, ENABLE);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource0, GPIO_AF3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF3);
    /* initialize DFSDM ports CKIN0-PC0(AF3)  DATIN0-PC1(AF3)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* initialize the parameters of DFSDM */
    DFSDM_ChannelStructInit(&DFSDM_ChannelInitStructure);
    DFSDM_FilterStructInit(&DFSDM_FilterInitStructure);
    DFSDM_RcStructInit(&DFSDM_RcInitStructure);

    /* initialize DFSDM channel 0 */
    DFSDM_ChannelInitStructure.DFSDM_ChDataPackMode = DFSDM_StandardMode;
    DFSDM_ChannelInitStructure.DFSDM_ChDataMultiplexer = DFSDM_SerialInput;
    DFSDM_ChannelInitStructure.DFSDM_ChShortCircuitDetMode = ENABLE;
    DFSDM_ChannelInitStructure.DFSDM_ChClockAbsenceDetMode = DISABLE;
    DFSDM_ChannelInitStructure.DFSDM_ChInPinSelect = DFSDM_SelectCurrent;
    DFSDM_ChannelInitStructure.DFSDM_ChSCDCntthreshold = 220;
    DFSDM_ChannelInitStructure.DFSDM_ChSerialInterface = DFSDM_ExternalClkIn;
    DFSDM_ChannelInitStructure.DFSDM_ChSPIClockSource = DFSDM_SPIRising;
    DFSDM_ChannelInit(DFSDM_Channel0, &DFSDM_ChannelInitStructure);

    /* initialize DFSDM filter 0 */
    DFSDM_FilterInitStructure.DFSDM_FltAWDChannel = DFSDM_AWD_Channel0;
    DFSDM_FilterInitStructure.DFSDM_FltAWDHighThreshold = high_threshold_val;
    DFSDM_FilterInitStructure.DFSDM_FltAWDLowThreshold = low_threshold_val;
    DFSDM_FilterInitStructure.DFSDM_FltExtremeChannel = DFSDM_Extremes_Channel0;
    DFSDM_FilterInitStructure.DFSDM_FltSincOrder = FlT_Sinc_Order;
    DFSDM_FilterInitStructure.DFSDM_FltOverSample = FLT_Over_Sample;
    DFSDM_FilterInitStructure.DFSDM_FltIntegratorOverSample = 1;
    DFSDM_FilterInit(DFSDM_FLT0, &DFSDM_FilterInitStructure);

    /* initialize DFSDM filter0 regular conversions */
    DFSDM_RcInitStructure.DFSDM_RcChannel = DFSDM_RC_Channel0;
    DFSDM_RcInitStructure.DFSDM_RcContinuousMode = ENABLE;
    DFSDM_RcInitStructure.DFSDM_RcSynchronousMode = DISABLE;
    DFSDM_RcInitStructure.DFSDM_RcFastMode = DISABLE;
    DFSDM_RcInitStructure.DFSDM_RcDMAMode = ENABLE;
    DFSDM_RcInit(DFSDM_FLT0, &DFSDM_RcInitStructure);

    /* initialize DFSDM IRQn */
    NVIC_SetPriority(DFSDM0_IRQn, 0);   
    NVIC_EnableIRQ(DFSDM0_IRQn);   

    /* enable DFSDM channel0 */
    DFSDM_ChannelCmd(DFSDM_Channel0, ENABLE);

    /* enable DFSDM FLT0 */
    DFSDM_FilterCmd(DFSDM_FLT0, ENABLE);

    /* enable DFSDM interrupt */
    DFSDM_ITConfig(DFSDM_FLT0, DFSDM_IT_FLTx_AWDIE, ENABLE);
    DFSDM_ITConfig(DFSDM_FLT0, DFSDM_IT_FLT0_SCDIE, ENABLE);

    /* enable DFSDM interface */
    DFSDM_Cmd(ENABLE);
    /* enable regular channel conversion by software */
    DFSDM_RcSoftStartConversion(DFSDM_FLT0);
}

/*********************************************************************
 * @fn      DMA_Rx_Init
 *
 * @brief   Initializes the DFSDM filter 0 DMA Channelx configuration.
 *
 * @param   DMA_CHx - Select the DMA channel.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Rx_Init(DMA_Channel_TypeDef *DMA_CHx, int32_t ppadr, int32_t memadr, uint16_t bufsize)
{
 DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_Memory0BaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);  
}

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DFSDM filter 0 DMA Channelx configuration.
 *
 * @param   DMA_CHx - Select the DMA channel.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, int32_t ppadr, int32_t memadr, uint16_t bufsize)
{
	DMA_InitTypeDef DMA_InitStructure={0};

	RCC_HBPeriphClockCmd( RCC_HBPeriph_DMA1, ENABLE );

	DMA_DeInit(DMA_CHx);

	DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
	DMA_InitStructure.DMA_Memory0BaseAddr = memadr;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = bufsize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init( DMA_CHx, &DMA_InitStructure );
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
	uint32_t i = 0;

    SPI_Data();
    DFSDM_Function_Init();
    SPI_Function_Init();

    DMA_Tx_Init(DMA1_Channel1, (uint32_t)&SPI2->DATAR, (uint32_t)SPI_Tx_Data, TxbuffSize);
    DMA_Rx_Init(DMA1_Channel2, (int32_t)& DFSDM_FLT0->RDATAR, (int32_t)Rxdata, RxBuffSize);
    DMA_MuxChannelConfig(DMA_MuxChannel1, 65);
    DMA_MuxChannelConfig(DMA_MuxChannel2, 107);
    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
    
    DMA_Cmd(DMA1_Channel1, ENABLE);
    DMA_Cmd(DMA1_Channel2, ENABLE);

    while((!DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC1)) &&  (!DMA_GetFlagStatus(DMA1, DMA1_FLAG_TC2)));

    if(!ErrorFlag)
    {
        for(i = 0; i < RxBuffSize; i++)
        {
            printf("%d\r\n", Rxdata[i] >> 8);
        }
    }  

    while(1);
}

/*********************************************************************
 * @fn      DFSDM0_IRQHandler
 *
 * @brief   This function handles DFSDM0 global interrupt request.
 *
 * @return  none
 */
void DFSDM0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DFSDM0_IRQHandler(void)
{
    if(DFSDM_GetITStatus(DFSDM_FLT0, DFSDM_IT_FLAG_FLTx_AWDF) != RESET)
    {
        ErrorFlag = 1;
        printf("Exceeded the threshold of the analog watchdog!\r\n");
        DFSDM_ClearITPendingBit(DFSDM_FLT0, DFSDM_IT_FLAG_FLTx_AWDF);
    }
    if(DFSDM_GetITStatus(DFSDM_FLT0, DFSDM_IT_FLAG_FLT0_SCDF0) != RESET)
    {
        ErrorFlag = 1;
        printf("The channel0 input Short-circuit occurred!\r\n");
        DFSDM_ClearITPendingBit(DFSDM_FLT0, DFSDM_IT_FLAG_FLT0_SCDF0);
    }
}