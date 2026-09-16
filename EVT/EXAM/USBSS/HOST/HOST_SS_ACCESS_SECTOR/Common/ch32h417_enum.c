/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_enum.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2026/09/09
* Description        : This file provides the enumeration function of the host.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "ch32h417_usbss_host.h"
#include "ch32h417_enum.h"
#include "ch32h417_udisk.h"

/* Constant, variable, structure definition */
USB_SETUP_REQ *Ctl_Setup;	  													/* Control the transmission data packet */
uint8_t   gDeviceClassType;														/* Current device type */
volatile uint8_t   gDeviceConnectstatus;										/* USB connection status */
uint8_t   gDeviceUsbType = 0;												    /* USB connection status£º0x01--USB2.0&1.1  0x02--USB3.0 */
uint8_t   gUdisk_flag = 0;
#define	  DEVICE_ADDR				0x08										/* Device address of USB30 */

#define DEF_WAIT_USB_TOUT_200US     1000
#define DEF_CTRL_TRANS_TIMEOVER_CNT 60000                						/* Control transmission delay timing */

extern __attribute__((aligned(4))) uint8_t  RxBuffer[ 64 ];     				/* IN, must even address */
extern __attribute__((aligned(4))) uint8_t  TxBuffer[ 64 ];     				/* OUT, must even address */

uint8_t UsbDevEndp0Size = 8;

/*********************************************************************
 * @fn      U30HostCtrlTransfer
 *
 * @brief   USB3.0 host control transfer function
 *
 * @param   ReqBuf  - Request buffer
 *		    DatBuf  - Data buffer
 *		    RetLen  - Length
 *
 * @return  Status 
 */
uint8_t U30HostCtrlTransfer( PUINT8 ReqBuf, PUINT8 DatBuf, PUINT16 RetLen )  		// ReqBuf points to an 8-byte request code, and DatBuf is the send and receive buffer
{
	uint16_t total, temp;
	uint32_t timeout;

	memcpy( USBSS_EP0_Tx_Buf, ReqBuf, 8 );
	USBSSH->UH_TX_DMA = (uint32_t)USBSS_EP0_Tx_Buf;                     			// Set tx dma address
    USBSSH->UH_RX_DMA = (uint32_t)USBSS_EP0_Rx_Buf;                     			// Set rx dma address
    USBSSH->HOST_TX_NUMP = 1;
    USBSSH->UH_TX_CTRL = UH_TX_SETUP | UH_RTX_VALID | (1<<24) | 8; 					// Tx SETUP
	timeout = 0;

	/* Setup Stage */
	while(1)
	{					
		if( gDeviceConnectstatus == USB_INT_DISCONNECT ) return USB_INT_DISCONNECT;
		if( USBSSH->UH_TX_CTRL & UH_INT_FLAG )
		{
			if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_ACK )
			{
				USBSSH->UH_TX_CTRL = 0x0;
				if( (((USBSSH->USB_STATUS) >>16) & 0x1f) != 0 )
				{
					break;													// Normal. Exit directly
				}
				/* NumP == 0: the device has no EP0 credit yet - keep spinning until
				 * it sends ERDY (handled below). */
			}
		    else if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_FAILED )
            {
                USBSSH->UH_TX_CTRL = UH_TX_SETUP | UH_RTX_VALID | (1<<24) | 8; 		// Tx SETUP
            }
		}		
		if( USBSSH->USB_STATUS & USB_ERDY_FLAG )									// The device has granted EP0 credit
		{
			/* Do NOT clear USB_ERDY_FLAG here. Clearing the grant before the DATA
			 * stage is armed drops the credit again: the NumP=0 flow-control latch
			 * re-engages and the DATA DP is never launched. The DATA stage below
			 * arms first and clears the flag afterwards (re-arm, then clear). */
			break;
		}
		timeout++;
		if( timeout > 0xfffffff )	return USB_CH417USBTIMEOUT;
	}	

	/* USBSSH_Init() programs UEP_TX_EN = USBSS_UH_TX_EN (0x02), i.e. it enables the
	 * transmitter for ep1 only - ep0 is never enabled, so a control transfer with
	 * an OUT DATA stage can never launch its data packet. Enable ep0 here; the
	 * write is idempotent. */
	USBSSH->UEP_TX_EN |= 0x01;
	Delay_Ms(1);

	/* Data Stage */
	total = *( ReqBuf + 6 );
	if( total && DatBuf ) 
	{
		timeout = 0;
		if( *ReqBuf & 0x80 ) 														// Read data
		{  	
			USBSSH->USB_STATUS = USB_ACT_FLAG;										// Clear the SETUP-stage ACT before arming the IN
			USBSSH->HOST_RX_NUMP = 1;												// Setup  A packet of data
			USBSSH->UH_RX_CTRL = 0 << 16 | UH_RTX_VALID | 1<<24 | 0 <<12; 			// 0<<16 Synchronization flag£¬1<<24 Sudden number of brust£¬0 <<12 Endpoint number

			while(1)
			{
				if( ++timeout > 0x3fffff )											// Was unbounded: a device that never answers wedged the CPU here
				{
					USBSSH->UH_RX_CTRL = 0x0;
					return USB_CH417USBTIMEOUT;
				}
				if( gDeviceConnectstatus == USB_INT_DISCONNECT )	return USB_INT_DISCONNECT;

				if(USBSSH->UH_RX_CTRL & UH_INT_FLAG)
				{
					USBSSH->UH_RX_CTRL &= ~UH_INT_FLAG;
					if( (USBSSH->USB_STATUS & USB_RX_RES_MASK) == RX_RES_ACK )
					{
						USBSSH->UH_RX_CTRL = 0x0;										// Suspend data reception
						USBSSH->USB_STATUS = USB_ACT_FLAG;								// Clear the USB interrupt register
						temp = USBSSH->UH_RX_CTRL & 0x7ff;								// The length of the read data is 0 to 10 bytes
						*RetLen = USBSSH->UH_RX_CTRL & 0x7ff;				
						memcpy( DatBuf, USBSS_EP0_Rx_Buf, temp );						// Read the returned data
						break;															// Normal data, exit directly

					}
					else if( (USBSSH->USB_STATUS & USB_RX_RES_MASK) == RX_RES_NRDY )	// The device has not returned to the ERDY state and is waiting for the ERADY state
					{
						USBSSH->UH_RX_CTRL = 0x0;
					}
					else if( (USBSSH->USB_STATUS & USB_RX_RES_MASK) == RX_RES_STALL ){	//STALL
						USBSSH->UH_RX_CTRL = 0x00;		// Clear the interrupt status
						return USB_INT_DISK_ERR;
					}
					else								// Other Errors
					{
						USBSSH->UH_RX_CTRL = 0x00;		// Clear the interrupt status					
					}
				}	
				else if( (USBSSH->USB_STATUS & USB_ERDY_FLAG) )							//The USB device is in ERDY status
				{
					USBSSH->UH_RX_CTRL = 0 << 16 | UH_RTX_VALID | 1<<24 | 0 <<12; 		// ACK-TP(IN)
					USBSSH->USB_STATUS = USB_ERDY_FLAG;
				}
				if( gDeviceConnectstatus == USB_INT_DISCONNECT )	return USB_INT_DISCONNECT;
			}
		}		
		else												// Send data
		{					
			if( total )
			{
				timeout = 0;
				memcpy( USBSS_EP0_Tx_Buf, DatBuf, total );

				/* RM 27.2.1.2: the SETUP stage is acknowledged with an ACK-TP carrying
				 * NumP = 0, which latches RB_HOST_ACK_NUMP (USB_STATUS[20:16]) to zero
				 * and hard-gates the EP0 OUT DATA packet. That field is writable -
				 * inject one packet of credit before arming, otherwise the DATA DP is
				 * never launched and the transfer times out (or the device STALLs). */
				USBSSH->USB_STATUS = 0x00010000;

				USBSSH->UH_TX_DMA = (uint32_t)USBSS_EP0_Tx_Buf;
				USBSSH->HOST_TX_NUMP = 1;
				USBSSH->UH_TX_CTRL = 0 <<16 | UH_RTX_VALID | 1<<24 | 0 <<12 | total;		// Seq_num 0, Burst_nump 1, Endp 0
				/* Only now, with the DP armed, acknowledge the SETUP-stage grant. */
				USBSSH->USB_STATUS = USB_ERDY_FLAG;
				while(1)
				{
					uint32_t txc, res;
					if( gDeviceConnectstatus == USB_INT_DISCONNECT )	return USB_INT_DISCONNECT;
					txc = USBSSH->UH_TX_CTRL;
					if( txc & UH_INT_FLAG )
					{
						res = USBSSH->USB_STATUS & USB_TX_RES_MASK;
						/* Clear the completion flag only. Writing UH_TX_CTRL = 0 after a
						 * launched transfer leaves the transmit engine in a state where
						 * the next USBSS register access stalls the AHB bus. */
						USBSSH->UH_TX_CTRL = txc & ~UH_INT_FLAG;
						if( res == TX_RES_ACK )
						{
							break;
						}
						else if( res == TX_RES_STALL )
						{
							return USB_INT_DISK_ERR;
						}
						else if( res == TX_RES_FAILED )
						{
							USBSSH->UH_TX_CTRL |= UH_RTX_VALID;						// Re-send the same DP
						}
						/* TX_RES_NRDY: the device is momentarily busy and has dropped the
						 * credit; wait for its ERDY below and re-arm there. Note that the
						 * stock code tested this transmit result against USB_RX_RES_MASK. */
					}
					else if( USBSSH->USB_STATUS & USB_ERDY_FLAG )
					{
						/* Re-arm first, clear the grant second - the reverse order drops
						 * the credit the device has just given back. */
						USBSSH->UH_TX_DMA = (uint32_t)USBSS_EP0_Tx_Buf;
						USBSSH->HOST_TX_NUMP = 1;
						USBSSH->UH_TX_CTRL = 0 <<16 | UH_RTX_VALID | 1<<24 | 0 <<12 | total;
						USBSSH->USB_STATUS = USB_ERDY_FLAG;
					}
					if( gDeviceConnectstatus == USB_INT_DISCONNECT )	return USB_INT_DISCONNECT;
					timeout++;
					if( timeout > 0xfffff )	return USB_CH417USBTIMEOUT;
				}
			}	
		}
	}
	Delay_Ms(1);																											// This delay function is required£¬otherwise the STATUS package cannot be transmitted 
	timeout = 0;
    USBSSH->HOST_TX_NUMP = 1;
    USBSSH->UH_TX_DMA = (uint32_t)USBSS_EP0_Tx_Buf;                                                  // The DATA stage may have re-pointed it
    USBSSH->UH_TX_CTRL = UH_TX_STATUS | UH_RTX_VALID;																		// Tx status-TP
	while( 1 ) 																												// Wait STATUS-TP complete
	{
		if( gDeviceConnectstatus == USB_INT_DISCONNECT )	return USB_INT_DISCONNECT;
		if( USBSSH->UH_TX_CTRL & UH_INT_FLAG  ) 
		{
			if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_ACK )
			{
				USBSSH->UH_TX_CTRL = 0x0;
				USBSSH->USB_STATUS = USB_ACT_FLAG;
				break;
			}
			else if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_NRDY )
			{
				USBSSH->UH_TX_CTRL = 0x0;
                {   uint32_t _bt = 0;                                                       // Was unbounded: a device that
                    while( !(USBSSH->USB_STATUS & USB_ERDY_FLAG) )                          // never sends ERDY wedged
                    {                                                                       // the CPU here
                        if( ++_bt > 0x3fffff )	return USB_CH417USBTIMEOUT;
                    }
                }
                USBSSH->USB_STATUS = USB_ERDY_FLAG;
                USBSSH->UH_TX_CTRL = UH_TX_STATUS | UH_RTX_VALID;															// Tx status-TP
			}
			else if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_STALL )
			{
				USBSSH->UH_TX_CTRL = 0x0;
				USBSSH->USB_STATUS = USB_ACT_FLAG;
				return USB_INT_DISK_ERR;
			}
			else
			{
				USBSSH->USB_STATUS = USB_ACT_FLAG;
			}
		}
		if( gDeviceConnectstatus == USB_INT_DISCONNECT )	return USB_INT_DISCONNECT;
		timeout++;
		if( timeout > 0xfffff )	return USB_CH417USBTIMEOUT;
	}
	return USB_INT_SUCCESS;
}

/*********************************************************************
 * @fn      U30OST_GetDeviceDescr
 *
 * @brief   The USB host obtains the device descriptor
 *
 * @param   *buf  - Data buffer
 *		   *len  - Data length
 *
 * @return  Status 
 */
uint8_t U30OST_GetDeviceDescr( uint8_t *buf ,uint16_t *len )
{
	uint16_t l;
	uint8_t  status;
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0x80;
	Setup_buf[1] = 0x06;
	Setup_buf[2]  = 0x00;
	Setup_buf[3]  = 0x01;
	Setup_buf[4]  = 0x00;
	Setup_buf[5]  = 0x00;
	Setup_buf[6] = *len;
	Setup_buf[7] = 0x00;
	l = *len;

	status = U30HostCtrlTransfer( Setup_buf, buf, &l );
	if( status == USB_INT_SUCCESS )
	{
		*len = l;
	}
	return status;
}

/*********************************************************************
 * @fn      U30HOST_GetConfigDescr
 *
 * @brief   The USB host obtains the configuration descriptor
 *
 * @param   *buf  - Data buffer
 *		   *len  - Data length
 *
 * @return  Status 
 */
uint8_t U30HOST_GetConfigDescr( uint8_t *buf ,uint16_t *len )
{
	uint16_t l;
	uint8_t  status;
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0x80;
	Setup_buf[1] = 0x06;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x02;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x40;
	Setup_buf[7] = 0x00;
	l = 0x40;
	status = U30HostCtrlTransfer( Setup_buf, buf, &l );

	if( status == USB_INT_SUCCESS ){
		Setup_buf[6] = (( (PUSB_CFG_DESCR)buf ) -> wTotalLength)&0xff;
		Setup_buf[7] = (( (PUSB_CFG_DESCR)buf ) -> wTotalLength)>>8;
		l = Ctl_Setup->wLength;
		status = U30HostCtrlTransfer( Setup_buf, buf, &l );
		if( status == USB_INT_SUCCESS )
		{
			*len = l;
		}
		else
		{
			*len = 0x00;
		}
	}
	return status;
}

/*********************************************************************
 * @fn      U30HOST_GetBosDescr
 *
 * @brief   The USB host obtains the BOS descriptor
 *
 * @param   *buf  - Data buffer
 *		   *len  - Data length
 *
 * @return  Status 
 */
uint8_t U30HOST_GetBosDescr( uint8_t *buf ,uint16_t *len )
{
	uint16_t l;
	uint8_t  status;
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0x80;
	Setup_buf[1] = 0x06;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x0F;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0xFF;
	Setup_buf[7] = 0x00;
	l = 0xFF;

	status = U30HostCtrlTransfer( Setup_buf, buf, &l );
	if( status == USB_INT_SUCCESS )
	{
		*len = l;
	}
	return status;
}

/*********************************************************************
 * @fn      U30HOST_SetInterface
 *
 * @brief   The USB host Set Interface
 *
 * @param   inf  - Interface
 *
 * @return  Status 
 */
uint8_t U30HOST_SetInterface( uint8_t inf )
{
	uint8_t  status;
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0x01;
	Setup_buf[1] = 0x0b;
	Setup_buf[2] = inf;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	status = U30HostCtrlTransfer( Setup_buf, NULL, NULL );
	if( status == USB_INT_SUCCESS ){
	}
	return status;
}

/*********************************************************************
 * @fn      U30HOST_SetIsochDelay
 *
 * @brief   Set the synchronization delay for the USB host
 *
 * @param   none
 *
 * @return  Status 
 */
uint8_t U30HOST_SetIsochDelay( void )
{
	uint8_t status;
	uint8_t Setup_buf[8];

	Setup_buf[0] = 0x00;
	Setup_buf[1] = 0x31;
	Setup_buf[2] = 0xff;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	status = U30HostCtrlTransfer( Setup_buf, NULL, NULL );
	if( status == USB_INT_SUCCESS )
	{
		USBSSH->LINK_ISO_DLY = 0xff;
	}
	return status;
}

/*********************************************************************
 * @fn      U30HOST_SetSel
 *
 * @brief   The USB3.0 Host Set Sel 
 *
 * @param   buf  - Data buffer
 *          len  - Data length
 *
 * @return  Current  Status 
 */
uint8_t U30HOST_SetSel( uint8_t *buf ,uint16_t *len )
{
	uint8_t status;
	uint8_t Setup_buf[8];

	Setup_buf[0] = 0x00;
	Setup_buf[1] = 0x30;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x06;
	Setup_buf[7] = 0x00;

	status = U30HostCtrlTransfer( Setup_buf, buf, len );
	if( status == USB_INT_SUCCESS ){
	}
	return status;
}

/*********************************************************************
 * @fn      U30HOST_SetFeatureU1
 *
 * @brief   The USB3.0 Host Set FeatureU1
 *
 * @param   none
 *
 * @return  Current  Status 
 */
uint8_t U30HOST_SetFeatureU1( void )
{
	uint8_t  status;
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0x00;
	Setup_buf[1] = 0x03;
	Setup_buf[2]  = 0x30;
	Setup_buf[3]  = 0x00;
	Setup_buf[4]  = 0x00;
	Setup_buf[5]  = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	status = U30HostCtrlTransfer( Setup_buf, NULL, NULL );
	if( status == USB_INT_SUCCESS ){
	}
	return status;
}

/*********************************************************************
 * @fn      U30HOST_SetFeatureU2
 *
 * @brief   The USB3.0 Host Set FeatureU2
 *
 * @param   none
 *
 * @return  Status 
 */
uint8_t U30HOST_SetFeatureU2( void )
{
	uint8_t  status;
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0x00;
	Setup_buf[1] = 0x03;
	Setup_buf[2] = 0x31;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	status = U30HostCtrlTransfer( Setup_buf,NULL,NULL );
	if( status == USB_INT_SUCCESS ){
	}
	return status;
}

/*********************************************************************
 * @fn      U30HOST_SetAddress
 *
 * @brief   The USB3.0 Host Set FeatureU2
 *
 * @param   addr  - Address value
 *
 * @return  Status 
 */
uint8_t U30HOST_SetAddress( uint8_t addr )
{
	uint8_t status;
	uint8_t Setup_buf[8];

	Setup_buf[0] = 0X00;
	Setup_buf[1] = 0x05;
	Setup_buf[2] = addr;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	/* Set the address of the USB device */
	status = U30HostCtrlTransfer( Setup_buf, NULL, NULL );
	if( status == USB_INT_SUCCESS )
	{
		USBSSH->USB_CONTROL &= 0x00ffffff;
		USBSSH->USB_CONTROL |= addr<<24; 										// SET ADDRESS
	}
	mDelaymS( 5 );  											  				// Wait for the USB device to complete the operation
	return status;
}

/*********************************************************************
 * @fn      U30HOST_SetConfig
 *
 * @brief   USB host Settings configuration values
 *
 * @param   cfg  - Configuration value
 *
 * @return  Status 
 */
uint8_t U30HOST_SetConfig( uint8_t cfg )
{
	uint8_t status;
	uint8_t Setup_buf[8];

	Setup_buf[0] = 0x00;
	Setup_buf[1] = 0x09;
	Setup_buf[2] = cfg;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	/* USB host Settings configuration values */
	status = U30HostCtrlTransfer( Setup_buf, NULL, NULL );
	return status;
}

/*********************************************************************
 * @fn      U30HOST_ClearEndpStall
 *
 * @brief   USB host clear endpoint
 *
 * @param   endp  - Endpoint number
 *
 * @return  Status 
 */
uint8_t U30HOST_ClearEndpStall( uint8_t endp )
{
	uint8_t  status;
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0x02;
	Setup_buf[1] = 0x01;
	Setup_buf[2] = 0X00;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = endp;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	status = U30HostCtrlTransfer( Setup_buf, NULL, NULL  );	
	return status;
}

/*********************************************************************
 * @fn      U30HOST_CofDescrAnalyse
 *
 * @brief   USB host analysis configuration descriptor
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status 
 */
uint8_t U30HOST_CofDescrAnalyse( uint8_t *pbuf )
{
	gDeviceClassType = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> itf_descr.bInterfaceClass;
	if( ( gDeviceClassType <= 0x09 ) || ( gDeviceClassType == 0xFF ) )
	{
		return USB_INT_SUCCESS;
	}
	else
	{
		gDeviceClassType = USB_DEV_CLASS_RESERVED;								// The current device type is unknown by default
		return ERR_USB_UNKNOWN;  									 	    // Not supported£¬ Return
	}
}

/*********************************************************************
 * @fn      U30HSOT_Enumerate
 *
 * @brief   USB30 host enumeration operation
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status
 */
uint8_t U30HSOT_Enumerate( uint8_t *pbuf )
{
	uint8_t status;
	uint16_t len,i;
	uint8_t cfg;
//	mDelaymS( 1000 );       													// There is a necessary delay. When encountering some USB drives, the device descriptor cannot be obtained without delay

/* Sets Address */
	printf("start_enmu\n");
    USBSSH->USB_CONTROL &= 0x00ffffff;
    USBSSH->USB_CONTROL |= 0<<24; 												// SET ADDRESS                                         

    mDelaymS(1);
	status = U30HOST_SetAddress( DEVICE_ADDR );									// U30 first sets the address and then obtains the device descriptor
	printf("set_addr=%02x\n",status);
	if( status != USB_INT_SUCCESS ){
		printf("SetAddr_ERROR=%02x\n",status);
		return status;
	}
	mDelaymS(10);

/* Get Device Descriptor */
	printf("get_dev:\n");
	len = 0x12;
	status = U30OST_GetDeviceDescr( pbuf, &len );								// Get The Device Descriptor
	if( status != USB_INT_SUCCESS ){
		printf("GetDev_ERROR=%02x\n",status);
		return USB_OPERATE_ERROR;
	}
	printf("DeviceDescr:");
	for( i=0;i!=len;i++ ){
		printf("%02x ",*(pbuf+i));
	}
	printf("\n");
	mDelaymS( 100 );			

/* Get Configuration Descriptor */
	len = 0;
	status = U30HOST_GetConfigDescr( pbuf,&len );								// Get The Configuration Descriptor
	if( status != USB_INT_SUCCESS ){
		printf("GetConfig_ERROR=%02x\n",status);
		return USB_OPERATE_ERROR;
	}
	cfg = ( (PUSB_CFG_DESCR)pbuf ) -> bConfigurationValue;
	printf("cfg=%02x\n",cfg);
	printf("ConfigDescr:");
	for( i=0;i!=len;i++ ){
		printf("%02x ",*(pbuf+i));
	}
	printf("\n");

/* Analyze Configuration Descriptor */
	status = U30HOST_CofDescrAnalyse( pbuf );									// Analyze The Configuration Descriptor
	if( status != USB_INT_SUCCESS )
	{
		printf( "Current Device is Not USB Mass Storage Device\n" );
		return( USB_OPERATE_ERROR ); 		 									// Return directly
	}
	status = U30HOST_MS_CofDescrAnalyse( pbuf );
	printf("status-1=%02x\n",status);
	status = U30HOST_GetBosDescr( pbuf,&len );
	if( status != USB_INT_SUCCESS )
	{
		printf( "GetBos_ERROR=%02x\n",status );
		return( USB_OPERATE_ERROR ); 		 									// Return directly
	}
	status = U30HOST_SetIsochDelay();											// Maybe Removed because some USB drives do not support it, nor does ADATA Drive
	printf("SetIsochDelay=%02x\n",status);
	if( status != USB_INT_SUCCESS )
	{
		if( status != 0x1f ){
			printf( "SetIsochDelay_ERROR=%02x\n",status );
			return( USB_OPERATE_ERROR ); 		 								// Return directly
		}
	}

/* Settings Configuration */
	status = U30HOST_SetConfig( cfg ); 											// Set The USB Device Configuration
	if ( status != USB_INT_SUCCESS )
	{
		printf( "SetConfig_ERROR = %02X\n", (UINT16)status );
		return( USB_OPERATE_ERROR ); 		 									// Return directly 
	}

#if 0
	status = U30HOST_SetFeatureU1();
	if ( status != USB_INT_SUCCESS )
	{
		printf( "SetFeatureU1_ERROR = %02X\n", (UINT16)status );
		return( USB_OPERATE_ERROR ); 		 									// Return directly
	}

	status = U30HOST_SetFeatureU2();
	if ( status != USB_INT_SUCCESS )
	{
		printf( "SetFeatureU2_ERROR = %02X\n", (UINT16)status );
		return( USB_OPERATE_ERROR ); 		 									// Return directly
	}
#endif

	return USB_OPERATE_SUCCESS;
}

/*********************************************************************
 * @fn      U30HSOT_Enumerate_Hotrst
 *
 * @brief   USB30 host Enumeration Operation.
 *
 * @param   pbuf  - Data buffer.
 *
 * @return  Status.
 */
uint8_t U30HSOT_Enumerate_Hotrst( uint8_t *pbuf )
{
    uint8_t status;
    uint16_t len;
    uint8_t cfg;

    USBSSH->USB_CONTROL &= 0x00ffffff;
    USBSSH->USB_CONTROL |= 0<<24; 						// Set address                                          

    mDelaymS(1);
    status = U30HOST_SetAddress( DEVICE_ADDR );         // Set the address first and then obtain the device descriptor
    if( status != USB_INT_SUCCESS ){
        printf("SetAddr_ERROR=%02x\n",status);
    	return status;
    }
    len = 0x12;
    status = U30OST_GetDeviceDescr( pbuf,&len );		// Obtain the device descriptor
    if( status != USB_INT_SUCCESS ){
        return USB_OPERATE_ERROR;
    }
    mDelaymS( 20 );
    len = 0;
    status = U30HOST_GetConfigDescr( pbuf,&len );		// Obtain the configuration descriptor
    if( status != USB_INT_SUCCESS ){

        return USB_OPERATE_ERROR;
    }
    cfg = ( (PUSB_CFG_DESCR)pbuf ) -> bConfigurationValue;

    status = U30HOST_SetConfig( cfg ); 					// Set the USB device configuration
    if ( status != USB_INT_SUCCESS )
    {
        printf( "SetConfig_ERROR = %02X\n", (UINT16)status );
        return( USB_OPERATE_ERROR );                                          
    }
    return status;
}

/************************The Following Are The Operation Functions For USB2.0 Host*****************************/
/*********************************************************************
 * @fn      USBHSH_Transact
 *
 * @brief   Perform USB transaction.
 *
 * @param   endp_pid  - Token PID.
 *          endp_tog  - Toggle
 *          timeout  - Timeout time.
 *
 * @return  USB transfer result.
 */
uint8_t USBHSH_Transact( uint8_t endp_pid_number, uint16_t endp_tog, uint32_t timeout )
{
    uint8_t   r, trans_retry;
    uint16_t  i;
    uint8_t endp_number = endp_pid_number&0xf0;
    uint8_t endp_pid = endp_pid_number&0x0f;
    if( endp_pid == USB_PID_IN )
    {
        if( (endp_tog & 0x80) || (endp_tog & USBHS_UH_T_TOG_DATA1) )
        {
            endp_tog = USBHS_UH_T_TOG_DATA1;
        }
        else
        {
            endp_tog = 0;
        }
    }
    else if( endp_pid == USB_PID_OUT )
    {
        if( (endp_tog & 0x40) || (endp_tog & USBHS_UH_T_TOG_DATA1) )
        {
            endp_tog = USBHS_UH_T_TOG_DATA1;
        }
        else
        {
            endp_tog = 0;
        }
    }
    else
    {
        endp_tog = 0;
    }

    trans_retry = 0;
    do
    {
        USBHSH->CONTROL = USBHS_UH_HOST_ACTION | endp_pid | endp_tog | endp_number; 						// IN(0x69)
		
        USBHSH->INT_FLAG = USBHS_UHIF_TRANSFER;
        for( i = DEF_WAIT_USB_TOUT_200US; ( i != 0 ) && ( ( USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER ) == 0 ); i-- )
        {
            Delay_Us( 1 );
        }

        USBHSH->CONTROL = ( USBHSH->CONTROL & ~ USBHS_UH_T_TOKEN_MASK );

        if( ( USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER ) == 0 )
        {
            return ERR_USB_UNKNOWN;
        }

        if( USBHSH->PORT_STATUS_CHG & USBHS_UHIF_PORT_CONNECT )
        {
            USBHSH->PORT_STATUS_CHG = USBHS_UHIF_PORT_CONNECT;
            Delay_Us( 200 );
            if( USBHSH->PORT_STATUS & USBHS_UHIS_PORT_CONNECT )
            {
                if( USBHSH->CFG & USBHS_UH_SOF_EN )
                {
                    return ERR_USB_CONNECT;
                }
            }
            else
            {
                return ERR_USB_DISCON;
            }
        }
        else if( USBHSH->INT_FLAG & USBHS_UHIF_TRANSFER )                   // The packet transmission was successful
        {
            r = USBHSH->INT_ST & USBHS_UH_T_TOKEN_MASK;
            if( endp_pid  == USB_PID_IN )
            {
                if( (r == USB_PID_DATA0 && endp_tog == USBHS_UH_T_TOG_DATA0) || (r == USB_PID_DATA1 && endp_tog == USBHS_UH_T_TOG_DATA1) )
                {
                    return ERR_SUCCESS; 									// Packet token match
                }
            }
            else
            {
                if( ( r == USB_PID_ACK ) || ( r == USB_PID_NYET ) )
                {
                    return ERR_SUCCESS;
                }
            }
            if( r == USB_PID_STALL )
            {
                return ( r | ERR_USB_TRANSFER );
            }

            if( r == USB_PID_NAK )
            {
                if( timeout == 0 )
                {
                    return ( r | ERR_USB_TRANSFER );
                }
                if( timeout < 0xFFFF )
                {
                    timeout--;
                }
                --trans_retry;
            }
            else switch( endp_pid )
            {
                case USB_PID_SETUP:

                case USB_PID_OUT:
                    if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                case USB_PID_IN:
                    if( ( r == USB_PID_DATA0 ) || ( r == USB_PID_DATA1 ) )
                    {
                        ;
                    }
                    else if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                default:
                    return ERR_USB_UNKNOWN;
            }
        }
        else
        {
            USBHSH->INT_FLAG = USBHS_UHIF_WKUP_ACT | USBHS_UHIF_RESUME_ACT | USBHS_UHIF_TRANSFER | USBHS_UHIF_SOF_ACT | USBHS_UHIF_TX_HALT | USBHS_UHIF_FIFO_OVER;
        }
        Delay_Us( 15 );
    } while( ++trans_retry < 10 );

    return ERR_USB_TRANSFER;
}

/*********************************************************************
 * @fn      USBHSH_CtrlTransfer
 *
 * @brief   USB host control transfer.
 *
 * @param   ReqBuf  - Device endpoint 0 size
 *          pbuf  - Data buffer
 *          plen  - Data length
 *
 * @return  USB control transfer result.
 */
uint8_t U20HostCtrlTransfer(uint8_t *ReqBuf, uint8_t *pbuf, uint8_t *plen )
{
    uint8_t  s;
    uint16_t rem_len, rx_len, rx_cnt, tx_cnt;
	USBHSH->TX_DMA = (uint32_t)TxBuffer;
	USBHSH->RX_DMA = (uint32_t)RxBuffer;
	memcpy( TxBuffer, ReqBuf, 8 );
    Delay_Us( 100 );
    if( plen )
    {
        *plen = 0;
    }
    USBHSH->TX_LEN = sizeof( USB_SETUP_REQ );
    s = USBHSH_Transact( ( USB_PID_SETUP  ) | 0x00, 0x00, DEF_CTRL_TRANS_TIMEOVER_CNT ); // SETUP stage
    if( s != ERR_SUCCESS )
    {
        return s;
    }

    USBHSH->CONTROL = (USBHSH->CONTROL &~ USBHS_UH_T_TOG_MASK ) | USBHS_UH_T_TOG_DATA1;

	rem_len = *( ReqBuf + 6 );
    if( rem_len && pbuf ) 
    {
        if( *ReqBuf & USB_REQ_TYP_IN ) 								//device to host
        {
            /* Receive data */
            while( rem_len )
            {
                Delay_Us( 100 );
                s = USBHSH_Transact( USB_PID_IN , USBHSH->CONTROL, DEF_CTRL_TRANS_TIMEOVER_CNT );
                if( s != ERR_SUCCESS )
                {
                    return s;
                }
                USBHSH->CONTROL ^= USBHS_UH_T_TOG_DATA1;

                rx_len = ( USBHSH->RX_LEN < rem_len )? USBHSH->RX_LEN : rem_len;
                rem_len -= rx_len;
                if( plen )
                {
                    *plen += rx_len;
                }
                for( rx_cnt = 0; rx_cnt != rx_len; rx_cnt++ )
                {
                    *pbuf = RxBuffer[ rx_cnt ];
                    pbuf++;
                }
                if( ( USBHSH->RX_LEN == 0 ) || ( USBHSH->RX_LEN & ( UsbDevEndp0Size - 1 ) ) )
                {
                    break;
                }
            }
            USBHSH->TX_LEN = 0;
        }
        else
        {                                                           // host to device
            /* Send data */
            while( rem_len )
            {
                Delay_Us( 100 );
                USBHSH->TX_LEN = ( rem_len >= UsbDevEndp0Size )? UsbDevEndp0Size : rem_len;
                for( tx_cnt = 0; tx_cnt != USBHSH->TX_LEN; tx_cnt++ )
                {
                    TxBuffer[ tx_cnt ] = *pbuf;
                    pbuf++;
                }
                s = USBHSH_Transact( USB_PID_OUT | 0x00, USBHSH->CONTROL, DEF_CTRL_TRANS_TIMEOVER_CNT );
                if( s != ERR_SUCCESS )
                {
                    return s;
                }
                USBHSH->CONTROL ^= USBHS_UH_T_TOG_DATA1;

                rem_len -= USBHSH->TX_LEN;
                if( plen )
                {
                    *plen += USBHSH->TX_LEN;
                }
            }
        }
    }

    Delay_Us( 100 );
    s = USBHSH_Transact( ( USBHSH->TX_LEN )? ( USB_PID_IN | 0x00 ) : ( USB_PID_OUT | 0x00 ), USBHS_UH_T_TOG_DATA1, DEF_CTRL_TRANS_TIMEOVER_CNT );
    if( s != ERR_SUCCESS )
    {
        return s;
    }

    if( USBHSH->TX_LEN == 0 )
    {
        return ERR_SUCCESS;    //status stage is out, send a zero-length packet.
    }

    if( USBHSH->RX_LEN == 0 )
    {
        return ERR_SUCCESS;    //status stage is in, a zero-length packet is returned indicating success.
    }

    return ERR_USB_BUF_OVER;
}

/*********************************************************************
 * @fn      U20HOST_GetDeviceDescr
 *
 * @brief   The USB Host Get the device descriptor
 *
 * @param   buf  - Data buffer
 *          len  - Data Length
 *
 * @return  Status.
 */
uint8_t U20HOST_GetDeviceDescr( uint8_t *buf ,uint16_t *len )
{
	uint8_t l;
	uint8_t status;
	uint8_t Setup_buf[8];
	l = *len;

	Setup_buf[0] = 0x80;
	Setup_buf[1] = 0x06;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x01;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = (uint8_t)l;
	Setup_buf[7] = 0x00;

	status = U20HostCtrlTransfer( Setup_buf,buf, &l );
	if( status == USB_INT_SUCCESS ){
		*len = l;
		UsbDevEndp0Size = *(buf+7);
	}
	return status;
}

/*********************************************************************
 * @fn      U20HOST_GetConfigDescr
 *
 * @brief   The USB Host Get the configuration descriptor
 *
 * @param   buf  - Data buffer
 *          len  - Data Length
 *
 * @return  Status.
 */
uint8_t U20HOST_GetConfigDescr( uint8_t *buf ,uint16_t *len )
{
	uint8_t l;
	uint8_t status;
	uint8_t Setup_buf[8];

	Setup_buf[0] = 0x80;
	Setup_buf[1] = 0x06;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x02;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x40;
	Setup_buf[7] = 0x00;
	l = 0x40;

	status = U20HostCtrlTransfer( Setup_buf, buf, &l );
	if( status == USB_INT_SUCCESS ){
		Setup_buf[6] = (( (PUSB_CFG_DESCR)buf ) -> wTotalLength)&0xff;
		Setup_buf[7] = (( (PUSB_CFG_DESCR)buf ) -> wTotalLength)>>8;

		l = Ctl_Setup->wLength;
		status = U20HostCtrlTransfer( Setup_buf,buf, &l );
		if( status == USB_INT_SUCCESS )
		{
			*len = l;
		}
		else
		{
			*len = 0x00;
		}
	}
	return status;
}

/*********************************************************************
 * @fn      U20HOST_SetAddress
 *
 * @brief   USB20 Host Set Address
 *
 * @param   addr  - Address
 *
 * @return  Status.
 */
uint8_t U20HOST_SetAddress( uint8_t addr )
{
	uint8_t status;
	uint8_t Setup_buf[8];

	Setup_buf[0] = 0X00;
	Setup_buf[1] = 0x05;
	Setup_buf[2] = addr;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	status = U20HostCtrlTransfer( Setup_buf ,NULL, NULL );
	if( status == USB_INT_SUCCESS )
	{
	    USBHSH->DEV_ADDR = addr;                               
	    mDelaymS( 10 );                                        
	}
	mDelaymS( 5 );  										
	return( status );
}

/*********************************************************************
 * @fn      U20HOST_SetConfig
 *
 * @brief   USB20 Host Set the configuration value
 *
 * @param   cfg  - Configuration value
 *
 * @return  Status.
 */
uint8_t U20HOST_SetConfig( uint8_t cfg )
{
	uint8_t status;
	uint8_t Setup_buf[8];

	Setup_buf[0] = 0x00;
	Setup_buf[1] = 0x09;
	Setup_buf[2] = cfg;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	status = U20HostCtrlTransfer( Setup_buf, NULL, NULL );
	return( status );
}

/*********************************************************************
 * @fn      U20HOST_ClearEndpStall
 *
 * @brief   USB Host clear endpoint
 *
 * @param   endp  - Endpoint number 
 *
 * @return  Status.
 */
uint8_t U20HOST_ClearEndpStall( uint8_t endp )
{
	uint8_t status;
	uint8_t Setup_buf[8];

	Setup_buf[0] = 0x02;
	Setup_buf[1] = 0x01;
	Setup_buf[2] = 0X00;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = endp;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	status = U20HostCtrlTransfer( Setup_buf, NULL, NULL  );
	return( status );
}

/*********************************************************************
 * @fn      U20HOST_CofDescrAnalyse
 *
 * @brief   USB host analysis configuration descriptor
 *
 * @param   pbuf  - Descriptor buffer
 *
 * @return  Status.
 */
uint8_t U20HOST_CofDescrAnalyse( uint8_t *pbuf )
{
	gDeviceClassType = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass;
	if( ( gDeviceClassType <= 0x09 ) || ( gDeviceClassType == 0xFF ) )
	{
		return( USB_INT_SUCCESS );
	}
	else
	{
		gDeviceClassType = USB_DEV_CLASS_RESERVED;								
		return( ERR_USB_UNKNOWN );  									 	    
	}
}

/*********************************************************************
 * @fn      U20HSOT_Enumerate
 *
 * @brief   USB20 Host enumerate function
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status.
 */
uint8_t U20HSOT_Enumerate( uint8_t *pbuf )
{
	uint8_t status;
	uint8_t cfg,i;
	uint16_t len;

	USBHSH->PORT_STATUS_CHG = 0xff;
	USBHSH->PORT_CTRL |= USBHS_UH_SET_PORT_RESET;     																   // Bus Reset
	USBHSH->CFG |= USBHS_UH_SOF_EN;

	while(1){																										   // Recheck the device attach
		if(USBHSH->PORT_STATUS_CHG & USBHS_UHIF_PORT_RESET){      													   // Wait Bus Reset     
			if( (USBHSH->PORT_STATUS & USBHS_UHIS_PORT_CONNECT) && ( USBHSH->PORT_STATUS & USBHS_UHIS_PORT_EN ) ) {    // Device Attach
				printf("PORT_STATUS_CHG = %02x, PORT_STATUS = %02x\n", USBHSH->PORT_STATUS_CHG, USBHSH->PORT_STATUS);
				USBHSH->PORT_STATUS_CHG = USBHS_UHIF_PORT_RESET;													   // Clear Port Status
				printf("USBHSH->PORT_STATUS_CHG = %02x, PORT_STATUS = %02x\n", USBHSH->PORT_STATUS_CHG, USBHSH->PORT_STATUS);
				break;
			}
		}
		else{
			if( (USBHSH->PORT_STATUS & USBHS_UHIS_PORT_CONNECT)  == 0x00){
				USBHSH->PORT_STATUS_CHG = 0xff;																		   // Clear Port Status
				gDeviceUsbType = 0x00;
				gDeviceConnectstatus = 0x00;
				printf("USB2.0 DEVICE DISCONNECT !\n");
			}
		}
	    mDelaymS(1000); 
	}
	USBHSH->DEV_ADDR = 0x00;
	len = 8;
	status = U20HOST_GetDeviceDescr( pbuf, &len );
	if( status != USB_INT_SUCCESS ){
		printf("GetDev_ERROR=%02x\n", status );
		return USB_OPERATE_ERROR;
	}
	UsbDevEndp0Size = *(pbuf+7);
	printf("UsbDevEndp0Size=%02x\n",UsbDevEndp0Size);
	mDelaymS(1);
	status = U20HOST_SetAddress( DEVICE_ADDR );
	if( status != USB_INT_SUCCESS ){
		printf("SetAddr_ERROR=%02x\n", status );
		return USB_OPERATE_ERROR;
	}
	len = 0x12;
	status = U20HOST_GetDeviceDescr( pbuf, &len );
	if( status != USB_INT_SUCCESS ){
		printf("GetDev_ERROR=%02x\n", status );
		return USB_OPERATE_ERROR;
	}
	status = U20HOST_GetConfigDescr( pbuf, &len );
	if( status != USB_INT_SUCCESS ){
		printf("GetDev_ERROR=%02x\n", status );
		return USB_OPERATE_ERROR;
	}
	cfg = ( (PUSB_CFG_DESCR)pbuf ) -> bConfigurationValue;
	printf("cfg=%02x\n",cfg);
	printf("ConfigDescr:");
	for( i=0;i!=len;i++ ){
		printf("%02x ",*(pbuf+i));
	}
	printf("\n");
	status=U20HOST_CofDescrAnalyse( pbuf );
	if( status != USB_INT_SUCCESS )
	{
		printf( "Current Device is Not USB Mass Storage Device\n" );
		return( USB_OPERATE_ERROR ); 		 																		   // Return directly
	}
	status = U20HOST_MS_CofDescrAnalyse( pbuf );
	status = U20HOST_SetConfig( cfg ); 																				   // USB Host Set Configuration
	if ( status != USB_INT_SUCCESS )
	{
		printf( "SetConfig_ERROR = %02X\n", (UINT16) status );
		return( USB_OPERATE_ERROR ); 		 																		   // Return directly
	}
	return status;
}

