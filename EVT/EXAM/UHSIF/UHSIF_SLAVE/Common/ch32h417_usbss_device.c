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

USBSS_Dev_Info_t USBSS_Dev_Info;
volatile uint8_t  USB_Enum_Status = UNINIT;

/*********************************************************************
 * @fn      USBSS_RCC_Init
 *
 * @brief   Initializes USB high-speed rcc.
 *
 * @return  none
 */
void USBSS_RCC_Init(FunctionalState sta)
{
    if (sta)
    {
        USBSS_PLL_Init( ENABLE );
        RCC_HBPeriphClockCmd( RCC_HBPeriph_USBSS, ENABLE );
        RCC_PIPECmd( ENABLE );
        RCC_UTMIcmd( ENABLE );
        RCC_USBSS_PLLCmd(ENABLE);
    }
    else
    {
        RCC_HBPeriphClockCmd(RCC_HBPeriph_USBSS, DISABLE);
        RCC_UTMIcmd(DISABLE);
        RCC_PIPECmd( DISABLE );
        if((RCC->PLLCFGR & RCC_SYSPLL_SEL) != RCC_SYSPLL_USBSS)
        {
            RCC_USBSS_PLLCmd( DISABLE );
        }
    }
}
/*********************************************************************
 * @fn      USBSS_Device_Init
 *
 * @brief   Initializes USB susper-speed device.
 *
 * @return  none
 */
void USBSS_Device_Init( FunctionalState sta )                                                     
{
    if( sta )
    {
        USBSS_RCC_Init( ENABLE );
        USBSS_CFG_MOD( );
        USBSSD->LINK_CFG = LINK_RX_EQ_EN | LINK_TX_DEEMPH_MASK | LINK_PHY_RESET;   
        USBSSD->LINK_CTRL = LINK_P2_MODE | LINK_GO_DISABLED;                        
        USBSSD->LINK_CFG = LINK_RX_EQ_EN | LINK_TX_DEEMPH_MASK | LINK_LTSSM_MODE | LINK_TOUT_MODE;
#ifdef DEF_UP_U1_EN         /* disable U1 */
        USBSSD->LINK_CFG |= LINK_U1_PING_EN;
#endif
        USBSSD->LINK_LPM_CR |= LINK_LPM_EN;                            
        USBSSD->LINK_CFG |= LINK_RX_TERM_EN;                                     
        USBSSD->LINK_INT_CTRL =  LINK_IE_TX_LMP | LINK_IE_RX_LMP | LINK_IE_RX_LMP_TOUT | LINK_IE_STATE_CHG
                                    | LINK_IE_WARM_RST | LINK_IE_TERM_PRES;
        USBSSD->LINK_CTRL = LINK_P2_MODE;
        USBSSD->LINK_U1_WKUP_TMR = 120;
        USBSSD->LINK_U1_WKUP_FILTER = 50;
        USBSSD->LINK_U2_WKUP_FILTER = 0;
        USBSSD->LINK_U3_WKUP_FILTER = 0;
        USBSSD->USB_CONTROL |= USBSS_FORCE_RST;
        USBSSD->USB_STATUS = USBSS_UIF_TRANSFER;
        USBSSD->USB_CONTROL = USBSS_UIE_TRANSFER | USBSS_UDIE_SETUP | USBSS_UDIE_STATUS | USBSS_DMA_EN | USBSS_SETUP_FLOW;
        USBSS_Device_Endp_Init ( );
        NVIC_EnableIRQ( USBSS_IRQn );
        NVIC_EnableIRQ( USBSS_LINK_IRQn );
    }
    else 
    {
        NVIC_DisableIRQ( USBSS_LINK_IRQn );
        NVIC_DisableIRQ( USBSS_IRQn );
        USBSSD->USB_CONTROL = USBSS_FORCE_RST;
        USBSSD->LINK_CFG |= LINK_PHY_RESET | U3_LINK_RESET;
        Delay_Us( 100 );
        USBSSD->USB_CONTROL &= ~USBSS_FORCE_RST;
        USBSSD->LINK_CFG &= ~( LINK_PHY_RESET | U3_LINK_RESET );
        USBSS_RCC_Init( DISABLE );
    }
}

/*********************************************************************
 * @fn      USBSS_Device_Endp_Deinit
 *
 * @brief   Deinitializes USB device endpoints.
 *
 * @return  none
 */
void USBSS_Device_Endp_Deinit ( )
{
    USBSSD->USB_CONTROL |= USBSS_USB_CLR_ALL;
    USBSSD->USB_CONTROL &= ~USBSS_USB_CLR_ALL;
}

/*********************************************************************
 * @fn      USBSS_Device_Endp_Init
 *
 * @brief   DeInitializes USB device endpoints.
 *
 * @return  none
 */
void USBSS_Device_Endp_Init( )
{
    USBSS_Device_Endp_Deinit( );

    USBSSD->UEP_TX_EN = USBSS_EP1_TX_EN;
    USBSSD->UEP_RX_EN = USBSS_EP1_RX_EN;

    USBSSD->UEP0_TX_CTRL = 0;
    USBSSD->UEP0_RX_CTRL = 0;

    USBSSD->UEP0_TX_DMA = (uint32_t)&USBSS_EP0_Buf;
    USBSSD->UEP0_RX_DMA = (uint32_t)&USBSS_EP0_Buf;

    USBSSD->EP1_RX.UEP_RX_DMA = (uint32_t)&USBSS_EP1_Rx_Buf;
    USBSSD->EP1_RX.UEP_RX_CHAIN_MAX_NUMP = DEF_ENDP1_OUT_BURST_LEVEL;

    USBSSD->EP1_TX.UEP_TX_DMA = (uint32_t)&USBSS_EP1_Tx_Buf;
    USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = DEF_USB_EP1_SS_SIZE;
}
/*********************************************************************
 * @fn      USBSS_Reset_Init
 *
 * @brief   Initializes USB3.0 Warm Reset.
 *
 * @return  none
 */
void USBSS_Reset_Init( FunctionalState sta )
{
    if( sta )
    {
        USBSSD->USB_CONTROL |= USBSS_FORCE_RST;
        USBSSD->USB_STATUS = USBSS_UIF_TRANSFER;
        USBSSD->USB_CONTROL =  USBSS_UIE_TRANSFER | USBSS_UDIE_SETUP | USBSS_UDIE_STATUS | USBSS_DMA_EN | USBSS_SETUP_FLOW;
        USBSS_Device_Endp_Init( );
    }
    else 
    {
        USBSSD->USB_CONTROL = USBSS_FORCE_RST ;
        USBSSD->LINK_CFG |= LINK_PHY_RESET | U3_LINK_RESET;
        Delay_Us( 100 );
        USBSSD->USB_CONTROL &= ~USBSS_FORCE_RST;
        USBSSD->LINK_CFG &= ~( LINK_PHY_RESET | U3_LINK_RESET );
        USBSS_RCC_Init( DISABLE );
    }
}

/*********************************************************************
 * @fn      USB_Timer_Init
 *
 * @brief   Initializes TIM12 output compare.
 *
 * @param   none.
 *
 * @return  none
 */
void USB_Timer_Init( void )
{
    RCC_ClocksTypeDef tim_clocks;
    TIM9_12_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_TIM12, ENABLE );

    RCC_GetClocksFreq(&tim_clocks);

    TIM_TimeBaseInitStructure.TIM_Period = 5000 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = tim_clocks.HCLK_Frequency / 10000 - 1;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM9_12_TimeBaseInit( TIM12, &TIM_TimeBaseInitStructure);

    TIM_ClearITPendingBit( TIM12, TIM_IT_Update );

	NVIC_SetPriority(TIM12_IRQn,0);
}


/*********************************************************************
 * @fn      USB_Timer_Start
 *
 * @brief   USB timer enabled.
 *
 * @return  none
 */
void USB_Timer_Start( FunctionalState sta )
{
    if( sta )
    {
        TIM12->CNT = 0;
        TIM12->INTFR = ~TIM_IT_Update;
        NVIC_EnableIRQ( TIM12_IRQn );
        TIM12->DMAINTENR |= TIM_IT_Update;
        TIM12->CTLR1 |= TIM_CEN;
    }
    else
    {
        NVIC_DisableIRQ( TIM12_IRQn );
        TIM12->DMAINTENR &= ~TIM_IT_Update;
        TIM12->CTLR1 &= ~TIM_CEN;
    }
}

/*********************************************************************
 * @fn      TIM12_IRQHandler
 *
 * @brief   This function handles TIM12 exception.
 *
 * @return  none
 */
void TIM12_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM12_IRQHandler(void)
{
    static uint8_t u3_first_count = 0;
    if( TIM_GetITStatus( TIM12, TIM_IT_Update ) == SET )
    {   
        TIM_ClearITPendingBit( TIM12, TIM_IT_Update );

        if( USB_Enum_Status == U3_INI_FRIST )
        {
            u3_first_count++;
            if( u3_first_count > 2 )  
            {
                u3_first_count = 0;
                NVIC_DisableIRQ( USBSS_IRQn );
                NVIC_DisableIRQ( USBSS_LINK_IRQn );
                USBSS_Device_Init( DISABLE );
                USBHS_Device_Init( ENABLE ); 
                USB_Timer_Start( DISABLE );
            }
        }
        if( USB_Enum_Status == U2_INIT )
        {
            NVIC_DisableIRQ( USBSS_IRQn );
            NVIC_DisableIRQ( USBSS_LINK_IRQn );
            USBSS_Device_Init( DISABLE );
            USBHS_Device_Init( ENABLE ); 
            USB_Timer_Start( DISABLE );
        }
    }
}
/*********************************************************************
 * @fn      USBSS_LINK_Handle
 *
 * @brief   Handle USB 3.0 Link Layer state changes and related interrupts.
 *          This function processes different link states and link management
 *          packet (LMP) interrupts for USB 3.0 device or host controller.
 *
 * @param   USBSSHx   - Pointer to USB SuperSpeed host/device controller register structure.
 *       
 *
 * @return  none
 */
void USBSS_LINK_Handle( USBSSH_TypeDef *USBSSHx ) 
{
    static uint8_t link_low_power = 0;
    uint32_t link_state;
    uint32_t link_int;
    uint32_t link_lpm_r_data0 = 0;

    link_state = USBSSHx->LINK_STATUS & LINK_STATE_MASK;
    link_int = USBSSHx->LINK_INT_FLAG;

    if( link_int & LINK_IF_STATE_CHG )
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_STATE_CHG;

        if( link_state == LINK_STATE_RXDET )
        {
            USBSS_Dev_Info.u1_enable = DISABLE;
            USBSS_Dev_Info.u2_enable = DISABLE;

        }
        else if( link_state == LINK_STATE_RECOVERY )
        {
            if( link_low_power )
            {
                link_low_power = 0;
                Delay_Us( 100 );
                USBSS_PHY_Cfg( 0 ,0x12, 0x67c8 );      
            }
 
        }     
        else if( link_state == LINK_STATE_DISABLE )
        {
            USBSSHx->LINK_CTRL &= ~LINK_GO_DISABLED;

            if( USB_Enum_Status == UNINIT )
            {
                USB_Enum_Status = U3_INI_FRIST;
                USB_Timer_Start( ENABLE );
            }
            else if( USB_Enum_Status == U3_INIT_SECOND )
            {
                USB_Enum_Status = U2_INIT;
                USBHS_Device_Init( DISABLE ); 
                USB_Timer_Start( ENABLE );
            }
            else if( USB_Enum_Status == U2U3_SUCC )
            {
                USB_Enum_Status = UNINIT;
                USBHS_Device_Init( ENABLE ); 
            }
        }
        else if( link_state == LINK_STATE_U3 )
        {
            link_low_power = 0x01;
            USBSS_PHY_Cfg( 0 ,0x12, 0x67c8 & ( ~( 1 << 9 )));  
        }
        else if( link_state == LINK_STATE_U1 )
        {
            link_low_power = 0x01;
            USBSS_PHY_Cfg( 0 ,0x12, 0x67c8 & ( ~( 1 << 9 )));  
        }
        else if( link_state == LINK_STATE_U2 )
        {
            link_low_power = 0x01;
            USBSS_PHY_Cfg( 0 ,0x12, 0x67c8 & ( ~( 1 << 9 )));  
        }
        else if( link_state == LINK_STATE_HOTRST )
        { 
            USBSS_Dev_Info.u1_enable = DISABLE;
            USBSS_Dev_Info.u2_enable = DISABLE;
            USBSS_Reset_Init( ENABLE );
            USBSS_DevEnumStatus = 0;
            USBSSHx->LINK_CTRL &= ~LINK_HOT_RESET;                                          // HOT RESET end(device mode)

        }
        else if( link_state == LINK_STATE_INACTIVE )
        {
            USB_Enum_Status = U3_INI_FRIST;
            USBHS_Device_Init( ENABLE ); 
        }
        else if( link_state == LINK_STATE_POLLING )
        { 

        }
        else if( link_state == LINK_STATE_U0 )
        {
 
        }
    }
    else if( link_int & LINK_IF_TERM_PRES )
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_TERM_PRES;
        USBSS_DevEnumStatus = 0;
        if( USBSSHx->LINK_STATUS & LINK_RX_TERM_PRES )
        {

        }
    }
    else if( link_int & LINK_IF_RX_LMP_TOUT )                           // port config err
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_RX_LMP_TOUT;
        USBSSHx->LINK_CTRL |= LINK_GO_DISABLED;                         // upstream
        USBSSHx->LINK_CTRL |= LINK_GO_RX_DET;                           // upstream 
    }
    else if( link_int & LINK_IF_TX_LMP )
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
    else if( link_int & LINK_IF_RX_LMP )
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_RX_LMP;                        // clear int flag
        link_lpm_r_data0 = USBSSHx->LINK_LMP_RX_DATA0;

        if( USBSSHx->LINK_CFG & LINK_DOWN_MODE )                        // HOST MODE
        {
            if(( link_lpm_r_data0 & LMP_SUBTYPE_MASK ) == LMP_PORT_CAP )  // RX PORT_CAP
            {
                /* LMP, TX PORT_CFG & RX PORT_CFG_RES */ 
                USBSSHx->LINK_LMP_TX_DATA0 = LMP_LINK_SPEED | LMP_PORT_CFG | LMP_HP;
                USBSSHx->LINK_LMP_TX_DATA1 = 0x0;
                USBSSHx->LINK_LMP_TX_DATA2 = 0x0;
            }
            else if(( link_lpm_r_data0 & LMP_SUBTYPE_MASK ) == LMP_PORT_CFG_RES ) // RX PORT_CFG_RES
            {
                USBSSHx->LINK_LMP_PORT_CAP |= LINK_LMP_TX_CAP_VLD;                          // clear timer(20us timeout)               
            }
        }
        else                                                            // UPSTREAM
        {
            if(( link_lpm_r_data0 & LMP_SUBTYPE_MASK ) == LMP_PORT_CFG )           // device RX PORT_CFG, return PORT_CFG_RES
            {
                USBSSHx->LINK_LMP_TX_DATA0 = LMP_LINK_SPEED | LMP_PORT_CFG_RES | LMP_HP;
                USBSSHx->LINK_LMP_TX_DATA1 = 0x0;
                USBSSHx->LINK_LMP_TX_DATA2 = 0x0;
                USBSSHx->LINK_LMP_PORT_CAP |= LINK_LMP_TX_CAP_VLD;
                USB_Enum_Status = U2U3_SUCC;
                USB_Timer_Start( DISABLE );
                USBHS_Device_Init( DISABLE );    
            }
            else if(( link_lpm_r_data0 & LMP_SUBTYPE_MASK ) == LMP_U2_INACT_TOUT )
            {
                USBSSHx->LINK_U2_INACT_TIMER = ( link_lpm_r_data0 >> 9 ) & 0xff;     

            }
            else if( ( link_lpm_r_data0 & LMP_SUBTYPE_MASK ) == LMP_SET_LINK_FUNC )
            {
                if( USBSSHx->LINK_LMP_RX_DATA0 & ( 0x02 << 9 ))
                {
                    USBSSD->LINK_CFG |= LINK_U1_ALLOW;
                    USBSSD->LINK_CFG |= LINK_U2_ALLOW;
                    USBSS_Dev_Info.u1_enable = ENABLE;
                    USBSS_Dev_Info.u2_enable = ENABLE;
                }
                else
                {
                    USBSSD->LINK_CFG &= ~LINK_U1_ALLOW;
                    USBSSD->LINK_CFG &= ~LINK_U2_ALLOW;
                    USBSS_Dev_Info.u1_enable = DISABLE;
                    USBSS_Dev_Info.u2_enable = DISABLE;
                }
            }

        }
    }
    else if( link_int & LINK_IF_WARM_RST )      
    {
        USBSSHx->LINK_INT_FLAG = LINK_IF_WARM_RST;
        if( USBSSHx->LINK_STATUS & LINK_RX_WARM_RST )
        {
            USBSS_DevEnumStatus = 0; 
            USBSS_Reset_Init( ENABLE );                                                     // WARM RESET Init
            USBSSHx->LINK_CTRL |= LINK_GO_DISABLED;
            __NOP( );__NOP( );__NOP( );__NOP( );
            USBSSHx->LINK_CTRL &= ~LINK_GO_DISABLED;           
        }
    }
}

/*********************************************************************
 * @fn      SET_Device_Address
 *
 * @brief   Set the USB device address in the USB controller register.
 *
 * @param   address - The device address to be set (7-bit USB address).
 *          USBSSx  - Pointer to the USB controller register structure.
 *
 * @return  none
 */
void SET_Device_Address( uint32_t address, USBSSH_TypeDef *USBSSx)
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
void USBSS_CFG_MOD( void )
{
    /* PHY */
    USBSS_PHY_Cfg( 0, 0x03, 0x7c12 );            
    USBSS_PHY_Cfg( 0, 0x0D, 0x79AA );           
    USBSS_PHY_Cfg( 0, 0x15, 0x4430 );       
    USBSS_PHY_Cfg( 0, 0x13, 0x0010 );

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
        while(( RCC->CTLR & (uint32_t)RCC_USBSS_PLLRDY) != (uint32_t)RCC_USBSS_PLLRDY );
    }
    else 
    {
        RCC->CTLR &= ~(uint32_t)RCC_USBSS_PLLON;
    }
}
