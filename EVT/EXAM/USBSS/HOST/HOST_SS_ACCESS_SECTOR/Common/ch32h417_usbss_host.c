/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_usbss_host.c
* Author             : WCH
* Version            : V1.0
* Date               : 2025/05/23
* Description        : Initialization of the USBSS host PHY and USBSS link layer
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32h417_usbss_host.h"
#include "ch32h417_enum.h"

/* Endpoint Buffer */
__attribute__ ((aligned(4))) uint8_t USBSS_EP0_Tx_Buf[ DEF_USBSSD_UEP0_SIZE ];
__attribute__ ((aligned(4))) uint8_t USBSS_EP0_Rx_Buf[ DEF_USBSSD_UEP0_SIZE ];

void USBSS_LINK_IRQHandler (void) __attribute__((interrupt("WCH-Interrupt-fast"))); 

/*********************************************************************
 * @fn      USBSS_LINK_IRQHandler
 *
 * @brief   Handle USB 3.0 Link Layer state changes and related interrupts.
 *
 * @return  none
 */
void USBSS_LINK_IRQHandler (void)
{
    USBSS_LINK_Handle( USBSSH, 0 );
}

/*********************************************************************
 * @fn      USBSS_Endp_Deinit
 *
 * @brief   Initializes USB endpoints.
 *
 * @return  none
 */
void USBSS_Endp_Deinit( )
{
    USBSSH->USB_CONTROL |= USBSS_USB_CLR_ALL;
    USBSSH->USB_CONTROL &= ~USBSS_USB_CLR_ALL;
}

/*********************************************************************
 * @fn      USBSS_Endp_Disable
 *
 * @brief   Initializes USB endpoints.
 *
 * @return  none
 */
void USBSS_Endp_Disable( )
{
    USBSSH->LINK_CFG =0x00;
    USBSSH->LINK_CTRL = 0x00;
    USBSSH->LINK_INT_CTRL = 0;
    USBSSH->USB_CONTROL |= USBSS_USB_CLR_ALL;
    USBSSH->USB_CONTROL &= ~USBSS_USB_CLR_ALL;
}

/*********************************************************************
 * @fn      USBSSH_Init
 *
 * @brief   Initializes USB Susper-Speed Host.
 *
 * @return  none
 */
void USBSSH_Init( ) 
{
    USBSS_Endp_Deinit( );
    USBSSH->UH_TX_CTRL = 0;
    USBSSH->UH_RX_CTRL = 0;
    USBSSH->LINK_CFG = LINK_RX_EQ_EN | LINK_TX_DEEMPH_MASK  | LINK_DOWN_MODE | LINK_PHY_RESET;
    USBSSH->LINK_CTRL = POWER_MODE_2 | LINK_GO_DISABLED;                                                    // Change U3-PHY enter P2
    USBSSH->LINK_CFG = LINK_RX_EQ_EN | LINK_TX_DEEMPH_MASK  | LINK_DOWN_MODE | LINK_U2_RXDET | LINK_LTSSM_MODE;
    USBSSH->LINK_LPM_CR |= LINK_LPM_EN;

    {   /* bounded: the stock unbounded wait wedges the CPU if this init runs
         * from the LINK ISR (warm-reset path) while the PHY never settles. */
        uint32_t _bt = 0;
        while( (USBSSD->LINK_STATUS & LINK_BUSY) && ++_bt < 1000000 );          // Wait power mode switch done
    }

    USBSSH->LINK_CFG |= LINK_RX_TERM_EN;                                                                    // Term enable
    USBSSH->LINK_INT_CTRL =  LINK_IE_TX_LMP | LINK_IE_RX_LMP | LINK_IE_RX_LMP_TOUT | LINK_IE_STATE_CHG
                             | LINK_IE_WARM_RST; 
    USBSSH->LINK_CTRL = POWER_MODE_2;
    USBSSH->LINK_ITP_PRE = 122;                                                                             // Enable ITP collision(122Us)
    USBSSH->USB_CONTROL |= USBSS_FORCE_RST;

    USBSSH->USB_CONTROL = USBSS_HOST_MODE | USBSS_ITP_EN | USBSS_DMA_EN | USBSS_HP_PENDING;                 // Host_mode, release link reset
    USBSSH->UH_TX_CTRL = 0x0;   
    USBSSH->USB_STATUS = USBSS_UIF_TRANSFER;
    USBSSH->UH_TX_DMA = (uint32_t)USBSS_EP0_Tx_Buf;                                                         // Set tx dma address
    USBSSH->UH_RX_DMA = (uint32_t)USBSS_EP0_Rx_Buf;                                                         // Set rx dma address

    USBSSH->UEP_TX_EN = USBSS_UH_TX_EN;
    USBSSH->UEP_RX_EN = USBSS_UH_RX_EN;
    
    USBSS_CFG_MOD();
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

    /* Handle the training-critical Polling hand-offs FIRST, before the STATE_CHG
     * else-if chain: STATE_CHG fires on every RXDET<->POLLING transition and
     * would otherwise starve these. Two steps the stock handler was missing, so
     * a link whose partner keeps its Rx terminations up (e.g. an app-mode device
     * re-training after a warm reset, without a fresh cold attach) oscillated
     * RXDET<->POLLING and never reached U0:
     *   TERM_PRES : partner terminations present -> arm Polling (LINK_POLLING_EN)
     *   TXEQ      : Polling.RxEQ done -> wait Rx.Detect clears -> switch PHY to P0
     * Without the P2->P0 switch at TXEQ the LTSSM cannot advance to U0. */
    if( USBSSHx->LINK_INT_FLAG & LINK_IF_TXEQ )
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_TXEQ;
        if( USBSSHx->LINK_STATUS & LINK_RX_TERM_PRES )
        {
            uint32_t g = 0;
            while( (USBSSHx->LINK_STATUS & LINK_RX_DETECT) && ++g < 200000 );
            USBSSHx->LINK_CTRL = (USBSSHx->LINK_CTRL & ~LINK_PD_MODE_MASK) | LINK_P0_MODE;
            g = 0;
            while( (USBSSHx->LINK_STATUS & LINK_BUSY) && ++g < 200000 );
        }
        return;
    }
    if( (USBSSHx->LINK_INT_FLAG & LINK_IF_TERM_PRES) &&
        (USBSSHx->LINK_STATUS & LINK_RX_TERM_PRES) )
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_TERM_PRES;
        USBSSHx->LINK_CTRL |= LINK_POLLING_EN;
        return;
    }

    if( USBSSHx->LINK_INT_FLAG & LINK_IF_STATE_CHG )
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_STATE_CHG;

        if( link_state == LINK_STATE_RXDET )                                                // RX_DET
        {
            gDeviceConnectstatus = USB_INT_DISCONNECT;
            gDeviceUsbType = 0;
        }
        else if( link_state == LINK_STATE_POLLING )                                         // POLLING
        {

        }
        else if( link_state == LINK_STATE_HOTRST )
        { 
            USBSSHx->LINK_CTRL &= ~LINK_HOT_RESET;                                          // HOT RESET end(device mode)
        }   
        else if( link_state == LINK_STATE_DISABLE ){
            USBSSHx->LINK_CTRL &= ~LINK_GO_DISABLED;
        }
        else if( link_state == LINK_STATE_INACTIVE ){                                       // "DISABLE" requires a return to the RXDET state

        }
        else if( link_state == LINK_STATE_U3 ){                                             // U3

        }
        else if( link_state == LINK_STATE_RECOVERY ){                                       // RECOVERY

        }
    }
    else if( USBSSHx->LINK_INT_FLAG & LINK_IF_TERM_PRES )
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_TERM_PRES;
    }
    else if( USBSSHx->LINK_INT_FLAG & LINK_IF_RX_LMP_TOUT )                                 // Port config err
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_RX_LMP_TOUT;
        USBSSHx->LINK_CTRL |= LINK_GO_DISABLED;                                             // Upstream
        USBSSHx->LINK_CTRL |= LINK_GO_RX_DET;                                               // Upstream
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
        USBSSHx->LINK_INT_FLAG = LINK_IF_RX_LMP;                                            // Clear interrupt flag
        if( USBSSHx->LINK_CFG & LINK_DOWN_MODE )                                            // HOST MODE
        {
            if( (USBSSHx->LINK_LMP_RX_DATA0 & LMP_SUBTYPE_MASK)==LMP_PORT_CAP )             // RX PORT_CAP
            {
                USBSSHx->LINK_LMP_TX_DATA0 = LMP_LINK_SPEED | LMP_PORT_CFG | LMP_HP;
                USBSSHx->LINK_LMP_TX_DATA1 = 0x0;
                USBSSHx->LINK_LMP_TX_DATA2 = 0x0;
            }
            else if((USBSSHx->LINK_LMP_RX_DATA0 & LMP_SUBTYPE_MASK)==LMP_PORT_CFG_RES)      // RX PORT_CFG_RES
            {
                USBSSHx->LINK_LMP_PORT_CAP |= LINK_LMP_TX_CAP_VLD;                          // Clear timer(20us timeout)
                gDeviceConnectstatus = USB_INT_CONNECT;
			    gDeviceUsbType = USB_U30_SPEED;
            }
        }
        else
        {
            if( (USBSSHx->LINK_LMP_RX_DATA0 & LMP_SUBTYPE_MASK) == LMP_PORT_CFG )           // Device RX PORT_CFG, return PORT_CFG_RES
            {
                USBSSHx->LINK_LMP_TX_DATA0 = LMP_LINK_SPEED | LMP_PORT_CFG_RES | LMP_HP;
                USBSSHx->LINK_LMP_TX_DATA1 = 0x0;
                USBSSHx->LINK_LMP_TX_DATA2 = 0x0;
                USBSSHx->LINK_LMP_PORT_CAP |= LINK_LMP_TX_CAP_VLD;
            }
            else if( (USBSSHx->LINK_LMP_RX_DATA0 & LMP_SUBTYPE_MASK) == LMP_U2_INACT_TOUT ){
                USBSSHx->LINK_U2_INACT_TIMER = (USBSSHx->LINK_LMP_RX_DATA0>>9) & 0xff;
            }
        }
    }
    else if(USBSSHx->LINK_INT_FLAG & LINK_IF_WARM_RST)
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_WARM_RST;
        if( USBSSHx->LINK_STATUS & LINK_RX_WARM_RST )
        {
            /* Do NOT call USBSSH_Init() (busy-waits) or printf() from the
             * fast link ISR - flag the disconnect and let the main loop re-arm. */
            gDeviceConnectstatus = USB_INT_DISCONNECT;
            gDeviceUsbType = 0;
        }
    }
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
 * @param   port_num - USB port number
 *          addr     - FUN data
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
 * @return  none
 */
void USBSS_CFG_MOD( )
{
    /* PHY */
    USBSS_PHY_Cfg( 0, 0x03, 0x7c12 );            
    USBSS_PHY_Cfg( 0, 0x0D, 0x79AA );       
    USBSS_PHY_Cfg( 0, 0x13, 0x0010 );    
    USBSS_PHY_Cfg( 0, 0x15, 0x4430 );  
    USBSS_PHY_Cfg( 0, 0x11, 0x0501 );   

    /* Register Section */
    ( *((__IO uint32_t *)0x5003C018 )) = 0xB0054000;      
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
        while(( !(RCC->CTLR & (uint32_t)RCC_USBSS_PLLRDY)) );
    }
    else 
    {
        RCC->CTLR &= ~(uint32_t)RCC_USBSS_PLLON;
    }
}