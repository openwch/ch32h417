/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/15
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "string.h"


/*
 *@Note
 SPI interface operation flash peripheral routine:
 Master:SPI2_SCK(PB13);SPI2_MISO(PC2);SPI2_MOSI(PC1).
 This example demonstrates SPI operation Winbond W25Qxx SPIFLASH.
 pins:
    CS  -- PB12
    DO  -- PC2(SPI2_MISO)
    WP   -- 3.3V
    DI   -- PC1(SPI2_MOSI)
    CLK  -- PB13(SPI2_SCK)
    HOLD -- 3.3V

*/

/* Winbond SPIFalsh ID */
#define W25Q80                0XEF13
#define W25Q16                0XEF14
#define W25Q32                0XEF15
#define W25Q64                0XEF16
#define W25Q128               0XEF17

/* Winbond SPIFalsh Instruction List */
#define W25X_WriteEnable      0x06
#define W25X_WriteDisable     0x04
#define W25X_ReadStatusReg    0x05
#define W25X_WriteStatusReg   0x01
#define W25X_ReadData         0x03
#define W25X_FastReadData     0x0B
#define W25X_FastReadDual     0x3B
#define W25X_PageProgram      0x02
#define W25X_BlockErase       0xD8
#define W25X_SectorErase      0x20
#define W25X_ChipErase        0xC7
#define W25X_PowerDown        0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID         0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID    0x9F

/* Global define */

/* Global Variable */
u8       SPI_FLASH_BUF[4096];
const u8 TEXT_Buf[] = {"CH32H417 SPI FLASH W25Qxx"};
#define SIZE sizeof(TEXT_Buf)

/*********************************************************************
 * @fn      SPI2_ReadWriteByte
 *
 * @brief   SPI2 read or write one byte.
 *
 * @param   TxData - write one byte data.
 *
 * @return  Read one byte data.
 */
u8 SPI2_ReadWriteByte(u8 TxData)
{
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
        ;

    SPI_I2S_SendData(SPI2, TxData);

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
        ;

    return SPI_I2S_ReceiveData(SPI2);
}

/*********************************************************************
 * @fn      SPI_Flash_Init
 *
 * @brief   Configuring the SPI for operation flash.
 *
 * @return  none
 */
void SPI_Flash_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure  = {0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC , ENABLE );
	RCC_HB1PeriphClockCmd( RCC_HB1Periph_SPI2, ENABLE );	

	// CS PB12
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);

	// SCK PB13(AF5)
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// MOSI PC1(AF5)
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// MISO PC2(AF5)
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode6;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_Cmd(SPI2, ENABLE);
}

/*********************************************************************
 * @fn      SPI_Flash_ReadSR
 *
 * @brief   Read W25Qxx status register.
 *         BIT7  6   5   4   3   2   1   0
 *         SPR   RV  TB  BP2 BP1 BP0 WEL BUSY
 *
 * @return  byte - status register value.
 */
u8 SPI_Flash_ReadSR(void)
{
    u8 byte = 0;

    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
    SPI2_ReadWriteByte(W25X_ReadStatusReg);
    byte = SPI2_ReadWriteByte(0Xff);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);

    return byte;
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_SR
 *
 * @brief   Write W25Qxx status register.
 *
 * @param   sr - status register value.
 *
 * @return  none
 */
void SPI_FLASH_Write_SR(u8 sr)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
    SPI2_ReadWriteByte(W25X_WriteStatusReg);
    SPI2_ReadWriteByte(sr);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);
}

/*********************************************************************
 * @fn      SPI_Flash_Wait_Busy
 *
 * @brief   Wait flash free.
 *
 * @return  none
 */
void SPI_Flash_Wait_Busy(void)
{
    while ((SPI_Flash_ReadSR() & 0x01) == 0x01)
        ;
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_Enable
 *
 * @brief   Enable flash write.
 *
 * @return  none
 */
void SPI_FLASH_Write_Enable(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
    SPI2_ReadWriteByte(W25X_WriteEnable);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);
}

/*********************************************************************
 * @fn      SPI_FLASH_Write_Disable
 *
 * @brief   Disable flash write.
 *
 * @return  none
 */
void SPI_FLASH_Write_Disable(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
    SPI2_ReadWriteByte(W25X_WriteDisable);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);
}

/*********************************************************************
 * @fn      SPI_Flash_ReadID
 *
 * @brief   Read flash ID.
 *
 * @return  Temp - FLASH ID.
 */
u16 SPI_Flash_ReadID(void)
{
    u16 Temp = 0;

    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
    SPI2_ReadWriteByte(W25X_ManufactDeviceID);
    SPI2_ReadWriteByte(0x00);
    SPI2_ReadWriteByte(0x00);
    SPI2_ReadWriteByte(0x00);
    Temp |= SPI2_ReadWriteByte(0xFF) << 8;
    Temp |= SPI2_ReadWriteByte(0xFF);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);

    return Temp;
}

/*********************************************************************
 * @fn      SPI_Flash_Erase_Sector
 *
 * @brief   Erase one sector(4Kbyte).
 *
 * @param   Dst_Addr - 0 ~ 2047
 *
 * @return  none
 */
void SPI_Flash_Erase_Sector(u32 Dst_Addr)
{
    Dst_Addr *= 4096;
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
    SPI2_ReadWriteByte(W25X_SectorErase);
    SPI2_ReadWriteByte((u8)((Dst_Addr) >> 16));
    SPI2_ReadWriteByte((u8)((Dst_Addr) >> 8));
    SPI2_ReadWriteByte((u8)Dst_Addr);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);
    SPI_Flash_Wait_Busy();
}

/*********************************************************************
 * @fn      SPI_Flash_Read
 *
 * @brief   Read data from flash.
 *
 * @param   pBuffer -
 *          ReadAddr -Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Read(u8 *pBuffer, u32 ReadAddr, u16 size)
{
    u16 i;

    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
    SPI2_ReadWriteByte(W25X_ReadData);
    SPI2_ReadWriteByte((u8)((ReadAddr) >> 16));
    SPI2_ReadWriteByte((u8)((ReadAddr) >> 8));
    SPI2_ReadWriteByte((u8)ReadAddr);

    for (i = 0; i < size; i++)
    {
        pBuffer[i] = SPI2_ReadWriteByte(0XFF);
    }

    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);
}

/*********************************************************************
 * @fn      SPI_Flash_Write_Page
 *
 * @brief   Write data by one page.
 *
 * @param   pBuffer -
 *          WriteAddr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write_Page(u8 *pBuffer, u32 WriteAddr, u16 size)
{
    u16 i;

    SPI_FLASH_Write_Enable();
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
    SPI2_ReadWriteByte(W25X_PageProgram);
    SPI2_ReadWriteByte((u8)((WriteAddr) >> 16));
    SPI2_ReadWriteByte((u8)((WriteAddr) >> 8));
    SPI2_ReadWriteByte((u8)WriteAddr);

    for (i = 0; i < size; i++)
    {
        SPI2_ReadWriteByte(pBuffer[i]);
    }

    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);
    SPI_Flash_Wait_Busy();
}

/*********************************************************************
 * @fn      SPI_Flash_Write_NoCheck
 *
 * @brief   Write data to flash.(need Erase)
 *          All data in address rang is 0xFF.
 *
 * @param   pBuffer -
 *          WriteAddr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write_NoCheck(u8 *pBuffer, u32 WriteAddr, u16 size)
{
    u16 pageremain;

    pageremain = 256 - WriteAddr % 256;

    if (size <= pageremain)
        pageremain = size;

    while (1)
    {
        SPI_Flash_Write_Page(pBuffer, WriteAddr, pageremain);

        if (size == pageremain)
        {
            break;
        }
        else
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;
            size -= pageremain;

            if (size > 256)
                pageremain = 256;
            else
                pageremain = size;
        }
    }
}

/*********************************************************************
 * @fn      SPI_Flash_Write
 *
 * @brief   Write data to flash.(no need Erase)
 *
 * @param   pBuffer -
 *          WriteAddr - Initial address(24bit).
 *          size - Data length.
 *
 * @return  none
 */
void SPI_Flash_Write(u8 *pBuffer, u32 WriteAddr, u16 size)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;

    secpos    = WriteAddr / 4096;
    secoff    = WriteAddr % 4096;
    secremain = 4096 - secoff;

    if (size <= secremain)
        secremain = size;

    while (1)
    {
        SPI_Flash_Read(SPI_FLASH_BUF, secpos * 4096, 4096);

        for (i = 0; i < secremain; i++)
        {
            if (SPI_FLASH_BUF[secoff + i] != 0XFF)
                break;
        }

        if (i < secremain)
        {
            SPI_Flash_Erase_Sector(secpos);

            for (i = 0; i < secremain; i++)
            {
                SPI_FLASH_BUF[i + secoff] = pBuffer[i];
            }

            SPI_Flash_Write_NoCheck(SPI_FLASH_BUF, secpos * 4096, 4096);
        }
        else
        {
            SPI_Flash_Write_NoCheck(pBuffer, WriteAddr, secremain);
        }

        if (size == secremain)
        {
            break;
        }
        else
        {
            secpos++;
            secoff = 0;

            pBuffer += secremain;
            WriteAddr += secremain;
            size -= secremain;

            if (size > 4096)
            {
                secremain = 4096;
            }
            else
            {
                secremain = size;
            }
        }
    }
}

/*********************************************************************
 * @fn      SPI_Flash_Erase_Chip
 *
 * @brief   Erase all FLASH pages.
 *
 * @return  none
 */
void SPI_Flash_Erase_Chip(void)
{
    SPI_FLASH_Write_Enable();
    SPI_Flash_Wait_Busy();
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
    SPI2_ReadWriteByte(W25X_ChipErase);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);
    SPI_Flash_Wait_Busy();
}

/*********************************************************************
 * @fn      SPI_Flash_PowerDown
 *
 * @brief   Enter power down mode.
 *
 * @return  none
 */
void SPI_Flash_PowerDown(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
    SPI2_ReadWriteByte(W25X_PowerDown);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);
    Delay_Us(3);
}

/*********************************************************************
 * @fn      SPI_Flash_WAKEUP
 *
 * @brief   Power down wake up.
 *
 * @return  none
 */
void SPI_Flash_WAKEUP(void)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
    SPI2_ReadWriteByte(W25X_ReleasePowerDown);
    GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);
    Delay_Us(3);
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
    u8  datap[SIZE];
    u16 Flash_Model;


    SPI_Flash_Init();

    Flash_Model = SPI_Flash_ReadID();

    switch (Flash_Model)
    {
        case W25Q80:
            printf("W25Q80 OK!\r\n");

            break;

        case W25Q16:
            printf("W25Q16 OK!\r\n");

            break;

        case W25Q32:
            printf("W25Q32 OK!\r\n");

            break;

        case W25Q64:
            printf("W25Q64 OK!\r\n");

            break;

        case W25Q128:
            printf("W25Q128 OK!\r\n");

            break;

        default:
            printf("Fail!\r\n");

            break;
    }

    printf("Start Erase W25Qxx....\r\n");
    SPI_Flash_Erase_Sector(0);
    printf("W25Qxx Erase Finished!\r\n");

    Delay_Ms(500);
    printf("Start Read W25Qxx....\r\n");
    SPI_Flash_Read(datap, 0x0, SIZE);
    printf("%s\r\n", datap);

    Delay_Ms(500);
    printf("Start Write W25Qxx....\r\n");
    SPI_Flash_Write((u8 *)TEXT_Buf, 0, SIZE);
    printf("W25Qxx Write Finished!\r\n");

    Delay_Ms(500);
    printf("Start Read W25Qxx....\r\n");
    SPI_Flash_Read(datap, 0x0, SIZE);
    printf("%s\r\n", datap);


    while (1)
        ;
}
