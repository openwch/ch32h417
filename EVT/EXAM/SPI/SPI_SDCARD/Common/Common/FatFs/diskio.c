/*-----------------------------------------------------------------------*/
/* Low level disk I/O module glue for FatFs                              */
/*-----------------------------------------------------------------------*/
/* This file attaches the SD card SPI block driver (sd_card.c) to the    */
/* FatFs module with the defined disk I/O API.                           */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "../sd_card.h"	/* SD card SPI block driver */
#include "debug.h"		/* printf */

/* Physical drive number of the SD card */
#define DEV_MMC     0

#define SD_SECTOR_SIZE  512


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    if(pdrv != DEV_MMC)
    {
        return STA_NOINIT;
    }

    if(SD_Type == SD_TYPE_UNKNOWN)
    {
        return STA_NOINIT;
    }

    return 0;
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive nmuber to identify the drive */
)
{
    uint8_t ret;

    if(pdrv != DEV_MMC)
    {
        return STA_NOINIT;
    }

    /* Skip re-initialization if the card is already initialized */
    if(SD_Type != SD_TYPE_UNKNOWN)
    {
        return 0;
    }

    ret = SD_InitCard();
    if(ret != SD_OK)
    {
        return STA_NOINIT;
    }

    return 0;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    LBA_t sector,   /* Start sector in LBA */
    UINT count      /* Number of sectors to read */
)
{
    UINT i;
    uint8_t ret;
    uint8_t retry;

    if(pdrv != DEV_MMC)
    {
        return RES_PARERR;
    }

    for(i = 0; i < count; i++)
    {
        /* The first block read after the card initialization may not be
           answered by some cards, so retry a few times before giving up */
        for(retry = 0; retry < 5; retry++)
        {
            ret = SD_ReadBlock((uint32_t)(sector + i), buff + i * SD_SECTOR_SIZE, SD_SECTOR_SIZE);
            if(ret == SD_OK)
            {
                break;
            }
            Delay_Ms(50);
        }

        if(ret != SD_OK)
        {
            printf("disk_read fail: sec=%d err=%d\r\n", (int)(sector + i), ret);
            return RES_ERROR;
        }
    }

    return RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    LBA_t sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to write */
)
{
    UINT i;
    uint8_t ret;

    if(pdrv != DEV_MMC)
    {
        return RES_PARERR;
    }

    for(i = 0; i < count; i++)
    {
        ret = SD_WriteBlock((uint32_t)(sector + i), (const uint8_t *)(buff + i * SD_SECTOR_SIZE), SD_SECTOR_SIZE);
        if(ret != SD_OK)
        {
            printf("disk_write fail: sec=%d err=%d\r\n", (int)(sector + i), ret);
            return RES_ERROR;
        }
    }

    return RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    if(pdrv != DEV_MMC)
    {
        return RES_PARERR;
    }

    switch(cmd)
    {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = SD_SectorCount;
            return RES_OK;

        case GET_SECTOR_SIZE:
            *(WORD *)buff = SD_SECTOR_SIZE;
            return RES_OK;

        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            return RES_OK;

        case CTRL_TRIM:
            return RES_OK;
    }

    return RES_PARERR;
}


/*-----------------------------------------------------------------------*/
/* User defined function to give a current time to fatfs module          */
/*-----------------------------------------------------------------------*/

DWORD get_fattime (void)
{
    return 0;
}
