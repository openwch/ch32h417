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
#include "SoftUART.h"
/*
 *@Note
 *
 *This program used DMA and TIM to emulate the TX of uart,and used TIM and EXTI to emulate the RX of uart(9600bps).
 *This example is used to receive data and send specified quantity of data
 *You can redistribute it and/or modify it by changing the clock source and destination of struct in the function In SoftUART.c(RxInit & TxInit & Init)
 * 
 *Presently Hardware connection:PE4 -- Rx  
 *                              PE3 -- Tx  
 */

/* Global Variable */
vu8 val;

uint8_t tmp[20] = {0x01, 0x80, 0xaa, 0x55};

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   IO simulation serial port data transmission and reception.
 *
 * @return  none
 */
void Hardware(void)
{

#ifdef Core_V3F
    SoftUARTFunc.Init(9600);
#elif defined(Core_V5F)

#endif

	while(1)
	{
        if (SoftUARTFunc.ReceiveBytes(tmp, 0xff))
            SoftUARTFunc.TransmitBytes(tmp, 4);
	}
}
