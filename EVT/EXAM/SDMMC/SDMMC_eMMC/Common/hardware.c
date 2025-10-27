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
/* Global define */
#define Buf_Size 32

/* Global Variable */
extern eMMC_CardInfo eMMCCardInfo;
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
	 int i=0;
	u32 Sector_Nums;
	
	RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE1); 
	while(eMMC_Init())
    {
        printf("eMMC Card Error!\r\n");
        Delay_Ms(1000);
    }
	show_eMMCcard_info();
    printf("eMMC Card initial OK!\r\n");
 	for(i=0; i<512; i++)
 	{
        Readbuf[i] = 0;
		 buf[i] = i+80;
    }
    Sector_Nums = ((u32)(eMMCCardInfo.SectorNums));
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

        Delay_Ms(100);
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
	while (1)
		;
}
