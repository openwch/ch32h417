/********************************** (C) COPYRIGHT *******************************
* File Name          : IAP_Task.h
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/09/24
* Description        : This file contains the headers of the IAP task.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __IAP_TASK_H__
#define __IAP_TASK_H__

#include "debug.h"
#include "ch32h417.h"

/*
flash:~480K (to 0x08078000)
BIM area (Image Boot Manager): 192K  (V3F @0x0/64K + V5F @0x10000/128K)
USER area (user code area): 128K
BACKUP area (code backup area): 128K
address assignment:
0x08000000  size:0x30000
0x08030000  size:0x20000
0x08050000  size:0x20000
update flag @ 0x08070F00
APP config  @ 0x08077000  (ETH_CFG last 4KB page)
*/

/*address assignment */
#define BIM_IMAGE_START_ADD             ((uint32_t) 0x08000000)
#define BIM_IMAGE_MAX_SIZE              0x30000

#define USER_IMAGE_START_ADD            ((uint32_t) 0x08030000)
#define USER_IMAGE_MAX_SIZE             0x20000

#define BACKUP_IMAGE_START_ADD          ((uint32_t) 0x08050000)
#define BACKUP_IMAGE_MAX_SIZE           0x20000

#define UPDATA_FLAG_STORAGE_ADD         ((uint32_t) 0x08070F00)

#define READ_DATA_LEN                   2048

#define FLASH_PAGE_SIZE                 256

/*The update flag is stored in the last page of the BACKUP area
 *(UPDATA_FLAG_STORAGE_ADD), so the image data that fits into the BACKUP area
 *is BACKUP_IMAGE_MAX_SIZE - FLASH_PAGE_SIZE. Any image larger than this must
 *be rejected before a single byte is written.*/
#define BACKUP_IMAGE_MAX_DATA_SIZE      (BACKUP_IMAGE_MAX_SIZE - FLASH_PAGE_SIZE)

/*Stack size declared by __stack_size in Ld/Link.ld. Keep the two in sync.
 *The copy buffer of IAPCopyFlashDeal is READ_DATA_LEN bytes and must stay
 *static, see the assertion in IAP_Task.c.*/
#define IAP_STACK_SIZE                  4096

/*Flash operation information*/
#define BIN_INF_LEN        512

#define FILE_FLAG          "WCHNET"

#define IMAGE_FLAG_UPDATE   0x57434820   //'W','C','H',' '

typedef struct
{
    u8  iapFileFlag[8];                  //8 bytes IAP flag, fixed as "WCHNET", the extra space is 0
    u32 iapFileLen;                      //4 bytes IAP file length, which is bin file length + 512 bytes
    u32 iapFileCheckSum;                 //4 bytes checksum
}iapFileHeader;

typedef struct{
    u16 readIndex;
    u16 writeIndex;
    u16 buffUsedLen;
}ethDataDeal;

extern ethDataDeal dataDeal;
extern iapFileHeader iapPara;
extern u32 fileDataLen;
extern u32 fileCheckSum;

extern u8 IAPCopyFlashDeal(void);
extern void IAPParaInit(void);
extern void saveUpdatedFile(void);
extern void receUpdatedFile(u8 id);
extern void IAPFileParaCheck(u8 id);
extern void IAP_EEPROM_ERASE_USER(u32 StartAddr);
extern void IAP_EEPROM_READ( u32 StartAddr, u8 *Buffer, u32 Length );
extern void IAP_EEPROM_ERASE(uint32_t Page_Address, u32 Length);
extern void IAP_EEPROM_WRITE( u32 StartAddr, u8 *Buffer, u32 Length );

#endif /* __IAP_TASK_H__ */
