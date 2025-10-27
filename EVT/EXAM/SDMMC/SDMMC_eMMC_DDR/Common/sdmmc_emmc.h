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

#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((u8)6)
#define SD_CMD_SD_APP_STAUS                        ((u8)13)
#define SD_CMD_TUNING_CMD                          ((u8)21)
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((u8)22)
#define SD_CMD_SD_APP_OP_COND                      ((u8)41)
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((u8)42)
#define SD_CMD_SD_APP_SEND_SCR                     ((u8)51)
#define SD_CMD_SDIO_RW_DIRECT                      ((u8)52)
#define SD_CMD_SDIO_RW_EXTENDED                    ((u8)53)

#define SDMMC_TOCNT_MAX                            ((uint32_t)0x0000000F)
/* SDMMC_DIV */
#define RB_SDMMC_DIV_MASK                          ((uint32_t)0x0000001F)      

/*eMMC！！CSD*/
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
} eMMC_CSD;

/*eMMC！！ Extended CSD*/
typedef union
{
        struct _EXT_CSD
        {
            __IO uint8_t    Reserved19[15];
                       __IO uint8_t    CMDQ_MODE_EN;
                       __IO uint8_t    SECURE_REMOVAL_TYPE;
                       __IO uint8_t    PRODUCT_STATE_AWARENESS_ENABLEMENT;
                       __IO uint8_t    MAX_PRE_LOADING_DATA_SIZE[4];
                       __IO uint8_t    PRE_LOADING_DATA_SIZE[4];
                       __IO uint8_t    FFU_STATUS;
                       __IO uint8_t    Reserved18[2];
                       __IO uint8_t    MODE_OPERATION_CODES;
                       __IO uint8_t    MODE_CONFIG;
                       __IO uint8_t    BARRIER_CTRL;
                       __IO uint8_t     FLUSH_CACHE;
                       __IO uint8_t     CACHE_CTRL;
                       __IO uint8_t     POWER_OFF_NOTIFICATION;
                       __IO uint8_t     PACKED_FAILURE_INDEX;
                       __IO uint8_t     PACKED_COMMAND_STATUS;
                       __IO uint8_t     CONTEXT_CONF[15];
                       __IO uint8_t     EXT_PARTITIONS_ATTRIBUTE[2];
                       __IO uint8_t     EXCEPTION_EVENTS_STATUS[2];
                       __IO uint8_t     EXCEPTION_EVENTS_CTRL[2];
                       __IO uint8_t     DYNCAP_NEEDED;
                       __IO uint8_t     CLASS_6_CTRL;
                       __IO uint8_t     INI_TIMEOUT_EMU;
                       __IO uint8_t     DATA_SECTOR_SIZE;
                       __IO uint8_t     USE_NATIVE_SECTOR;
                       __IO uint8_t     NATIVE_SECTOR_SIZE;
                       __IO uint8_t     VENDOR_SPECIFIC_FIELD[64];
                       __IO uint8_t    Reserved17[2];
                       __IO uint8_t     PROGRAM_CID_CSD_DDR_SUPPORT;
                       __IO uint8_t     PERIODIC_WAKEUP;
                       __IO uint8_t     TCASE_SUPPORT;
                       __IO uint8_t    PRODUCTION_STATE_AWARENESS;
                       __IO uint8_t     SEC_BAD_BLK_MGMNT;
                       __IO uint8_t    Reserved16;
                       __IO uint8_t     ENH_START_ADDR[4];
                       __IO uint8_t     ENH_SIZE_MULT[3];
                       __IO uint8_t     GP_SIZE_MULT[12];
                       __IO uint8_t     PARTITION_SETTING_COMPLETED;
                       __IO uint8_t     PARTITIONS_ATTRIBUTE;
                       __IO uint8_t     MAX_ENH_SIZE_MULT[3];
                       __IO uint8_t     PARTITIONING_SUPPORT;
                       __IO uint8_t     HPI_MGMT;
                       __IO uint8_t     RST_n_FUNCTION;
                       __IO uint8_t     BKOPS_EN;
                       __IO uint8_t     BKOPS_START;
                       __IO uint8_t     SANITIZE_START;
                       __IO uint8_t     WR_REL_PARAM;
                       __IO uint8_t     WR_REL_SET;
                       __IO uint8_t     RPMB_SIZE_MULT;
                       __IO uint8_t     FW_CONFIG;
                       __IO uint8_t    Reserved15;
                       __IO uint8_t     USER_WP;
                       __IO uint8_t    Reserved14;
                       __IO uint8_t     BOOT_WP;
                       __IO uint8_t     BOOT_WP_STATUS;
                       __IO uint8_t     ERASE_GROUP_DEF;
                       __IO uint8_t    Reserved13;
                       __IO uint8_t    BOOT_BUS_CONDITIONS;
                       __IO uint8_t    BOOT_CONFIG_PROT;
                       __IO uint8_t     PARTITION_CONFIG;
                       __IO uint8_t    Reserved12;
                       __IO uint8_t    ERASED_MEM_CONT;
                       __IO uint8_t    Reserved11;
                       __IO uint8_t    BUS_WIDTH;
                       __IO uint8_t    STROBE_SUPPORT;
                       __IO uint8_t    HS_TIMING;
                       __IO uint8_t    Reserved10;
                       __IO uint8_t    POWER_CLASS;
                       __IO uint8_t    Reserved9;
                       __IO uint8_t    CMD_SET_REV;
                       __IO uint8_t    Reserved8;
                       __IO uint8_t    CMD_SET;
                       __IO uint8_t    EXT_CSD_REV ;
                       __IO uint8_t    Reserved7;
                       __IO uint8_t    CSD_STRUCTURE;
                       __IO uint8_t    Reserved6;
                       __IO uint8_t    DEVICE_TYPE;
                       __IO uint8_t    DRIVER_STRENGTH;
                       __IO uint8_t    OUT_OF_INTERRUPT_TIME;
                       __IO uint8_t    PARTITION_SWITCH_TIME;
                       __IO uint8_t    PWR_CL_52_195;
                       __IO uint8_t    PWR_CL_26_195;
                       __IO uint8_t    PWR_CL_52_360;
                       __IO uint8_t    PWR_CL_26_360;
                       __IO uint8_t    Reserved5;
                       __IO uint8_t    MIN_PERF_R_4_26;
                       __IO uint8_t    MIN_PERF_W_4_26;
                       __IO uint8_t    MIN_PERF_R_8_26_4_52;
                       __IO uint8_t    MIN_PERF_W_8_26_4_52;
                       __IO uint8_t    MIN_PERF_R_8_52;
                       __IO uint8_t    MIN_PERF_W_8_52;
                       __IO uint8_t    SECURE_WP_INFO;
                       __IO uint8_t    SEC_COUNT[4];
                       __IO uint8_t    SLEEP_NOTIFICATION_TIME;
                       __IO uint8_t    S_A_TIMEOUT;
                       __IO uint8_t    PRODUCTION_STATE_AWARENESS_TIMEOUT;
                       __IO uint8_t    S_C_VCCQ;
                       __IO uint8_t    S_C_VCC;
                       __IO uint8_t    HC_WP_GRP_SIZE;
                       __IO uint8_t    REL_WR_SEC_C;
                       __IO uint8_t    ERASE_TIMEOUT_MULT;
                       __IO uint8_t    HC_ERASE_GRP_SIZE;
                       __IO uint8_t    ACC_SIZE;
                       __IO uint8_t    BOOT_SIZE_MULTI;
                       __IO uint8_t    Reserved4;
                       __IO uint8_t    BOOT_INFO;
                       __IO uint8_t    SEC_TRIM_MULT;
                       __IO uint8_t    SEC_ERASE_MULT;
                       __IO uint8_t    SEC_FEATURE_SUPPORT;
                       __IO uint8_t    TRIM_MULT;
                       __IO uint8_t    Reserved3;
                       __IO uint8_t    MIN_PERF_DDR_R_8_52;
                       __IO uint8_t    MIN_PERF_DDR_W_8_52;
                       __IO uint8_t    PWR_CL_200_130;
                       __IO uint8_t    PWR_CL_200_195;
                       __IO uint8_t    PWR_CL_DDR_52_195;
                       __IO uint8_t    PWR_CL_DDR_52_360;
                       __IO uint8_t    CACHE_FLUSH_POLICY;
                       __IO uint8_t    INI_TIMEOUT_AP;
                       __IO uint8_t    CORRECTLY_PRG_SECTORS_NUM[4];
                       __IO uint8_t    BKOPS_STATUS;
                       __IO uint8_t    OWER_OFF_LONG_TIME;
                       __IO uint8_t    GENERIC_CMD6_TIME;
                       __IO uint8_t    CACHE_SIZE[4];
                       __IO uint8_t    PWR_CL_DDR_200_360;
                       __IO uint8_t    FIRMWARE_VERSION[8];
                       __IO uint8_t    DEVICE_VERSION[2];
                       __IO uint8_t    OPTIMAL_TRIM_UNIT_SIZE;
                       __IO uint8_t    OPTIMAL_WRITE_SIZE;
                       __IO uint8_t    OPTIMAL_READ_SIZE;
                       __IO uint8_t    PRE_EOL_INFO;
                       __IO uint8_t    DEVICE_LIFE_TIME_EST_TYP_A;
                       __IO uint8_t    DEVICE_LIFE_TIME_EST_TYP_B;
                       __IO uint8_t    VENDOR_PROPRIETARY_HEALTH_REPORT[32];
                       __IO uint8_t    NUMBER_OF_FW_SECTORS_CORRECTLY_PROGRAMMED[4];
                       __IO uint8_t    ReservedX;
                       __IO uint8_t    CMDQ_DEPTH;
                       __IO uint8_t    CMDQ_SUPPORT;
                       __IO uint8_t    Reserved2[177];
                       __IO uint8_t    BARRIER_SUPPORT;
                       __IO uint8_t    FFU_ARG[4];
                       __IO uint8_t    OPERATION_CODE_TIMEOUT;
                       __IO uint8_t    FFU_FEATURES;
                       __IO uint8_t    SUPPORTED_MODES;
                       __IO uint8_t    EXT_SUPPORT;
                       __IO uint8_t    LARGE_UNIT_SIZE_M1;
                       __IO uint8_t    CONTEXT_CAPABILITIES;
                       __IO uint8_t    TAG_RES_SIZE;
                       __IO uint8_t    TAG_UNIT_SIZE;
                       __IO uint8_t    DATA_TAG_SUPPORT;
                       __IO uint8_t    MAX_PACKED_WRITES;
                       __IO uint8_t    MAX_PACKED_READS;
                       __IO uint8_t    BKOPS_SUPPORT;
                       __IO uint8_t    HPI_FEATURES;
                       __IO uint8_t    S_CMD_SET;
                       __IO uint8_t    EXT_SECURITY_ERR;
                       __IO uint8_t    Reserved1[6];
        } EXT_CSD;
   __IO u8 CsdBuf[512];
}   eMMC_EXT_CSD;


/*eMMC-CID*/
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
} eMMC_CID;

/*eMMC！！CID*/

typedef struct
{
  eMMC_CSD eMMC_csd;
  eMMC_EXT_CSD eMMC_ExtCsd;
  eMMC_CID eMMC_cid;
//  u32 CardCapacity;
  u32 SectorNums;
  u32 CardBlockSize;
  u16 RCA;
  u8 CardType;
} eMMC_CardInfo;




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


#define SD_MAX_VOLT_TRIAL                  ((uint32_t)0x0000FFFF)
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

SD_Error eMMC_Init();
void eMMC_GPIO_Init();
SD_Error SD_PowerON();
SD_Error eMMC_Read_CID_CSD( void );
SD_Error SD_SelectDeselect( u32 addr );
SD_Error eMMC_ReadExtCsd(eMMC_CardInfo *eMMC);
SD_Error eMMC_GetCardInfo(eMMC_CardInfo *eMMC, u32 *CSD_Tab, u32 *CID_Tab, u16 Rca);
SD_Error MMC_ReadEXTCSD_Polling( void );
SD_Error SDMMC_SetCommand( uint8_t command, uint32_t arg, uint16_t respexpect );
SD_Error SD_EnableWideBusOperation( u32 wmode );
SD_Error SD_EnableWideBusOperationDDR( u32 wmode );
SD_Error SD_ReadBlock( uint8_t *buf, long long addr, uint16_t blksize );
SD_Error SD_ReadMultiBlocks( uint8_t *buf, long long addr, uint16_t blksize, uint32_t nblks );
SD_Error SD_WriteBlock( uint8_t *buf, long long addr,  uint16_t blksize );
SD_Error SD_WriteMultiBlocks( uint8_t *buf, long long addr, uint16_t blksize, uint32_t nblks );
SD_Error SD_ReadDisk( uint8_t *buf, uint32_t sector, uint8_t cnt );
SD_Error SD_WriteDisk( uint8_t *buf, uint32_t sector, uint8_t cnt );
uint8_t SDMMC_WaitData0( void );
SD_Error SDMMC_SpeedTuning();