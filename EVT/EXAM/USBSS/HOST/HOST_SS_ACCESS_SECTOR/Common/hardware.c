/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/09/09
* Description        : This file provides all the CRC firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "ch32h417_usbss_host.h"
#include "ch32h417_usbhs_host.h"
#include "ch32h417_enum.h"
#include "ch32h417_udisk.h"
#include "CHRV3UFI.h"
#include "string.h"

void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/* Variable */
uint8_t DATA_BUFFER[ 34*1024 ];

/*********************************************************************
 * @fn      TIM1_UP_IRQHandler
 *
 * @brief   The interrupt handling function of TIM1
 *
 * @param   none
 *
 * @return  none
 */
void TIM1_UP_IRQHandler(void)
{
    if( TIM_GetITStatus(TIM1, TIM_IT_Update) == SET )
    {
        printf("--------updata1\r\n");
        printf("gDeviceConnectstatus_time = %02x\n", gDeviceConnectstatus);
        if( (gDeviceConnectstatus == USB_INT_CONNECT_U20) || (gDeviceConnectstatus == USB_INT_DISCONNECT))
        {
            USBSS_Endp_Disable();       
            gDeviceConnectstatus = USB_INT_CONNECT;
            printf("--------updata2\r\n");
            gDeviceUsbType = USB_U20_SPEED;
        }
    }
    TIM_ClearITPendingBit( TIM1, TIM_IT_Update );
    TIM_ITConfig( TIM1, TIM_IT_Update, DISABLE );
    TIM_Cmd( TIM1, DISABLE );
}

/*********************************************************************
 * @fn      TIM1_INT_Init
 *
 * @brief   Initializes TIM1 output compare.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *
 * @return  none
 */
void TIM1_INT_Init( uint16_t arr, uint16_t psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM1, ENABLE );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 50;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure );

    TIM_ClearITPendingBit( TIM1, TIM_IT_Update );

	NVIC_SetPriority(TIM1_UP_IRQn,0);
	NVIC_EnableIRQ(TIM1_UP_IRQn);

    TIM_ITConfig( TIM1, TIM_IT_Update, ENABLE );
    TIM_Cmd( TIM1, ENABLE );
}

/*********************************************************************
 * @fn      HOST_SS_ACCESS_SECTOR
 *
 * @brief   USBSS scans the sectors of UDISK.
 *
 * @param   none.
 *
 * @return  none
 */
void HOST_SS_ACCESS_SECTOR( )
{
    uint8_t  s, buf[ 512 ];
    uint32_t addr = 0;
    uint32_t num = 0;

    if( USBHSH->PORT_STATUS_CHG & USBHS_UHIF_PORT_CONNECT )                                             /* Device attach (The status of the USBHS host port has changed) */
    {                                              
        USBHSH->PORT_STATUS_CHG = USBHS_UHIF_PORT_CONNECT;
        if( USBHSH->PORT_STATUS & USBHS_UHIS_PORT_CONNECT )                                             /* Device attach(Determine the connection status of the port) */
        {                                          
            printf("USB2.0 DEVICE ATTACH =%02x,%02x\n", USBHSH->PORT_STATUS_CHG, USBHSH->PORT_STATUS );
            gDeviceUsbType = USB_U20_SPEED;
            gDeviceConnectstatus = USB_INT_CONNECT_U20;
            USBHSH->DEV_ADDR = 0x00;    
            TIM1_INT_Init( 200-1, 14400-1 );       
        }
    }
    if( gDeviceConnectstatus == USB_INT_CONNECT )
    {
        if( gDeviceUsbType == USB_U30_SPEED )                                                           /* USB3.0 DEVICE CONNECT */
        {                                                          
            U30HSOT_Enumerate( buf );           
        }
        else if( gDeviceUsbType == USB_U20_SPEED )                                                      /* USB2.0 DEVICE CONNECT */
        {                                                     
            printf("usb2.0\n");
            s = U20HSOT_Enumerate( buf );
            if( s != USB_INT_SUCCESS ) goto WAIT_DISCONNECT;
            printf("U20HSOT_Enumerate=%02x\n", s);
        }

        printf("UDISK_INIT:\n");
        s = MS_Init( buf );
        if( s != USB_OPERATE_SUCCESS ) goto WAIT_DISCONNECT;        
        addr = 0;
        num = 64;
        memset( DATA_BUFFER, 0xAA, 34*1024 );
        while(1)
        {
            addr += 64;
            printf("The Scanned Sector Address = %02x\n", addr);
            s = MS_ReadSector( addr, num, DATA_BUFFER );
            if( s != USB_OPERATE_SUCCESS )
            {
                for(uint16_t i=0; i!= 100; i++)
                {
                    printf("%02x ",DATA_BUFFER[ 32*1024 - 10 + i ]);
                }
                printf("\n");
                break;
            }
            if( addr >= 10000 )
            {
                addr = 0;
            }
        }

/* Wait for the disconnection */
WAIT_DISCONNECT:
        printf("wait_device_disconnect=%02x,%02x\n",gDeviceConnectstatus,gDeviceUsbType);
        while( gDeviceConnectstatus == USB_INT_CONNECT )
        {         
            if( gDeviceUsbType == USB_U20_SPEED )
            {
                printf("USBHSH->PORT_STATUS_CHG=%02x,%02x\n",USBHSH->PORT_STATUS_CHG,USBHSH->PORT_STATUS);
                if( (USBHSH->PORT_STATUS &USBHS_UHIS_PORT_CONNECT) == 0 )
                {
                    USBHSH->PORT_STATUS_CHG = 0xff;
                    printf("u20_disconnect=%02x\n",USBHSH->PORT_STATUS_CHG );
                    gDeviceUsbType = 0;
                    gDeviceConnectstatus = 0;
                }
            }    
            mDelaymS(100); 
        }
        mDelaymS(100); 
        USBSSH_Init( );
        printf("device_disconnect\n");
    }
}
/*********************************************************************
 * @fn      Reset_Special_Processing
 *
 * @brief   USB special reset processing. Perform USB host controller
 *          reset and re-initialization.
 *
 * @return  none
 */
void Reset_Special_Processing()
{
    USBHSH->PORT_TEST_CT |= ( 1 << 1 );
    Delay_Ms(100);
    USBHSH->PORT_TEST_CT &= ~( 1 << 1 ); 
    USBHS_Host_Init( DISABLE );    
    USBHS_Host_Init( ENABLE );
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
    /* Disable SWD */
    RCC_HB2PeriphClockCmd( RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB, ENABLE );
    GPIO_PinRemapConfig( GPIO_Remap_SWJ_Disable, ENABLE );

    /* USB3.0 and USB2.0 Inital */
    USBSS_PLL_Init( ENABLE );
    RCC_HBPeriphClockCmd( RCC_HBPeriph_USBSS, ENABLE );
    RCC_PIPECmd( ENABLE );
    gDeviceConnectstatus = 0;
    gDeviceUsbType = 0;
    NVIC_EnableIRQ( USBSS_LINK_IRQn );
    USBSSH_Init( );
    USBHS_Host_Init( ENABLE );
    Reset_Special_Processing();

    while(1)
    {
        HOST_SS_ACCESS_SECTOR( );
    }
}
