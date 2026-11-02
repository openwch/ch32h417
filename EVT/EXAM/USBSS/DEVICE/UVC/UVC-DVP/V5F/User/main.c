/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2026/04/10
 * Description        : Main program body for V5F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
/*
 *@Note
  Example routine for a UVC camera device (DVP interface, YUY2).
  This routine captures image data from an OV2640/OV5640 sensor through DVP and
  streams it to the host as a UVC video device over USBSS or USBHS.
  The OV2640/OV5640 model is detected at power-up and the matching configuration
  descriptors are selected before USB initialization.
  The device can be previewed and its resolutions switched with CameraTestTool.
*/

#include "debug.h"
#include "hardware.h"
#include "ch32h417_usbss_device.h"

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
	Chip = (( DBGMCU_GetCHIPID() >> 4 ) & 0x0F);

#if (Run_Core == Run_Core_V3FandV5F) || (Run_Core == Run_Core_V5F)
	Hardware();

#elif (Run_Core == Run_Core_V3F)

#endif

	while(1)
	{

	}

}
