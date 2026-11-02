/********************************** (C) COPYRIGHT *******************************
 * File Name          : sd_card.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/09/23
 * Description        : SD card driver in SPI mode.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "sd_card.h"
#include "debug.h"

/* SPI peripheral and chip select pin in use */
#define SD_SPI        SPI1
#define SD_CS_GPIO    GPIOA
#define SD_CS_PIN     GPIO_Pin_2

/* Chip select control: low level selects the SD card */
#define SD_CS_LOW()   GPIO_ResetBits(SD_CS_GPIO, SD_CS_PIN)
#define SD_CS_HIGH()  GPIO_SetBits(SD_CS_GPIO, SD_CS_PIN)

/* SD card SPI command numbers */
#define CMD0          0  /* Reset to idle state */
#define CMD8          8  /* Send interface condition, check card version */
#define CMD9          9  /* Read CSD register (capacity information) */
#define CMD12         12 /* Stop multiple block transfer */
#define CMD16         16 /* Set block length */
#define CMD17         17 /* Read single block */
#define CMD18         18 /* Read multiple blocks */
#define CMD24         24 /* Write single block */
#define CMD25         25 /* Write multiple blocks */
#define CMD55         55 /* Prefix command, next one is an ACMD */
#define CMD58         58 /* Read OCR register */
#define ACMD41        41 /* Initialize card (after CMD55) */

#define SD_BLOCK_SIZE 512 /* Bytes per block */

/* Card type and total sector count (global) */
uint8_t  SD_Type        = SD_TYPE_UNKNOWN;
uint32_t SD_SectorCount = 0;

/*********************************************************************
 * @fn      SPI_ReadWriteByte
 *
 * @brief   SPI full-duplex read/write one byte (send and receive simultaneously).
 *
 * @param   tx - Byte to be sent.
 *
 * @return  Byte received.
 */
static uint8_t SPI_ReadWriteByte(uint8_t tx)
{
    /* Wait for the transmit buffer to be empty */
    while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_TXE) == RESET)
    {
    }

    SPI_I2S_SendData(SD_SPI, tx);

    /* Wait for the receive buffer to be not empty */
    while (SPI_I2S_GetFlagStatus(SD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
    {
    }

    return (uint8_t)SPI_I2S_ReceiveData(SD_SPI);
}

/*********************************************************************
 * @fn      SD_SPI_SetSpeed
 *
 * @brief   Reconfigure the SPI clock prescaler (switch communication speed).
 *
 * @param   prescaler - SPI_BaudRatePrescaler_ModeX value.
 *
 * @return  none
 */
static void SD_SPI_SetSpeed(uint16_t prescaler)
{
    SPI_InitTypeDef SPI_InitStructure = {0};

    /* SD card SPI mode 0: CPOL=0, CPHA=0, MSB first */
    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = prescaler;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;

    SPI_Cmd(SD_SPI, DISABLE);
    SPI_Init(SD_SPI, &SPI_InitStructure);
    SPI_Cmd(SD_SPI, ENABLE);
}

/*********************************************************************
 * @fn      SD_Init
 *
 * @brief   Initialize GPIO and SPI peripheral used by the SD card (no card protocol).
 *          SCK=PA5, MISO=PA6, MOSI=PA7, CS=PA2.
 *
 * @return  none
 */
void SD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOA | RCC_HB2Periph_SPI1, ENABLE);

    /* SPI1_SCK PA5, SPI1_MISO PA6, SPI1_MOSI PA7 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF5);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF5);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF5);

    /* CS: PA2, push-pull output */
    GPIO_InitStructure.GPIO_Pin   = SD_CS_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(SD_CS_GPIO, &GPIO_InitStructure);
    SD_CS_HIGH();

    /* SPI1_SCK PA5 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SPI1_MISO PA6 */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SPI1_MOSI PA7 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Use a low speed clock during initialization */
    SD_SPI_SetSpeed(SPI_BaudRatePrescaler_Mode7);
}

/*********************************************************************
 * @fn      SD_EndCmd
 *
 * @brief   End a command: set chip select high and give 8 clocks.
 *
 * @return  none
 */
static void SD_EndCmd(void)
{
    SD_CS_HIGH();
    SPI_ReadWriteByte(0xFF);
}

/*********************************************************************
 * @fn      SD_SendCmdRaw
 *
 * @brief   Send a command and wait for the R1 response with CS already low.
 *
 * @param   cmd - Command number (without the 0x40 start bit).
 *          arg - 4-byte command argument.
 *          crc - CRC check byte.
 *
 * @return  R1 response byte, bit7 = 0 means valid; 0xFF means timeout.
 */
static uint8_t SD_SendCmdRaw(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t  r;
    uint16_t retry;

    SPI_ReadWriteByte(0xFF);                 /* Extra clocks between commands */
    SPI_ReadWriteByte(cmd | 0x40);           /* Command number + start bit */
    SPI_ReadWriteByte((uint8_t)(arg >> 24)); /* 4-byte argument, MSB first */
    SPI_ReadWriteByte((uint8_t)(arg >> 16));
    SPI_ReadWriteByte((uint8_t)(arg >> 8));
    SPI_ReadWriteByte((uint8_t)arg);
    SPI_ReadWriteByte(crc);

    /* Wait up to 0xFF bytes until a response with bit7 = 0 is received */
    for (retry = 0; retry < 0xFF; retry++)
    {
        r = SPI_ReadWriteByte(0xFF);
        if ((r & 0x80) == 0)
        {
            return r;
        }
    }

    return 0xFF;
}

/*********************************************************************
 * @fn      SD_GetCapacity
 *
 * @brief   Parse the card capacity by reading the CSD register.
 *
 * @return  Total sector count (512 bytes/sector), 0 on failure.
 */
uint32_t SD_GetCapacity(void)
{
    uint8_t  r;
    uint8_t  csd[16];
    uint8_t  i;
    uint32_t csize;
    uint32_t csize_mult;
    uint32_t read_bl_len;

    /* CMD9: read CSD */
    SD_CS_LOW();
    r = SD_SendCmdRaw(CMD9, 0, 0x01);
    if (r != 0x00)
    {
        SD_EndCmd();
        return 0;
    }

    /* Wait for the data token 0xFE */
    for (i = 0; i < 0xFF; i++)
    {
        if (SPI_ReadWriteByte(0xFF) == 0xFE)
        {
            break;
        }
    }

    /* Read 16-byte CSD, followed by 2-byte CRC */
    for (i = 0; i < 16; i++)
    {
        csd[i] = SPI_ReadWriteByte(0xFF);
    }
    SPI_ReadWriteByte(0xFF);
    SPI_ReadWriteByte(0xFF);
    SD_EndCmd();

    /* CSD version 2.0 (SDHC/SDXC): capacity = (C_SIZE+1)*512KB, in sectors */
    if ((csd[0] & 0xC0) == 0x40)
    {
        csize = ((uint32_t)(csd[7] & 0x3F) << 16) | ((uint32_t)csd[8] << 8) | csd[9];
        return (csize + 1) * 1024;
    }

    /* CSD version 1.0 (standard capacity): capacity = (C_SIZE+1)*2^(C_SIZE_MULT+2)*2^READ_BL_LEN */
    read_bl_len = csd[5] & 0x0F;
    csize       = ((uint32_t)(csd[6] & 0x03) << 10) | ((uint32_t)csd[7] << 2) | (csd[8] >> 6);
    csize_mult  = ((csd[9] & 0x03) << 1) | (csd[10] >> 7);

    return ((csize + 1) * (1 << (csize_mult + 2)) * (1 << read_bl_len)) / SD_BLOCK_SIZE;
}

/*********************************************************************
 * @fn      SD_InitCard
 *
 * @brief   SD card power-on initialization, identify card type and set block length.
 *
 * @return  SD_OK or an error code.
 */
uint8_t SD_InitCard(void)
{
    uint8_t  r;
    uint8_t  i;
    uint8_t  buf[4];
    uint16_t retry;

    SD_Type        = SD_TYPE_UNKNOWN;
    SD_SectorCount = 0;

    /* Keep CS high and send at least 74 clocks after power-on to enter SPI mode */
    SD_CS_HIGH();
    Delay_Ms(10);

    for (i = 0; i < 10; i++)
    {
        SPI_ReadWriteByte(0xFF);
    }

    SD_SPI_SetSpeed(SPI_BaudRatePrescaler_Mode7);

    /* CMD0: reset. R1 = 0x01: entered idle; 0x00: card already ready (warm reset) */
    SD_CS_LOW();
    r = SD_SendCmdRaw(CMD0, 0, 0x95);
    SD_EndCmd();
    if (r != 0x01 && r != 0x00)
    {
        printf("CMD0 resp=0x%02x\r\n", r);
        return SD_ERR_RESPONSE;
    }

    /* CMD8: send 0x1AA interface condition, 0x01 means a V2.0 card */
    SD_CS_LOW();
    r = SD_SendCmdRaw(CMD8, 0x1AA, 0x87);
    if (r == 0x01)
    {
        /* Read the 4 bytes of the R7 response (voltage and check echo) */
        for (i = 0; i < 4; i++)
        {
            buf[i] = SPI_ReadWriteByte(0xFF);
        }
        SD_EndCmd();

        if (buf[2] != 0x01 || buf[3] != 0xAA)
        {
            return SD_ERR_NOT_SUPPORT;
        }

        /* ACMD41: request initialization with HCS bit until the card returns 0x00 (ready) */
        retry = 0;
        do
        {
            SD_CS_LOW();
            r = SD_SendCmdRaw(CMD55, 0, 0x01);
            SD_EndCmd();

            SD_CS_LOW();
            r = SD_SendCmdRaw(ACMD41, 0x40000000, 0x77);
            SD_EndCmd();
            Delay_Ms(1);
            retry++;
        } while (r != 0x00 && retry < 3000);

        if (retry >= 3000)
        {
            return SD_ERR_TIMEOUT;
        }

        /* CMD58: read OCR, CCS bit = 1 means high capacity card (SDHC/SDXC) */
        SD_CS_LOW();
        r = SD_SendCmdRaw(CMD58, 0, 0x01);
        if (r == 0x00)
        {
            for (i = 0; i < 4; i++)
            {
                buf[i] = SPI_ReadWriteByte(0xFF);
            }
            SD_Type = (buf[0] & 0x40) ? SD_TYPE_SDHC : SD_TYPE_SD2;
        }
        else
        {
            SD_Type = SD_TYPE_SD2;
        }
        SD_EndCmd();
    }
    else
    {
        /* No CMD8 response: V1.x standard capacity card or MMC */
        SD_EndCmd();

        retry = 0;
        do
        {
            SD_CS_LOW();
            r = SD_SendCmdRaw(CMD55, 0, 0x01);
            SD_EndCmd();

            SD_CS_LOW();
            r = SD_SendCmdRaw(ACMD41, 0, 0x77);
            SD_EndCmd();
            Delay_Ms(1);
            retry++;
        } while (r != 0x00 && retry < 3000);

        if (retry >= 3000)
        {
            return SD_ERR_TIMEOUT;
        }

        SD_Type = SD_TYPE_SD1;
    }

    /* CMD16: set the read/write block length to 512 bytes */
    SD_CS_LOW();
    r = SD_SendCmdRaw(CMD16, SD_BLOCK_SIZE, 0x01);
    SD_EndCmd();
    if (r != 0x00)
    {
        return SD_ERR_RESPONSE;
    }

    /* Read capacity and switch to a high speed clock */
    SD_SectorCount = SD_GetCapacity();

    SD_SPI_SetSpeed(SPI_BaudRatePrescaler_Mode3);

    return SD_OK;
}

/*********************************************************************
 * @fn      SD_ReadBlock
 *
 * @brief   Read a single block (512 bytes).
 *
 * @param   sector - Sector number.
 *          buf - Receive buffer.
 *          len - Data length, must be 512.
 *
 * @return  SD_OK or an error code.
 */
uint8_t SD_ReadBlock(uint32_t sector, uint8_t* buf, uint16_t len)
{
    uint8_t  r;
    uint16_t i;
    uint32_t addr;
    uint16_t retry;

    if (len != SD_BLOCK_SIZE)
    {
        return SD_ERR_NOT_SUPPORT;
    }

    /* SDHC/SDXC uses sector address, standard capacity card uses byte address */
    addr = (SD_Type == SD_TYPE_SDHC) ? sector : (sector << 9);

    /* CMD17: read single block */
    SD_CS_LOW();
    r = SD_SendCmdRaw(CMD17, addr, 0x01);
    if (r != 0x00)
    {
        SD_EndCmd();
        return SD_ERR_RESPONSE;
    }

    /* Wait for the data token 0xFE */
    for (retry = 0; retry < 0xFFFF; retry++)
    {
        r = SPI_ReadWriteByte(0xFF);
        if (r == 0xFE)
        {
            break;
        }
    }
    if (r != 0xFE)
    {
        SD_EndCmd();
        return SD_ERR_TIMEOUT;
    }

    /* Read 512 bytes of data */
    for (i = 0; i < SD_BLOCK_SIZE; i++)
    {
        buf[i] = SPI_ReadWriteByte(0xFF);
    }

    /* Read 2-byte CRC and end */
    SPI_ReadWriteByte(0xFF);
    SPI_ReadWriteByte(0xFF);
    SD_EndCmd();

    return SD_OK;
}

/*********************************************************************
 * @fn      SD_WriteBlock
 *
 * @brief   Write a single block (512 bytes).
 *
 * @param   sector - Sector number.
 *          buf - Data buffer to be written.
 *          len - Data length, must be 512.
 *
 * @return  SD_OK or an error code.
 */
uint8_t SD_WriteBlock(uint32_t sector, const uint8_t* buf, uint16_t len)
{
    uint8_t  r;
    uint16_t i;
    uint32_t addr;
    uint32_t retry;

    if (len != SD_BLOCK_SIZE)
    {
        return SD_ERR_NOT_SUPPORT;
    }

    addr = (SD_Type == SD_TYPE_SDHC) ? sector : (sector << 9);

    /* CMD24: write single block */
    SD_CS_LOW();
    r = SD_SendCmdRaw(CMD24, addr, 0x01);
    if (r != 0x00)
    {
        SD_EndCmd();
        return SD_ERR_RESPONSE;
    }

    /* Send the start data token 0xFE and 512 bytes of data */
    SPI_ReadWriteByte(0xFF);
    SPI_ReadWriteByte(0xFE);

    for (i = 0; i < SD_BLOCK_SIZE; i++)
    {
        SPI_ReadWriteByte(buf[i]);
    }

    /* Send 2-byte CRC (not checked in SPI mode, any value is acceptable) */
    SPI_ReadWriteByte(0xFF);
    SPI_ReadWriteByte(0xFF);

    /* Read the data response token, low 5 bits = 0x05 means the write is accepted */
    r = SPI_ReadWriteByte(0xFF);
    if ((r & 0x1F) != 0x05)
    {
        SD_EndCmd();
        return SD_ERR_CRC;
    }

    /* Keep CS low and wait for the internal programming to finish (busy flag 0x00) */
    for (retry = 0; retry < 0xFFFFF; retry++)
    {
        if (SPI_ReadWriteByte(0xFF) == 0xFF)
        {
            break;
        }
    }
    SD_CS_HIGH();
    SPI_ReadWriteByte(0xFF);
    if (retry >= 0xFFFFF)
    {
        return SD_ERR_TIMEOUT;
    }

    return SD_OK;
}

/*********************************************************************
 * @fn      SD_GetType
 *
 * @brief   Get the identified card type.
 *
 * @return  SD_TYPE_xxx.
 */
uint8_t SD_GetType(void)
{
    return SD_Type;
}
