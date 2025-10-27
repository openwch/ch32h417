/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/05/23
* Description        : USBSS functions Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32h417_it.h"
#include "ch32h417_usb.h"
#include "usb_desc.h"
#include "ch32h417_usbss_device.h"
#include "string.h"
#include "ch32h417_uhsif.h"
 
/* Global */
const uint8_t    *pUSBSS_Descr;

/* Setup Request */
volatile uint8_t  USBSS_SetupReqType;
volatile uint8_t  USBSS_SetupReqCode;
volatile uint16_t USBSS_SetupReqValue;
volatile uint16_t USBSS_SetupReqIndex;
volatile uint16_t USBSS_SetupReqLen;

/* USB Device Status */
volatile uint8_t  USBSS_DevConfig;
volatile uint8_t  USBSS_DevAddr;
volatile uint16_t USBSS_DevMaxPackLen;
volatile uint8_t  USBSS_DevSpeed;
volatile uint8_t  USBSS_DevSleepStatus;
volatile uint8_t  USBSS_DevEnumStatus;

/* Endpoint Buffer */
__attribute__ ((aligned(4))) uint8_t USBSS_EP0_Buf[ DEF_USBSSD_UEP0_SIZE ];

__attribute__ ((aligned(32),section(".da1"))) uint8_t USBSS_EP1_Tx_Buf[ DEF_USB_EP1_SS_SIZE * DEF_ENDP1_IN_BURST_LEVEL * DEF_UHSIF_RXBUF_CNT ];
__attribute__ ((aligned(32),section(".da1"))) uint8_t USBSS_EP1_Rx_Buf[ DEF_USB_EP1_SS_SIZE * DEF_ENDP1_OUT_BURST_LEVEL * DEF_UHSIF_TXBUF_CNT ];  

void USBSS_IRQHandler (void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USBSS_LINK_IRQHandler (void) __attribute__((interrupt("WCH-Interrupt-fast"))); 


/*********************************************************************
 * @fn      USBSS_LINK_IRQHandler
 *
 * @brief   This function handles USBSS LINK exception.
 *
 * @return  none
 */
void USBSS_LINK_IRQHandler( void )          
{
    USBSS_LINK_Handle( USBSSH );
}

/*********************************************************************
 * @fn      USBSS_Endp_Clear_Frature
 *
 * @brief   Clear the endpoint feature.
 *
 * @return  Whether it was cleared successfully.
 */
uint8_t USBSS_Endp_Clear_Frature( uint8_t dir_endp )         
{
    if(( dir_endp & DEF_UEP_NUM ) > DEF_UEP15 )
    {
        return 0xff;
    }
    if(( dir_endp & DEF_UEP_IN ))
    {
        USBSS_EP_TX_TypeDef* endp = (USBSS_EP_TX_TypeDef*)( &USBSSD->EP1_TX + ((( dir_endp - 1 ) & 0x7f ) * 2));
        endp->UEP_TX_CR = USBSS_EP_TX_CLR | USBSS_EP_TX_CHAIN_CLR;
    }
    else
    {
        USBSS_EP_RX_TypeDef* endp = (USBSS_EP_RX_TypeDef*)( &USBSSD->EP1_RX + ( dir_endp - 1 ) * 2 );
        endp->UEP_RX_CR |= USBSS_EP_RX_CLR | USBSS_EP_RX_CHAIN_CLR;
        endp->UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP1_OUT_BURST_LEVEL;
    }
    return 0x00;
}

/*********************************************************************
 * @fn      USBSS_Endp_Set_Frature
 *
 * @brief   Set the endpoint feature.
 *
 * @return  Whether it was cleared successfully.
 */
uint8_t USBSS_Endp_Set_Frature( uint8_t dir_endp )         
{
    if(( dir_endp & DEF_UEP_NUM ) > DEF_UEP15 )
    {
        return 0xff;
    }
    if(( dir_endp & DEF_UEP_IN ))
    {
        USBSS_EP_TX_TypeDef* endp = (USBSS_EP_TX_TypeDef*)( &USBSSD->EP1_TX + ((( dir_endp - 1 ) & 0x7f ) * 2));
        endp->UEP_TX_CR |= USBSS_EP_TX_HALT;
    }
    else
    {
        USBSS_EP_RX_TypeDef* endp = (USBSS_EP_RX_TypeDef*)( &USBSSD->EP1_RX + ( dir_endp - 1 ) * 2 );
        endp->UEP_RX_CR |= USBSS_EP_RX_HALT;
    }
    return 0x00;
}

/*********************************************************************
 * @fn      USBSS_Get_Endp_Status
 *
 * @brief   Get the endpoint status.
 *
 * @return  Whether it was cleared successfully.
 */
uint8_t USBSS_Get_Endp_Status( uint8_t dir_endp )         
{
    if(( dir_endp & DEF_UEP_NUM ) > DEF_UEP15 )
    {
        return 0xff;
    }
    if(( dir_endp & DEF_UEP_NUM ) == DEF_UEP0 )
    {
        return 0x00;
    }
    if(( dir_endp & DEF_UEP_IN ))
    {
        USBSS_EP_TX_TypeDef* endp = (USBSS_EP_TX_TypeDef*)( &USBSSD->EP1_TX + ((( dir_endp - 1 ) & 0x7f ) * 2 ));
        if( endp->UEP_TX_CR & USBSS_EP_TX_HALT )
        {
            USBSS_EP0_Buf[ 0 ] = 0x01;
        }
    }
    else
    {
        USBSS_EP_RX_TypeDef* endp = (USBSS_EP_RX_TypeDef*)( &USBSSD->EP1_RX + ( dir_endp - 1 ) * 2 );
        if( endp->UEP_RX_CR & USBSS_EP_RX_HALT )
        {
            USBSS_EP0_Buf[ 0 ] = 0x01;
        }
    }
    return 0x00;
}
/*********************************************************************
 * @fn      USBSS_IRQHandler
 *
 * @brief   This function handles USBSS exception.
 *
 * @return  none
 */
void USBSS_IRQHandler( void )              
{
    uint8_t endp_num;
    uint8_t  errflag;
    uint16_t len;
    uint32_t ep_dir;
    uint32_t status = 0;
    uint32_t cnt = DEF_TX_CHAIN_TIMEOUT;
    status = USBSSD->USB_STATUS;

    if(( status & USBSS_UDIF_SETUP ) && !( status & USBSS_UDIF_STATUS ))    // SETUP-DPH & ACK-TP
    {
        USBSS_Dev_Info.set_devaddr = 0;
        USBSS_Dev_Info.set_isoch_delay = 0;

        /* Store All Setup Values */
        USBSS_SetupReqType  = pUSBSS_SetupReqPak->bRequestType;
        USBSS_SetupReqCode  = pUSBSS_SetupReqPak->bRequest;
        USBSS_SetupReqLen   = pUSBSS_SetupReqPak->wLength;
        USBSS_SetupReqValue = pUSBSS_SetupReqPak->wValue;
        USBSS_SetupReqIndex = pUSBSS_SetupReqPak->wIndex;

        len = 0;
        errflag = 0;
        if(( USBSS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
        {
            /* usb non-standard request processing */
            errflag = 0xFF;
        }
        else 
        {
            switch( USBSS_SetupReqCode )
            {
                /* get device/configuration/string/report/... descriptors */
                case USB_GET_DESCRIPTOR:
                    switch((uint8_t)( USBSS_SetupReqValue >> 8 ))
                    {
                        /* get usb device descriptor */
                        case USB_DESCR_TYP_DEVICE:
                            pUSBSS_Descr = SS_DeviceDescriptor;
                            len = DEF_USBSSD_DEVICE_DESC_LEN;
                            break;

                        /* get usb configuration descriptor */
                        case USB_DESCR_TYP_CONFIG:
                            pUSBSS_Descr = SS_ConfigDescriptor;
                            len = DEF_USBSSD_CONFIG_DESC_LEN;
                            break;

                        /* get usb string descriptor */
                        case USB_DESCR_TYP_STRING:
                            switch((uint8_t)( USBSS_SetupReqValue & 0xFF ))
                            {
                                /* Descriptor 0, Language descriptor */
                                case DEF_STRING_DESC_LANG:
                                    pUSBSS_Descr = MyLangDescr;
                                    len = DEF_USBSSD_LANG_DESC_LEN;
                                    break;

                                /* Descriptor 1, Manufacturers String descriptor */
                                case DEF_STRING_DESC_MANU:
                                    pUSBSS_Descr = MyManuInfo;
                                    len = DEF_USBSSD_MANU_DESC_LEN;
                                    break;

                                /* Descriptor 2, Product String descriptor */
                                case DEF_STRING_DESC_PROD:
                                    pUSBSS_Descr = MyProdInfo;
                                    len = DEF_USBSSD_PROD_DESC_LEN;
                                    break;

                                /* Descriptor 3, Serial-number String descriptor */
                                case DEF_STRING_DESC_SERN:
                                    pUSBSS_Descr = MySerNumInfo;
                                    len = DEF_USBSSD_SN_DESC_LEN;
                                    break;
                                
                                case DEF_STRING_DESC_OS:
                                    pUSBSS_Descr = OSStringDescriptor;
                                    len = DEF_USBSSD_OS_DESC_LEN;
                                    break;

                                default:
                                    errflag = 0xFF;
                                    break;
                            }
                            break;

                        /* get usb device qualify descriptor */
                        case USB_DESCR_TYP_QUALIF:
                            pUSBSS_Descr = MyQuaDesc;
                            len = DEF_USBSSD_QUA_DESC_LEN;
                            break;

                        /* get usb BOS descriptor */
                        case USB_DESCR_TYP_BOS:
                            /* USB 2.00 DO NOT support BOS descriptor */
                            pUSBSS_Descr = MyBOSDesc_SS;
                            len = DEF_USBSSD_BOS_DESC_LEN;
                            break;

                        default :
                            errflag = 0xFF;
                            break;
                    }

                    /* Copy Descriptors to Endp0 DMA buffer */
                    if( USBSS_SetupReqLen > len )
                    {
                        USBSS_SetupReqLen = len;
                    }
                    len = (USBSS_SetupReqLen >= DEF_USBSSD_UEP0_SIZE) ? DEF_USBSSD_UEP0_SIZE : USBSS_SetupReqLen;
                    memcpy( USBSS_EP0_Buf, pUSBSS_Descr, len );
                    pUSBSS_Descr += len;
                    break;

                /* Set usb address */
                case USB_SET_ADDRESS:

                    USBSS_Dev_Info.set_devaddr = 1;
                    USBSS_Dev_Info.devaddr = (uint16_t)( USBSS_SetupReqValue & 0xFF );
                    break;

                /* Get usb configuration now set */
                case USB_GET_CONFIGURATION:
                    USBSS_EP0_Buf[ 0 ] = USBSS_DevConfig;
                    if ( USBSS_SetupReqLen > 1 )
                    {
                        USBSS_SetupReqLen = 1;
                    }
                    break;
                    
                /* set iso delay */
                case USB_SET_ISOCH_DLY:
                    USBSS_Dev_Info.set_isoch_delay = 1;
                    USBSS_Dev_Info.set_isoch_value = (uint16_t)( USBSS_SetupReqValue & 0xFFFF );
                    break;

                /* set sel */
                case USB_SET_SEL:
                    break;

                /* Set usb configuration to use */
                case USB_SET_CONFIGURATION:
                    USBSS_DevConfig = (uint8_t)( USBSS_SetupReqValue & 0xFF );
                    if( USBSS_DevConfig && ( USBSS_DevConfig != SS_ConfigDescriptor[ 5 ] ))
                    {
                        USBSS_DevConfig = SS_ConfigDescriptor[ 5 ];
                        errflag = 0xFF;
                    }
                    else if( USBSS_DevConfig == 0 )
                    {
                        USBSS_DevEnumStatus = 0x00;
                    }
                    else 
                    {
                        USBSS_DevEnumStatus = 0x01;
                    }
                    if( USBSS_DevConfig != ( USBSS_SetupReqValue & 0xFF ))
                    {
                        USBSS_Dev_Info.u1_enable = DISABLE;
                        USBSS_Dev_Info.u2_enable = DISABLE;
                    }
 
                    break;

                /* Clear or disable one usb feature */    
                case USB_CLEAR_FEATURE:
                    if( ( USBSS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                    {
                        /* clear one device feature */
                        if((uint8_t)( USBSS_SetupReqValue & 0xFF ) == 0x01 )
                        {
                            /* clear usb sleep status, device not prepare to sleep */
                            USBSS_DevSleepStatus &= ~0x01;
                        }
                        else if((uint8_t)( USBSS_SetupReqValue & 0xFF ) == USB_U1_ENABLE )
                        {

#ifdef DEF_UP_U1_EN         /* disable U1 */
                            USBSSD->LINK_CFG &= ~LINK_U1_ALLOW;
                            USBSS_Dev_Info.u1_enable = DISABLE;
#endif


                        }
                        else if((uint8_t)( USBSS_SetupReqValue & 0xFF ) == USB_U2_ENABLE )
                        {
#ifdef DEF_UP_U2_EN         /* disable U2 */
                            USBSSD->LINK_CFG &= ~LINK_U2_ALLOW;
                            USBSS_Dev_Info.u2_enable = DISABLE;
#endif
                        }
                        else
                        {
                            errflag = 0xFF;
                        }
                    }
                    else if(( USBSS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                    {
                        /* Set End-point Feature */
                        if((uint8_t)( USBSS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
                        {
                            /* Clear End-point Feature */
                            USBSS_Endp_Clear_Frature((uint8_t)( USBSS_SetupReqIndex & 0xFF ));
                        }
                        else
                        {
                            errflag = 0xFF;
                        }
                    }
                    else
                    {
                        errflag = 0xFF;
                    }
                    break;
                case USB_SET_FEATURE:
                    if( ( USBSS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                    {
                        /* Set Device Feature */
                        if((uint8_t)(USBSS_SetupReqValue & 0xFF ) == USB_U1_ENABLE )
                        {
                            if( USBSS_DevEnumStatus )
                            {
#ifdef DEF_UP_U1_EN             /* enable U1 */
                                USBSSD->LINK_CFG |= LINK_U1_ALLOW;
                                USBSS_Dev_Info.u1_enable = ENABLE;
#endif
                            }
                            else
                            {
                                errflag = 0xFF;
                            }
                        }
                        else if ((uint8_t)(USBSS_SetupReqValue & 0xFF ) == USB_U2_ENABLE ) 
                        {                      
#ifdef DEF_UP_U2_EN             /* enable U2 */
                            if( USBSS_DevEnumStatus )
                            {
                                USBSSD->LINK_CFG |= LINK_U2_ALLOW;
                                USBSS_Dev_Info.u2_enable = ENABLE;
                            }
                            else
                            {
                                errflag = 0xFF;
                            }
#endif                           
                        }
                        else
                        {
                            errflag = 0xFF;
                        }
                    }
                    else if(( USBSS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                    {
                        /* Set End-point Feature */
                        if( (uint8_t)( USBSS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
                        {
                            /* Set end-points status stall */
                            USBSS_Endp_Set_Frature((uint8_t)(USBSS_SetupReqIndex & 0xFF ));
                        }
                    }

                    break;

                
                case USB_SET_INTERFACE:
                    break;

                /* This request allows the host to select another setting for the specified interface  */
                case USB_GET_INTERFACE:
                    USBSS_EP0_Buf[ 0 ] = 0x00;
                    if ( USBSS_SetupReqLen > 1 )
                    {
                        USBSS_SetupReqLen = 1;
                    }
                    break;

                /* host get status of specified device/interface/end-points */
                case USB_GET_STATUS:
                    USBSS_EP0_Buf[ 0 ] = 0x00;
                    USBSS_EP0_Buf[ 1 ] = 0x00;
                    if( ( USBSS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                    {
                        errflag = USBSS_Get_Endp_Status((uint8_t)( USBSS_SetupReqIndex & 0xFF ));
                    }
                    else if( ( USBSS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                    {
                        if( USBSS_DevSleepStatus & 0x01 )
                        {
                            USBSS_EP0_Buf[ 0 ] = 0x02;
                        }
                        USBSS_EP0_Buf[ 0 ] |= ( USBSS_Dev_Info.u2_enable << 3 ) | ( USBSS_Dev_Info.u1_enable << 2 );
                    }
                    if ( USBSS_SetupReqLen > 2 )
                    {
                        USBSS_SetupReqLen = 2;
                    }
                    break;

                case USB_SET_ENDPOINT:
                    break;

                default :
                    errflag = 0xFF;
                    break;
            }
        }

        /* errflag = 0xFF means a request not support or some errors occurred, else correct */
        if( errflag == 0xFF )
        {
            /* if one request not support, return stall */
            USBSSD->UEP0_TX_CTRL = USBSS_EP0_TX_STALL;
            USBSSD->UEP0_RX_CTRL = USBSS_EP0_RX_ERDY | USBSS_EP0_RX_STALL;
        }
        else
        {
            /* end-point 0 data Tx/Rx */
            if( USBSS_SetupReqType & DEF_UEP_IN )
            {
                /* tx */
                if( USBSS_SetupReqLen == 0 )
                {
                    USBSSD->UEP0_RX_CTRL = USBSS_EP0_RX_ERDY | USBSS_EP0_RX_ACK;
                }
                else 
                {
                    len = ( USBSS_SetupReqLen > DEF_USBSSD_UEP0_SIZE ) ? DEF_USBSSD_UEP0_SIZE : USBSS_SetupReqLen;
                    USBSS_SetupReqLen -= len;
                    USBSSD->UEP0_TX_CTRL = USBSS_EP0_TX_DPH | len;                         // DATA stage
                    USBSSD->UEP0_TX_CTRL |= USBSS_EP0_TX_ERDY;
                }
            }
            else
            {
                /* rx */
                if( USBSS_SetupReqLen == 0 )
                {
                    USBSSD->UEP0_TX_CTRL = USBSS_EP0_TX_DPH;                        
                    USBSSD->UEP0_TX_CTRL |= USBSS_EP0_TX_ERDY;
                    USBSSD->UEP0_RX_CTRL = USBSS_EP0_RX_ERDY | USBSS_EP0_RX_ACK;
                    
                }
                else
                {
                    USBSSD->UEP0_RX_CTRL = USBSS_EP0_RX_ERDY | USBSS_EP0_RX_ACK;
                }
            }
        }
        USBSSD->USB_STATUS = USBSS_UDIF_SETUP;

    }
    else if( status & USBSS_UDIF_STATUS )
    {
        USBSSD->USB_STATUS = USBSS_UDIF_STATUS;                                         // clear int flag

        if( USBSS_Dev_Info.set_devaddr )
        {
            SET_Device_Address( USBSS_Dev_Info.devaddr, USBSSH );                           // SET ADDRESS 
            USBSS_Dev_Info.set_devaddr = 0;
        }
        else if( USBSS_Dev_Info.set_isoch_delay )
        {
            USBSSD->LINK_ISO_DLY = USBSS_Dev_Info.set_isoch_value;
            USBSS_Dev_Info.set_isoch_delay = 0;
        }

        USBSSD->UEP0_TX_CTRL = 0;
        USBSSD->UEP0_RX_CTRL = 0;

    }
    else if( status & USBSS_UIF_TRANSFER )
    {
        endp_num = ( status & USBSS_EP_ID_MASK ) >> 8;
        ep_dir = status & USBSS_EP_DIR_MASK;                                       // 1: IN, 0:OUT

        if( ep_dir )
        {
            switch( endp_num )
            {
                /* end-point 0 data in interrupt */
                case DEF_UEP0:
                    if( USBSS_SetupReqLen == 0 )
                    {
                        USBSSD->UEP0_TX_CTRL = USBSS_EP0_TX_DPH;                            // Zero Length
                        USBSSD->UEP0_RX_CTRL = USBSS_EP0_RX_ERDY | USBSS_EP0_RX_ACK ;       // ready status step
                    }
                    else
                    {
                        /* Standard request endpoint 0 Data upload */
                        switch(USBSS_SetupReqCode)
                        {
                            case USB_GET_DESCRIPTOR:
                                len = USBSS_SetupReqLen >= DEF_USBSSD_UEP0_SIZE ? DEF_USBSSD_UEP0_SIZE : USBSS_SetupReqLen;
                                memcpy(USBSS_EP0_Buf, pUSBSS_Descr, len);
                                USBSS_SetupReqLen -= len;
                                pUSBSS_Descr += len;                        
                                USBSSD->UEP0_TX_CTRL = USBSS_EP0_TX_DPH | len | (((( USBSSD->UEP0_TX_CTRL >> 16 ) & 0x1F ) + 1 ) << 16 );                    
                                USBSSD->UEP0_TX_CTRL |= USBSS_EP0_TX_ERDY ;
                                break;

                            default:
                                USBSSD->UEP0_TX_CTRL = USBSS_EP0_TX_DPH;
                                break;
                        }
                    }
                    break;

                /* end-point 1 data in interrupt */
                case DEF_UEP1:
                    USBSSD->EP1_TX.UEP_TX_CHAIN_ST |= USBSS_EP_TX_CHAIN_IF;

                    UHSIF_Trans_Cfg( DEF_LINE0, UHSIF_RD_COMM.deal, 0 );

                    UHSIF_RD_COMM.deal++;
                    if( UHSIF_RD_COMM.deal >= DEF_UHSIF_RXBUF_CNT )               
                    {
                        UHSIF_RD_COMM.deal = 0;
                    }
                    USBSSD->EP1_TX.UEP_TX_DMA = UHSIF_RD_Pack[ UHSIF_RD_COMM.deal ].adr;
                    USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = DEF_USB_EP1_SS_SIZE;
                    
                    if( UHSIF_RD_COMM.total )
                    {
                        NVIC_DisableIRQ(UHSIF_IRQn);
                        UHSIF_RD_COMM.total--;
                        NVIC_EnableIRQ(UHSIF_IRQn);
            
                        if( UHSIF_RD_COMM.total )
                        {
                            if( UHSIF_RD_Pack[ UHSIF_RD_COMM.deal ].len < DEF_BUFF_SIZE )
                            {
                                /* 短包处理 */
                                while( cnt-- )
                                {
                                    if( ( USBSSD->EP1_TX.UEP_TX_ST & USBSS_EP_TX_CHAIN_EN_MASK ) != USBSS_EP_TX_CHAIN_EN_MASK )
                                    {
                                        if( UHSIF_RD_Pack[ UHSIF_RD_COMM.deal ].len % 1024 )
                                        {
                                            USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = UHSIF_RD_Pack[ UHSIF_RD_COMM.deal ].len % 1024;
                                            USBSSD->EP1_TX.UEP_TX_CHAIN_EXP_NUMP = UHSIF_RD_Pack[ UHSIF_RD_COMM.deal ].len / 1024 + 1;
                                        }
                                        else 
                                        {
                                            USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = 1024;
                                            USBSSD->EP1_TX.UEP_TX_CHAIN_EXP_NUMP = UHSIF_RD_Pack[ UHSIF_RD_COMM.deal ].len / 1024;
                                        }
                                        break;
                                    }
                                }
                            }
                            else 
                            {
                                /* 长包处理 */ 
                                while( cnt-- )
                                {
                                    if( ( USBSSD->EP1_TX.UEP_TX_ST & USBSS_EP_TX_CHAIN_EN_MASK ) != USBSS_EP_TX_CHAIN_EN_MASK )
                                    {
                                        USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = DEF_USB_EP1_SS_SIZE;
                                        USBSSD->EP1_TX.UEP_TX_CHAIN_EXP_NUMP = ( DEF_BUFF_SIZE / 1024 );
                                        break;
                                    }
                                }                   
                            }                           
                        }                
                    }
                
                    break;  
                default:
                   errflag = 0xFF;
                break;                                     
            }
        }
        else 
        {
            switch( endp_num )
            {
                /* end-point 0 data out interrupt */
                case DEF_UEP0:
                    USBSSD->UEP0_RX_CTRL = USBSS_EP0_RX_ERDY | USBSS_EP0_RX_ACK ;       // ready status step
                    break;

                /* end-point 1 data out interrupt */
                case DEF_UEP1:
                    UHSIF_WR_Pack[ UHSIF_WR_COMM.load ].len = ( USBSSD->EP1_RX.UEP_RX_CHAIN_NUMP - 1 ) * DEF_USB_EP1_SS_SIZE + USBSSD->EP1_RX.UEP_RX_CHAIN_LEN; 

             
                    UHSIF_Trans_Cfg( DEF_LINE1, UHSIF_WR_COMM.load, UHSIF_WR_Pack[ UHSIF_WR_COMM.load ].len );
                    UHSIF_WR_COMM.load++;                                      
                    if( UHSIF_WR_COMM.load >= DEF_UHSIF_TXBUF_CNT )                 
                    {
                        UHSIF_WR_COMM.load = 0;
                    }
                                              
                    NVIC_DisableIRQ(UHSIF_IRQn);
                    UHSIF_WR_COMM.total++;     
                    NVIC_EnableIRQ(UHSIF_IRQn);

                    USBSSD->EP1_RX.UEP_RX_DMA = UHSIF_WR_Pack[ UHSIF_WR_COMM.load ].adr;
                    if( UHSIF_WR_COMM.total >= DEF_UHSIF_TXBUF_CNT - 1 )
                    {
                        UHSIF_WR_COMM.stop = 1;
                        USBSSD->EP1_RX.UEP_RX_CHAIN_MAX_NUMP = 0;
                    }
                    else 
                    {
                        USBSSD->EP1_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP1_OUT_BURST_LEVEL;
                    }

                    USBSSD->EP1_RX.UEP_RX_CHAIN_ST |= USBSS_EP_RX_CHAIN_IF;

                    break;  

                default:
                   errflag = 0xFF;
                break;                  
            }
        }
    }
}
