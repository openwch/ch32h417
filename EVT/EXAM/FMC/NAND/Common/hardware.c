/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/09/17
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* Global define */
#include "hardware.h"
#include "debug.h"
#include "NAND_Flash.h"
#include "string.h"

/* Global Variable */
u8 writebuf[2048];
u8 readbuf[2048];

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   write and read NANDFLASH (DR).
 *
 * @return  none
 */
void Hardware(void)
{
	u32 i=0;
    u8 t=0;
	memset(readbuf, 0, 2048);
	memset(writebuf, 0, 2048);
	printf("clear  \n");
    printf("\n");

    while(NAND_Init())
    {
        printf("NAND Error!\n");
        Delay_Ms(500);
        printf("Please Check\n");
        Delay_Ms(500);
    }
    printf("NAND Size:%dMB\n", (nand_dev.block_totalnum/1024)*(nand_dev.page_mainsize/1024)*nand_dev.block_pagenum);

    /* Erase */
    t = NAND_EraseBlock(0);
    if(t)
    {
        printf("Erase fail\n");
    }
    else
    {
        printf("Erase success\n");
    }

  /* Read */
    printf("read data\n");
    t = NAND_ReadPage( 0, 0,readbuf,2048);
    if(t)
    {
        printf("read fail\n");
        printf("Err %02x\n", t);
    }
    else
    {
        for(i=0; i<2048; i++)
        {
            printf("%02x ", readbuf[i]);
        }
        printf("\n");
    }

    /* Write */
    printf("Write data:\n");
    for(i=1; i<2048; i++)
    {
        writebuf[i]=0x11;
    }
    writebuf[0] = 0X15;
    writebuf[512] = 0X13;
    writebuf[1024] = 0X14;
    writebuf[1536] = 0X18;

    for(i=0;  i<2048; i++)
    {
        printf("%02x ", writebuf[i]);
    }
    printf("\r\n");
   t = NAND_WritePage(0,0,writebuf,2048);
    // t = NAND_WritePagewithEcc(0,0,writebuf,2048);
    if(t)
    {
        printf("write fail\n");
    }
    else
    {
        printf("write success\n");
    }

    /* Read */
    printf("read data\n");
   t = NAND_ReadPage( 0,0,readbuf,2048);
    //  t = NAND_ReadPageWithEcc( 0,0,readbuf,2048);
         if(t)
         {
             printf("read fail\n");
             printf("Err %02x\n", t);
           }
         else
         {
             for(i=0; i<2048; i++)
             {
                 printf("%x ", readbuf[i]);
             }
             printf("\r\n");
         }

    while(1);

}
