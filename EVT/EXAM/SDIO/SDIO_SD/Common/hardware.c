/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
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
u32 SRC_BUF[Buf_Size] = {0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
						 0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20, 0x21222324, 0x25262728,
						 0x292A2B2C, 0x2D2E2F30, 0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
						 0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50, 0x51525354, 0x55565758,
						 0x595A5B5C, 0x5D5E5F60, 0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
						 0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80};

u32 CRCValue = 0;

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

    Sector_Nums = ((u32)(SDCardInfo.CardCapacity>>20))/2;
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
