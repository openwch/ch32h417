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

/* Global define */

/* Global Variable */
uint8_t step=1;



#ifdef Core_V3F
void SPI2_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void SPI3_IRQHandler(void)    __attribute__((interrupt("WCH-Interrupt-fast")));
#else
void WWDG_IRQHandler(void)   __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI15_8_IRQHandler(void)    __attribute__((interrupt("WCH-Interrupt-fast")));
void SPI1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void RTC_IRQHandler(void)    __attribute__((interrupt("WCH-Interrupt-fast")));
void FLASH_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));
void RCC_IRQHandler(void)    __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void)  __attribute__((interrupt("WCH-Interrupt-fast")));
#endif

/*********************************************************************
 * @fn      Interrupt_Init
 *
 * @brief   Initializes interruption.
 *
 * @return  none
 */
void Interrupt_Init(void)
{

#ifdef Core_V3F	
    NVIC_EnableIRQ(SPI2_IRQn);
    NVIC_EnableIRQ(SPI3_IRQn);
#else
    NVIC_EnableIRQ(WWDG_IRQn);
    NVIC_EnableIRQ(EXTI15_8_IRQn);
    NVIC_EnableIRQ(SPI1_IRQn);
    NVIC_EnableIRQ(RTC_IRQn);
    NVIC_EnableIRQ(FLASH_IRQn);
    NVIC_EnableIRQ(RCC_IRQn);
    NVIC_EnableIRQ(EXTI7_0_IRQn);
    NVIC_EnableIRQ(USART2_IRQn);
#endif

#ifdef Core_V3F		
    NVIC_SetPriority(SPI2_IRQn,  (1<<7) | (0x01<<4)); 
    NVIC_SetPriority(SPI3_IRQn,   (0<<7) | (0x01<<4));

#else	
    NVIC_SetPriority(WWDG_IRQn,  (7<<5) | (0x01<<4)); 
    NVIC_SetPriority(EXTI15_8_IRQn,   (6<<5) | (0x01<<4));
    NVIC_SetPriority(SPI1_IRQn, (5<<5) | (0x01<<4));
    NVIC_SetPriority(RTC_IRQn,   (4<<5) | (0x01<<4));
    NVIC_SetPriority(FLASH_IRQn, (3<<5) | (0x01<<4));
    NVIC_SetPriority(RCC_IRQn,   (2<<5) | (0x01<<4));
    NVIC_SetPriority(EXTI7_0_IRQn, (1<<5) | (0x01<<4));
    NVIC_SetPriority(USART2_IRQn, (0<<5) | (0x01<<4));
#endif
}

#ifdef Core_V3F	
/*********************************************************************
 * @fn      SPI2_IRQHandler
 *
 * @brief   This function handles SPI2 exception.
 *
 * @return  none
 */
void SPI2_IRQHandler(void)
{
    step++;
    printf(" 1.SPI2_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(SPI3_IRQn);
    Delay_Ms(10);
    printf(" 1.SPI2\r\n");
}

/*********************************************************************
 * @fn      SPI3_IRQHandler
 *
 * @brief   This function handles SPI3 exception.
 *
 * @return  none
 */
void SPI3_IRQHandler(void)
{
    step++;
    printf(" 2.SPI3_IRQHandler");
    printf("  step:%d\r\n",step);

    Delay_Ms(10);
    printf(" 2.SPI3\r\n");
	printf("GISR:%08x\r\n",PFIC->GISR);
}


#else	
/*********************************************************************
 * @fn      WWDG_IRQHandler
 *
 * @brief   This function handles WWDG exception.
 *
 * @return  none
 */
void WWDG_IRQHandler(void)
{
    step++;
    printf(" 1.WWDG_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(EXTI15_8_IRQn);
    Delay_Ms(10);
    printf(" 1.WWDG\r\n");
}

/*********************************************************************
 * @fn      EXTI15_8_IRQHandler
 *
 * @brief   This function handles EXTI15_8 exception.
 *
 * @return  none
 */
void EXTI15_8_IRQHandler(void)
{
    step++;
    printf(" 2.EXTI15_8_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(SPI1_IRQn);
    Delay_Ms(10);
    printf(" 2.EXTI15_8\r\n");
}

/*********************************************************************
 * @fn      SPI1_IRQHandler
 *
 * @brief   This function handles SPI1 exception.
 *
 * @return  none
 */
void SPI1_IRQHandler(void)
{
    step++;
    printf(" 3.SPI1_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(RTC_IRQn);
    Delay_Ms(10);
    printf(" 3.SPI1\r\n");
}

/*********************************************************************
 * @fn      RTC_IRQHandler
 *
 * @brief   This function handles RTC exception.
 *
 * @return  none
 */
void RTC_IRQHandler(void)
{
    step++;
    printf(" 4.RTC_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(FLASH_IRQn);
    Delay_Ms(10);
    printf(" 4.RTC\r\n");
}

/*********************************************************************
 * @fn      FLASH_IRQHandler
 *
 * @brief   This function handles FLASH exception.
 *
 * @return  none
 */
void FLASH_IRQHandler(void)
{
    step++;
    printf(" 5.FLASH_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(RCC_IRQn);
    Delay_Ms(10);
    printf(" 5.FLASH\r\n");
}

/*********************************************************************
 * @fn      RCC_IRQHandler
 *
 * @brief   This function handles RCC exception.
 *
 * @return  none
 */
void RCC_IRQHandler(void)
{
    step++;
    printf(" 6.RCC_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(EXTI7_0_IRQn);
    Delay_Ms(10);
    printf(" 6.RCC\r\n");
}

/*********************************************************************
 * @fn      EXTI7_0_IRQHandler
 *
 * @brief   This function handles EXTI7_0 exception.
 *
 * @return  none
 */
void EXTI7_0_IRQHandler(void)
{
    step++;
    printf(" 7.EXTI7_0_IRQHandler");
    printf("  step:%d\r\n",step);

    NVIC_SetPendingIRQ(USART2_IRQn);
    Delay_Ms(10);
    printf(" 7.EXTI7_0\r\n");
}

/*********************************************************************
 * @fn      USART2_IRQHandler
 *
 * @brief   This function handles USART2 exception.
 *
 * @return  none
 */
void USART2_IRQHandler(void)
{
    step++;
    printf(" 8.USART2_IRQHandler");
    printf("  step:%d\r\n",step);
    Delay_Ms(10);
    printf(" 8.USART2\r\n");
	printf("GISR:%08x\r\n",PFIC->GISR);
}
#endif


/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
	printf("Interrupt Nest Test\r\n");
	Interrupt_Init();
	printf("Enter lowest interrupt\r\n");
#ifdef Core_V3F	
    NVIC_SetPendingIRQ(SPI2_IRQn);
#else
	NVIC_SetPendingIRQ(WWDG_IRQn);
#endif

    printf("Quit lowest interrupt\r\n");
    printf("Quit step:%d\r\n",step);

    while(1)
   {

   }

}
