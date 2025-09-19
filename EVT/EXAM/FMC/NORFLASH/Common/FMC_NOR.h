/********************************** (C) COPYRIGHT *******************************
* File Name          : FMC_NOR.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/09/17
* Description        : This file contains the headers of the FMC_NOR.
*********************************************************************************
* Copyright (c) 2024 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include"debug.h"

#define BUFFER_SIZE        0x200
#define Bank1_NOR_ADDR    ((u32)(0x60000000))
#define NOR_WRITE(Address, Data)  (*(__IO uint16_t *)(Address) = (Data))
#define ADDR_SHIFT(A)     (Bank1_NOR_ADDR + ( 2*(A)))
#define WRITE_READ_ADDR   ((uint32_t)0x03F0000)
#define ChipErase_Timeout     ((uint32_t)0x30000000)
#define BlockErase_Timeout    ((uint32_t)0x00A00000)
#define ChipErase_Timeout     ((uint32_t)0x30000000)
#define Program_Timeout       ((uint32_t)0x00001400)

typedef enum
{
  NOR_SUCCESS = 0,
  NOR_ONGOING,
  NOR_ERROR,
  NOR_TIMEOUT
}NOR_Status;

typedef struct
{
  uint16_t Manufacturer_Code;
  uint16_t Device_Code1;
  uint16_t Device_Code2;
  uint16_t Device_Code3;
}NOR_IDTypeDef;

void FMC_NorFlash_Init(void);
NOR_Status NOR_GetStatus(uint32_t Timeout);
void NOR_Read_CFI();
void NOR_ReadID();
void NOR_Read_ID_DMA();
NOR_Status NOR_ReturnToReadMode(void);
NOR_Status NOR_EraseBlock(uint32_t BlockAddr);
NOR_Status NOR_EraseChip(void);
void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset);
NOR_Status NOR_WriteHalfWord(uint32_t WriteAddr, uint16_t Data);
NOR_Status NOR_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
void NOR_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead);

