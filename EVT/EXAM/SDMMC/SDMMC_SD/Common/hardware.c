/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/12
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "sdmmc_sd.h"
#include "string.h"
#include "math.h"
/* Global define */
#define Buf_Size 32

/* Global Variable */
__attribute__((aligned(16))) u8 buf[512*10];
__attribute__((aligned(16))) u8 Readbuf[512*10];

/*********************************************************************
 * @fn      show_sdcard_info
 *
 * @brief   SD Card information.
 *
 * @return  none
 */
void show_sdcard_info(void)
{
    switch(SDCardInfo.CardType)
    {
        case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
        case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
        case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
        case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
    }
    printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);
    printf("Card RCA:%d\r\n",SDCardInfo.RCA);
    printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));
    printf("Card BlockSize:%dB\r\n\r\n",SDCardInfo.CardBlockSize);
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
	 int i=0;
	u32 Sector_Nums;

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);

	while(SD_Init())
    {
        printf("SD Card Error!\r\n");
        Delay_Ms(1000);
    }
	show_sdcard_info();
    printf("SD Card initial OK!\r\n");
 	for(i=0; i<512*10; i++)
 	{
        Readbuf[i] = 0;
		 buf[i] = i+80;
    }
    Sector_Nums = ((u32)(SDCardInfo.CardCapacity)/512);
    for(i=0; i<Sector_Nums; i++)
    {
        if(SD_WriteDisk(buf,i,1))
        {
            printf("Wr %d sector fail\n", i);
        }

        else
        {
            printf("Wr %d sector success\n", i);
         
        }
        if(SD_ReadDisk(Readbuf,i,1))
        {
            printf("Rd %d sector fail\n", i);
        }
        else
        {
            printf("Rd %d sector success\n", i);
            
        }
  
        if(memcmp(buf, Readbuf, 512))
        {
            printf(" %d sector Verify fail\n\r", i);
            break;
        }
	}
    printf("end\n");
	while (1)
		;
}
