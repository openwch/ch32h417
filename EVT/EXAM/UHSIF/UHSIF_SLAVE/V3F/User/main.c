/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/05/07
 * Description        : Main program body for V3F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
/*
 *@Note
An example routine for simulating a custom USB device (CH372 device).

This routine demonstrates the use of a USBSS device to simulate the custom device CH372.
If CH372 is a USB2.0 device, its functions are to download and upload data respectively through endpoint 1/2
Endpoint 3 and Endpoint 4 upload and download data respectively.
If CH372 is a USB3.0 device, the function is that the data downloaded from endpoint 1 is output through UHSIF, and the data input from UHSIF is uploaded through endpoint 1.
This device can be operated through bushound or other upper computer software.

Note: This routine needs to be demonstrated together with the host software.

When the USB is set to high speed, it is recommended to use an external crystal oscillator for the clock source.
*/

#include "debug.h"
#include "hardware.h"
#include "ch32h417_usbss_device.h"
#include "ch32h417_usbhs_device.h"

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 *
 */
int main(void)
{
    SystemInit();
    SystemAndCoreClockUpdate();

    /*  The current in the VDD12 power domain of the chip is relatively high. 
     If the VDD12 power supply is externally provided and the internal LDO is turned off, 
     the following function can be executed to reduce the chip's power consumption and 
     alleviate overheating. Note that this operation should only be performed when the 
     external 1.2V power supply has stabilized.*/
    PWR_VDD12ExternPower();
	Delay_Init();
	USART_Printf_Init(921600);

	printf("SystemClk:%d\r\n", SystemClock);
	printf("V3F SystemCoreClk:%d\r\n", SystemCoreClock);
    printf("CH372Device Running On USBSS Controller\n");

#if (Run_Core == Run_Core_V3FandV5F)
    NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
    HSEM_ITConfig(HSEM_ID0, ENABLE);
    NVIC->SCTLR |= 1<<4;
	RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR,ENABLE);
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
	HSEM_ClearFlag(HSEM_ID0);
    printf("V3F wake up\r\n");

    Hardware();

#elif (Run_Core == Run_Core_V3F)
     Hardware();

#elif (Run_Core == Run_Core_V5F)


    NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
    PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
    printf("V3F wake up\r\n");
#endif

    while(1)
    {

    }

}
