/********************************** (C) COPYRIGHT *******************************
* File Name          : sdmmc_emmc.h
* Author             : WCH
* Version            : V1.0.2
* Date               : 2025/10/22
* Description        : This file contains the headers of the sdmmc_emmc.
*********************************************************************************
* Copyright (c) 2024 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include"debug.h"

#define SDMMC_AutoClockControl_ClockEnable()    SDMMC_ClockCmd( ENABLE )        
#define SDMMC_AutoClockControl_ClockDisable()   SDMMC_ClockCmd( DISABLE )      
/* SD definition */
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((u32)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((u32)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((u32)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((u32)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((u32)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((u32)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((u32)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((u32)0x00000007)



#define SD_VOLTAGE_WINDOW_SD                       ((u32)0x80100000)
#define SD_HIGH_CAPACITY                           ((u32)0x40000000)
#define SD_STD_CAPACITY                            ((u32)0x00000000)
#define SD_CHECK_PATTERN                           ((u32)0x000001AA)
#define SD_VOLTAGE_WINDOW_MMC                      ((u32)0x80FF8000)



#define SD_CARD_LOCKED                           ((u32)0x02000000)
#define SD_ALLZERO                               ((u32)0x00000000)
#define SD_WIDE_BUS_SUPPORT                      ((u32)0x00040000)
#define SD_MAX_VOLT_TRIAL                        ((u32)0x0000FFFF)
//argument

#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((u8)6)
#define SDIO_SEND_IF_COND                          ((u8)8)
#define SD_CMD_SD_APP_STAUS                        ((u8)13)
#define SD_CMD_TUNING_CMD                          ((u8)21)
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((u8)22)
#define SD_CMD_SD_APP_OP_COND                      ((u8)41)
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((u8)42)
#define SD_CMD_SD_APP_SEND_SCR                     ((u8)51)
#define SD_CMD_SDIO_RW_DIRECT                      ((u8)52)
#define SD_CMD_SDIO_RW_EXTENDED                    ((u8)53)

#define SDMMC_TOCNT_MAX                            ((u32)0x0000000F)
/* SDMMC_DIV */
#define RB_SDMMC_DIV_MASK                          ((u32)0x0000001F)      


/*SD-CSD*/
typedef struct
{
    u8  CSDStruct;
    u8  SysSpecVersion;
    u8  Reserved1;
    u8  TAAC;
    u8  NSAC;
    u8  MaxBusClkFrec;
    u16 CardComdClasses;
    u8  RdBlockLen;
    u8  PartBlockRead;
    u8  WrBlockMisalign;
    u8  RdBlockMisalign;
    u8  DSRImpl;
    u8  Reserved2;
    u32 DeviceSize;
    u8  MaxRdCurrentVDDMin;
    u8  MaxRdCurrentVDDMax;
    u8  MaxWrCurrentVDDMin;
    u8  MaxWrCurrentVDDMax;
    u8  DeviceSizeMul;
    u8  EraseGrSize;
    u8  EraseGrMul;
    u8  WrProtectGrSize;
    u8  WrProtectGrEnable;
    u8  ManDeflECC;
    u8  WrSpeedFact;
    u8  MaxWrBlockLen;
    u8  WriteBlockPaPartial;
    u8  Reserved3;
    u8  ContentProtectAppli;
    u8  FileFormatGrouop;
    u8  CopyFlag;
    u8  PermWrProtect;
    u8  TempWrProtect;
    u8  FileFormat;
    u8  ECC;
    u8  CSD_CRC;
    u8  Reserved4;
} SD_CSD;   

/*SD-CID*/
typedef struct
{
    u8  ManufacturerID;
    u16 OEM_AppliID;
    u32 ProdName1;
    u8  ProdName2;
    u8  ProdRev;
    u32 ProdSN;
    u8  Reserved1;
    u16 ManufactDate;
    u8  CID_CRC;
    u8  Reserved2;
} SD_CID;

typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  long long CardCapacity;
  u32 CardBlockSize;
  u16 RCA;
  u8 CardType;
} SD_CardInfo;
extern SD_CardInfo SDCardInfo;

/* SDIO instruction*/
#define SD_CMD_GO_IDLE_STATE                       ((u8)0)
#define SD_CMD_SEND_OP_COND                        ((u8)1)
#define SD_CMD_ALL_SEND_CID                        ((u8)2)
#define SD_CMD_SET_REL_ADDR                        ((u8)3)
#define SD_CMD_SET_DSR                             ((u8)4)
#define SD_CMD_SDIO_SEN_OP_COND                    ((u8)5)
#define SD_CMD_HS_SWITCH                           ((u8)6)
#define SD_CMD_SEL_DESEL_CARD                      ((u8)7)
#define SD_CMD_HS_SEND_EXT_CSD                     ((u8)8)
#define SD_CMD_SEND_CSD                            ((u8)9)
#define SD_CMD_SEND_CID                            ((u8)10)
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((u8)11)
#define SD_CMD_STOP_TRANSMISSION                   ((u8)12)
#define SD_CMD_SEND_STATUS                         ((u8)13)
#define SD_CMD_HS_BUSTEST_READ                     ((u8)14)
#define SD_CMD_GO_INACTIVE_STATE                   ((u8)15)
#define SD_CMD_SET_BLOCKLEN                        ((u8)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((u8)17)
#define SD_CMD_READ_MULT_BLOCK                     ((u8)18)
#define SD_CMD_HS_BUSTEST_WRITE                    ((u8)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((u8)20) 
#define SD_CMD_SET_BLOCK_COUNT                     ((u8)23) 
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((u8)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((u8)25)
#define SD_CMD_PROG_CID                            ((u8)26)
#define SD_CMD_PROG_CSD                            ((u8)27)
#define SD_CMD_SET_WRITE_PROT                      ((u8)28)
#define SD_CMD_CLR_WRITE_PROT                      ((u8)29)
#define SD_CMD_SEND_WRITE_PROT                     ((u8)30)
#define SD_CMD_SD_ERASE_GRP_START                  ((u8)32)
#define SD_CMD_SD_ERASE_GRP_END                    ((u8)33)
#define SD_CMD_ERASE_GRP_START                     ((u8)35)
#define SD_CMD_ERASE_GRP_END                       ((u8)36)
#define SD_CMD_ERASE                               ((u8)38)
#define SD_CMD_FAST_IO                             ((u8)39)
#define SD_CMD_GO_IRQ_STATE                        ((u8)40)
#define SD_CMD_LOCK_UNLOCK                         ((u8)42)
#define SD_CMD_APP_CMD                             ((u8)55)
#define SD_CMD_GEN_CMD                             ((u8)56)
#define SD_CMD_NO_CMD                              ((u8)64)


#define SDMMC_TOUT_TIMES                   ( 0xFFFFF )                             

/*******************  Bit definition for R32_SDMMC_DMA_BEG1 register  ********************/
#define RB_SDMMC_DMAAD1_MASK               ((uint32_t)0x0001FFFF)
 

/*SDIO Err define */
typedef enum
{
    /* special err */
    SD_CMD_CRC_FAIL                    = (1),
    SD_DATA_CRC_FAIL                   = (2),
    SD_CMD_RSP_TIMEOUT                 = (3),
    SD_DATA_TIMEOUT                    = (4),
    SD_TX_UNDERRUN                     = (5),
    SD_RX_OVERRUN                      = (6),
    SD_START_BIT_ERR                   = (7),
    SD_CMD_OUT_OF_RANGE                = (8),
    SD_ADDR_MISALIGNED                 = (9),
    SD_BLOCK_LEN_ERR                   = (10),
    SD_ERASE_SEQ_ERR                   = (11),
    SD_BAD_ERASE_PARAM                 = (12),
    SD_WRITE_PROT_VIOLATION            = (13),
    SD_LOCK_UNLOCK_FAILED              = (14),
    SD_COM_CRC_FAILED                  = (15),
    SD_ILLEGAL_CMD                     = (16),
    SD_CARD_ECC_FAILED                 = (17),
    SD_CC_ERROR                        = (18),
    SD_GENERAL_UNKNOWN_ERROR           = (19),
    SD_STREAM_READ_UNDERRUN            = (20),
    SD_STREAM_WRITE_OVERRUN            = (21),
    SD_CID_CSD_OVERWRITE               = (22),
    SD_WP_ERASE_SKIP                   = (23),
    SD_CARD_ECC_DISABLED               = (24),
    SD_ERASE_RESET                     = (25),
    SD_AKE_SEQ_ERROR                   = (26),
    SD_INVALID_VOLTRANGE               = (27),
    SD_ADDR_OUT_OF_RANGE               = (28),
    SD_SWITCH_ERROR                    = (29),
    SD_SDIO_DISABLED                   = (30),
    SD_SDIO_FUNCTION_BUSY              = (31),
    SD_SDIO_FUNCTION_FAILED            = (32),
    SD_SDIO_UNKNOWN_FUNCTION           = (33),
    /* Standard err */
    SD_INTERNAL_ERROR,
    SD_NOT_CONFIGURED,
    SD_REQUEST_PENDING,
    SD_REQUEST_NOT_APPLICABLE,
    SD_INVALID_PARAMETER,
    SD_UNSUPPORTED_FEATURE,
    SD_UNSUPPORTED_HW,
    SD_ERROR,
    SD_OK = 0,
    SD_ERROR_NOTDEF = 0xFF
} SD_Error;

SD_Error SD_Init();
void SD_GPIO_Init();
SD_Error SD_PowerON();
SD_Error SD_Read_CID_CSD( void );
SD_Error SD_SelectDeselect( u32 addr );
SD_Error SD_GetCardInfo( SD_CardInfo *cardinfo );
SD_Error SDMMC_SetCommand( uint8_t command, uint32_t arg, uint16_t respexpect );
SD_Error SD_EnableWideBusOperation( u32 wmode );
SD_Error SD_ReadBlock( uint8_t *buf, long long addr, uint16_t blksize );
SD_Error SD_ReadMultiBlocks( uint8_t *buf, long long addr, uint16_t blksize, uint32_t nblks );
SD_Error SD_WriteBlock( uint8_t *buf, long long addr,  uint16_t blksize );
SD_Error SD_WriteMultiBlocks( uint8_t *buf, long long addr, uint16_t blksize, uint32_t nblks );
SD_Error SD_ReadDisk( uint8_t *buf, uint32_t sector, uint8_t cnt );
SD_Error SD_WriteDisk( uint8_t *buf, uint32_t sector, uint8_t cnt );
uint8_t SDMMC_WaitData0( void );