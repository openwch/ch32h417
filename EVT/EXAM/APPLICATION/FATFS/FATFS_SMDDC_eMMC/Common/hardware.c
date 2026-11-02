/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.2
* Date               : 2025/10/22
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "sdmmc_emmc.h"
#include "string.h"
#include "math.h"
#include "ff.h"
#include "diskio.h"
FATFS fs;
FIL fnew;
UINT fnum;
FRESULT res_sd;
BYTE work[FF_MAX_SS];
u8 WriteBuf[]={"Hello WCH!"};
u8 ReadSDBuf[11]={0};
MKFS_PARM opt = {0};


/* Global define */

/* Global Variable */

__attribute__((aligned(16))) u8 buf[512];
__attribute__((aligned(16))) u8 Readbuf[512];

/*********************************************************************
 * @fn      show_eMMCcard_info
 *
 * @brief   eMMC Card information.
 *
 * @return  none
 */

void show_eMMCcard_info(void)
{
    switch(eMMCCardInfo.CardType)
    {
        case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
        case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
        case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
        case SDIO_MULTIMEDIA_CARD:printf("Card Type:eMMC Card\r\n");break;
    }
    printf("Card ManufacturerID:0x%x\r\n",eMMCCardInfo.eMMC_cid.ManufacturerID);
    printf("Card SectorNums:0x%08x\n", eMMCCardInfo.SectorNums);
    printf("Card Capacity:%d MB\r\n",(u32)((eMMCCardInfo.SectorNums>>20)*512));
    printf("Card BlockSize:%dB\r\n",eMMCCardInfo.CardBlockSize);

}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief    Write and read eMMC card.
 *
 * @return  non
 */
void Hardware(void)
{

	RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE1); 
	while(eMMC_Init())
    {
        printf("eMMC Card Error!\r\n");
        Delay_Ms(1000);
    }
    
	printf("eMMC Card OK\r\n");
    opt.fmt = FM_FAT32;
    opt.n_fat = 1;
    opt.align = 0;
    opt.au_size = 0;
 	res_sd=f_mount(&fs, "1:", 1);//emmc mount
	if(res_sd==FR_OK)
	{
	    printf("Disk mounted successfully\r\n");
	}
	if(res_sd == FR_NO_FILESYSTEM)
	{
	    printf("Disk formatting\r\n");
	    res_sd=f_mkfs("1:", &opt, work, sizeof(work));
	    if (res_sd == FR_OK)
	    {
	        printf("Disk formatted successfully\r\n");
	        res_sd = f_mount(&fs, "1:", 1);
	        if (res_sd == FR_OK)
	        {
	            printf("Disk mounted successfully\r\n");
	        }
	        else
	        {
	            printf("Disk mounting failed\r\n");
	            printf("error code%x\r\n",res_sd);
	        }
	    }
	    else {
	        printf("Disk formatting failed!(%d)\r\n", res_sd);
	    }
	}
    res_sd= f_open(&fnew,(const char*)"1:/testWCH.txt",FA_CREATE_ALWAYS|FA_WRITE);
      if(res_sd!=FR_OK)
      {
          printf("Create file error\r\n");
      }
      else
      {
          printf("Create file successfully\r\n");   
      }
      printf("Writing......................\r\n");
      res_sd= f_write(&fnew,WriteBuf,11,&fnum);
      for(int j=0;j<sizeof(WriteBuf);j++)
      {
          printf("WriteBuf[%d]=%d\r\n",j,WriteBuf[j]);
      }
      if(res_sd!=FR_OK)
      {
          printf("Write error:%X\r\n",res_sd);
      }
      f_close(&fnew);
      res_sd= f_open(&fnew,(const char*)"1:/testWCH.txt",FA_OPEN_EXISTING|FA_READ);
      if(res_sd!=FR_OK)
      {
          printf("Open file error\r\n");
      }
      printf("Reading......................\r\n");
      res_sd= f_read(&fnew,ReadSDBuf,11,(UINT*)&fnum);
      if(res_sd!=FR_OK)
      {
          printf("Read error:%x\r\n",res_sd);
      }
      for(int i=0;i<sizeof(ReadSDBuf);i++)
      {
          printf("ReadBuf[%d]=%d\r\n",i,ReadSDBuf[i]) ;
      }
    while(1);
}
