/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"           /* Obtains integer types */
#include "diskio.h"       /* Declarations of disk functions */
#include "hardware.h"

/* Definitions of physical drive number for each drive */
#define SPI_FLASH     0   /* Example: Map SPI FLASH to physical drive 0 */
// #define DEV_MMC     1   /* Example: Map MMC/SD card to physical drive 1 */
// #define DEV_USB     2   /* Example: Map USB MSD to physical drive 2 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{

    uint16_t id=0;
    DSTATUS stat= STA_NOINIT;
    switch (pdrv) {
    case SPI_FLASH :
        id =  SPI_Flash_ReadID();
    switch (id)
    {
        case 0XEF13:
            stat&= ~STA_NOINIT;
            break;
        case 0XEF14:
            stat&= ~STA_NOINIT;
            break;
        case 0XEF15:
            stat&= ~STA_NOINIT;
            break;
        case 0XEF16:
            stat&= ~STA_NOINIT;
            break;
        case 0XEF17:
            stat&= ~STA_NOINIT;
            break;
        default:
            stat= STA_NOINIT;
            break;
    }
        return stat;
    }
    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;
    switch (pdrv) {
    case SPI_FLASH :
        stat = disk_status(SPI_FLASH);
        return stat;
    }
    return STA_NOINIT;
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
    DRESULT res= RES_PARERR;

    switch (pdrv) {
    case SPI_FLASH :
    sector+=1536;
    SPI_Flash_Read(buff, sector <<12, count<<12);
    res = RES_OK;
    return res;
    }

    return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    LBA_t sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to write */
)
{
     uint32_t write_addr;
    DRESULT res= RES_PARERR;
    switch (pdrv) {
    case SPI_FLASH :
    sector+=1536;

write_addr = sector<<12;

SPI_Flash_Write((u8 *)buff,write_addr,count<<12);
        res = RES_OK;
        return res;
    }

    return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res= RES_PARERR;
    switch (pdrv) {
    case SPI_FLASH :
        switch(cmd){
        case CTRL_SYNC:
                   break;
               case GET_SECTOR_COUNT:
                   *(DWORD*)buff = 2560;
                   break;
               case GET_SECTOR_SIZE:
                   *(WORD*)buff = 4096;
                   break;
               case GET_BLOCK_SIZE:
                *(WORD*)buff =1;
                   break;
               case CTRL_TRIM:
                   break;
        }
        res = RES_OK;
        return res;
    }

    return RES_PARERR;
}

//User defined function to give a current time to fatfs module
DWORD get_fattime (void)
{
    return 0;
}



