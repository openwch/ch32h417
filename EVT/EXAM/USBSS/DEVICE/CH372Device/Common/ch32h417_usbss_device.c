/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_usbss_device.c
* Author             : WCH
* Version            : V1.0
* Date               : 2025/05/23
* Description        : This file provides all the USBSS firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "ch32h417_usbss_device.h"
#include "ch32h417_usbhs_device.h"

USBSS_DEV_Info ss_dev_info;
vu8 EP1_Chain_Sel = 0;
vu8 EP2_Chain_Sel = 0;
vu8 U3_UP_Flag = 0;
vu32 U3_Time_Out_Count = 0;

/*********************************************************************
 * @fn      USBSS_Device_Init
 *
 * @brief   Initializes USB susper-speed device.
 *
 * @return  none
 */
void USBSS_Device_Init( FunctionalState sta )                                                     
{
    if(sta)
    {
        USBSSD->LINK_CFG = LINK_RX_EQ_EN | LINK_TX_DEEMPH_MASK | LINK_PHY_RESET;   
        USBSSD->LINK_CTRL = POWER_MODE_2 | LINK_GO_DISABLED;                        
        USBSSD->LINK_CFG = LINK_RX_EQ_EN | LINK_TX_DEEMPH_MASK | LINK_U2_RXDET;

        USBSSD->LINK_LPM_CR |= LINK_LPM_EN;

        while( USBSSD->LINK_STATUS & LINK_BUSY );                                   
        USBSSD->LINK_CFG |= LINK_RX_TERM_EN;                                     
        USBSSD->LINK_INT_CTRL =  LINK_IE_TX_LMP | LINK_IE_RX_LMP | LINK_IE_RX_LMP_TOUT | LINK_IE_STATE_CHG
                                    | LINK_IE_WARM_RST | LINK_IE_TERM_PRES;
        USBSSD->LINK_CTRL = POWER_MODE_2;
        USBSSD->LINK_U1_WKUP_FILTER = 0;
        USBSSD->LINK_U2_WKUP_FILTER = 0;
        USBSSD->LINK_U3_WKUP_FILTER = 0;
        USBSSD->USB_CONTROL |= USBSS_FORCE_RST;
        USBSSD->USB_STATUS = USBSS_UIF_TRANSFER;
        USBSSD->USB_CONTROL =  USBSS_UIE_TRANSFER | USBSS_UDIE_SETUP | USBSS_UDIE_STATUS | USBSS_DMA_EN | USBSS_SETUP_FLOW;
        USBSS_Device_Endp_Init ( );
    }
    else 
    {
        USBSSD->USB_CONTROL |= USBSS_FORCE_RST;
        USBSSD->USB_CONTROL &= ~USBSS_FORCE_RST;
    }
}

/*********************************************************************
 * @fn      USBHS_Device_Endp_Init
 *
 * @brief   Initializes USB device endpoints.
 *
 * @return  none
 */
void USBSS_Device_Endp_Deinit ( )
{
    USBSSD->USB_CONTROL |= USBSS_USB_CLR_ALL;
    USBSSD->USB_CONTROL &= ~USBSS_USB_CLR_ALL;
}

/*********************************************************************
 * @fn      USBHS_Device_Endp_Init
 *
 * @brief   DeInitializes USB device endpoints.
 *
 * @return  none
 */
void USBSS_Device_Endp_Init( )
{
    USBSS_Device_Endp_Deinit( );

    USBSSD->UEP_TX_EN = USBSS_EP1_TX_EN | USBSS_EP2_TX_EN | USBSS_EP3_TX_EN | USBSS_EP4_TX_EN;
    USBSSD->UEP_RX_EN = USBSS_EP1_RX_EN | USBSS_EP2_RX_EN | USBSS_EP3_RX_EN | USBSS_EP4_RX_EN;

    USBSSD->UEP0_TX_CTRL = 0;
    USBSSD->UEP0_RX_CTRL = 0;

    USBSSD->UEP0_TX_DMA = (uint32_t)&USBSS_EP0_Buf;
    USBSSD->UEP0_RX_DMA = (uint32_t)&USBSS_EP0_Buf;
    
    EP1_Chain_Sel = 0;
    USBSSD->EP1_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP1_Rx_Buf[ DEF_USB_EP1_SS_SIZE * DEF_ENDP1_OUT_BURST_LEVEL * EP1_Chain_Sel ];
    USBSSD->EP1_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP1_OUT_BURST_LEVEL; 
    EP1_Chain_Sel++;
    USBSSD->EP1_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP1_Rx_Buf[ DEF_USB_EP1_SS_SIZE * DEF_ENDP1_OUT_BURST_LEVEL * EP1_Chain_Sel ];
    USBSSD->EP1_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP1_OUT_BURST_LEVEL;  

    EP2_Chain_Sel = 0;
    USBSSD->EP2_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP2_Rx_Buf[ DEF_USB_EP2_SS_SIZE * DEF_ENDP2_OUT_BURST_LEVEL * EP2_Chain_Sel ];
    USBSSD->EP2_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP2_OUT_BURST_LEVEL; 
    EP2_Chain_Sel++;
    USBSSD->EP2_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP2_Rx_Buf[ DEF_USB_EP2_SS_SIZE * DEF_ENDP2_OUT_BURST_LEVEL * EP2_Chain_Sel ];
    USBSSD->EP2_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP2_OUT_BURST_LEVEL;  

    USBSSD->EP3_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP3_Rx_Buf;
    USBSSD->EP3_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP3_OUT_BURST_LEVEL;

    USBSSD->EP3_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP3_Rx_Buf[ DEF_USB_EP3_SS_SIZE * DEF_ENDP3_OUT_BURST_LEVEL ];
    USBSSD->EP3_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP3_OUT_BURST_LEVEL;

    USBSSD->EP3_TX.UEP_TX_DMA = (uint32_t)&USBSS_EP3_Tx_Buf;
    USBSSD->EP3_TX.UEP_TX_CHAIN_LEN = DEF_USB_EP3_SS_SIZE;
    USBSSD->EP3_TX.UEP_TX_CHAIN_EXP_NUMP = DEF_ENDP3_IN_BURST_LEVEL;

    USBSSD->EP3_TX.UEP_TX_DMA = (uint32_t)&USBSS_EP3_Tx_Buf[ DEF_USB_EP3_SS_SIZE * DEF_ENDP3_IN_BURST_LEVEL ];
    USBSSD->EP3_TX.UEP_TX_CHAIN_LEN = DEF_USB_EP3_SS_SIZE;
    USBSSD->EP3_TX.UEP_TX_CHAIN_EXP_NUMP = DEF_ENDP3_IN_BURST_LEVEL;

    USBSSD->EP4_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP4_Rx_Buf;
    USBSSD->EP4_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP4_OUT_BURST_LEVEL;

    USBSSD->EP4_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP4_Rx_Buf[ DEF_USB_EP4_SS_SIZE * DEF_ENDP4_OUT_BURST_LEVEL ];
    USBSSD->EP4_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP4_OUT_BURST_LEVEL;

    USBSSD->EP4_TX.UEP_TX_DMA = (uint32_t)&USBSS_EP4_Tx_Buf;
    USBSSD->EP4_TX.UEP_TX_CHAIN_LEN = DEF_USB_EP4_SS_SIZE;
    USBSSD->EP4_TX.UEP_TX_CHAIN_EXP_NUMP = DEF_ENDP4_IN_BURST_LEVEL;

    USBSSD->EP4_TX.UEP_TX_DMA = (uint32_t)&USBSS_EP4_Tx_Buf[ DEF_USB_EP4_SS_SIZE * DEF_ENDP4_IN_BURST_LEVEL ];
    USBSSD->EP4_TX.UEP_TX_CHAIN_LEN = DEF_USB_EP4_SS_SIZE;
    USBSSD->EP4_TX.UEP_TX_CHAIN_EXP_NUMP = DEF_ENDP4_IN_BURST_LEVEL;

    U3_UP_Flag = 0;
    U3_Time_Out_Count = 0;
}

/*********************************************************************
 * @fn      USBSS_LINK_Handle
 *
 * @brief   Handle USB 3.0 Link Layer state changes and related interrupts.
 *          This function processes different link states and link management
 *          packet (LMP) interrupts for USB 3.0 device or host controller.
 *
 * @param   USBSSHx   - Pointer to USB SuperSpeed host/device controller register structure.
 *          port_num  - Port number for which the link is being handled.
 *
 * @return  none
 */
void USBSS_LINK_Handle( USBSSH_TypeDef *USBSSHx ,uint8_t port_num ) 
{
    uint32_t link_state;

    link_state = USBSSHx->LINK_STATUS & LINK_STATE_MASK;
 

    if( USBSSHx->LINK_INT_FLAG & LINK_IF_STATE_CHG )
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_STATE_CHG;

        if( link_state == LINK_STATE_RXDET )
        {

        }
        else if( link_state == LINK_STATE_POLLING )
        { 

        }
        else if( link_state == LINK_STATE_HOTRST )
        { 
            USBSS_Device_Init( ENABLE );
            USBSS_DevEnumStatus = 0;
            USBSSHx->LINK_CTRL &= ~LINK_HOT_RESET;                                          // HOT RESET end(device mode)

        }
        else if( link_state == LINK_STATE_DISABLE )
        {
            USBHS_Device_Init( ENABLE ); 
        }
        else if( link_state == LINK_STATE_INACTIVE )
        {

        }
        else if( link_state == LINK_STATE_U0 )
        {
            U3_UP_Flag = 1;
            U3_Time_Out_Count = 0;
        }       
    }
    else if( USBSSHx->LINK_INT_FLAG & LINK_IF_TERM_PRES )
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_TERM_PRES;
        USBSS_DevEnumStatus = 0;
        if( USBSSHx->LINK_STATUS & LINK_RX_TERM_PRES )
        {
            U3_UP_Flag = 0; 
            U3_Time_Out_Count = 0;

            USBHS_Device_Init( DISABLE ); 
        }
        else 
        {
            USBHS_Device_Init( ENABLE ); 
        }
    }
    else if( USBSSHx->LINK_INT_FLAG & LINK_IF_RX_LMP_TOUT )                                 // port config err
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_RX_LMP_TOUT;
        USBSSHx->LINK_CTRL |= LINK_GO_DISABLED;                                             // upstream
        USBSSHx->LINK_CTRL |= LINK_GO_RX_DET;                                               // upstream 
    }
    else if( USBSSHx->LINK_INT_FLAG & LINK_IF_TX_LMP )
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_TX_LMP;
        USBSSHx->LINK_LMP_TX_DATA0 = LMP_LINK_SPEED | LMP_PORT_CAP | LMP_HP;

        if( USBSSHx->LINK_CFG & LINK_DOWN_MODE )
        {
            USBSSHx->LINK_LMP_TX_DATA1 = DOWN_STREAM | NUM_HP_BUF;
        }
        else
        {
            USBSSHx->LINK_LMP_TX_DATA1 = UP_STREAM | NUM_HP_BUF;
        }
        USBSSHx->LINK_LMP_TX_DATA2 = 0x0;
    }
    else if( USBSSHx->LINK_INT_FLAG & LINK_IF_RX_LMP )
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_RX_LMP;                                            // clear int flag
        if( USBSSHx->LINK_CFG & LINK_DOWN_MODE )                                            // HOST MODE
        {
            if( (USBSSHx->LINK_LMP_RX_DATA0 & LMP_SUBTYPE_MASK)==LMP_PORT_CAP )             // RX PORT_CAP
            {
                /* LMP, TX PORT_CFG & RX PORT_CFG_RES */ 
                USBSSHx->LINK_LMP_TX_DATA0 = LMP_LINK_SPEED | LMP_PORT_CFG | LMP_HP;
                USBSSHx->LINK_LMP_TX_DATA1 = 0x0;
                USBSSHx->LINK_LMP_TX_DATA2 = 0x0;
            }
            else if((USBSSHx->LINK_LMP_RX_DATA0 & LMP_SUBTYPE_MASK)==LMP_PORT_CFG_RES)      // RX PORT_CFG_RES
            {
                USBSSHx->LINK_LMP_PORT_CAP |= LINK_LMP_TX_CAP_VLD;                          // clear timer(20us timeout)               
            }
        }
        else                                                                                // UPSTREAM
        {
            if( (USBSSHx->LINK_LMP_RX_DATA0 & LMP_SUBTYPE_MASK) == LMP_PORT_CFG )           // device RX PORT_CFG, return PORT_CFG_RES
            {
                USBSSHx->LINK_LMP_TX_DATA0 = LMP_LINK_SPEED | LMP_PORT_CFG_RES | LMP_HP;
                USBSSHx->LINK_LMP_TX_DATA1 = 0x0;
                USBSSHx->LINK_LMP_TX_DATA2 = 0x0;
                USBSSHx->LINK_LMP_PORT_CAP |= LINK_LMP_TX_CAP_VLD;
            }
            else if( (USBSSHx->LINK_LMP_RX_DATA0 & LMP_SUBTYPE_MASK) == LMP_U2_INACT_TOUT )
            {
                USBSSHx->LINK_U2_INACT_TIMER = (USBSSHx->LINK_LMP_RX_DATA0>>9) & 0xff;
            }
        }
    }
    else if(USBSSHx->LINK_INT_FLAG & LINK_IF_WARM_RST)      
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_WARM_RST;
        if( USBSSHx->LINK_STATUS & LINK_RX_WARM_RST )
        {
            USBSS_Device_Init( ENABLE );
            USBSS_DevEnumStatus = 0; 
            printf("port%d Rx warm-reset begin! \n\n", port_num );
        }
    }
}

/*********************************************************************
 * @fn      SET_Devic_Address
 *
 * @brief   Set the USB device address in the USB controller register.
 *
 * @param   address - The device address to be set (7-bit USB address).
 *          USBSSx  - Pointer to the USB controller register structure.
 *
 * @return  none
 */
void SET_Devic_Address( uint32_t address, USBSSH_TypeDef *USBSSx)
{
    USBSSx->USB_CONTROL &= 0x00ffffff;
    USBSSx->USB_CONTROL |= ( address << 24 );  
}

/*********************************************************************
 * @fn      USBSS_PHY_Cfg
 *
 * @brief   Configure the USB3.0 PHY register via control interface.
 *
 * @param   port_num - USB port number (only port 0 is supported).
 *          addr     - PHY register address to write.
 *          data     - Data to write to the PHY register.
 *
 * @return  none
 */
uint32_t USBSS_PHY_Cfg( uint8_t port_num, uint8_t addr, uint16_t data )
{
    if( port_num == 0 )
    {
        USBSS_PHY_CFG_CR = ( 1 << 23 ) | ( addr << 16 ) | data;
        USBSS_PHY_CFG_DAT = 0x01;
        return( USBSS_PHY_CFG_DAT );
    }
    else 
    {
        return( 0 );
    }
}
/*********************************************************************
 * @fn      USBSS_ReadPHYData
 *
 * @brief   Obtain the USB3.0 FUN register configuration.
 *
 * @param   port_num: USB port number
 *              addr: FUN data
 *
 * @return  none
 */
uint32_t USBSS_ReadPHYData( uint8_t port_num, uint8_t addr )
{
    if( port_num == 0 )
    {
        USBSS_PHY_CFG_CR &= ~( 0x1f << 16 );
        USBSS_PHY_CFG_CR = ( 0x1f << 16 | 0x0 ) | ( 1 << 23 );
        USBSS_PHY_CFG_DAT = 0x1;
        USBSS_PHY_CFG_CR &= ~( 0x1f << 16 );
        USBSS_PHY_CFG_CR = ( addr << 16 );
        return USBSS_PHY_CFG_DAT;
    }
    return 0;
}

  /*********************************************************************
 * @fn      USBSS_CFG_MOD
 *
 * @brief   USB3.0 FUN configuration. 
 *
 * @param   none
 *
 * @return  none
 */
void USBSS_CFG_MOD( )
{
    /* PHY */
    USBSS_PHY_Cfg( 0, 0x03, 0x7c0e );            
    USBSS_PHY_Cfg( 0, 0x0D, 0x79AA );           

    ( *((__IO uint32_t *)0x5003C018 )) = 0xB0054000;        
    USBSS_PHY_Cfg( 0, 0x0F, 0x009c );
}

 /*********************************************************************
 * @fn      USBSS_PLL_Init
 *
 * @brief   initializes the USB3.0 PLL 
 *
 * @param   sta - ENABLE: Open the USB3.0 PLL 
 *                DISABLE: Turn off the USB3.0 PLL 
 *
 * @return  none
 */
void USBSS_PLL_Init( FunctionalState sta )
{
    if(sta)
    {
        RCC->CTLR |= (uint32_t)RCC_USBSS_PLLON;
        /* Wait till USBSS PLL is ready */
        while(( RCC->CTLR & (uint32_t)RCC_USBSS_PLLRDY) == (uint32_t)RCC_USBSS_PLLRDY );
    }
    else 
    {
        RCC->CTLR &= ~(uint32_t)RCC_USBSS_PLLON;
    }
}
