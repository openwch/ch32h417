/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
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
#include "hub.h"
#include "string.h"

USB_HUB_SaveData SS_HUB_SaveData;
USB_HUB_SaveData HS_HUB_SaveData;
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

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
 * @fn      HOST_SS_HUB_ACCESS_SECTOR
 *
 * @brief   USBSS can scan the sectors of the USB drive by operating it through a HUB.
 *
 * @param   none.
 *
 * @return  none
 */
void HOST_SS_HUB_ACCESS_SECTOR( void )
{
    uint8_t  s, buf[ 512 ];
    if( USBHSH->PORT_STATUS_CHG & USBHS_UHIF_PORT_CONNECT )                                             // Device attach (The status of the USBHS host port has changed)
    {                                              
        USBHSH->PORT_STATUS_CHG = USBHS_UHIF_PORT_CONNECT;
        if( USBHSH->PORT_STATUS & USBHS_UHIS_PORT_CONNECT )                                             // Device attach(Determine the connection status of the port)
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
        if( gDeviceUsbType == USB_U30_SPEED )                                                           // USB3.0 DEVICE CONNECT
        {   
            printf("USB3.0\n");               
            SS_HUB_SaveData.Depth = 0;  
            SS_HUB_SaveData.HUB_Info.DevAddr = USB_SSDEVICE_ADDR;                                                  
            U30HSOT_Enumerate( buf,SS_HUB_SaveData.HUB_Info.DevAddr,0xff );     
            if( SS_HUB_SaveData.Device_Type == HUB_DEVICE ) 
            {                                           // The device is a USB 3.0 HUB. An USB 2.0 HUB needs to be enumerated
                HS_HUB_SaveData.HUB_Info.DevAddr = USB_HSDEVICE_ADDR;   
                s = U20HSOT_Enumerate( buf,HS_HUB_SaveData.HUB_Info.DevAddr,0XFF );
            }     
        }
        else if( gDeviceUsbType == USB_U20_SPEED )                                                      // USB2.0 DEVICE CONNECT
        {                                                     
            printf("USB2.0\n");
            HS_HUB_SaveData.HUB_Info.DevAddr = USB_HSDEVICE_ADDR; 
            s = U20HSOT_Enumerate( buf,HS_HUB_SaveData.HUB_Info.DevAddr,0XFF );
            if( s != USB_INT_SUCCESS ) goto WAIT_DISCONNECT;
            printf("U20HSOT_Enumerate=%02x\n", s);
        }
        while(1){
            /* USBSS */
            if( SS_HUB_SaveData.Device_Type == HUB_DEVICE )
            {        
                s = U30HOST_ConnectDevice();
                if( s != ERR_SUCCESS )
                {
                    printf("USBSS_HUB_STATUS=%02x\n",s);                    
                    break;
                }    
                U30HOST_HUB_DEV_MainProcess();
            }   
            /* USBHS */
            if( HS_HUB_SaveData.Device_Type == HUB_DEVICE )
            {        
                s = U20HOST_ConnectDevice();
                if( s != ERR_SUCCESS )
                {
                    printf("USBHS_HUB_STATUS=%02x\n",s);
                    break;
                }   
                U20HOST_HUB_DEV_MainProcess(); 
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
        memset( &HS_HUB_SaveData,0x00,sizeof(HS_HUB_SaveData) );
        memset( &SS_HUB_SaveData,0x00,sizeof(HS_HUB_SaveData) );
        mDelaymS(100); 
        USBSSH_Init( );
        printf("device_disconnect\n");
    }
}

/*********************************************************************
 * @fn      Reset_Special_Processing
 *
 * @brief   USB special reset processing. Perform USB host controller
 *          reset and re-initialization, clear hub save data.
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
    memset( &HS_HUB_SaveData,0x00,sizeof(HS_HUB_SaveData) );
    memset( &SS_HUB_SaveData,0x00,sizeof(HS_HUB_SaveData) );
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
        HOST_SS_HUB_ACCESS_SECTOR( );
    }
}
