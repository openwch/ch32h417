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

/* Global define */
#define Buf_Size 32

/* Global Variable */
#define Bank5_SDRAM_ADDR    ((u32)(0XC0000000)) //SDRAM¿ªÊ¼µØÖ·
#define FMC_SDRAM_CMD_TARGET_BANK2            FMC_SDCMR_CTB2
#define FMC_SDRAM_CMD_TARGET_BANK1            FMC_SDCMR_CTB1
#define FMC_SDRAM_CMD_TARGET_BANK1_2          ((uint32_t)0x00000018U)

#define FMC_SDRAM_CMD_NORMAL_MODE             ((uint32_t)0x00000000U)
#define FMC_SDRAM_CMD_CLK_ENABLE              ((uint32_t)0x00000001U)
#define FMC_SDRAM_CMD_PALL                    ((uint32_t)0x00000002U)
#define FMC_SDRAM_CMD_AUTOREFRESH_MODE        ((uint32_t)0x00000003U)
#define FMC_SDRAM_CMD_LOAD_MODE               ((uint32_t)0x00000004U)
#define FMC_SDRAM_CMD_SELFREFRESH_MODE        ((uint32_t)0x00000005U)
#define FMC_SDRAM_CMD_POWERDOWN_MODE          ((uint32_t)0x00000006U)


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
 * @fn      FMC_SDRAM_WriteBuffer
 *
 * @brief   Write buffer.
 *
 * @return  none
 */

void FMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*(vu8*)(Bank5_SDRAM_ADDR+WriteAddr)=*pBuffer;
		WriteAddr++;
		pBuffer++;
	}
}

/*********************************************************************
 * @fn      FMC_SDRAM_ReadBuffer
 *
 * @brief   Read buffer.
 *
 * @return  none
 */

void FMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*pBuffer++=*(vu8*)(Bank5_SDRAM_ADDR+ReadAddr);
		ReadAddr++;
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
    Delay_Us(500);                   
	SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_PALL,1,0);       
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_AUTOREFRESH_MODE,8,0);
	temp=(u32)SDRAM_MODEREG_BURST_LENGTH_1          |	
              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |	
              SDRAM_MODEREG_CAS_LATENCY_2           |	
              SDRAM_MODEREG_OPERATING_MODE_STANDARD |   
              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     
    SDRAM_Send_Cmd(0,FMC_SDRAM_CMD_LOAD_MODE,1,temp);   
    

	FMC_SDRAM_SetRefreshCnt(700);	
}	
/*********************************************************************
 * @fn      GPIO_Config
 *
 * @brief   Init SDRAM GPIO.
 *
 * @return  none
 */
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOF, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOD, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE, ENABLE);

    // CLK PF2(AF12)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource2, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // CS_N[0] PC2(AF12)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // CKE[0] PC5(AF12)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    // RAS_N PF11(AF12)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource11, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // CAS_N PF12(AF12)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // WE_N PC0(AF12)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource0, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // DQM[0] PC12(AF0)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF0);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // DQM[1] PC11(AF0)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF0);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // DQM[2] PC10(AF0)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF0);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // DQM[3] PA15(AF0)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF0);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // BA[0] PB14(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // BA[1] PB15(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ADDR[0] PF5(AF12)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // ADDR[1] PB3(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ADDR[2] PB4(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ADDR[3] PB8(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ADDR[4] PB9(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ADDR[5] PB6(AF11)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF11);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ADDR[6] PA10(AF10)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADDR[7] PA11(AF10)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADDR[8] PA12(AF10)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADDR[9] PA13(AF10)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADDR[10] PB10(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ADDR[11] PB11(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    // D[0] PD14(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[1] PD15(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[2] PD0(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[3] PD1(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[4] PE7(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[5] PE8(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[6] PE9(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[7] PE10(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[8] PE11(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[9] PE12(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[10] PE13(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[11] PE14(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[12] PE15(AF12)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // D[13] PD8(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[14] PD9(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[15] PD10(AF12)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // D[16] PC3(AF11)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF11);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // D[17] PB5(AF11)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF11);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // D[20] PF8(AF12)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // D[21] PF9(AF12)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // D[22] PF10(AF12)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // D[23] PA0(AF12)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // D[24] PA1(AF12)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // D[25] PA2(AF12)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // D[26] PA3(AF12)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // D[27] PA4(AF12)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // D[28] PA5(AF12)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // D[29] PB0(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // D[30] PB1(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // D[31] PB2(AF12)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource2, GPIO_AF12);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

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
    FMC_SDRAM_InitTypeDef  SDRAMInitStructure={0};
    FMC_SDRAM_TimingTypeDef SDRAM_Timing={0};
    
    RCC_HBPeriphClockCmd(RCC_HBPeriph_FMC,ENABLE);

   GPIO_Config();


    SDRAMInitStructure.FMC_Bank=FMC_Bank5_SDRAM;                          	 
    SDRAMInitStructure.FMC_ColumnBitsNumber=FMC_ColumnBitsNumber_9;     
    SDRAMInitStructure.FMC_RowBitsNumber=FMC_ROWBitsNumber_12;          
    SDRAMInitStructure.FMC_MemoryDataWidth=FMC_MemoryDataWidth_32;       
    SDRAMInitStructure.FMC_InternalBankNumber=FMC_InternalBankNumber_4;  
    SDRAMInitStructure.FMC_CASLatency=FMC_CASLatency_2CLk;               
    SDRAMInitStructure.FMC_WriteProtection=FMC_WriteProtection_Disable;
    SDRAMInitStructure.FMC_SDClockPeriod=1;           
    SDRAMInitStructure.FMC_ReadBurst=FMC_ReadBurst_Enable;                
    SDRAMInitStructure.FMC_ReadPipeDelay=FMC_ReadPipeDelay_none;            
    SDRAMInitStructure.FMC_PHASE_SEL=0xF;
    
    SDRAM_Timing.FMC_LoadToActiveDelay=2;                                  
    SDRAM_Timing.FMC_ExitSelfRefreshDelay=8;                               
    SDRAM_Timing.FMC_SelfRefreshTime=6;                                                                    
    SDRAM_Timing.FMC_RowCycleDelay=6;                                       
    SDRAM_Timing.FMC_WriteRecoveryTime=2;                                   
    SDRAM_Timing.FMC_RPDelay=2;                                            
    SDRAM_Timing.FMC_RCDDelay=2;                                                       
    SDRAMInitStructure.FMC_SDRAM_Timing = &SDRAM_Timing;

    FMC_SDRAM_Init(&SDRAMInitStructure);  
	FMC_Bank5_6->MISC|=(1<<16);
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
	u32 i=0;
    u32 p=0;
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); 
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);
	SDRAM_Init();

	for(i=0; i<5120; i++){
      *(u32*)(Bank5_SDRAM_ADDR + 4*i) = i;
	}

	printf("Read data:\n");
	for(i=0; i<5120; i++){
    p=*(u32*)(Bank5_SDRAM_ADDR + 4*i);
      printf("%08x ", p);
    } 
    printf("end\n");

	while(1);
}
