/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/05/07
 * Description        : Main program body for V5F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
/*
 *@Note
  Example routine to emulate a custom USB device (CH372 device).
  This routine demonstrates the use of a USBHS Device to emulate a custom device, the CH372,
  with endpoints 1/3/5 downlinking data and uploading via endpoints 1/4/6 respectively
  Endpoint 1 uploads and downlinks via ring buffer with no data reversal, endpoints 3/4, and endpoints 5/6 copy and upload.
  The device can be operated using Bushund or other upper computer software.
  Note: This routine needs to be demonstrated in conjunction with the host software.

  If the USB is set to high-speed, an external crystal oscillator is recommended for the clock source.
*/

#include "debug.h"
#include "hardware.h"
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	SystemAndCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(921600);
	printf("V5F SystemCoreClk:%d\r\n", SystemCoreClock);

#if (Run_Core == Run_Core_V3FandV5F)
    HSEM_FastTake(HSEM_ID0);
    HSEM_ReleaseOneSem(HSEM_ID0, 0);

#elif (Run_Core == Run_Core_V3F)

#elif (Run_Core == Run_Core_V5F)
    Hardware();
#endif
    
    while(1)
    {
        ;
    }
}
