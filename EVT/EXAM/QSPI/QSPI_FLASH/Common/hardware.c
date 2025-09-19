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
#include "string.h"

#define FLASH_CMD_EnableReset             0x66
#define FLASH_CMD_ResetDevice             0x99
#define FLASH_CMD_JedecID                 0x9F
#define FLASH_CMD_WriteEnable             0X06
#define FLASH_CMD_WriteDisable            0X04
#define FLASH_CMD_ManufactDeviceID        0x90
#define FLASH_CMD_ManufactDeviceID_DualIO 0x92
#define FLASH_CMD_ManufactDeviceID_QuadIO 0x94
#define FLASH_CMD_SectorErase             0x20
#define FLASH_CMD_Read                    0x03
#define FLASH_CMD_FastReadQuadIO          0xEB
#define FLASH_CMD_FastReadDualIO          0xBB
#define FLASH_CMD_PageProgram             0x02
#define FLASH_CMD_PageProgramQuad         0x32
#define FLASH_CMD_ReadStatus_REG1         0X05
#define FLASH_CMD_ReadStatus_REG2         0X35

uint8_t readBuffer[512];
uint8_t verifyBuffer[512];
uint8_t writeBuffer[256];

/*********************************************************************
 * @fn      QSPI_Send
 * 
 * @brief   Send data to QSPI
 * 
 * @param   pBuffer - pointer to the data buffer
 * @param   len - length of the data to be send
 * 
 * @return  none
 */
void QSPI_Send(uint8_t* pBuffer, uint32_t len)
{
    uint32_t i = 0;
    while (i < len)
    {
        if (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_FT))
        {
            QSPI_SendData8(QSPI1, pBuffer[i]);
            i++;
        }
    }
}

/*********************************************************************
 * @fn      QSPI_Receive
 * 
 * @brief   Receive data from QSPI
 * 
 * @param   pBuffer - pointer to the data buffer
 * @param   len - length of the data to be received
 * 
 * @return  none
 */
void QSPI_Receive(uint8_t* pBuffer, uint32_t len)
{
    uint32_t i = 0;
    while (i < len)
    {
        if (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_FT))
        {
            pBuffer[i] = QSPI_ReceiveData8(QSPI1);
            i++;
        }
    }
}

/*********************************************************************
 * @fn      GPIO_Config
 * 
 * @brief   Configure the GPIO pins
 * 
 * @param   none
 */
static void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOF, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE, ENABLE);

    // SCK PB2(AF9)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource2, GPIO_AF9);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SCSN PB6(AF10)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SIO0 PF8(AF10)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // SIO1 PF9(AF10)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // SIO2 PF7(AF10)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // SIO3 PF6(AF10)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF10);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      QSPI1_Config
 * 
 * @brief   Configure the QSPI1
 * 
 * @param   none
 */
static void QSPI1_Config(void)
{
    QSPI_InitTypeDef QSPI_InitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_QSPI1, ENABLE);

    QSPI_InitStructure.QSPI_Prescaler = 3;
    QSPI_InitStructure.QSPI_CKMode    = QSPI_CKMode_Mode3;
    QSPI_InitStructure.QSPI_CSHTime   = QSPI_CSHTime_8Cycle;

    // size = 2 ** (FSize + 1) = 2 ** 23 = 8MB
    QSPI_InitStructure.QSPI_FSize = 22;

    QSPI_InitStructure.QSPI_FSelect = QSPI_FSelect_1;
    QSPI_InitStructure.QSPI_DFlash  = QSPI_DFlash_Disable;

    QSPI_Init(QSPI1, &QSPI_InitStructure);
    QSPI_SetFIFOThreshold(QSPI1, 10);
}

/*********************************************************************
 * @fn      QSPI_CmdWithAddress
 * 
 * @brief   Send a command with an address to the QSPI
 * 
 * @param   cmd - the command to be sent
 *          addr - the address to be sent
 * 
 * @return  none
 */
void QSPI_CmdWithAddress(uint8_t cmd, uint32_t addr)
{
    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_NoData;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = cmd;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetAddress(QSPI1, addr);
    QSPI_SetDataLength(QSPI1, 0);

    QSPI_Start(QSPI1);

    while (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_TC) == RESET)
        ;

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
}

/*********************************************************************
 * @fn      QSPI_WriteCmd
 * 
 * @brief   Write a command to the QSPI
 * 
 * @param   cmd - the command to be written
 * 
 * @return  none
 */
void QSPI_WriteCmd(uint8_t cmd)
{
    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_NoAddress;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_NoData;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = cmd;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;

    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetDataLength(QSPI1, 0);

    QSPI_Start(QSPI1);

    while (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_TC) == RESET)
    {
    }

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
}

/*********************************************************************
 * @fn      QSPI_CmdWithData
 * 
 * @brief   Send a command with data to the QSPI
 * 
 * @param   cmd - the command to be sent
 *          data - the data to be sent
 * 
 * @return  none
 */
void QSPI_CmdWithData(uint8_t cmd, uint8_t data)
{
    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_NoAddress;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Write;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = cmd;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetDataLength(QSPI1, 1);

    QSPI_Start(QSPI1);

    QSPI_SendData8(QSPI1, data);

    while (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_TC) == RESET)
        ;

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
}

/*********************************************************************
 * @fn      QSPI_ReadSR
 * 
 * @brief   Read the status register of the QSPI
 * 
 * @param   cmd - the command to be written
 * 
 * @return  the status register value
 */
uint8_t QSPI_ReadSR(uint8_t cmd)
{
    uint32_t res = 0;

    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_NoAddress;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = cmd;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetDataLength(QSPI1, 1);

    QSPI_Start(QSPI1);
    while (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_FT) == RESET)
    {
    }

    res = QSPI_ReceiveData8(QSPI1);

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
    return res;
}

/*********************************************************************
 * @fn      QSPI_WaitForBsy
 * 
 * @brief   Wait for the QSPI to be ready
 * 
 * @param   none
 * 
 * @return  none
 */
void QSPI_WaitForBsy(void)
{
    while (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_BUSY) == SET)
        ;
    QSPI_SetDataLength(QSPI1, 0x00);
    QSPI_AutoPollingMode_Config(QSPI1, 0x0000, 0x01, QSPI_PMM_AND);
    QSPI_AutoPollingModeStopCmd(QSPI1, ENABLE);

    QSPI_AutoPollingMode_SetInterval(QSPI1, 2500);

    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Auto_Polling;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_NoAddress;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = FLASH_CMD_ReadStatus_REG1;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetDataLength(QSPI1, 1);

    QSPI_Start(QSPI1);

    while (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_SM) == RESET)
    {
    }
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_SM);

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
}

/*********************************************************************
 * @fn      QSPI_WriteEnable
 * 
 * @brief   Enable the write operation
 * 
 * @param   none
 * 
 * @return  none
 */
static inline void QSPI_WriteEnable(void)
{
    QSPI_WriteCmd(FLASH_CMD_WriteEnable);
}

/*********************************************************************
 * @fn      QSPI_WriteDisable
 * 
 * @brief   Disable the write operation
 * 
 * @param   none
 * 
 * @return  none
 */
static inline void QSPI_WriteDisable(void)
{
    QSPI_WriteCmd(FLASH_CMD_WriteDisable);
}

/*********************************************************************
 * @fn      QSPI_EraseSector
 * 
 * @brief   Erase a sector of the QSPI
 * 
 * @param   addr - the address of the sector to be erased
 * 
 * @return  none
 */
void QSPI_EraseSector(uint32_t addr)
{
    QSPI_WriteEnable();
    QSPI_CmdWithAddress(FLASH_CMD_SectorErase, addr);
    QSPI_WaitForBsy();
}

/*********************************************************************
 * @fn      QSPI_WritePage
 * 
 * @brief   Write a page of the QSPI
 * 
 * @param   addr - the address of the page to be written
 *          data - the data to be written
 *          len - the length of the data to be written
 * 
 * @return  none
 */
void QSPI_WritePage(uint32_t addr, uint8_t* data, uint16_t len)
{
    while (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_BUSY) == SET)
        ;

    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Write;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = FLASH_CMD_PageProgram;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetAddress(QSPI1, addr);
    QSPI_SetDataLength(QSPI1, len);

    QSPI_Start(QSPI1);

    QSPI_Send(data, len);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
}

/*********************************************************************
 * @fn      QSPI_WritePageQuad
 * 
 * @brief   Write a page of the QSPI in 4 lines mode
 * 
 * @param   addr - the address of the page to be written
 *          data - the data to be written
 *          len - the length of the data to be written
 * 
 * @return  none
 */
void QSPI_WritePageQuad(uint32_t addr, uint8_t* data, uint16_t len)
{
    while (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_BUSY) == SET)
        ;

    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_4Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Write;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = FLASH_CMD_PageProgramQuad;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetAddress(QSPI1, addr);
    QSPI_SetDataLength(QSPI1, len);

    QSPI_EnableQuad(QSPI1, ENABLE);
    QSPI_Start(QSPI1);

    QSPI_Send(data, len);

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);

    QSPI_EnableQuad(QSPI1, DISABLE);
}

/*********************************************************************
 * @fn      QSPI_ProgramPage
 * 
 * @brief   Program a page of the QSPI
 * 
 * @param   addr - the address of the page to be programmed
 *          data - the data to be programmed
 *          len - the length of the data to be programmed
 * 
 * @return  none
 */
void QSPI_ProgramPage(uint32_t addr, uint8_t* data, uint16_t len)
{
    QSPI_WriteEnable();
    QSPI_WritePage(addr, data, len);
    QSPI_WaitForBsy();
}

/*********************************************************************
 * @fn      QSPI_ProgramPageQuad
 * 
 * @brief   Program a page of the QSPI in 4 lines mode
 * 
 * @param   addr - the address of the page to be programmed
 *          data - the data to be programmed
 *          len - the length of the data to be programmed
 * 
 * @return  none
 */
void QSPI_ProgramPageQuad(uint32_t addr, uint8_t* data, uint16_t len)
{
    QSPI_WriteEnable();
    QSPI_WritePageQuad(addr, data, len);
    QSPI_WaitForBsy();
}
/*********************************************************************
 * @fn      QSPI_GetManufacturerID
 * 
 * @brief   Read the manufacturer and device ID of the QSPI
 * 
 * @param   none
 * 
 * @return  the manufacturer and device ID of the QSPI
 */
uint32_t QSPI_GetManufacturerID()
{
    uint32_t res = 0;

    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = FLASH_CMD_ManufactDeviceID;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetAddress(QSPI1, 0);
    QSPI_SetDataLength(QSPI1, 2);

    QSPI_Start(QSPI1);

    uint32_t i = 0;

    while (i < 2)
    {
        if (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_FT))
        {
            res <<= 8;
            res |= QSPI_ReceiveData8(QSPI1);
            i++;
        }
    }

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
    return res;
}

/*********************************************************************
 * @fn      QSPI_GetManufacturerID_DualIO
 * 
 * @brief   Read the manufacturer and device ID of the QSPI in 2 lines mode
 * 
 * @param   none
 * 
 * @return  the manufacturer and device ID of the QSPI
 */
uint32_t QSPI_GetManufacturerID_DualIO()
{
    uint32_t res = 0;

    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_2Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_2Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = FLASH_CMD_ManufactDeviceID_DualIO;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 4;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetDataLength(QSPI1, 2);
    QSPI_EnableQuad(QSPI1, ENABLE);
    QSPI_Start(QSPI1);

    uint32_t i = 0;
    while (i < 2)
    {
        if (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_FT))
        {
            res <<= 8;
            res |= QSPI_ReceiveData8(QSPI1);
            i++;
        }
    }

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
    QSPI_EnableQuad(QSPI1, DISABLE);
    return res;
}

/*********************************************************************
 * @fn      QSPI_GetManufacturerID_QuadIO
 * 
 * @brief   Read the manufacturer and device ID of the QSPI in 4 lines mode
 * 
 * @param   none
 * 
 * @return  the manufacturer and device ID of the QSPI
 */
uint32_t QSPI_GetManufacturerID_QuadIO()
{
    uint32_t res = 0;

    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_4Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_4Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = FLASH_CMD_ManufactDeviceID_QuadIO;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 6;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetDataLength(QSPI1, 2);
    QSPI_EnableQuad(QSPI1, ENABLE);
    QSPI_Start(QSPI1);

    uint32_t i = 0;
    while (i < 2)
    {
        if (QSPI_GetFlagStatus(QSPI1, QSPI_FLAG_FT))
        {
            res <<= 8;
            res |= QSPI_ReceiveData8(QSPI1);
            i++;
        }
    }

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
    QSPI_EnableQuad(QSPI1, DISABLE);
    return res;
}

/*********************************************************************
 * @fn      QSPI_GetData
 * 
 * @brief   Read data from the QSPI
 * 
 * @param   addr - the address of the data to be read
 *          data - the buffer to store the data
 *          len - the length of the data to be read
 * 
 * @return  none
 */
void QSPI_GetData(uint32_t addr, uint8_t* data, uint32_t len)
{

    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = FLASH_CMD_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 0;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetAddress(QSPI1, addr);
    QSPI_SetDataLength(QSPI1, len);
    QSPI_Start(QSPI1);

    QSPI_Receive(data, len);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
}

void QSPI_GetData_DualIO(uint32_t addr, uint8_t* data, uint32_t len)
{
    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_2Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_2Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = FLASH_CMD_FastReadDualIO;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 4;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetAddress(QSPI1, addr);
    QSPI_SetDataLength(QSPI1, len);
    QSPI_Start(QSPI1);

    QSPI_Receive(data, len);

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
}

/*********************************************************************
 * @fn      QSPI_GetData_QuadIO
 * 
 * @brief   Read data from the QSPI in 4 lines mode
 * 
 * @param   addr - the address of the data to be read
 *          data - the buffer to store the data
 *          len - the length of the data to be read
 * 
 * @return  none    
 */
void QSPI_GetData_QuadIO(uint32_t addr, uint8_t* data, uint32_t len)
{
    QSPI_ComConfig_InitTypeDef QSPI_ComConfig_InitStructure = {0};
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_IMode       = QSPI_ComConfig_IMode_1Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADMode      = QSPI_ComConfig_ADMode_4Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DMode       = QSPI_ComConfig_DMode_4Line;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABMode      = QSPI_ComConfig_ABMode_NoAlternateByte;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_FMode       = QSPI_ComConfig_FMode_Indirect_Read;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_SIOOMode    = QSPI_ComConfig_SIOOMode_Disable;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ABSize      = QSPI_ComConfig_ABSize_8bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_ADSize      = QSPI_ComConfig_ADSize_24bit;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_Ins         = FLASH_CMD_FastReadQuadIO;
    QSPI_ComConfig_InitStructure.QSPI_ComConfig_DummyCycles = 6;
    QSPI_ComConfig_Init(QSPI1, &QSPI_ComConfig_InitStructure);

    QSPI_SetAddress(QSPI1, addr);
    QSPI_SetDataLength(QSPI1, len);

    QSPI_EnableQuad(QSPI1, ENABLE);
    QSPI_Start(QSPI1);

    QSPI_Receive(data, len);

    QSPI_ClearFlag(QSPI1, QSPI_FLAG_FT);
    QSPI_ClearFlag(QSPI1, QSPI_FLAG_TC);
    QSPI_EnableQuad(QSPI1, DISABLE);
}

/*********************************************************************
 * @fn      Buffer_Cmp
 * 
 * @brief   Compare two buffers
 * 
 * @param   buf1 - the first buffer
 *          buf2 - the second buffer
 *          len - the length of the buffers
 * 
 * @return  the index of the first different byte, or -1 if the buffers are identical
 */
int32_t Buffer_Cmp(uint8_t* buf1, uint8_t* buf2, uint32_t len)
{
    int32_t res = -1;
    for (int i = 0; i < len; i++)
    {
        if (buf1[i] != buf2[i])
        {
            res = i;
            break;
        }
    }
    return res;
}

/*********************************************************************
 * @fn      Print_Buffer
 * 
 * @brief   Print a buffer
 * 
 * @param   buf - the buffer to print
 *          len - the length of the buffer
 * 
 * @return  none
 */
void Print_Buffer(u8* buf, u32 len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        printf("%02x ", buf[i]);
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
    printf("\nEND\n");
}

/*********************************************************************
 * @fn      Write_Test_Data
 * 
 * @brief   Write test data to the QSPI in 4 lines mode
 * 
 * @param   write_page_count - the number of pages to write
 * 
 * @return  none
 */
void Write_Test_Data(uint32_t write_page_count)
{
    const uint32_t max_count = write_page_count * 256;

    uint32_t count = 0;
    while (count < max_count)
    {
        uint32_t* wr = (uint32_t*)writeBuffer;
        for (size_t t = 0; t < 256 / 4; t++)
        {
            *wr = count;
            wr++;
            count++;
        }
        uint32_t addr = (count * 4) - 256;
        if (addr % 4096 == 0)
        {
            QSPI_EraseSector(addr);
            printf("erase sector %x\n", addr);
        }
        QSPI_ProgramPageQuad(addr, writeBuffer, 256);
        printf("write page %x\n", addr);
    }
}

/*********************************************************************
 * @fn      Hardware
 * 
 * @brief   Main function
 * 
 * @return  none
 */
void Hardware(void)
{

    printf("QSPI TEST\n");

    GPIO_Config();
    QSPI1_Config();

    QSPI_Cmd(QSPI1, ENABLE);

    QSPI_WriteCmd(FLASH_CMD_EnableReset);
    QSPI_WriteCmd(FLASH_CMD_ResetDevice);
    Delay_Us(100);

    QSPI_WaitForBsy();

#if 0
    // Some chips do not have QUAD mode enabled by default.
    // Use the following command to enable QUAD mode
    QSPI_WriteEnable();
    QSPI_WriteCmd(0x50);
    QSPI_CmdWithData(0x31, 0x02);
#endif

    printf("GetManufacturerID %x\n", QSPI_GetManufacturerID());
    printf("GetManufacturerID_DualIO %x\n", QSPI_GetManufacturerID_DualIO());
    printf("GetManufacturerID_QuadIO %x\n", QSPI_GetManufacturerID_QuadIO());
    printf("sr1 %x\n", QSPI_ReadSR(FLASH_CMD_ReadStatus_REG1));
    printf("sr2 %x\n", QSPI_ReadSR(FLASH_CMD_ReadStatus_REG2));

    const uint32_t rd_addr = 0x0;
    const uint32_t rd_len  = 256;
    int32_t        res     = 0;

    printf("one line read\n");
    QSPI_GetData(rd_addr, verifyBuffer, rd_len);
    Print_Buffer(verifyBuffer, rd_len);
    Delay_Ms(1);

    printf("DualIO read\n");
    memset(readBuffer, 0, rd_len);
    QSPI_GetData_DualIO(rd_addr, readBuffer, rd_len);

    res = Buffer_Cmp(verifyBuffer, readBuffer, rd_len);

    if (res != -1)
    {
        Print_Buffer(readBuffer, rd_len);
        printf("error %d\n", res);
    }
    else
    {
        printf("DualIO read success\n");
    }

    printf("QuadIO read\n");
    memset(readBuffer, 0, rd_len);
    QSPI_GetData_QuadIO(rd_addr, readBuffer, rd_len);
    res = Buffer_Cmp(verifyBuffer, readBuffer, rd_len);

    if (res != -1)
    {
        Print_Buffer(readBuffer, rd_len);
        printf("error %d\n", res);
    }
    else
    {
        printf("QuadIO read success\n");
    }

    printf("read test and verify end\n");

    while (1)
    {
    }
}
