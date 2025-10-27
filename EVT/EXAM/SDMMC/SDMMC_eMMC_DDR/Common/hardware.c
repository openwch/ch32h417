/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.2
* Date               : 2025/10/23
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
static void SDMMC_PLLclk_300M_HSE(void)
{

  __IO uint32_t StartUpCounter = 0, HSEStatus = 0, FLASH_Temp = 0;


  RCC->CTLR |= (uint32_t)0x00000001;
  RCC->CFGR0 &= (uint32_t)0x305C0000;
  while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x00)
  {
  }
  RCC->CFGR0 &= (uint32_t)0xFFBFFFFF;

  RCC->PLLCFGR &= (uint32_t)0x7FFFFFFF;

  RCC->CTLR &= (uint32_t)0x6AA6FFFF;
  RCC->CTLR &= (uint32_t)0xFFFBFFFF;

  RCC->PLLCFGR &= (uint32_t)0x0FFFC000;
  RCC->PLLCFGR |= (uint32_t)0x00000004;

  RCC->INTR = 0x00FF0000;
  RCC->CFGR2 &= 0x0C600000;
  RCC->PLLCFGR2 &= 0xFFF0E080;
  RCC->PLLCFGR2 |= 0x00080020;
   
  RCC->CTLR |= ((uint32_t)RCC_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* configure PLL Clock */  
    RCC->PLLCFGR &= (uint32_t)((uint32_t)~(RCC_PLLMUL)); 
    RCC->PLLCFGR |= (uint32_t)RCC_PLLMUL12;   
    RCC->PLLCFGR &= (uint32_t)((uint32_t)~(RCC_PLL_SRC_DIV)); 
    RCC->PLLCFGR |= (uint32_t)RCC_PLL_SRC_DIV1;
    RCC->PLLCFGR &= (uint32_t)((uint32_t)~(RCC_PLLSRC)); 
    RCC->PLLCFGR |= (uint32_t)RCC_PLLSRC_HSE;

    /* Wait till HSE clock is used as PLL clock source */
    while ((RCC->PLLCFGR & (uint32_t)RCC_PLLSRC) != (uint32_t)RCC_PLLSRC_HSE)
    {
    }     

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CTLR & RCC_PLLRDY) != (uint32_t)RCC_PLLRDY)
    {
    }

    /* Select PLL Clock as SYSPLL clock source */
    RCC->PLLCFGR &= (uint32_t)((uint32_t)~(RCC_SYSPLL_GATE)); 
    RCC->PLLCFGR &= (uint32_t)((uint32_t)~(RCC_SYSPLL_SEL)); 

    /* Wait till PLL is used as system clock source */
    while ((RCC->PLLCFGR & (uint32_t)RCC_SYSPLL_SEL) != (uint32_t)0x00)
    {
    }

    /* V5F core clock = SYSCLK */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_HPRE));
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1; 

    /* V3F core clock = HCLK = SYSCLK/4 */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_FPRE));
    RCC->CFGR0 |= (uint32_t)RCC_FPRE_DIV4;  

    /* Select FLASH clock frequency*/
    FLASH_Temp = FLASH->ACTLR;
    FLASH_Temp &= ~((uint32_t)0x3);
    FLASH_Temp |= FLASH_ACTLR_LATENCY_HCLK_DIV2;
    FLASH->ACTLR = FLASH_Temp;

    /* Select PLL as system clock source */
    RCC->PLLCFGR |= (uint32_t)RCC_SYSPLL_GATE; 
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }

  }
  else
  { 
        /* If HSE fails to start-up, the application will have wrong clock
     * configuration. User can add here some code to deal with this error 
         */
  }  
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
    SDMMC_PLLclk_300M_HSE();
    SystemAndCoreClockUpdate();
    Delay_Init();
	USART_Printf_Init(115200);
    printf("SDMMC CLOCK \r\n");
    printf("SystemClk:%d\r\n", SystemClock);
	printf("V3F SystemCoreClk:%d\r\n", SystemCoreClock);
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
