/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32h417_crc.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/17
* Description        : This file provides all the CRC firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "dvp.h"
#include "ch32h417_dvp.h"
#include "ch32h417_swpmi.h"
#include "ch32h417_usbss_device.h"
#include "ch32h417_usbhs_device.h"

 /*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @param   none
 *
 * @return  none
 */

void Hardware(void)
{
    RCC_HB2PeriphClockCmd( RCC_HB2Periph_AFIO, ENABLE ); 
    RCC_HB1PeriphClockCmd( RCC_HB1Periph_SWPMI, ENABLE );
    SWPMI_BypassCmd( ENABLE );

    /* Disable SWD */
    GPIO_PinRemapConfig( GPIO_Remap_SWJ_Disable, ENABLE );
    
	/*Initialize camera OV2640 */
	while( OV2640_Init( ) )
	{
		printf("Camera model Err\r\n");
		Delay_Ms(500);
	}

    /* Initialize DVP */
	dvp_Init();

    /* Initialize USB */
    USB_Timer_Init( );
    USBSS_Device_Init( ENABLE );
    
	while (1)
    {

    }
		
}
