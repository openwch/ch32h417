/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/17
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "sdio.h"
#include "string.h"
/* Global define */
#define Buf_Size 32

/* Global Variable */

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
    printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);
}

u8 buf[512];
u8 Readbuf[512];


/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Write and read SD card.
 *
 * @return  none
 */
void Hardware(void)
{
    u32 i;
    u32 Sector_Nums;


	Delay_Init();
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    while(SD_Init())
    {
        printf("SD Card Error!\r\n");
        delay_ms(1000);
    }
    show_sdcard_info();

    printf("SD Card OK\r\n");

    Sector_Nums = ((u32)(SDCardInfo.CardCapacity>>20))*2048;
    printf("Sector_Nums:0x%08x\n", Sector_Nums);

    for(i=0; i<512; i++){
        buf[i] = i;
    }

    for(i=0; i<Sector_Nums; i++){
        if(SD_WriteDisk(buf,i,1)) printf("Wr %d sector fail\n", i);
        if(SD_ReadDisk(Readbuf,i,1)) printf("Rd %d sector fail\n", i);

        if(memcmp(buf, Readbuf, 512)){
            printf(" %d sector Verify fail\n", i);
            break;
        }
    }
    printf("end\n");
    while(1);
}
