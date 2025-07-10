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
float val1=33.14;

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
    int t,t1;

	printf("FPU TEST\r\n");

	val1 = (val1/2+11.12)*2;
    t=(int)(val1*10)%10;
    t1=(int)(val1*100)%10;

	printf("%d.%d%d\n", (int)val1, t, t1);

}
