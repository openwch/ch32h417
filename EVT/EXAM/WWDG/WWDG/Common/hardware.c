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
#define WWDG_CNT    0X7F

/* Global Variable */

void WWDG_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      WWDG_IRQHandler
 *
 * @brief   This function handles WWDG exception.
 *
 * @return  none
 */
void WWDG_IRQHandler(void)
{
  WWDG_ClearFlag();
  WWDG_SetCounter(WWDG_CNT);
}


/*********************************************************************
 * @fn      WWDG_NVIC_Config
 *
 * @brief   WWDG INT init.
 *
 * @return  none
 */
static void WWDG_NVIC_Config(void)
{
    NVIC_SetPriority(WWDG_IRQn,0);
    NVIC_EnableIRQ(WWDG_IRQn);
}

/*********************************************************************
 * @fn      WWDG_Config
 *
 * @brief   Configure WWDG.
 *
 * @param   tr - The value of the decrement counter(0x7f~0x40)
 *          wr - Window value(0x7f~0x40)
 *          prv - Prescaler value
 *            WWDG_Prescaler_1
 *            WWDG_Prescaler_2
 *            WWDG_Prescaler_4
 *            WWDG_Prescaler_8
 *
 * @return  none
 */
void WWDG_Config(uint8_t tr, uint8_t wr, uint32_t prv)
{
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_WWDG, ENABLE);

    WWDG_SetCounter(tr);
    WWDG_SetPrescaler(prv);
    WWDG_SetWindowValue(wr);
    WWDG_Enable(WWDG_CNT);
    WWDG_ClearFlag();
    WWDG_NVIC_Config();
    WWDG_EnableIT();
}

/*********************************************************************
 * @fn      WWDG_Feed
 *
 * @brief   Feed WWDG.
 *
 * @return  none
 */
void WWDG_Feed(void)
{
    WWDG_SetCounter(WWDG_CNT);
}


/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
	u8 wwdg_tr, wwdg_wr;
	// printf("WWDG\r\n");
    WWDG_Config(0x7f, 0x5f, WWDG_Prescaler_8); 
    wwdg_wr = WWDG->CFGR & 0x7F;
    while(1)
    {
        // Delay_Ms(10);
        printf("**********\r\n");
        wwdg_tr = WWDG->CTLR & 0x7F;
        if(wwdg_tr < wwdg_wr)
        {
            WWDG_Feed();
        }
        printf("##########\r\n");
    }
}
