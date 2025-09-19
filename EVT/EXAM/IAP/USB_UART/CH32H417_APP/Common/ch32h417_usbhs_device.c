/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32h417_usbhs_device.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/03/05
 * Description        : USBHS
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32h417_usbhs_device.h"
#include "iap.h"

#define Version_Num   0x0100   //V0100

void USBHS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/* Endpoint Buffer */
__attribute__((aligned(4))) uint8_t USBHS_EP0_Buf[DEF_USBD_UEP0_SIZE];
__attribute__((aligned(4))) uint8_t USBHS_EP1_Tx_Buf[USBHS_MAX_PACK_SIZE];
__attribute__((aligned(4))) uint8_t USBHS_EP1_Rx_Buf[USBHS_MAX_PACK_SIZE];
__attribute__((aligned(4))) uint8_t USBHS_EP2_Tx_Buf[USBHS_MAX_PACK_SIZE];
__attribute__((aligned(4))) uint8_t USBHS_EP2_Rx_Buf[USBHS_MAX_PACK_SIZE];
__attribute__((aligned(4))) uint8_t USBHS_EP3_Tx_Buf[USBHS_MAX_PACK_SIZE];
__attribute__((aligned(4))) uint8_t USBHS_EP3_Rx_Buf[USBHS_MAX_PACK_SIZE];


#define pMySetupReqPak        ((PUSB_SETUP_REQ)Ep0Buffer)

const UINT8 *pUSBHS_Descr;
/* Setup Request */
volatile uint8_t USBHS_SetupReqCode;
volatile uint8_t USBHS_SetupReqType;
volatile uint16_t USBHS_SetupReqValue;
volatile uint16_t USBHS_SetupReqIndex;
volatile uint16_t USBHS_SetupReqLen;

/* USB Device Status */
volatile uint8_t USBHS_DevConfig;
volatile uint8_t USBHS_DevAddr;
volatile uint16_t USBHS_DevMaxPackLen;
volatile uint8_t USBHS_DevSpeed;
volatile uint8_t USBHS_DevSleepStatus;
volatile uint8_t USBHS_DevEnumStatus;

u8 Endp1Busy = 0;

u8 EP1_OUT_Flag = 0;
u8 EP2_OUT_Flag = 0;
u8 Endp3Busy = 0;
u8 Flag_LED = 0;
u8 EP1_Rx_Cnt, EP2_Rx_Cnt;
/******************************************************************************/
/* USB device descriptor */
UINT8 MyDevDescr[18] = { 0x12, 0x01, 0x10, 0x01, 0xFF, 0x80, 0x55, 0x40, 0x86,
        0x1A, 0xe0, 0x55,  //USB MODULE
        (u8) Version_Num, (u8) (Version_Num >> 8), //°æ±¾ºÅ V0100
        0x00, 0x00, 0x00, 0x01 };

/* USB configuration descriptor (full speed) */
const UINT8 MyCfgDescr_FS[] = { 0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80,
        0x32, 0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0x80, 0x55, 0x00, 0x07, 0x05,
        0x82, 0x02, 0x40, 0x00, 0x00, 0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00 };

/* USB Configuration Descriptor (High Speed) */
const UINT8 MyCfgDescr_HS[] = { 0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80,
        0x32, 0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0x80, 0x55, 0x00, 0x07, 0x05,
        0x82, 0x02, 0x40, 0x00, 0x00, 0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00 };

/* USB string descriptor */
const UINT8 MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };

/* USB产商字符串描述符 */
const UINT8 MyManuInfo[] = {
/* wch.cn */
0x0E, 0x03, 'w', 0x00, 'c', 0x00, 'h', 0x00, '.', 0x00, 'c', 0x00, 'n', 0x00 };

/* USB vendor string descriptor */
const UINT8 MyProdInfo[] = {
/* WCH USB2.0 DEVICE */
0x26, 0x03, 'W', 0x00, 'C', 0x00, 'H', 0x00, ' ', 0x00, 'U', 0x00, 'S', 0x00,
        'B', 0x00, '2', 0x00, '.', 0x00, '0', 0x00, ' ', 0x00, 'D', 0x00, 'E',
        0x00, 'V', 0x00, 'I', 0x00, 'C', 0x00, 'E', 0x00, ' ', 0x00 };

/* USB serial number string descriptor */
const UINT8 MySerNumInfo[] = {
/* 0123456789 */
0x16, 0x03, '0', 0x00, '1', 0x00, '2', 0x00, '3', 0x00, '4', 0x00, '5', 0x00,
        '6', 0x00, '7', 0x00, '8', 0x00, '9', 0x00, };

/* USB device qualified descriptor */
const UINT8 MyUSBQUADesc[] = { 0x0A, 0x06, 0x00, 0x02, 0xFF, 0x00, 0xFF, 0x40,
        0x01, 0x00, };
const UINT8 MyBOSDesc[] = { 0x05, 0x0f, 0x16, 0x00, 0x02, 0x07, 0x10, 0x02,
        0x02, 0x00, 0x00, 0x00, 0x0a, 0x10, 0x03, 0x00, 0x0e, 0x00, 0x01, 0x0a,
        0xff, 0x07, };

/* USB high speed mode, other speed configuration descriptors */
UINT8 TAB_USB_HS_OSC_DESC[sizeof(MyCfgDescr_FS)] = { 0x09, 0x07, /* Other parts are copied by the program */
};

/*********************************************************************
 * @fn      USBHS_RCC_Init
 *
 * @brief   Initializes the clock for USB2.0 High speed device.
 *
 * @return  none
 */
void USBHS_RCC_Init(FunctionalState sta)
{
    if (sta)
    {
        if((RCC->PLLCFGR & RCC_SYSPLL_SEL) != RCC_SYSPLL_USBHS)
        {
            /* Initialize USBHS 480M PLL */
            RCC_USBHS_PLLCmd(DISABLE);
            RCC_USBHSPLLCLKConfig(RCC_USBHSPLLSource_HSE);
            RCC_USBHSPLLReferConfig(RCC_USBHSPLLRefer_25M);
            RCC_USBHSPLLClockSourceDivConfig(RCC_USBHSPLL_IN_Div1);
            RCC_USBHS_PLLCmd(ENABLE);
			while (!(RCC->CTLR & RCC_USBHS_PLLRDY));
        }
        /* Enable UTMI Clock */
        RCC_UTMIcmd(ENABLE);
        /* Enable USBHS Clock */
        RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS, ENABLE);
    }
    else
    {
        RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS, DISABLE);
        RCC_UTMIcmd(DISABLE);
        if((RCC->PLLCFGR & RCC_SYSPLL_SEL) != RCC_SYSPLL_USBHS)
        {
            RCC_USBHS_PLLCmd(DISABLE);
        }
    }
}

/*********************************************************************
 * @fn      USBHS_Device_Endp_Init
 *
 * @brief   USBHS Device Endp Init
 *
 * @return  none
 */
void USBHS_Device_Endp_Init(void) {
 
    USBHSD->UEP_TX_EN = USBHS_UEP0_T_EN | USBHS_UEP1_T_EN | USBHS_UEP2_T_EN;
    USBHSD->UEP_RX_EN = USBHS_UEP0_R_EN | USBHS_UEP1_R_EN | USBHS_UEP2_R_EN;

    USBHSD->UEP0_MAX_LEN = 64;
    USBHSD->UEP1_MAX_LEN = 64;
    USBHSD->UEP2_MAX_LEN = 64;

    USBHSD->UEP0_DMA = (UINT32) (UINT8 *) USBHS_EP0_Buf;
    USBHSD->UEP1_RX_DMA = (UINT32) (UINT8 *) USBHS_EP1_Rx_Buf;
    USBHSD->UEP1_TX_DMA = (UINT32) (UINT8 *) (&USBHS_EP1_Tx_Buf);
    USBHSD->UEP2_RX_DMA = (UINT32) (UINT8 *) USBHS_EP2_Rx_Buf;
    USBHSD->UEP2_TX_DMA = (UINT32) (UINT8 *) (&USBHS_EP2_Tx_Buf);

    USBHSD->UEP0_TX_LEN = 0;
    USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_RES_NAK;
    USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_RES_ACK;

    USBHSD->UEP1_TX_LEN = 0;
    USBHSD->UEP1_TX_CTRL = USBHS_UEP_T_RES_NAK;
    USBHSD->UEP1_RX_CTRL = USBHS_UEP_R_RES_ACK;

    USBHSD->UEP2_TX_LEN = 0;
    USBHSD->UEP2_TX_CTRL = USBHS_UEP_T_RES_NAK;
    USBHSD->UEP2_RX_CTRL = USBHS_UEP_R_RES_ACK;

}

/*********************************************************************
 * @fn      USBHS_Device_Init
 *
 * @brief   USBHS Device Init
 *
 * @return  none
 */
void USBHS_Device_Init(FunctionalState sta) {
    if(sta)
    {
        USBHS_RCC_Init(ENABLE);
        USBHSD->CONTROL = USBHS_UD_RST_LINK | USBHS_UD_PHY_SUSPENDM;
        USBHSD->INT_EN = USBHS_UDIE_BUS_RST | USBHS_UDIE_SUSPEND | USBHS_UDIE_BUS_SLEEP | USBHS_UDIE_LPM_ACT | USBHS_UDIE_TRANSFER | USBHS_UDIE_LINK_RDY;
        USBHS_Device_Endp_Init();
        USBHSD->BASE_MODE = USBHS_UD_SPEED_HIGH;
        USBHSD->CONTROL = USBHS_UD_DEV_EN | USBHS_UD_DMA_EN | USBHS_UD_LPM_EN | USBHS_UD_PHY_SUSPENDM;
        NVIC_EnableIRQ(USBHS_IRQn);
    }
    else
    {
        USBHSD->CONTROL |= USBHS_UD_RST_SIE;
        USBHSD->CONTROL &= ~USBHS_UD_RST_SIE;
        NVIC_DisableIRQ(USBHS_IRQn);
        USBHS_RCC_Init(DISABLE);
    }
}

/*********************************************************************
 * @fn      USBHS_Device_SetAddress
 *
 * @brief   USBHS Device SetAddress
 *
 * @return  none
 */
void USBHS_Device_SetAddress(UINT32 address) {
    USBHSD->DEV_AD = 0;
    USBHSD->DEV_AD = address & 0xff;
}
/*********************************************************************
 * @fn      USBHS_IRQHandler
 *
 * @brief   This function handles USBHS exception.
 *
 * @return  none
 */
void USBHS_IRQHandler(void)
{
    uint8_t intflag, intst, errflag;
    uint16_t len;
    uint8_t endp_num;

    intflag = USBHSD->INT_FG;
    intst = USBHSD->INT_ST;
    if(intflag & USBHS_UDIF_TRANSFER)
    {
        endp_num = intst & USBHS_UDIS_EP_ID_MASK;
        if(!(USBHSD->INT_ST & USBHS_UDIS_EP_DIR))  // SETUP/OUT Transaction
        {
            switch(endp_num)
            {
            case DEF_UEP0:
                if(USBHSD->UEP0_RX_CTRL & USBHS_UEP_R_SETUP_IS)
                {
                    /* Store All Setup Values */
                    USBHS_SetupReqType = pUSBHS_SetupReqPak->bRequestType;
                    USBHS_SetupReqCode = pUSBHS_SetupReqPak->bRequest;
                    USBHS_SetupReqLen = pUSBHS_SetupReqPak->wLength;
                    USBHS_SetupReqValue = pUSBHS_SetupReqPak->wValue;
                    USBHS_SetupReqIndex = pUSBHS_SetupReqPak->wIndex;

                    len = 0;
                    errflag = 0;
                    if((USBHS_SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD)
                    {
                        /* usb non-standard request processing */
                        errflag = 0xFF;
                    }
                    else
                    {
                        /* usb standard request processing */
                        switch(USBHS_SetupReqCode)
                        {
                        /* get device/configuration/string/report/... descriptors */
                        case USB_GET_DESCRIPTOR:
                            switch((uint8_t)(USBHS_SetupReqValue >> 8))
                            {
                            /* get usb device descriptor */
                            case USB_DESCR_TYP_DEVICE:
                                pUSBHS_Descr = MyDevDescr;
                                len =  MyDevDescr[0];
                                break;

                            /* get usb configuration descriptor */
                            case USB_DESCR_TYP_CONFIG:
                                /* Load usb configuration descriptor by speed */
                                /* High speed mode */
                                pUSBHS_Descr = MyCfgDescr_HS;
                                len = MyCfgDescr_HS[2];
                                break;

                            /* get usb string descriptor */
                            case USB_DESCR_TYP_STRING:
                                switch((uint8_t)(USBHS_SetupReqValue & 0xFF))
                                {
                                /* Descriptor 0, Language descriptor */
                                case DEF_STRING_DESC_LANG:
                                    pUSBHS_Descr = MyLangDescr;
                                    len = MyLangDescr[0];
                                    break;

                                /* Descriptor 1, Manufacturers String descriptor */
                                case DEF_STRING_DESC_MANU:
                                    pUSBHS_Descr = MyManuInfo;
                                    len = sizeof(MyManuInfo);
                                    break;

                                /* Descriptor 2, Product String descriptor */
                                case DEF_STRING_DESC_PROD:
                                    pUSBHS_Descr = MyProdInfo;
                                    len = sizeof(MyProdInfo);
                                    break;

                                /* Descriptor 3, Serial-number String descriptor */
                                case DEF_STRING_DESC_SERN:
                                    pUSBHS_Descr = MySerNumInfo;
                                    len = sizeof(MySerNumInfo);
                                    break;

                                default:
                                    errflag = 0xFF;
                                    break;
                                }
                                break;

                            /* get usb device qualify descriptor */
                            case USB_DESCR_TYP_QUALIF:
                                pUSBHS_Descr = MyUSBQUADesc;
                                len = sizeof(MyUSBQUADesc);
                                break;

                            /* get usb BOS descriptor */
                            case USB_DESCR_TYP_BOS:
                                /* USB 2.00 DO NOT support BOS descriptor */
                                errflag = 0xFF;
                                break;

                            /* get usb other-speed descriptor */
                            case USB_DESCR_TYP_SPEED:
                                if(USBHS_DevSpeed == USBHS_SPEED_HIGH)
                                {
                                    /* High speed mode */
                                    errflag = 0xFF; 
                                }
                                break;

                            default:
                                errflag = 0xFF;
                                break;
                            }

                            /* Copy Descriptors to Endp0 DMA buffer */
                            if(USBHS_SetupReqLen > len)
                            {
                                USBHS_SetupReqLen = len;
                            }
                            len = (USBHS_SetupReqLen >= DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                            memcpy(USBHS_EP0_Buf, pUSBHS_Descr, len);
                            pUSBHS_Descr += len;
                            break;

                        /* Set usb address */
                        case USB_SET_ADDRESS:
                            USBHS_DevAddr = (uint16_t)(USBHS_SetupReqValue & 0xFF);
                            break;

                        /* Get usb configuration now set */
                        case USB_GET_CONFIGURATION:
                            USBHS_EP0_Buf[0] = USBHS_DevConfig;
                            if(USBHS_SetupReqLen > 1)
                            {
                                USBHS_SetupReqLen = 1;
                            }
                            break;

                        /* Set usb configuration to use */
                        case USB_SET_CONFIGURATION:
                            USBHS_DevConfig = (uint8_t)(USBHS_SetupReqValue & 0xFF);
                            USBHS_DevEnumStatus = 0x01;
                            break;

                        /* Clear or disable one usb feature */
                        case USB_CLEAR_FEATURE:
                            if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE)
                            {
                                /* clear one device feature */
                                if((uint8_t)(USBHS_SetupReqValue & 0xFF) == 0x01)
                                {
                                    /* clear usb sleep status, device not prepare to sleep */
                                    USBHS_DevSleepStatus &= ~0x01;
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                            }
                            else if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
                            {
                                /* Set End-point Feature */
                                if((uint8_t)(USBHS_SetupReqValue & 0xFF) == USB_REQ_FEAT_ENDP_HALT)
                                {
                                    /* Clear End-point Feature */
                                    switch((uint8_t)(USBHS_SetupReqIndex & 0xFF))
                                    {
                                    case(DEF_UEP1 | DEF_UEP_OUT):
                                        /* Set End-point 1 OUT ACK */
                                        USBHSD->UEP1_RX_CTRL = USBHS_UEP_R_RES_ACK;
                                        break;

                                    case(DEF_UEP1 | DEF_UEP_IN):
                                        /* Set End-point 1 IN NAK */
                                        USBHSD->UEP1_TX_CTRL = USBHS_UEP_T_RES_NAK;
                                        break;

                                    case(DEF_UEP3 | DEF_UEP_OUT):
                                        /* Set End-point 3 OUT ACK */
                                        USBHSD->UEP3_RX_CTRL = USBHS_UEP_R_RES_ACK;
                                        break;

                                    case(DEF_UEP4 | DEF_UEP_IN):
                                        /* Set End-point 4 IN NAK */
                                        USBHSD->UEP4_TX_CTRL = USBHS_UEP_T_RES_NAK;
                                        break;

                                    case(DEF_UEP5 | DEF_UEP_OUT):
                                        /* Set End-point 5 OUT ACK */
                                        USBHSD->UEP5_RX_CTRL = USBHS_UEP_R_RES_ACK;
                                        break;

                                    case(DEF_UEP6 | DEF_UEP_IN):
                                        /* Set End-point 6 IN NAK */
                                        USBHSD->UEP6_TX_CTRL = USBHS_UEP_T_RES_NAK;
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

                        /* set or enable one usb feature */
                        case USB_SET_FEATURE:
                            if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE)
                            {
                                /* Set Device Feature */
                                if((uint8_t)(USBHS_SetupReqValue & 0xFF) == USB_REQ_FEAT_REMOTE_WAKEUP)
                                {
                                    if(((USBHS_DevSpeed == USBHS_SPEED_HIGH) && (MyCfgDescr_HS[7] & 0x20)) ||
                                       ((USBHS_DevSpeed == USBHS_SPEED_FULL) && (MyCfgDescr_FS[7] & 0x20)))
                                    {
                                        /* Set Wake-up flag, device prepare to sleep */
                                        USBHS_DevSleepStatus |= 0x01;
                                    }
                                    else
                                    {
                                        errflag = 0xFF;
                                    }
                                }
                                else if((uint8_t)(USBHS_SetupReqValue & 0xFF) == 0x02)
                                {
                                    /* test mode deal */
                                    errflag = 0xFF;
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                            }
                            else if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
                            {
                                /* Set End-point Feature */
                                if((uint8_t)(USBHS_SetupReqValue & 0xFF) == USB_REQ_FEAT_ENDP_HALT)
                                {
                                    /* Set end-points status stall */
                                    switch((uint8_t)(USBHS_SetupReqIndex & 0xFF))
                                    {
                                    case(DEF_UEP1 | DEF_UEP_OUT):
                                        /* Set End-point 1 OUT STALL */
                                        USBHSD->UEP1_RX_CTRL = (USBHSD->UEP1_RX_CTRL & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_STALL;
                                        break;

                                    case(DEF_UEP1 | DEF_UEP_IN):
                                        /* Set End-point 1 IN STALL */
                                        USBHSD->UEP1_TX_CTRL = (USBHSD->UEP1_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_STALL;
                                        break;

                                    case(DEF_UEP3 | DEF_UEP_OUT):
                                        /* Set End-point 3 OUT STALL */
                                        USBHSD->UEP3_RX_CTRL = (USBHSD->UEP3_RX_CTRL & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_STALL;
                                        break;

                                    case(DEF_UEP4 | DEF_UEP_IN):
                                        /* Set End-point 4 IN STALL */
                                        USBHSD->UEP4_TX_CTRL = (USBHSD->UEP4_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_STALL;
                                        break;

                                    case(DEF_UEP5 | DEF_UEP_OUT):
                                        /* Set End-point 5 OUT STALL */
                                        USBHSD->UEP5_RX_CTRL = (USBHSD->UEP5_RX_CTRL & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_STALL;
                                        break;

                                    case(DEF_UEP6 | DEF_UEP_IN):
                                        /* Set End-point 6 IN STALL */
                                        USBHSD->UEP6_TX_CTRL = (USBHSD->UEP6_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_STALL;
                                        break;

                                    default:
                                        errflag = 0xFF;
                                        break;
                                    }
                                }
                            }
                            break;

                        /* This request allows the host to select another setting for the specified interface  */
                        case USB_GET_INTERFACE:
                            USBHS_EP0_Buf[0] = 0x00;
                            if(USBHS_SetupReqLen > 1)
                            {
                                USBHS_SetupReqLen = 1;
                            }
                            break;

                        case USB_SET_INTERFACE:
                            break;

                        /* host get status of specified device/interface/end-points */
                        case USB_GET_STATUS:
                            USBHS_EP0_Buf[0] = 0x00;
                            USBHS_EP0_Buf[1] = 0x00;
                            if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
                            {
                                switch((uint8_t)(USBHS_SetupReqIndex & 0xFF))
                                {
                                case(DEF_UEP1 | DEF_UEP_OUT):
                                    if(((USBHSD->UEP1_RX_CTRL) & USBHS_UEP_R_RES_MASK) == USBHS_UEP_R_RES_STALL)
                                    {
                                        USBHS_EP0_Buf[0] = 0x01;
                                    }

                                case(DEF_UEP1 | DEF_UEP_IN):
                                    if(((USBHSD->UEP1_TX_CTRL) & USBHS_UEP_T_RES_MASK) == USBHS_UEP_T_RES_STALL)
                                    {
                                        USBHS_EP0_Buf[0] = 0x01;
                                    }
                                    break;

                                case(DEF_UEP2 | DEF_UEP_IN):
                                    if(((USBHSD->UEP2_TX_CTRL) & USBHS_UEP_T_RES_MASK) == USBHS_UEP_T_RES_STALL)
                                    {
                                        USBHS_EP0_Buf[0] = 0x01;
                                    }
                                    break;

                                case(DEF_UEP3 | DEF_UEP_OUT):
                                    if(((USBHSD->UEP3_RX_CTRL) & USBHS_UEP_R_RES_MASK) == USBHS_UEP_R_RES_STALL)
                                    {
                                        USBHS_EP0_Buf[0] = 0x01;
                                    }
                                    break;

                                case(DEF_UEP4 | DEF_UEP_IN):
                                    if(((USBHSD->UEP4_TX_CTRL) & USBHS_UEP_T_RES_MASK) == USBHS_UEP_T_RES_STALL)
                                    {
                                        USBHS_EP0_Buf[0] = 0x01;
                                    }
                                    break;

                                case(DEF_UEP5 | DEF_UEP_OUT):
                                    if(((USBHSD->UEP5_RX_CTRL) & USBHS_UEP_R_RES_MASK) == USBHS_UEP_R_RES_STALL)
                                    {
                                        USBHS_EP0_Buf[0] = 0x01;
                                    }
                                    break;

                                case(DEF_UEP6 | DEF_UEP_IN):
                                    if(((USBHSD->UEP6_TX_CTRL) & USBHS_UEP_T_RES_MASK) == USBHS_UEP_T_RES_STALL)
                                    {
                                        USBHS_EP0_Buf[0] = 0x01;
                                    }
                                    break;

                                default:
                                    errflag = 0xFF;
                                    break;
                                }
                            }
                            else if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE)
                            {
                                if(USBHS_DevSleepStatus & 0x01)
                                {
                                    USBHS_EP0_Buf[0] = 0x02;
                                }
                            }

                            if(USBHS_SetupReqLen > 2)
                            {
                                USBHS_SetupReqLen = 2;
                            }
                            break;

                        default:
                            errflag = 0xFF;
                            break;
                        }
                    }

                    /* errflag = 0xFF means a request not support or some errors occurred, else correct */
                    if(errflag == 0xFF)
                    {
                        /* if one request not support, return stall */
                        USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_STALL;
                        USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_STALL;
                    }
                    else
                    {
                        /* end-point 0 data Tx/Rx */
                        if(USBHS_SetupReqType & DEF_UEP_IN)
                        {
                            /* tx */
                            len = (USBHS_SetupReqLen > DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                            USBHS_SetupReqLen -= len;
                            USBHSD->UEP0_TX_LEN = len;
                            USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
                        }
                        else
                        {
                            /* rx */
                            if(USBHS_SetupReqLen == 0)
                            {
                                USBHSD->UEP0_TX_LEN = 0;
                                USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
                            }
                            else
                            {
                                USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_ACK;
                            }
                        }
                    }
                }
                /* end-point 0 data out interrupt */
                else
                {
                    USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_RES_NAK;  // clear
                    len = USBHSD->UEP0_RX_LEN;

                    /* if any processing about rx, set it here */
                    if((USBHS_SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD)
                    {
                        USBHS_SetupReqLen = 0;
                        /* Non-standard request end-point 0 Data download */
                        if(USBHS_SetupReqCode == CDC_SET_LINE_CODING)
                        {
                        }
                    }
                    else
                    {
                        /* Standard request end-point 0 Data download */
                    }

                    if(USBHS_SetupReqLen == 0)
                    {
                        USBHSD->UEP0_TX_LEN = 0;
                        USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
                    }
                }
                USBHSD->UEP0_RX_CTRL &= ~USBHS_UEP_R_DONE;
                break;
            /* end-point 1 data out interrupt */
            case DEF_UEP1:
                if(USBHSD->UEP1_RX_CTRL & USBHS_UEP_R_TOG_MATCH)
                {

                }
                USBHSD->UEP1_RX_CTRL &= ~USBHS_UEP_R_DONE;
                break;

            /* end-point 3 data out interrupt */
            case DEF_UEP2:
                if(USBHSD->UEP2_RX_CTRL & USBHS_UEP_R_TOG_MATCH)
                {
                    EP2_Rx_Cnt = USBHSD->UEP2_RX_LEN;
                    EP2_OUT_Flag = 1;
                    USBHSD->UEP2_RX_CTRL ^= USBHS_UEP_R_TOG_DATA1;
                    USBHSD->UEP2_RX_CTRL = ((USBHSD->UEP2_RX_CTRL) & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
                    DevEPhs_OUT_Deal(EP2_Rx_Cnt);
                }
                USBHSD->UEP2_RX_CTRL &= ~USBHS_UEP_R_DONE;
                break;



            default:
                errflag = 0xFF;
                break;
            }
        }
        else
        {
            /* data-in stage processing */
            switch(endp_num)
            {
            /* end-point 0 data in interrupt */
            case DEF_UEP0:
                if(USBHS_SetupReqLen == 0)
                {
                    USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_ACK;
                }
                if((USBHS_SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD)
                {
                    /* Non-standard request endpoint 0 Data upload */
                }
                else
                {
                    /* Standard request endpoint 0 Data upload */
                    switch(USBHS_SetupReqCode)
                    {
                    case USB_GET_DESCRIPTOR:
                        len = USBHS_SetupReqLen >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                        memcpy(USBHS_EP0_Buf, pUSBHS_Descr, len);
                        USBHS_SetupReqLen -= len;
                        pUSBHS_Descr += len;
                        USBHSD->UEP0_TX_LEN = len;
                        USBHSD->UEP0_TX_CTRL ^= USBHS_UEP_T_TOG_DATA1;
                        USBHSD->UEP0_TX_CTRL = (USBHSD->UEP0_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_ACK;  // clear
                        break;

                    case USB_SET_ADDRESS:
                        USBHSD->DEV_AD = USBHS_DevAddr;
                        break;

                    default:
                        USBHSD->UEP0_TX_LEN = 0;
                        break;
                    }
                }

                USBHSD->UEP0_TX_CTRL &= ~USBHS_UEP_T_DONE;
                break;

            /* end-point 1 data in interrupt */
            case DEF_UEP1:
                USBHSD->UEP1_TX_CTRL &= ~USBHS_UEP_T_DONE;
                break;
            /* end-point 2 data in interrupt */
            case DEF_UEP2:
                USBHSD->UEP2_TX_CTRL = (USBHSD->UEP2_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_NAK;
                USBHSD->UEP2_TX_CTRL ^= USBHS_UEP_T_TOG_DATA1;
                USBHSD->UEP2_TX_CTRL &= ~USBHS_UEP_T_DONE;
                break;

            default:
                break;
            }
        }
    }
    else if(intflag & USBHS_UDIF_LINK_RDY)
    {
        USBHSD->INT_FG = USBHS_UDIF_LINK_RDY;
    }
    else if(intflag & USBHS_UDIF_SUSPEND)
    {
        USBHSD->INT_FG = USBHS_UDIF_SUSPEND;
        /* usb suspend interrupt processing */
        if(USBHSD->MIS_ST & USBHS_UDMS_SUSPEND)
        {
            USBHS_DevSleepStatus |= 0x02;
            if(USBHS_DevSleepStatus == 0x03)
            {
                /* Handling usb sleep here */
            }
        }
        else
        {
            USBHS_DevSleepStatus &= ~0x02;
        }
    }
    else if(intflag & USBHS_UDIF_BUS_RST)
    {
        /* usb reset interrupt processing */
        USBHS_DevConfig = 0;
        USBHS_DevAddr = 0;
        USBHS_DevSleepStatus = 0;
        USBHS_DevEnumStatus = 0;

        USBHSD->DEV_AD = 0;
        USBHS_Device_Endp_Init();
        USBHSD->INT_FG = USBHS_UDIF_BUS_RST;
    }
    else
    {
        /* other interrupts */
        USBHSD->INT_FG = intflag;
    }
}


/*********************************************************************
 * @fn      void DevEPhs_IN_Deal
 *
 * @brief   Device endpoint2 IN.
 *
 * @param   l - IN length(<64B)
 *
 * @return  none
 */
void DevEPhs_IN_Deal(UINT8 l) {
    USBHSD->UEP2_TX_LEN = l;
    USBHSD->UEP2_TX_CTRL = (USBHSD->UEP2_TX_CTRL & (~USBHS_UEP_T_RES_MASK))
            | USBHS_UEP_T_RES_ACK;
}

/*********************************************************************
 * @fn      DevEPhs_OUT_Deal
 *
 * @brief   Deal device Endpoint 2 OUT.
 *
 * @param   l - Data length.
 *
 * @return  none
 */
void DevEPhs_OUT_Deal(UINT8 l) {
    UINT8 s, EP2_Tx_Cnt;
    memcpy(EP2_Rx_Buffer, USBHS_EP2_Rx_Buf, l);
    s = RecData_Deal();
    if (s != ERR_End) {
        USBHS_EP2_Rx_Buf[0] = 0x00;
        if (s == ERR_ERROR)
            USBHS_EP2_Rx_Buf[1]  = 0x01;
        else
            USBHS_EP2_Rx_Buf[1] = 0x00;
        EP2_Tx_Cnt = 2;
        DevEPhs_IN_Deal(EP2_Tx_Cnt);

    }
}

