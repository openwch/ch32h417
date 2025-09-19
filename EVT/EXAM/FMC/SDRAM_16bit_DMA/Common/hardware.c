/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/17
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

/* Global define */
#define BUFFER_SIZE                              1024*5
#define WRITE_READ_ADDR                          ((uint32_t)0x0)

#define Bank5_SDRAM_ADDR                         ((u32)(0XC0000000)) //SDRAM address
#define FMC_SDRAM_CMD_TARGET_BANK2               FMC_SDCMR_CTB2
#define FMC_SDRAM_CMD_TARGET_BANK1               FMC_SDCMR_CTB1
#define FMC_SDRAM_CMD_TARGET_BANK1_2             ((uint32_t)0x00000018U)

#define FMC_SDRAM_CMD_NORMAL_MODE                ((uint32_t)0x00000000U)
#define FMC_SDRAM_CMD_CLK_ENABLE                 ((uint32_t)0x00000001U)
#define FMC_SDRAM_CMD_PALL                       ((uint32_t)0x00000002U)
#define FMC_SDRAM_CMD_AUTOREFRESH_MODE           ((uint32_t)0x00000003U)
#define FMC_SDRAM_CMD_LOAD_MODE                  ((uint32_t)0x00000004U)
#define FMC_SDRAM_CMD_SELFREFRESH_MODE           ((uint32_t)0x00000005U)
#define FMC_SDRAM_CMD_POWERDOWN_MODE             ((uint32_t)0x00000006U)

#define SDRAM_MODEREG_BURST_LENGTH_1             ((u16)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((u16)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((u16)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((u16)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((u16)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((u16)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((u16)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((u16)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((u16)0x0200)
/* Global Variable */
uint32_t TxBuffer[BUFFER_SIZE]={1,11,2,22,3,33,4,77};
uint32_t RxBuffer[BUFFER_SIZE]={1,11,2,22,3,33,4,77};
volatile uint32_t WriteReadStatus = 0, Index = 0;

typedef struct
{
  uint32_t CommandMode;                  /*!< Defines the command issued to the SDRAM device.
                                              This parameter can be a value of @ref FMC_SDRAM_Command_Mode.          */

  uint32_t CommandTarget;                /*!< Defines which device (1 or 2) the command will be issued to.
                                              This parameter can be a value of @ref FMC_SDRAM_Command_Target.        */

  uint32_t AutoRefreshNumber;            /*!< Defines the number of consecutive auto refresh command issued
                                              in auto refresh mode.
                                              This parameter can be a value between Min_Data = 1 and Max_Data = 16   */
  uint32_t ModeRegisterDefinition;       /*!< Defines the SDRAM Mode register content                                */
}FMC_SDRAM_CommandTypeDef;

/*********************************************************************
 * @fn      FMC_SDRAM_WriteBuffer_DMA
 *
 * @brief   Write buffer by DMA.
 *
 * @return  none
 */

void FMC_SDRAM_WriteBuffer_DMA(u32 *pBuffer,u32 WriteAddr,u32 n)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (volatile u32)(Bank5_SDRAM_ADDR+WriteAddr);
	DMA_InitStructure.DMA_Memory0BaseAddr = (volatile u32)pBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = n/8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_256;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_256;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_ClearFlag(DMA1,DMA1_FLAG_TC3);
    DMA_Cmd(DMA1_Channel3, ENABLE);
	while(DMA_GetFlagStatus(DMA1,DMA1_FLAG_TC3) == RESET)
    {
    }
}
/*********************************************************************
 * @fn      FMC_SDRAM_ReadBuffer_DMA
 *
 * @brief   Read buffer by DMA.
 *
 * @return  none
 */

void FMC_SDRAM_ReadBuffer_DMA(u32 *pBuffer,u32 ReadAddr,u32 n)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    DMA_Cmd(DMA1_Channel3, DISABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (volatile u32)(Bank5_SDRAM_ADDR+ReadAddr);
	DMA_InitStructure.DMA_Memory0BaseAddr = (volatile u32)pBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = n/8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_256;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_256;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    DMA_ClearFlag(DMA1,DMA1_FLAG_TC3);
    DMA_Cmd(DMA1_Channel3, ENABLE);
	while(DMA_GetFlagStatus(DMA1,DMA1_FLAG_TC3) == RESET)
    {
        
    }
}
/*********************************************************************
 * @fn      SDRAM_Send_Cmd
 *
 * @brief   Send command.
 *
 * @return  none
 */
void SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval)
{
    u32 target_bank=0;
    
    if(bankx==0) target_bank=FMC_SDRAM_CMD_TARGET_BANK1;       
    else if(bankx==1) target_bank=FMC_SDRAM_CMD_TARGET_BANK2;   

    FMC_SDRAM_SendCMDConfig(target_bank,cmd,refresh,regval);
}

/*********************************************************************
 * @fn      SDRAM_Initialization_Sequence
 *
 * @brief   Init Sequence.
 *
 * @return  none
 */
void SDRAM_Initialization_Sequence()
{
	u32 temp=0;
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_CLK_ENABLE,1,0);
    // Delay_Us(500);         
	SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_PALL,1,0);       
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_AUTOREFRESH_MODE,8,0);
	temp=(u32)SDRAM_MODEREG_BURST_LENGTH_1          |	
              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |	
              SDRAM_MODEREG_CAS_LATENCY_3          |	
              SDRAM_MODEREG_OPERATING_MODE_STANDARD |   
              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_LOAD_MODE,1,temp);   
    

	FMC_SDRAM_SetRefreshCnt(677);	
}	
/*********************************************************************
 * @fn      Fill_Buffer
 *
 * @brief   pBuffer: pointer on the Buffer to fill
 *          BufferSize: size of the buffer to fill
 *          Offset: first value to fill on the Buffer
 *
 * @param   None
 *
 * @return  None
 *          
 */

void Fill_Buffer(uint32_t *pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
    uint16_t IndexTmp = 0;

    /* Put in global buffer same values */
    for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
    {
        pBuffer[IndexTmp] =IndexTmp +Offset;
    }
}
/*********************************************************************
 * @fn      FMC_SDRAM_Init
 *
 * @brief   Init FMC
 *
 * @return  none
 */
void SDRAM_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};
    FMC_SDRAM_InitTypeDef  SDRAMInitStructure={0};
    FMC_SDRAM_TimingTypeDef SDRAM_Timing={0};
    

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA |RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC |RCC_HB2Periph_GPIOD | RCC_HB2Periph_GPIOE| RCC_HB2Periph_GPIOF |RCC_HB2Periph_AFIO,ENABLE);
    RCC_HBPeriphClockCmd(RCC_HBPeriph_FMC,ENABLE);

    /*
    PA10-A6 
    PA11-A7 
    PA12-A8 
    PA13-A9
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF10);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF10);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF10);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF10);

    /*
    PB3-A1 
    PB4-A2 
    PB8-A3 
    PB9-A4 
    PB6-A5  
    PB10-A10 
    PB11-A11 
    PB12-A12 
    PB14-BA0 
    PB15-BA1 
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_6|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF12);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF12);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF12);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF12);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF11);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF12);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF12);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF12);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF12);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF12);

    /*
    PC5-SDCKE0
    PC0-SDNWE
    PC2-SDNE0
    PC12-NBL0
    PC11-NBL1
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_0|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF12);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource0, GPIO_AF12);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF12);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF0);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF0);

    /*
    PD14-D0
    PD15-D1
    PD0-D2
    PD1-D3
    PD8-D13
    PD9-D14
    PD10-D15
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF12);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF12);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF12);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF12);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF12);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF12);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF12);

    /*
    PE7-D4
    PE8-D5
    PE9-D6
    PE10-D7
    PE11-D8
    PE12-D9
    PE13-D10
    PE14-D11
    PE15-D12
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF12);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF12);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF12);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF12);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF12);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF12);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF12);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF12);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF12);

    /*
    PF5-A0
    PF2-SDCLK
    PF11-SDNRAS
    PF12-SDNCAS
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_2|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF12);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource2, GPIO_AF12);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource11, GPIO_AF12);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF12);


    SDRAMInitStructure.FMC_Bank=FMC_Bank5_SDRAM;                          	 
    SDRAMInitStructure.FMC_ColumnBitsNumber=FMC_ColumnBitsNumber_9;     
    SDRAMInitStructure.FMC_RowBitsNumber=FMC_ROWBitsNumber_13;          
    SDRAMInitStructure.FMC_MemoryDataWidth=FMC_MemoryDataWidth_16;       
    SDRAMInitStructure.FMC_InternalBankNumber=FMC_InternalBankNumber_4;  
    SDRAMInitStructure.FMC_CASLatency=FMC_CASLatency_3CLk;               
    SDRAMInitStructure.FMC_WriteProtection=FMC_WriteProtection_Disable;
    SDRAMInitStructure.FMC_SDClockPeriod=1;           
    SDRAMInitStructure.FMC_ReadBurst=FMC_ReadBurst_Disable;                
    SDRAMInitStructure.FMC_ReadPipeDelay=FMC_ReadPipeDelay_none;            
    SDRAMInitStructure.FMC_PHASE_SEL=0xA;
    
    SDRAM_Timing.FMC_LoadToActiveDelay=2;                                  
    SDRAM_Timing.FMC_ExitSelfRefreshDelay=8;                               
    SDRAM_Timing.FMC_SelfRefreshTime=5;                                                                    
    SDRAM_Timing.FMC_RowCycleDelay=6;                                       
    SDRAM_Timing.FMC_WriteRecoveryTime=2;                                   
    SDRAM_Timing.FMC_RPDelay=2;                                            
    SDRAM_Timing.FMC_RCDDelay=2;                                           
    SDRAMInitStructure.FMC_SDRAM_Timing = &SDRAM_Timing;

    FMC_SDRAM_Init(&SDRAMInitStructure);  
    FMC_Bank5_6->MISC|=(1<<15);
	FMC_Bank5_6->MISC|=(1<<16);    //Enable SDRAM1
	SDRAM_Initialization_Sequence();
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
    printf("SDRAM DEMO\r\n");
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); 
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);
	SDRAM_Init();
    Fill_Buffer(TxBuffer, BUFFER_SIZE, 0x01);
    FMC_SDRAM_WriteBuffer_DMA(TxBuffer,WRITE_READ_ADDR,BUFFER_SIZE);
    FMC_SDRAM_ReadBuffer_DMA(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    for ( Index = 0x00; (Index < BUFFER_SIZE) && (WriteReadStatus == 0); Index++)
    {
        if (RxBuffer[Index] != TxBuffer[Index])
        {
            printf("error\n");
             WriteReadStatus = Index + 1;
            printf("RxBuffer[%d]=%x\r\n",Index,RxBuffer[Index]);
            printf("TxBuffer[%d]=%x\r\n",Index,TxBuffer[Index]);
        }
    }
  
        if (WriteReadStatus == 0)
    {
        /* OK */
        printf("sucess\r\n");
    }
    else
    {
        /* KO */
        printf("error\r\n");
    }
    printf("end\n");
	while(1);
}
