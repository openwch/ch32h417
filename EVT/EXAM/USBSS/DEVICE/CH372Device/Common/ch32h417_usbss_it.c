/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/05/23
* Description        : USBSS functions Interrupt Service Routines.
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32h417_it.h"
#include "ch32h417_usb.h"
#include "usb_desc.h"
#include "ch32h417_usbss_device.h"
#include "string.h"
 
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
__attribute__ ((aligned(4))) uint8_t USBSS_EP1_Rx_Buf[ DEF_USB_EP1_SS_SIZE * 32 ];
__attribute__ ((aligned(4))) uint8_t USBSS_EP2_Rx_Buf[ DEF_USB_EP2_SS_SIZE * 32 ];
__attribute__ ((aligned(4))) uint8_t USBSS_EP3_Tx_Buf[ DEF_USB_EP3_SS_SIZE * 32 ];
__attribute__ ((aligned(4))) uint8_t USBSS_EP3_Rx_Buf[ DEF_USB_EP3_SS_SIZE * 32 ];
__attribute__ ((aligned(4))) uint8_t USBSS_EP4_Tx_Buf[ DEF_USB_EP4_SS_SIZE * 32 ];
__attribute__ ((aligned(4))) uint8_t USBSS_EP4_Rx_Buf[ DEF_USB_EP4_SS_SIZE * 32 ];


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
    USBSS_LINK_Handle( USBSSH, 0 );
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

    if(( USBSSD->USB_STATUS & USBSS_UDIF_SETUP ) && !( USBSSD->USB_STATUS & USBSS_UDIF_STATUS ))    // SETUP-DPH & ACK-TP
    {
        ss_dev_info.set_devaddr = 0;
        ss_dev_info.set_depth_req = 0;
        ss_dev_info.set_isoch_delay =0;

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
                    switch( (uint8_t)(USBSS_SetupReqValue>>8) )
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
                            switch( (uint8_t)(USBSS_SetupReqValue&0xFF) )
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
                            // pUSBSS_Descr = MyQuaDesc;
                            // len = DEF_USBSSD_QUALFY_DESC_LEN;
                            break;

                        /* get usb BOS descriptor */
                        case USB_DESCR_TYP_BOS:
                            /* USB 2.00 DO NOT support BOS descriptor */
                            pUSBSS_Descr = MyBOSDesc_SS;
                            len = DEF_USBSSD_SN_DESC_LEN;
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
                    ss_dev_info.set_devaddr = 1;
                    ss_dev_info.devaddr = (uint16_t)( USBSS_SetupReqValue & 0xFF );
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
                case SET_ISOCH_DLY:
                    ss_dev_info.set_isoch_delay = 1;
                    ss_dev_info.set_isoch_value = (uint16_t)( USBSS_SetupReqValue & 0xFF );
                    break;

                /* set sel */
                case SET_SEL:
                    break;

                /* Set usb configuration to use */
                case USB_SET_CONFIGURATION:
                    USBSS_DevConfig = (uint8_t)( USBSS_SetupReqValue & 0xFF );
                    USBSS_DevEnumStatus = 0x01;
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
                        else if((uint8_t)( USBSS_SetupReqValue & 0xFF ) == 0x30 )
                        {
#ifdef UP_U1_EN             /* disable U1 */
                            USBSSD->LINK_CFG &= ~LINK_U1_ALLOW;
                            ss_dev_info.u1_enable = DISABLE;
#endif
                        }
                        else if((uint8_t)( USBSS_SetupReqValue & 0xFF ) == 0x31 )
                        {
#ifdef UP_U2_EN             /* disable U2 */
                            USBSSD->LINK_CFG &= ~LINK_U2_ALLOW;
                            ss_dev_info.u2_enable = DISABLE;
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
                            switch((uint8_t)( USBSS_SetupReqIndex & 0xFF ))
                            {
                                case ( DEF_UEP1 | DEF_UEP_OUT ):
                                        /* Set End-point 1 OUT ACK */
                                        USBSSD->EP1_RX.UEP_RX_CR |= USBSS_EP_RX_CLR;
                                    break;

                                case ( DEF_UEP1 | DEF_UEP_IN ):
                                        /* Set End-point 1 IN NAK */
                                        USBSSD->EP1_TX.UEP_TX_CR |= USBSS_EP_TX_CLR;
                                    break;

                                case ( DEF_UEP2 | DEF_UEP_OUT ):
                                        /* Set End-point 2 OUT ACK */
                                        USBSSD->EP2_RX.UEP_RX_CR |= USBSS_EP_RX_CLR;
                                    break;

                                case ( DEF_UEP2 | DEF_UEP_IN ):
                                        /* Set End-point 2 IN NAK */
                                        USBSSD->EP2_TX.UEP_TX_CR |= USBSS_EP_TX_CLR;
                                    break;

                                case ( DEF_UEP3 | DEF_UEP_OUT ):
                                        /* Set End-point 3 OUT ACK */
                                        USBSSD->EP3_RX.UEP_RX_CR |= USBSS_EP_RX_CLR;
                                    break;

                                case ( DEF_UEP3 | DEF_UEP_IN ):
                                        /* Set End-point 3 IN NAK */
                                        USBSSD->EP3_TX.UEP_TX_CR |= USBSS_EP_TX_CLR;
                                    break;

                                case ( DEF_UEP4 | DEF_UEP_OUT ):
                                        /* Set End-point 4 OUT ACK */
                                        USBSSD->EP4_RX.UEP_RX_CR |= USBSS_EP_RX_CLR;
                                    break;

                                case ( DEF_UEP4 | DEF_UEP_IN ):
                                        /* Set End-point 4 IN NAK */
                                        USBSSD->EP4_TX.UEP_TX_CR |= USBSS_EP_TX_CLR;
                                    break;

                                case ( DEF_UEP5 | DEF_UEP_OUT ):
                                        /* Set End-point 5 OUT ACK */
                                        USBSSD->EP5_RX.UEP_RX_CR |= USBSS_EP_RX_CLR;
                                    break;

                                case ( DEF_UEP6 | DEF_UEP_IN ):
                                        /* Set End-point 6 IN NAK */
                                        USBSSD->EP6_TX.UEP_TX_CR |= USBSS_EP_TX_CLR;
                                    break;

                                default:
                                    errflag = 0xFF;
                                    break;
                            }
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
                        if( (uint8_t)(USBSS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
                        {
                            if( USBSS_SetupReqValue == 0x30 )
                            {   
 #ifdef UP_U1_EN                /* enable U1 */
                                USBSSD->LINK_CFG |= LINK_U1_ALLOW;
#endif
                            }
                            else if( USBSS_SetupReqValue == 0x31 )
                            {                          
#ifdef UP_U2_EN                 /* enable U2 */
                                USBSSD->LINK_CFG |= LINK_U2_ALLOW;
#endif
                            }
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
                            switch( (uint8_t)(USBSS_SetupReqIndex & 0xFF ))
                            {
                                case ( DEF_UEP1 | DEF_UEP_OUT ):
                                        /* Set End-point 1 OUT STALL */
                                        USBSSD->EP1_RX.UEP_RX_CR |= USBSS_EP_RX_HALT;
                                    break;

                                case ( DEF_UEP1 | DEF_UEP_IN ):
                                        /* Set End-point 1 IN STALL */
                                        USBSSD->EP1_TX.UEP_TX_CR |= USBSS_EP_TX_HALT;
                                    break;

                                case ( DEF_UEP2 | DEF_UEP_OUT ):
                                        /* Set End-point 2 OUT STALL */
                                        USBSSD->EP2_RX.UEP_RX_CR |= USBSS_EP_RX_HALT;
                                    break;

                                case ( DEF_UEP2 | DEF_UEP_IN ):
                                        /* Set End-point 2 IN STALL */
                                        USBSSD->EP2_TX.UEP_TX_CR |= USBSS_EP_TX_HALT;
                                    break;

                                case ( DEF_UEP3 | DEF_UEP_OUT ):
                                        /* Set End-point 3 OUT STALL */
                                        USBSSD->EP3_RX.UEP_RX_CR |= USBSS_EP_RX_HALT;
                                    break;

                                case ( DEF_UEP3 | DEF_UEP_IN ):
                                        /* Set End-point 3 IN STALL */
                                        USBSSD->EP3_TX.UEP_TX_CR |= USBSS_EP_TX_HALT;
                                    break;

                                case ( DEF_UEP4 | DEF_UEP_OUT ):
                                        /* Set End-point 4 OUT STALL */
                                        USBSSD->EP4_RX.UEP_RX_CR |= USBSS_EP_RX_HALT;
                                    break;

                                case ( DEF_UEP4 | DEF_UEP_IN ):
                                        /* Set End-point 4 IN STALL */
                                        USBSSD->EP4_TX.UEP_TX_CR |= USBSS_EP_TX_HALT;
                                    break;

                                case ( DEF_UEP5 | DEF_UEP_OUT ):
                                        /* Set End-point 5 OUT STALL */
                                        USBSSD->EP5_RX.UEP_RX_CR |= USBSS_EP_RX_HALT;
                                    break;

                                case ( DEF_UEP6 | DEF_UEP_IN ):
                                        /* Set End-point 6 IN STALL */
                                        USBSSD->EP6_TX.UEP_TX_CR |= USBSS_EP_TX_HALT;
                                    break;

                                default:
                                    errflag = 0xFF;
                                    break;
                            }
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
                case GET_STATUS:
                    USBSS_EP0_Buf[ 0 ] = 0x00;
                    USBSS_EP0_Buf[ 1 ] = 0x00;
                    if( ( USBSS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                    {
                        switch( (uint8_t)( USBSS_SetupReqIndex & 0xFF ) )
                        {
                            case (DEF_UEP1 | DEF_UEP_OUT):                            
                                if( USBSSD->EP1_RX.UEP_RX_CR & USBSS_EP_RX_HALT )
                                {
                                    USBSS_EP0_Buf[ 0 ] = 0x01;
                                }
                                break;

                            case (DEF_UEP1 | DEF_UEP_IN):
                                if( USBSSD->EP1_TX.UEP_TX_CR & USBSS_EP_TX_HALT )
                                {
                                    USBSS_EP0_Buf[ 0 ] = 0x01;
                                }
                                break;

                            case (DEF_UEP2 | DEF_UEP_OUT):                            
                                if( USBSSD->EP2_RX.UEP_RX_CR & USBSS_EP_RX_HALT )
                                {
                                    USBSS_EP0_Buf[ 0 ] = 0x01;
                                }
                                 break;

                            case (DEF_UEP2 | DEF_UEP_IN):
                                if( USBSSD->EP2_TX.UEP_TX_CR & USBSS_EP_TX_HALT )
                                {
                                    USBSS_EP0_Buf[ 0 ] = 0x01;
                                }
                                break;

                            case (DEF_UEP3 | DEF_UEP_OUT):
                                if( USBSSD->EP3_RX.UEP_RX_CR & USBSS_EP_RX_HALT )
                                {
                                    USBSS_EP0_Buf[ 0 ] = 0x01;
                                }
                                break;

                            case (DEF_UEP3 | DEF_UEP_IN):
                                if( USBSSD->EP3_TX.UEP_TX_CR & USBSS_EP_TX_HALT )
                                {
                                    USBSS_EP0_Buf[ 0 ] = 0x01;
                                }
                                break;

                            case (DEF_UEP4 | DEF_UEP_OUT):
                                if( USBSSD->EP4_RX.UEP_RX_CR & USBSS_EP_RX_HALT )
                                {
                                    USBSS_EP0_Buf[ 0 ] = 0x01;
                                }
                                break;

                            case (DEF_UEP4 | DEF_UEP_IN):
                                if( USBSSD->EP4_TX.UEP_TX_CR & USBSS_EP_TX_HALT )
                                {
                                    USBSS_EP0_Buf[ 0 ] = 0x01;
                                }
                                break;

                            case (DEF_UEP5 | DEF_UEP_OUT):
                                if( USBSSD->EP5_RX.UEP_RX_CR & USBSS_EP_RX_HALT )
                                {
                                    USBSS_EP0_Buf[ 0 ] = 0x01;
                                }
                                break;

                            case (DEF_UEP6 | DEF_UEP_IN):
                                if( USBSSD->EP6_TX.UEP_TX_CR & USBSS_EP_TX_HALT )
                                {
                                    USBSS_EP0_Buf[ 0 ] = 0x01;
                                }
                                break;

                            default:
                                errflag = 0xFF;
                                break;
                        }
                    }
                    else if( ( USBSS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                    {
                            if( USBSS_DevSleepStatus & 0x01 )
                            {
                                USBSS_EP0_Buf[ 0 ] = 0x02;
                            }
                    }

                    if ( USBSS_SetupReqLen > 2 )
                    {
                        USBSS_SetupReqLen = 2;
                    }
                    break;

                case SET_ENDPOINT:
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
            USBSSD->UEP0_TX_CTRL = USBSS_EP0_TX_STALL ;
            USBSSD->UEP0_RX_CTRL = USBSS_EP0_TX_ERDY | USBSS_EP0_RX_STALL ;
        }
        else
        {
            /* end-point 0 data Tx/Rx */
            if( USBSS_SetupReqType & DEF_UEP_IN )
            {
                /* tx */
                len = ( USBSS_SetupReqLen > DEF_USBSSD_UEP0_SIZE ) ? DEF_USBSSD_UEP0_SIZE : USBSS_SetupReqLen;
                USBSS_SetupReqLen -= len;
                USBSSD->UEP0_RX_CTRL = USBSS_EP0_RX_ACK;                               // STATUS stage
                USBSSD->UEP0_TX_CTRL = USBSS_EP0_TX_DPH | len;                         // DATA stage
                USBSSD->UEP0_TX_CTRL |= USBSS_EP0_TX_ERDY;
            }
            else
            {
                /* rx */
                USBSSD->UEP0_TX_CTRL = 0 ;
                USBSSD->UEP0_RX_CTRL = USBSS_EP0_TX_ERDY | USBSS_EP0_RX_ACK ;
            }
        }
        USBSSD->USB_STATUS = USBSS_UDIF_SETUP;
    }
    else if( USBSSD->USB_STATUS & USBSS_UDIF_STATUS )
    {
        USBSSD->USB_STATUS = USBSS_UDIF_STATUS;                                         // clear int flag
        if( ss_dev_info.set_rmt_wkup || ss_dev_info.set_func_susp )
        {
            ss_dev_info.set_rmt_wkup = 0;
            ss_dev_info.set_func_susp = 0;
        }
        else
        {
            if( ss_dev_info.u2_enable == ENABLE && !ss_dev_info.port_lpwr )
            {
                USBSSD->LINK_CTRL |= LINK_TX_LGO_U2;
            }
            else if( ss_dev_info.u1_enable == ENABLE && !ss_dev_info.port_u1_st )
            {
                USBSSD->LINK_CTRL |= LINK_TX_LGO_U1;
            }
        } 
        if( ss_dev_info.set_devaddr )
        {
            SET_Devic_Address( ss_dev_info.devaddr, USBSSH );                           // SET ADDRESS 
            ss_dev_info.set_devaddr = 0;
        }
        else if( ss_dev_info.set_isoch_delay )
        {
            USBSSD->LINK_ISO_DLY = ss_dev_info.set_isoch_value;
            ss_dev_info.set_isoch_delay = 0;
        }

        USBSSD->UEP0_TX_CTRL = 0;
        USBSSD->UEP0_RX_CTRL = 0;

    }
    else if( USBSSD->USB_STATUS & USBSS_UIF_TRANSFER )
    {
        endp_num =  (USBSSD->USB_STATUS & USB_INT_EP_MASK) >> 8;
        ep_dir = USBSSD->USB_STATUS & USB_TX_DIR;                                       // 1: IN, 0:OUT

        if( ep_dir )
        {
            switch( endp_num )
            {
                /* end-point 0 data in interrupt */
                case DEF_UEP0:
                    USBSSD->UEP0_TX_CTRL = USBSS_EP0_TX_DPH ;                           // Zero Length
                    USBSSD->UEP0_RX_CTRL = USBSS_EP0_TX_ERDY | USBSS_EP0_RX_ACK ;       // ready status step
                    break;

                /* end-point 1 data in interrupt */
                case DEF_UEP1:                   
                    USBSSD->EP1_TX.UEP_TX_CHAIN_ST |= USBSS_EP_TX_CHAIN_IF;
                    EP2_Chain_Sel ^= 0x01; 
                    USBSSD->EP2_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP2_Rx_Buf[ DEF_USB_EP2_SS_SIZE * DEF_ENDP2_OUT_BURST_LEVEL * EP2_Chain_Sel ]; 
                    // USBSSD->EP2_RX.UEP_RX_DMA_OFS = DEF_USB_EP2_SS_SIZE;
                    USBSSD->EP2_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP2_OUT_BURST_LEVEL; 
                    break; 

                /* end-point 2 data in interrupt */
                case DEF_UEP2:
                    USBSSD->EP2_TX.UEP_TX_CHAIN_ST |= USBSS_EP_TX_CHAIN_IF; 
                    EP1_Chain_Sel ^= 0x01; 
                    USBSSD->EP1_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP1_Rx_Buf[ DEF_USB_EP1_SS_SIZE * DEF_ENDP1_OUT_BURST_LEVEL * EP1_Chain_Sel ]; 
                    // USBSSD->EP1_RX.UEP_RX_DMA_OFS = DEF_USB_EP1_SS_SIZE;
                    USBSSD->EP1_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP1_OUT_BURST_LEVEL; 
                    break;  

                /* end-point 3 data in interrupt */
                case DEF_UEP3:
                    USBSSD->EP3_TX.UEP_TX_CHAIN_ST |= USBSS_EP_TX_CHAIN_IF;
                    USBSSD->EP3_TX.UEP_TX_DMA = (uint32_t)&USBSS_EP3_Tx_Buf;
                    USBSSD->EP3_TX.UEP_TX_CHAIN_LEN = DEF_USB_EP3_SS_SIZE;
                    USBSSD->EP3_TX.UEP_TX_CHAIN_EXP_NUMP = DEF_ENDP3_IN_BURST_LEVEL;
                    break;  

                /* end-point 4 data in interrupt */
                case DEF_UEP4:
                    USBSSD->EP4_TX.UEP_TX_CHAIN_ST |= USBSS_EP_TX_CHAIN_IF;
                    USBSSD->EP4_TX.UEP_TX_DMA = (uint32_t)&USBSS_EP4_Tx_Buf;
                    USBSSD->EP4_TX.UEP_TX_CHAIN_LEN = DEF_USB_EP4_SS_SIZE;
                    USBSSD->EP4_TX.UEP_TX_CHAIN_EXP_NUMP = DEF_ENDP4_IN_BURST_LEVEL;
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
                    USBSSD->UEP0_RX_CTRL = USBSS_EP0_TX_ERDY | USBSS_EP0_RX_ACK ;       // ready status step
                    break;
                    
                /* end-point 1 data out interrupt */
                case DEF_UEP1:
                    USBSSD->EP2_TX.UEP_TX_DMA = USBSSD->EP1_RX.UEP_RX_DMA - ( USBSSD->EP1_RX.UEP_RX_CHAIN_NUMP * USBSSD->EP1_RX.UEP_RX_DMA_OFS );
                    USBSSD->EP2_TX.UEP_TX_DMA_OFS = USBSSD->EP1_RX.UEP_RX_DMA_OFS;
                    USBSSD->EP2_TX.UEP_TX_CHAIN_LEN = USBSSD->EP1_RX.UEP_RX_CHAIN_LEN;
                    USBSSD->EP2_TX.UEP_TX_CHAIN_EXP_NUMP = USBSSD->EP1_RX.UEP_RX_CHAIN_NUMP;
                    USBSSD->EP1_RX.UEP_RX_CHAIN_ST |= USBSS_EP_RX_CHAIN_IF;                   
                    break; 

                /* end-point 2 data out interrupt */
                case DEF_UEP2:
                    USBSSD->EP1_TX.UEP_TX_DMA = USBSSD->EP2_RX.UEP_RX_DMA - ( USBSSD->EP2_RX.UEP_RX_CHAIN_NUMP * USBSSD->EP2_RX.UEP_RX_DMA_OFS );
                    USBSSD->EP1_TX.UEP_TX_DMA_OFS = USBSSD->EP2_RX.UEP_RX_DMA_OFS;
                    USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = USBSSD->EP2_RX.UEP_RX_CHAIN_LEN;
                    USBSSD->EP1_TX.UEP_TX_CHAIN_EXP_NUMP = USBSSD->EP2_RX.UEP_RX_CHAIN_NUMP;
                    USBSSD->EP2_RX.UEP_RX_CHAIN_ST |= USBSS_EP_RX_CHAIN_IF; 
                    break;  

                /* end-point 3 data out interrupt */
                case DEF_UEP3:
                    USBSSD->EP3_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP3_Rx_Buf;
                    USBSSD->EP3_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP3_OUT_BURST_LEVEL;
                    USBSSD->EP3_RX.UEP_RX_CHAIN_ST |= USBSS_EP_RX_CHAIN_IF;
                    break;  

                /* end-point 4 data out interrupt */
                case DEF_UEP4:
                    USBSSD->EP4_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP4_Rx_Buf;
                    USBSSD->EP4_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP4_OUT_BURST_LEVEL;
                    USBSSD->EP4_RX.UEP_RX_CHAIN_ST |= USBSS_EP_RX_CHAIN_IF;
                    break;  

                default:
                   errflag = 0xFF;
                break;                  
            }
        }
    }
}
