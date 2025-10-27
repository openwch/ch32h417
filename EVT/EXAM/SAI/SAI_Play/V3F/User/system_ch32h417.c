/********************************** (C) COPYRIGHT *******************************
* File Name          : system_ch32h417.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/10/16
* Description        : CH32H417 Device Peripheral Access Layer System Source File.
*                      For HSE = 25Mhz
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32h417.h" 

/* 
* Uncomment the line corresponding to the desired System clock (SYSCLK) frequency (after 
* reset the HSI is used as SYSCLK source).
* If none of the define below is enabled, the HSI is used as System clock source. 
*/
// #define SYSCLK_295M_CoreCLK_HSE    295000000
#define SYSCLK_295M_CoreCLK_HSI    295000000


/* Clock Definitions */
uint32_t HCLKClock;
#ifdef SYSCLK_295M_CoreCLK_HSE
uint32_t SystemClock = SYSCLK_295M_CoreCLK_HSE;         /* System Clock Frequency */
uint32_t SystemCoreClock = 73750000;
#elif defined SYSCLK_295M_CoreCLK_HSI
uint32_t SystemClock = SYSCLK_295M_CoreCLK_HSI;        /* System Clock Frequency */
uint32_t SystemCoreClock = 73750000;


#else

uint32_t SystemClock = HSI_VALUE;       /* System Clock Frequency */
uint32_t SystemCoreClock = HSI_VALUE;
#endif

static __I uint8_t PLLMULTB[32] = {4,6,7,8,17,9,19,10,21,11,23,12,25,13,14,15,16,17,18,19,20,22,24,26,28,30,32,34,36,38,40,59};
static __I uint8_t HBPrescTB[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
static __I uint8_t SERDESPLLMULTB[16] = {25, 28, 30, 32, 35, 38, 40, 45, 50, 56, 60, 64, 70, 76, 80, 90};
static __I uint8_t FPRETB[4] = {0, 1, 2, 2};



/* system_private_function_proto_types */
static void SetSysClock(void);

#ifdef SYSCLK_295M_CoreCLK_HSE
static void SetSYSCLK_295M_CoreCLK_HSE( void );
#elif defined SYSCLK_295M_CoreCLK_HSI
static void SetSYSCLK_295M_CoreCLK_HSI( void );

#endif

/*********************************************************************
 * @fn      SystemInit
 *
 * @brief   Setup the microcontroller system Initialize the Embedded Flash Interface,
 *        the PLL and update the SystemCoreClock variable.
 *
 * @return  none
 */
void SystemInit (void)
{
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
  
  SetSysClock();
}

/*********************************************************************
 * @fn      SetSysClock
 *
 * @brief   Sets System clock frequency.
 *          Sets V5F Core clock frequency.
 *          Sets V3F Core clock frequency.
 *          configure HCLK prescalers.
 *
 * @return  none
 */
static void SetSysClock(void)
{
     GPIO_IPD_Unused();
#ifdef SYSCLK_295M_CoreCLK_HSE
    SetSYSCLK_295M_CoreCLK_HSE();
#elif defined SYSCLK_295M_CoreCLK_HSI
    SetSYSCLK_295M_CoreCLK_HSI();


#endif
 
 /* If none of the define above is enabled, the HSI is used as System clock
  * source (default after reset) 
    */
}

/*********************************************************************
 * @fn      SystemAndCoreClockUpdate
 *
 * @brief   Update SystemClock and CoreClock variable according to Clock Register Values.
 *
 * @return  none
 */
void SystemAndCoreClockUpdate (void)
{
    uint32_t tmp = 0,tmp1 = 0, tmp2 = 0, tmp3 = 0, pllmull = 0, pllsource = 0, presc = 0, presc1 = 0;

    tmp = RCC->CFGR0 & RCC_SWS;
    tmp2 = RCC->PLLCFGR & RCC_SYSPLL_SEL;

    switch(tmp)
    {
        case 0x00:
            SystemClock = HSI_VALUE;
            break;

        case 0x04:
            SystemClock = HSE_VALUE;
            break;

        case 0x08:
            switch(tmp2)
            { 
                case RCC_SYSPLL_PLL:
                    pllmull = RCC->PLLCFGR & RCC_PLLMUL;
                    pllsource = RCC->PLLCFGR & RCC_PLLSRC;
                    presc = (((RCC->PLLCFGR & RCC_PLL_SRC_DIV) >> 8) + 1);

                    if(pllsource == 0xA0)
                    {
                        tmp1 = 500000000 / presc;
                    }
                    else if(pllsource == 0xE0)
                    {
                        tmp1 = HSE_VALUE*SERDESPLLMULTB[RCC->PLLCFGR2>>16]/2/presc;
                    }

                    else if(pllsource == 0x80)
                    {
                        tmp1 = 480000000 / presc;
                    }
                    else if(pllsource == 0xC0)
                    {
                        tmp1 = 125000000 / presc;
                    }
                    else if(pllsource == 0x20)
                    {
                        tmp1 = HSE_VALUE / presc;
                    }
                    else
                    {
                        tmp1 = HSI_VALUE / presc;
                    }

                    if((pllmull == 4) || (pllmull == 6) || (pllmull == 8) || (pllmull == 10) || (pllmull == 12))
                    {
                        SystemClock = (tmp1 * PLLMULTB[pllmull]) >> 1;
                    }
                    else
                    {
                        SystemClock = tmp1 * PLLMULTB[pllmull];
                    }

                    break;

                case RCC_SYSPLL_USBHS:
                    SystemClock = 480000000;
                    break;

                case RCC_SYSPLL_ETH:
                    SystemClock = 500000000;
                    break;

                case RCC_SYSPLL_SERDES:
                    SystemClock = HSE_VALUE*SERDESPLLMULTB[RCC->PLLCFGR2>>16]/2;
                    break;

                case RCC_SYSPLL_USBSS:
                    SystemClock = 125000000;
                    break;

                default:
                    SystemClock = HSI_VALUE;
                    break;
            }  
            break;

        default:
            SystemClock = HSI_VALUE;
            break;
    }

    tmp = (RCC->CFGR0 & RCC_HPRE) >> 4;
    presc1 = HBPrescTB[tmp];

    tmp3 = SystemClock >> presc1;

    tmp = (RCC->CFGR0 & RCC_FPRE) >> 16;
    presc1 = FPRETB[tmp];
    HCLKClock = tmp3 >> presc1;

    if(NVIC_GetCurrentCoreID() == 0)//V3F
    {
        SystemCoreClock = HCLKClock;
    }
    else 
    {
         SystemCoreClock = tmp3;
    }
}

#ifdef SYSCLK_295M_CoreCLK_HSE

/*********************************************************************
 * @fn      SetSYSCLK_295M_CoreCLK_HSE
 *
 * @brief   Sets System clock frequency to 295MHz.
 *          Sets V5F Core clock frequency to 295MHz.
 *          Sets V3F Core clock frequency to 73.75MHz.
 *          configure HCLK prescalers.
 *
 * @return  none
 */
static void SetSYSCLK_295M_CoreCLK_HSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0, FLASH_Temp = 0;
   
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
    RCC->PLLCFGR |= (uint32_t)RCC_PLLMUL59;   
    RCC->PLLCFGR &= (uint32_t)((uint32_t)~(RCC_PLL_SRC_DIV)); 
    RCC->PLLCFGR |= (uint32_t)RCC_PLL_SRC_DIV5;
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

#elif defined SYSCLK_295M_CoreCLK_HSI

/*********************************************************************
 * @fn      SetSYSCLK_295M_CoreCLK_HSI
 *
 * @brief   Sets System clock frequency to 295MHz.
 *          Sets V5F Core clock frequency to 295MHz.
 *          Sets V3F Core clock frequency to 73.75MHz.
 *          configure HCLK prescalers.
 *
 * @return  none
 */
static void SetSYSCLK_295M_CoreCLK_HSI(void)
{
  __IO uint32_t FLASH_Temp = 0;
  /* configure PLL Clock */  
  RCC->PLLCFGR &= (uint32_t)((uint32_t)~(RCC_PLLMUL)); 
  RCC->PLLCFGR |= (uint32_t)RCC_PLLMUL59;   
  RCC->PLLCFGR &= (uint32_t)((uint32_t)~(RCC_PLL_SRC_DIV)); 
  RCC->PLLCFGR |= (uint32_t)RCC_PLL_SRC_DIV5;
  RCC->PLLCFGR &= (uint32_t)((uint32_t)~(RCC_PLLSRC)); 
  RCC->PLLCFGR |= (uint32_t)RCC_PLLSRC_HSI;

  /* Wait till HSI clock is used as PLL clock source */
  while ((RCC->PLLCFGR & (uint32_t)RCC_PLLSRC) != (uint32_t)RCC_PLLSRC_HSI)
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


#endif