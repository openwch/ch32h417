/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_enum.c
* Author             : WCH
* Version            : V1.0
* Date               : 2025/05/30
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
#include "hub.h"
#include "CHRV3UFI.h"

/* Constant, variable, structure definition */
USB_SETUP_REQ *Ctl_Setup;	  													/* Control the transmission data packet */
uint8_t   gDeviceClassType;														/* Current device type */
volatile uint8_t   gDeviceConnectstatus;										/* USB connection status */
uint8_t   gDeviceUsbType = 0;												    /* USB connection status:0x01--USB2.0&1.1  0x02--USB3.0 */
uint8_t   gUdisk_flag = 0;

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
	uint16_t total, wait_erdy, temp;
	uint32_t timeout;
	uint16_t RetLenTemp = 0;

	if( RetLen == NULL )
	{
		RetLen = &RetLenTemp;
	}

	memcpy( USBSS_EP0_Tx_Buf, ReqBuf, 8 );
	USBSSH->UH_TX_DMA = (uint32_t)USBSS_EP0_Tx_Buf;                     			// Set tx dma address
    USBSSH->UH_RX_DMA = (uint32_t)USBSS_EP0_Rx_Buf;                     			// Set rx dma address
    USBSSH->HOST_TX_NUMP = 1;
    USBSSH->UH_TX_CTRL = UH_TX_SETUP  | (1<<24) | 8; 					// Tx SETUP
	USBSSH->UH_TX_CTRL |= UH_RTX_VALID;
	timeout = 0;
	wait_erdy = 0;

	/* Setup Stage */
	while(1)
	{					
		if( gDeviceConnectstatus == USB_INT_DISCONNECT ) return USB_INT_DISCONNECT;
		if( USBSSH->UH_TX_CTRL & UH_INT_FLAG )
		{
			if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_ACK )
			{
				USBSSH->UH_TX_CTRL = 0x0;
				if( (((USBSSH->USB_STATUS) >>16) & 0x1f) == 0 )						// Wait for the USB device ERDY
				{					
					wait_erdy = 1;
				}
				else{
					break;															// Normal. Exit directly
				}
			}
		    else if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_FAILED )
            {
                USBSSH->UH_TX_CTRL = UH_TX_SETUP  | (1<<24) | 8; 		// Tx SETUP
				USBSSH->UH_TX_CTRL |=UH_RTX_VALID;
            }
		}		
		if( USBSSH->USB_STATUS & USB_ERDY_FLAG )									// Wait until the host receives ERDY and exits
		{									
			wait_erdy = 0;
			if( !(USBSSH->UH_TX_CTRL & UH_RTX_VALID ) )
			{		
				USBSSH->UH_TX_CTRL |=  UH_RTX_VALID;
			}
			USBSSH->USB_STATUS = USB_ERDY_FLAG;
			break;
		}
		timeout++;
		if( timeout > 0xfffffff )	return USB_CH417USBTIMEOUT;
	}	

	/* Data Stage */
	total = *( ReqBuf + 6 );
	if( total && DatBuf ) 
	{
		timeout = 0;
		wait_erdy = 0;
		if( *ReqBuf & 0x80 ) 														// Read data
		{  	
			USBSSH->HOST_RX_NUMP = 1;												// Setup  A packet of data
			USBSSH->UH_RX_CTRL = 0 << 16  | 1<<24 | 0 <<12; 			// 0<<16 Synchronization flag,1<<24 Sudden number of brust,0 <<12 Endpoint number
			USBSSH->UH_RX_CTRL |= UH_RTX_VALID;
			while(1)
			{
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
					USBSSH->UH_RX_CTRL = 0 << 16  | 1<<24 | 0 <<12; 		// ACK-TP(IN)
					USBSSH->UH_RX_CTRL |= UH_RTX_VALID;
					USBSSH->USB_STATUS = USB_ERDY_FLAG;
				}
				if( gDeviceConnectstatus == USB_INT_DISCONNECT )	return USB_INT_DISCONNECT;
			}
		}		
		else												// Send data
		{					
			if( total )
			{
				wait_erdy =0;
				timeout=0;
				memcpy( USBSS_EP0_Tx_Buf, DatBuf, *RetLen );	
				
				USBSSH->HOST_TX_NUMP = 1;
    			USBSSH->UH_TX_CTRL = 0 <<16 | 1<<24 | 0 <<12 | *RetLen;										// Seq_num --- 0 ,Burst_nump -- 1,Endp -- 0
				USBSSH->UH_TX_CTRL |= UH_RTX_VALID;
				while(1)
				{
					if( gDeviceConnectstatus == USB_INT_DISCONNECT )	return USB_INT_DISCONNECT;
					if( USBSSH->UH_TX_CTRL & UH_INT_FLAG ) 
					{
						if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_ACK )
						{
							USBSSH->UH_TX_CTRL = 0x0;
							{
								break;
							}
						}
						else if( (USBSSH->USB_STATUS & USB_RX_RES_MASK) == TX_RES_NRDY )
						{
							USBSSH->UH_TX_CTRL = 0x0;
							USBSSH->USB_STATUS = USB_ACT_FLAG;
						}
						else if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_STALL )
						{
							USBSSH->UH_TX_CTRL = 0x0;
							return USB_INT_DISK_ERR;
						}
						else if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_FAILED )
						{
							USBSSH->UH_TX_CTRL = USBSSH->UH_TX_CTRL = 0 <<16  | 1<<24 | 0 <<12 | *RetLen;		// Seq_num --- 0 , Burst_nump -- 1, endp -- 0
							USBSSH->UH_TX_CTRL |= UH_RTX_VALID;
						}
						else
						{
							USBSSH->UH_TX_CTRL = 0x0;
							return USB_INT_DISK_ERR;
						}
					}
					else if( (USBSSH->USB_STATUS & USB_ERDY_FLAG)  )
					{
						if( !(USBSSH->UH_TX_CTRL & UH_RTX_VALID ))
						{
							USBSSH->UH_TX_CTRL = 0 <<16 | 1<<24 | 0 <<12 | *RetLen;
							USBSSH->UH_TX_CTRL |= UH_RTX_VALID;
						}
						USBSSH->USB_STATUS = USB_ERDY_FLAG;
					}
					if( wait_erdy ){			
						if( USBSSH->USB_STATUS & USB_ERDY_FLAG )
						{															// Wait until the host receives ERDY and exits
							wait_erdy = 0;
							USBSSH->USB_STATUS = USB_ERDY_FLAG;
							break;
						}
					}
					if( gDeviceConnectstatus == USB_INT_DISCONNECT )	return USB_INT_DISCONNECT;
					timeout++;
					if( timeout > 0xfffff )	return USB_CH417USBTIMEOUT;
				}
			}	
		}
	}
	Delay_Ms(1);																											// This delay function is required,otherwise the STATUS package cannot be transmitted 
	timeout = 0;
    USBSSH->HOST_TX_NUMP = 1;
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
                timeout = 0;
                while( !(USBSSH->USB_STATUS & USB_ERDY_FLAG) )
                {
                    if( gDeviceConnectstatus == USB_INT_DISCONNECT )	return USB_INT_DISCONNECT;
                    timeout++;
                    if( timeout > 0xfffff )	return USB_CH417USBTIMEOUT;
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
		l = ( Setup_buf[ 7 ] << 8 ) | Setup_buf[ 6 ];
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
 * @fn      U30HOST_SetSelfAddress
 *
 * @brief   USB3.0 Host Set Device Address directly to register.
 *
 * @param   addr  - Device address to set.
 *
 * @return  none
 */
void U30HOST_SetSelfAddress( uint8_t addr )
{
	USBSSH->USB_CONTROL &= 0x00ffffff;
	USBSSH->USB_CONTROL |= addr<<24; 										// SET ADDRESS	
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
uint8_t U30HOST_CofDescrAnalyse( uint8_t *pbuf ,uint8_t port)
{
	gDeviceClassType = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> itf_descr.bInterfaceClass;
	if( ( gDeviceClassType <= 0x09 ) || ( gDeviceClassType == 0xFF ) )
	{
		if( (gDeviceClassType == HUB_DEVICE) && (SS_HUB_SaveData.Depth == 0)){			// HUB device
			SS_HUB_SaveData.Device_Type = gDeviceClassType;								// Device type
		}
		else if( SS_HUB_SaveData.Depth ){				// Device under HUB port
			SS_HUB_SaveData.HUB_Info.portD[port].DeviceType = gDeviceClassType;
		}
		else{											// Direct?connected device

		}
		return USB_INT_SUCCESS;
	}
	else
	{
		gDeviceClassType = USB_DEV_CLASS_RESERVED;								// Default: unknown device type
		return ERR_USB_UNKNOWN;  									 	    	// Unsupported device, return error
	}
}

/*********************************************************************
 * @fn      U30OST_GetHUBDevDescr
 *
 * @brief   USB host
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status 
 */
uint8_t U30HOST_GetHUBDevDescr( uint8_t *buf ,uint16_t *len )
{
	uint16_t l;
	uint8_t  status;
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0xA0;
	Setup_buf[1] = 0x06;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x2a;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x0c;
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
 * @fn      U30OST_GetHUBDevDescr
 *
 * @brief   USB host
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status 
 */
uint8_t U30HOST_GetHUBDevStatus( uint8_t *buf ,uint16_t *len )
{
	uint16_t l;
	uint8_t  status;
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0x80;
	Setup_buf[1] = 0x00;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x02;
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
 * @fn      U30OST_GetHUBDevDescr
 *
 * @brief   USB host
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status 
 */
uint8_t U30HOST_SetHUBDevDepth( uint8_t depth )
{

	uint8_t  status;
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0x20;
	Setup_buf[1] = 0x0c;
	Setup_buf[2] = depth;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	status = U30HostCtrlTransfer( Setup_buf, NULL, NULL );
	if( status == USB_INT_SUCCESS )
	{
	}
	return status;
}	

/*********************************************************************
 * @fn      U30OST_GetHUBDevDescr
 *
 * @brief   USB host
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status 
 */
uint8_t U30HOST_GetHubStatus( uint8_t *buf ,uint16_t *len )
{
	uint16_t l;
	uint8_t  status;
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0xA0;
	Setup_buf[1] = 0x00;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x04;
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
 * @fn      U30OST_GetHubPortStatus
 *
 * @brief   USB host
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status 
 */
uint8_t U30HOST_GetHubPortStatus( uint8_t port ,uint8_t *buf )
{
	uint16_t l;
	uint8_t  status;
	uint8_t  Setup_buf[8];
	Setup_buf[0] = 0xA3;
	Setup_buf[1] = 0x00;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = port;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x04;
	Setup_buf[7] = 0x00;
	status = U30HostCtrlTransfer( Setup_buf, buf, &l );
	if( status == USB_INT_SUCCESS )
	{
		
	}

	return status;
}	

/*********************************************************************
 * @fn      U30OST_ClrPortFeature
 *
 * @brief   USB host
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status 
 */
uint8_t U30HOST_ClrPortFeature( uint8_t port ,uint8_t port_status )
{
	uint8_t  status;
	uint8_t  Setup_buf[8];
	Setup_buf[0] = 0x23;
	Setup_buf[1] = 0x01;
	Setup_buf[2] = port_status;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = port;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;
	status = U30HostCtrlTransfer( Setup_buf, NULL, NULL );
	if( status == USB_INT_SUCCESS )
	{	
	}
	return status;
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
uint8_t U30HSOT_Enumerate( uint8_t *pbuf,uint8_t addr,uint8_t port )
{
	uint8_t status;
	uint16_t len,i;
	uint8_t cfg;
//	mDelaymS( 1000 );       													// There is a necessary delay. When encountering some USB drives, the device descriptor cannot be obtained without delay

/* Sets Address */
	printf("start_enmu\n");
    U30HOST_SetSelfAddress( 0x00 );                                        
    mDelaymS(1);
	status = U30HOST_SetAddress( addr );										// U30 first sets the address and then obtains the device descriptor
	printf("set_addr=%02x\n", status);
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
	status = U30HOST_CofDescrAnalyse( pbuf,port );								// Analyze The Configuration Descriptor
	if( status != USB_INT_SUCCESS )
	{
		printf( "Current Device is Not USB Mass Storage Device\n" );
		return( USB_OPERATE_ERROR ); 		 									// Return directly
	}
	status = U30HOST_MS_CofDescrAnalyse( pbuf,port );
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
	if( (SS_HUB_SaveData.Device_Type == HUB_DEVICE) && (SS_HUB_SaveData.Depth == 0)){					//HUB
		status = U30HOST_GetHUBDevDescr( pbuf,&len );							// Get the HUB descriptor
		printf("GetHUBDevDescr=%02x\n",status);
		SS_HUB_SaveData.HUB_Info.Speed = 3;
		SS_HUB_SaveData.HUB_Info.Numofport = *(pbuf+2);							// Get the HUB Port Number
		status = U30HOST_GetHUBDevStatus( pbuf,&len );
		printf("GetHUBDevStatus=%02x\n",status);
		status = U30HOST_SetHUBDevDepth( SS_HUB_SaveData.Depth );
		printf("SetHUBDevDepth=%02x\n",status);
		status = U30HOST_GetHubStatus( pbuf,&len  );
		printf("GetHubStatus=%02x\n",status);
		printf("SS_HUB_SaveData.HUB_Info.Numofport=%02x\n",SS_HUB_SaveData.HUB_Info.Numofport);
		for( i=0;i!=SS_HUB_SaveData.HUB_Info.Numofport;i++ ){	
			status = U30HOST_GetHubPortStatus( i+1 ,pbuf);		
		}
		SS_HUB_SaveData.Depth++;												// Deepth+1
	}
	return USB_OPERATE_SUCCESS;
}

/*********************************************************************
 * @fn      U30HOST_CLRPortpChangeField
 *
 * @brief   USB3.0 Host Clear Port Change Field for all port status bits.
 *
 * @param   port  - Port number to clear change field.
 *
 * @return  Status.
 */
uint8_t U30HOST_CLRPortpChangeField( uint8_t port )
{
	uint8_t status;
	if( SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField & C_PORT_CONNECT_ST ){		// DEVICE CONNECT
		status = U30HOST_ClrPortFeature( port+1,C_PORT_CONNECTION );
		if( status != USB_INT_SUCCESS )
		{
			printf("C_PORT_CONNECTION_ERROR\n");
			return status;
		}
		SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField &= ~C_PORT_CONNECT_ST;	
	}
	if( SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField & C_PORT_OVC_ST ){			// OVER CURRENT
		status = U30HOST_ClrPortFeature( port+1,C_PORT_OVER_CURRENT );
		if( status != USB_INT_SUCCESS )
		{
			printf("C_PORT_OVC_ST_ERROR\n");
			return status;
		}	
		SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField &= ~C_PORT_OVC_ST;
	}
	if( SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField & C_PORT_RESET_ST ){			// HOT_RESET
		status = U30HOST_ClrPortFeature( port+1,C_PORT_RESET );
		if( status != USB_INT_SUCCESS )
		{
			printf("C_PORT_RESET_ERROR\n");
			return status;
		}	
		SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField &= ~C_PORT_RESET_ST;
	}
	if( SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField & C_PORT_BHRESET_ST ){		// WARM_RESET
		status = U30HOST_ClrPortFeature( port+1,C_BH_PORT_RESET );
		if( status != USB_INT_SUCCESS )
		{
			printf("C_BH_PORT_RESET_ERROR\n");
			return status;
		}	
		SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField &= ~C_PORT_BHRESET_ST;
	}	
	if( SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField & C_PORT_LINK_STATE_ST ){		// LINK STATE CHANGE
		status = U30HOST_ClrPortFeature( port+1,C_PORT_LINK_STATE );
		if( status != USB_INT_SUCCESS )
		{
			printf("C_PORT_LINK_STATE_ERROR\n");
			return status;
		}
		SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField &= ~C_PORT_LINK_STATE_ST;
	}	
	if( SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField & C_PORT_CFG_ERR_ST )			// PORT CFG
	{		
		status = U30HOST_ClrPortFeature( port+1,C_PORT_CFG_ERR );
		if( status != USB_INT_SUCCESS )
		{
			printf("C_PORT_CFG_ERR_ERROR\n");
			return status;
		}	
		SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField &= ~C_PORT_CFG_ERR_ST;
	}	
	return status;
}

/*********************************************************************
 * @fn      U30HOST_ConnectDevice
 *
 * @brief   USB3.0 Host Connect Device. Check all hub ports for device connection/disconnection.
 *
 * @return  Status.
 */
uint8_t U30HOST_ConnectDevice( void )
{
	uint8_t i, status;
	uint8_t buf[ 4 ];
	uint16_t Port_Change_Field, Port_Status_Bits;
	USBSSH->LINK_TX_ROUTE = 0;															// Default setting: Switch to the ROOT-HUB port
	U30HOST_SetSelfAddress( SS_HUB_SaveData.HUB_Info.DevAddr );
	for( i=0; i != SS_HUB_SaveData.HUB_Info.Numofport; i++ )
	{
		status = U30HOST_GetHubPortStatus( i+1,buf );									// Get Port Status
		if( status == USB_INT_SUCCESS ){
			Port_Change_Field = buf[2];
    		Port_Change_Field |= ((uint16_t)buf[3]<<8);
			SS_HUB_SaveData.HUB_Info.portD[i].PortpChangeField = Port_Change_Field;
		}
		else{
			return status;		
		} 
	}
	for( i=0; i != SS_HUB_SaveData.HUB_Info.Numofport; i++ )
	{
		if( SS_HUB_SaveData.HUB_Info.portD[i].PortpChangeField ){						// It is stated that there are devices present.
			U30HOST_CLRPortpChangeField( i );
			Delay_Ms( 5 );		
			status = U30HOST_GetHubPortStatus( i+1,buf );								// Then obtain the port status to determine whether the device is connected
			if( status == USB_INT_SUCCESS ){
				Port_Status_Bits = buf[0];
				Port_Status_Bits |= ((uint16_t)buf[1]<<8);
				printf("SS_Port_Status=%02x\n",Port_Status_Bits);
				if( (Port_Status_Bits &  ( PORT_PWR_ST | PORT_ENABLE_ST | PORT_CONNECT_ST )) == ( PORT_PWR_ST | PORT_ENABLE_ST | PORT_CONNECT_ST ) )	
				{
					SS_HUB_SaveData.HUB_Info.portD[i].Addr = USB_SSDEVICE_ADDR+1+i;		// The addresses of The following devices
					SS_HUB_SaveData.HUB_Info.portD[i].Status = HUB_DEVICE_CONNECT;		// DEVICE CONNECT
					U30HOST_HUB_DEV_Enumerate( i );
				}
				else if( ((Port_Status_Bits &  LINK_HUB_STATE_MASK) == PORT_LINK_RXDET)    || 	// RDEXT
					     ((Port_Status_Bits &  LINK_HUB_STATE_MASK) == PORT_LINK_INACTIVE) || 	// INCATIVE
						 ((Port_Status_Bits &  LINK_HUB_STATE_MASK) == PORT_LINK_DISABLED))		// DISABLE
				{		
					SS_HUB_SaveData.HUB_Info.portD[i].Status = HUB_DEVICE_DISCONNECT;			// DEVICE DISCONNECT
					SS_HUB_SaveData.HUB_Info.portD[i].Addr = 0;
				}
			}
			else{
				 return status;	
			}	 		
		}
	}
	return status;
}

/*********************************************************************
 * @fn      U30HOST_CheckDeviceStatus
 *
 * @brief   USB3.0 Host Check Device Status for a specific port.
 *
 * @param   port  - Port number to check.
 *
 * @return  Status.
 */
uint8_t U30HOST_CheckDeviceStatus( uint8_t port )
{
	uint8_t status,buf[ 4 ];
	uint16_t Port_Change_Field,Port_Status_Bits;
	USBSSH->LINK_TX_ROUTE = 0;												// Default: Switch to the ROOT-HUB port
	U30HOST_SetSelfAddress( SS_HUB_SaveData.HUB_Info.DevAddr );	
	status = U30HOST_GetHubPortStatus( port+1,buf );						// Get Port Status
	if( status == USB_INT_SUCCESS ){
		Port_Change_Field = buf[2];
		Port_Change_Field |= ((uint16_t)buf[3]<<8);
		SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField = Port_Change_Field;
		if( SS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField ){
			U30HOST_CLRPortpChangeField( port );
			Delay_Ms( 5 );		
			status = U30HOST_GetHubPortStatus( port+1,buf );				// Get the Port Status to Determine Whether the Device is Connected
			if( status == USB_INT_SUCCESS ){
				Port_Status_Bits = buf[0];
				Port_Status_Bits |= ((uint16_t)buf[1]<<8);
				printf("USBSS_Port_Status=%02x\n",Port_Status_Bits);
				if( (Port_Status_Bits &  ( PORT_PWR_ST | PORT_ENABLE_ST | PORT_CONNECT_ST )) == ( PORT_PWR_ST | PORT_ENABLE_ST | PORT_CONNECT_ST ))	
				{
					SS_HUB_SaveData.HUB_Info.portD[port].Addr = USB_SSDEVICE_ADDR+1+port;		
					SS_HUB_SaveData.HUB_Info.portD[port].Status = HUB_DEVICE_CONNECT;			// DEVICE CONNECT
					U30HOST_HUB_DEV_Enumerate( port );
				}
				else if( ((Port_Status_Bits &  LINK_HUB_STATE_MASK) == PORT_LINK_RXDET)    || 	// RDEXT
						 ((Port_Status_Bits &  LINK_HUB_STATE_MASK) == PORT_LINK_INACTIVE) || 	// INCATIVE
						 ((Port_Status_Bits &  LINK_HUB_STATE_MASK) == PORT_LINK_DISABLED)		// DISABLE
					){		
					SS_HUB_SaveData.HUB_Info.portD[port].Status = HUB_DEVICE_DISCONNECT;		// DEVICE DISCONNECT
					SS_HUB_SaveData.HUB_Info.portD[port].Addr = 0;
				}
			}
			else{
				return status;	
			}	
		}
	}
	else{
		return status;		
	} 
	return status;
}

/*********************************************************************
 * @fn      U30HOST_HUBSaveVari
 *
 * @brief   USB3.0 Host Save/restore hub device parameters.
 *
 * @param   port   - Port number.
 *          index  - 0: Backup parameters, 1: Restore parameters.
 *
 * @return  none
 */
void U30HOST_HUBSaveVari( uint8_t port,uint8_t index  )
{
	uint8_t i;
	if( index == 0 ){																// Restore variables
		USBSSH->LINK_TX_ROUTE = port+1; 											// HUB routing information
		gDeviceUsbType = USB_U30_SPEED;
		U30HOST_SetSelfAddress( SS_HUB_SaveData.HUB_Info.portD[port].Addr );		// Perform subsequent initialization
		for( i=0;i != SS_HUB_SaveData.HUB_Info.portD[port].EndpNum;i++ ){
			if( SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num &0x80 ){		// IN endpoint
				gDiskBulkInEp = SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num&0x0f;
				gDiskBulkInBrust = SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Brust;
				gDiskBulkInTog = SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog;			
			}
			else{																	// OUT endpoint
				gDiskBulkOutEp = SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num;
				gDiskBulkOutBrust = SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Brust;
				gDiskBulkOutTog = SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog;
			}
		}
	}
	else if( index == 1 ){		
		for( i=0;i != SS_HUB_SaveData.HUB_Info.portD[port].EndpNum;i++ ){
			if( SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num &0x80 ){		// IN endpoint
				SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog = gDiskBulkInTog ;
			}
			else{																	// OUT endpoint
				SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog = gDiskBulkOutTog ;
			}
		}	
	}
}

/*********************************************************************
 * @fn      U30HOST_HUB_DEV_Enumerate
 *
 * @brief   USB3.0 Host Hub Device Enumeration.
 *
 * @param   port  - Hub port number.
 *
 * @return  Status.
 */
uint8_t U30HOST_HUB_DEV_Enumerate( uint8_t port )
{
	uint8_t status = 0xFF;
	uint8_t buf[ 512 ];

	if( SS_HUB_SaveData.HUB_Info.portD[port].Status == HUB_DEVICE_CONNECT ){		// Device connected, not initialized
		USBSSH->LINK_TX_ROUTE = port+1; 			// HUB routing information
		U30HOST_SetSelfAddress( 0x00 );				// Perform subsequent initialization
		status = U30HSOT_Enumerate( buf,SS_HUB_SaveData.HUB_Info.portD[port].Addr,port );
		if( status == USB_OPERATE_SUCCESS ){
			SS_HUB_SaveData.HUB_Info.portD[port].Status = 	HUB_DEVICE_CONFIG;
			printf("device_type:%02x\n",SS_HUB_SaveData.HUB_Info.portD[port].DeviceType);
			if( SS_HUB_SaveData.HUB_Info.portD[port].DeviceType == UDISK_DEVICE ){				// Initialize USB disk
				U30HOST_HUBSaveVari( port,0 );		// Restore variables
				status = MS_Init( buf );			// Init USB mass?storage device
				U30HOST_HUBSaveVari( port,1 );		// Save variables
				FAT_Init( port );
			}
			else if( SS_HUB_SaveData.HUB_Info.portD[port].DeviceType == HID_DEVICE ){			// HID device initialization
				printf("hid_device\n");
			}
		}		
	}
	return status;
}

/* USB Data Cache Area */
uint8_t DATA_BUFFER[ 34*1024 ];
uint8_t FAT_BUFFER[ 32 ];
uint32_t addr = 0;

/*********************************************************************
 * @fn      U30HOST_HUB_DEV_MainProcess
 *
 * @brief   USB3.0 Host Hub Device Main Process. Read sectors from UDISK devices.
 *
 * @return  none
 */
void U30HOST_HUB_DEV_MainProcess( void )
{
	uint8_t port, s, i;
	uint8_t *pd = DATA_BUFFER;
	for( port=0; port != SS_HUB_SaveData.HUB_Info.Numofport; port++ ){
		if( ( SS_HUB_SaveData.HUB_Info.portD[port].Status ==HUB_DEVICE_CONFIG ) && 			// Initialization Successful
			( SS_HUB_SaveData.HUB_Info.portD[port].DeviceType == UDISK_DEVICE ) ){			// The Device is a USB Flash Drive
			H417_RSaveVariable( port );														// Restore variable
			printf("USB3.0_FAT_PROCESS\n");
			strcpy( (char *)&mCmdParam.Create.mPathName,( const char * )"\\NEWFILE.TXT");  	// Create NEWFILE.TXT
			i = CHRV3FileOpen();
			printf("i=%x\n",i);
			if(i == ERR_FAT_ERROR){ 
				printf("File System Not Supported! Require FAT12/FAT16/FAT32\n");
			}else{
				if((i == ERR_MISS_DIR)||(i == ERR_MISS_FILE))
				{
					printf( "Find No File And Create\r\n" );
					i = CHRV3FileCreate();
				}
				mCmdParam.Close.mUpdateLen=1;
				i = CHRV3FileClose();                                   				    // Modify file information completed

				printf("Write DATA\n");
				strcpy( (char *)&mCmdParam.Open.mPathName,( const char * )"\\NEWFILE.TXT");
				i = CHRV3FileOpen();
				memset(FAT_BUFFER, 0x43, 32);
				mCmdParam.Write.mDataBuffer = FAT_BUFFER;
				mCmdParam.Write.mSectorCount = 16;
				i=CHRV3FileWrite( );                               						   // Write data to NEWFILE.txt
				for(s=0;s!=5;s++)  printf("%02x ",FAT_BUFFER[s]);
				printf("\n");
				mCmdParam.Close.mUpdateLen = 1;
				i= CHRV3FileClose();                                 					   // Write to NEWFILE.txt completed

				printf("Read DATA\n");
				strcpy( (char *)&mCmdParam.Open.mPathName,( const char * )"\\NEWFILE.TXT");    // Open File
				i = CHRV3FileOpen();
				mCmdParam.Read.mDataBuffer = pd;
				mCmdParam.Read.mSectorCount = 16;
				i=CHRV3FileRead( );
				for( s=0; s != 5; s++ )	printf("%02x ",*(pd+s));							   // Read File DATA
				printf("\n");				
				if( mCmdParam.Read.mSectorCount < 16 )
				{
					printf("Err Number of Sectors Read Only: %d\n",mCmdParam.Read.mSectorCount);
				}
				mCmdParam.Close.mUpdateLen = 1;
				i= CHRV3FileClose();
			}
			H417_SaveVariable( port );	
			SS_HUB_SaveData.HUB_Info.portD[port].Status = HUB_DEVICE_CONFIG_SUCESS;			   // Operation Completed
		}			
	}
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
    status = U30HOST_SetAddress( USB_SSDEVICE_ADDR );         // Set the address first and then obtain the device descriptor
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
                if( timeout < 0xFFFFFFFF )
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

		l = ( Setup_buf[ 7 ] << 8 ) | Setup_buf[ 6 ];
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
 * @fn      U20HOST_GetHUBDevDescr
 *
 * @brief   USB2.0 Host Get Hub Device Descriptor.
 *
 * @param   buf  - Data buffer.
 *          len  - Data length.
 *
 * @return  Status.
 */
uint8_t U20HOST_GetHUBDevDescr( uint8_t *buf, uint16_t *len )
{
	uint8_t l;
	uint8_t status;
	uint8_t Setup_buf[8];

	Setup_buf[0] = 0xA0;
	Setup_buf[1] = 0x06;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x29;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0X47;
	Setup_buf[7] = 0x00;

	status = U20HostCtrlTransfer( Setup_buf,buf, &l );
	if( status == USB_INT_SUCCESS ){
		*len = l;
	}
	return status;	
}	

/*********************************************************************
 * @fn      U20HOST_GetHubStatus
 *
 * @brief   USB2.0 Host Get Hub Status.
 *
 * @param   buf  - Data buffer.
 *          len  - Data length.
 *
 * @return  Status.
 */
uint8_t U20HOST_GetHubStatus( uint8_t *buf, uint16_t *len )
{
	uint8_t l;
	uint8_t status;
	uint8_t Setup_buf[8];

	Setup_buf[0] = 0xA0;
	Setup_buf[1] = 0x00;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0X04;
	Setup_buf[7] = 0x00;
	l = *len;
	status = U20HostCtrlTransfer( Setup_buf, buf, &l );
	if( status == USB_INT_SUCCESS ){
		*len = l;
	}
	return status;	
}

/*********************************************************************
 * @fn      U20HOST_GetHUBDevStatus
 *
 * @brief   USB2.0 Host Get Hub Device Status.
 *
 * @param   buf  - Data buffer.
 *          len  - Data length.
 *
 * @return  Status.
 */
uint8_t U20HOST_GetHUBDevStatus( uint8_t *buf ,uint16_t *len )
{
	uint8_t l;
	uint8_t status;
	uint8_t Setup_buf[8];
	l = *len;

	Setup_buf[0] = 0x80;
	Setup_buf[1] = 0x00;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0X02;
	Setup_buf[7] = 0x00;
	status = U20HostCtrlTransfer( Setup_buf,buf, &l );
	if( status == USB_INT_SUCCESS ){
		*len = l;
	}
	return status;	
}

/*********************************************************************
 * @fn      U20HOST_SetHUBPortFeatrue
 *
 * @brief   USB2.0 Host Set Hub Port Feature.
 *
 * @param   port  - Port number.
 *          FeatureSelt  - Feature selector.
 *
 * @return  Status.
 */
uint8_t U20HOST_SetHUBPortFeatrue( uint8_t port, uint8_t FeatureSelt )
{
	uint8_t status;
	uint8_t Setup_buf[8];
	Setup_buf[0] = 0x23;
	Setup_buf[1] = 0x03;
	Setup_buf[2] = FeatureSelt;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = port;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0X00;
	Setup_buf[7] = 0x00;
	status = U20HostCtrlTransfer( Setup_buf,NULL, NULL );	
	return status;
}

/*********************************************************************
 * @fn      U20HOST_GetHubPortStatus
 *
 * @brief   USB2.0 Host Get Hub Port Status.
 *
 * @param   port  - Port number.
 *          buf   - Data buffer.
 *
 * @return  Status.
 */
uint8_t U20HOST_GetHubPortStatus( uint8_t port ,uint8_t *buf )
{
	uint8_t l;
	uint8_t status;
	uint8_t Setup_buf[8];
	Setup_buf[0] = 0xA3;
	Setup_buf[1] = 0x00;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = port;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x04;
	Setup_buf[7] = 0x00;
	status = U20HostCtrlTransfer( Setup_buf, buf, &l );
	if( status == USB_INT_SUCCESS )
	{
		
	}

	return status;
}	

/*********************************************************************
 * @fn      U20HOST_ClrPortFeature
 *
 * @brief   USB host
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status 
 */
uint8_t U20HOST_ClrPortFeature( uint8_t port ,uint8_t port_status )
{
	uint8_t  status;
	uint8_t  Setup_buf[8];
	Setup_buf[0] = 0x23;
	Setup_buf[1] = 0x01;
	Setup_buf[2] = port_status;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = port;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;
	status = U20HostCtrlTransfer( Setup_buf, NULL, NULL );
	if( status == USB_INT_SUCCESS )
	{	

	}
	return status;
}	

/*********************************************************************
 * @fn      U20HOST_SetPortFeature
 *
 * @brief   USB host
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status 
 */
uint8_t U20HOST_SetPortFeature( uint8_t port ,uint8_t port_status )
{
	uint8_t  status;
	uint8_t  Setup_buf[8];
	Setup_buf[0] = 0x23;
	Setup_buf[1] = 0x03;
	Setup_buf[2] = port_status;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = port;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;
	status = U20HostCtrlTransfer( Setup_buf, NULL, NULL );
	if( status == USB_INT_SUCCESS )
	{	

	}
	return status;
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
uint8_t U20HOST_CofDescrAnalyse( uint8_t *pbuf ,uint8_t port )
{
	gDeviceClassType = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass;
	if( ( gDeviceClassType <= 0x09 ) || ( gDeviceClassType == 0xFF ) )
	{
		printf("gDeviceClassType=%02x\n",gDeviceClassType);
		if( (gDeviceClassType == HUB_DEVICE) && (HS_HUB_SaveData.Depth == 0)){			// HUB device
			HS_HUB_SaveData.Device_Type = gDeviceClassType;								// Device type
			printf("HS_HUB_SaveData.Device_Type=%02x\n",HS_HUB_SaveData.Device_Type);
		}
		else if( HS_HUB_SaveData.Depth ){												// Device under HUB port
			HS_HUB_SaveData.HUB_Info.portD[port].DeviceType = gDeviceClassType;
		}
		else{																			// Direct?connected device

		}
		return USB_INT_SUCCESS;
	}
	else
	{
		gDeviceClassType = USB_DEV_CLASS_RESERVED;								
		return( ERR_USB_UNKNOWN );  									 	    
	}
}

/*********************************************************************
 * @fn      U20HOST_SetSelfAddress
 *
 * @brief   USB2.0 Host Set Device Address directly to register.
 *
 * @param   addr  - Device address to set.
 *
 * @return  none
 */
void U20HOST_SetSelfAddress( uint8_t addr )
{
	USBHSH->DEV_ADDR = addr;
}

/*********************************************************************
 * @fn      U20HOST_CLRPortpChangeField
 *
 * @brief   USB2.0 Host Clear Port Change Field for all port status bits.
 *
 * @param   port  - Port number to clear change field.
 *
 * @return  Status.
 */
uint8_t U20HOST_CLRPortpChangeField( uint8_t port )
{
	uint16_t status;
	if( HS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField & C_PORT_CONNECT_ST ){	// DEVICE ATTACH
	status = U20HOST_ClrPortFeature( port+1,C_PORT_CONNECTION );
	if( status != USB_INT_SUCCESS )
	{
		printf("C_PORT_CONNECTION_error\n");
		return status;
	}
	HS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField &= ~C_PORT_CONNECT_ST;	
	}
	if( HS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField & C_PORT_OVC_ST ){		// Over CURRENT
		status = U20HOST_ClrPortFeature( port+1,C_PORT_OVER_CURRENT );
		if( status != USB_INT_SUCCESS )
		{
			printf("C_PORT_OVC_ST_error\n");
			return status;
		}	
		HS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField &= ~C_PORT_OVC_ST;
	}
	if( HS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField & C_PORT_RESET_ST ){		// RESET
		status = U20HOST_ClrPortFeature( port+1,C_PORT_RESET );
		if( status != USB_INT_SUCCESS )
		{
			printf("C_PORT_RESET_error\n");
			return status;
		}	
		HS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField &= ~C_PORT_RESET_ST;
	}
	return status;
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
uint8_t U20HSOT_Enumerate( uint8_t *pbuf,uint8_t addr,uint8_t port )
{
	uint8_t status;
	uint8_t cfg,i;
	uint16_t len,Port_Status_Bits,time_out;
	uint8_t  buf[4];
	time_out = 0;
	if( port == 0xff ){				// Port 0 is root HUB
		while(1){
			printf("USBHSH->PORT_STATUS=%02x\n",USBHSH->PORT_STATUS);
			if(USBHSH->PORT_STATUS & USBHS_UHIS_PORT_CONNECT)break;
			mDelaymS(50);
			time_out++;
			if( time_out>=20 )return 0xff;		// U2 connection abnormal
		}
		// mDelaymS(300); 
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
			mDelaymS(100); 
		}
	}
	else{																// Device under HUB, separate handling required
		mDelaymS(50);		
		U20HOST_SetSelfAddress( HS_HUB_SaveData.HUB_Info.DevAddr );		// HUB device address
		status = U20HOST_GetHubPortStatus( port+1 ,pbuf);
		if( status != USB_INT_SUCCESS )return status;
		mDelaymS( 5 );
		U20HOST_SetPortFeature( port+1,0x04 );
		do
		{
			mDelaymS( 10 );
			status = U20HOST_GetHubPortStatus(  port+1,buf );
			if( status != ERR_SUCCESS )
			{
				return( status );
			}
		}while( buf[ 0 ] & 0x10 );							// Wait for device enable

		HS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField = buf[2];
		HS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField |=  ((uint16_t)buf[3]<<8);
		U20HOST_CLRPortpChangeField( port );
		status = U20HOST_GetHubPortStatus(  port+1,buf );
		Port_Status_Bits = buf[0];
		Port_Status_Bits |=  ((uint16_t)buf[1]<<8);			// Device port status
		if( Port_Status_Bits & 0x400 ){
			HS_HUB_SaveData.HUB_Info.portD[port].Speed = HIGH_SPEED;		// HighSpeed device
		}
		else{
			if( Port_Status_Bits & 0x200 ){
				HS_HUB_SaveData.HUB_Info.portD[port].Speed = LOW_SPEED;		// LowSpeed device
			}
			else{
				HS_HUB_SaveData.HUB_Info.portD[port].Speed = FULL_SPEED;	// FullSpeed device
			}
		}
/* Judge device status below */
	}
	if( (port == 0xff) || (HS_HUB_SaveData.HUB_Info.portD[port].Speed == HIGH_SPEED) ){
		U20HOST_SetSelfAddress( 0x00 );
		len = 8;
		status = U20HOST_GetDeviceDescr( pbuf, &len );
		if( status != USB_INT_SUCCESS ){
			printf("GetDev_ERROR=%02x\n", status );
			return USB_OPERATE_ERROR;
		}
		UsbDevEndp0Size = *(pbuf+7);
		printf("UsbDevEndp0Size=%02x\n",UsbDevEndp0Size);
		mDelaymS(1);
		status = U20HOST_SetAddress( addr );
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
		status=U20HOST_CofDescrAnalyse( pbuf,port );
		if( status != USB_INT_SUCCESS )
		{
			printf( "Current Device is Not USB Mass Storage Device\n" );
			return( USB_OPERATE_ERROR ); 		 																		   // Return directly
		}
		status = U20HOST_MS_CofDescrAnalyse( pbuf ,port);
		status = U20HOST_SetConfig( cfg ); 																				   // USB Host Set Configuration
		if ( status != USB_INT_SUCCESS )
		{
			printf( "SetConfig_ERROR = %02X\n", (UINT16) status );
			return( USB_OPERATE_ERROR ); 		 																		   // Return directly
		}
		if( (HS_HUB_SaveData.Device_Type == HUB_DEVICE) && (HS_HUB_SaveData.Depth == 0)){								   // HUB device
			status = U20HOST_GetHUBDevDescr( pbuf,&len );					// Get HUB descriptor
			printf("GetHUBDevDescr=%02x\n",status);
			HS_HUB_SaveData.HUB_Info.Speed = 1;								// High?speed
			HS_HUB_SaveData.HUB_Info.Numofport = *(pbuf+2);					// Get HUB port count
			for( i=0;i!=5;i++ ){
				printf("%02x ",*(pbuf+i));
			}
			printf("\n");
			status = U20HOST_GetHUBDevStatus( pbuf,&len );
			printf("GetHUBDevStatus=%02x\n",status);
			status = U20HOST_GetHubStatus( pbuf,&len  );
			printf("GetHubStatus=%02x\n",status);
			for( i=0;i!=HS_HUB_SaveData.HUB_Info.Numofport;i++ ){			// Enable port power
				U20HOST_SetHUBPortFeatrue( i+1, 0x08);			
			}
			for( i=0;i!=HS_HUB_SaveData.HUB_Info.Numofport;i++ ){			// Get each port status
				status = U20HOST_GetHubPortStatus( i+1 ,pbuf);			
			}
			HS_HUB_SaveData.Depth++;			// Increase hub depth
		}	
		return status;
	}
	else if( (port != 0xff) ){					// Low / full?speed device processing

	}
	return status;
}

/*********************************************************************
 * @fn      U20HOST_HUBSaveVari
 *
 * @brief   USB2.0 Host Save/Restore hub device parameters.
 *
 * @param   port   - Port number.
 *          index  - 0: Backup parameters, 1: Restore parameters.
 *
 * @return  none
 */
void U20HOST_HUBSaveVari( uint8_t port,uint8_t index  )
{
	uint8_t i;
	if( index == 0 ){			// Restore variables
		gDeviceUsbType = USB_U20_SPEED;
		U20HOST_SetSelfAddress( HS_HUB_SaveData.HUB_Info.portD[port].Addr );		// Perform subsequent initialization
		for( i=0;i != HS_HUB_SaveData.HUB_Info.portD[port].EndpNum;i++ ){
			if( HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num &0x80 ){		// IN endpoint
				gDiskBulkInEp = HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num&0x0f;
				gDiskBulkInTog = HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog;				
			}
			else{																	// OUT endpoint
				gDiskBulkOutEp = HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num;
				gDiskBulkOutTog = HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog;
			}
		}
	}
	else if( index == 1 ){		// Save variables
		for( i=0;i != HS_HUB_SaveData.HUB_Info.portD[port].EndpNum;i++ ){
			if( HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num &0x80 ){		// IN endpoint
				HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog = gDiskBulkInTog ;
			}
			else{																	// OUT endpoint
				HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog = gDiskBulkOutTog ;
			}
		}	
	}
}

/*********************************************************************
 * @fn      U20HOST_HUB_DEV_Enumerate
 *
 * @brief   USB2.0 Host Hub Device Enumeration.
 *
 * @param   port  - Hub port number.
 *
 * @return  Status.
 */
uint8_t U20HOST_HUB_DEV_Enumerate( uint8_t port )
{
	uint8_t status = 0xFF;
	uint8_t buf[ 512 ];
	if( HS_HUB_SaveData.HUB_Info.portD[port].Status == HUB_DEVICE_CONNECT ){		// Device connected, not initialized
		U20HOST_SetSelfAddress( 0x00 );												// Perform subsequent initialization
		status = U20HSOT_Enumerate( buf,HS_HUB_SaveData.HUB_Info.portD[port].Addr,port );
		printf("status=%02x\n",status);
		if( status == ERR_SUCCESS ){
			HS_HUB_SaveData.HUB_Info.portD[port].Status = 	HUB_DEVICE_CONFIG;
			printf("device_type:%02x,%02x\n",HS_HUB_SaveData.HUB_Info.portD[port].DeviceType,gDeviceUsbType);
			if( HS_HUB_SaveData.HUB_Info.portD[port].DeviceType == UDISK_DEVICE ){				// Initialize USB?disk
				U20HOST_HUBSaveVari( port,0 );		// Restore variables
				status = MS_Init( buf );			// Initialize USB mass?storage device
				U20HOST_HUBSaveVari( port,1 );		// Save variables
				FAT_Init(port);
			}
			else if( HS_HUB_SaveData.HUB_Info.portD[port].DeviceType == HID_DEVICE ){			// HID device initialization
				printf("hid_device\n");
			}
		}		
	}
	return status;
}

/*********************************************************************
 * @fn      U20HOST_ConnectDevice
 *
 * @brief   USB2.0 Host Connect Device. Check all hub ports for device connection/disconnection.
 *
 * @return  Status.
 */
uint8_t U20HOST_ConnectDevice( void )
{
	uint8_t i, status;
	uint8_t buf[ 4 ];
	uint16_t Port_Change_Field,Port_Status_Bits;
	U20HOST_SetSelfAddress( HS_HUB_SaveData.HUB_Info.DevAddr );
	for( i=0;i!=HS_HUB_SaveData.HUB_Info.Numofport;i++ )
	{
		status = U20HOST_GetHubPortStatus( i+1,buf );						// GET PORT STATUS
		if( status == USB_INT_SUCCESS ){
			Port_Change_Field = buf[2];
    		Port_Change_Field |= ((uint16_t)buf[3]<<8);
			HS_HUB_SaveData.HUB_Info.portD[i].PortpChangeField = Port_Change_Field;
		}
		else{
			return status;		
		} 
	}
	for( i=0;i!=HS_HUB_SaveData.HUB_Info.Numofport;i++ )
	{
		if( HS_HUB_SaveData.HUB_Info.portD[i].PortpChangeField ){			// DEVICE ATTACH
			U20HOST_CLRPortpChangeField( i );
			Delay_Ms( 5 );		
			status = U20HOST_GetHubPortStatus( i+1,buf );					// GET PORT STATUS, ENSURE DEVICE CONNECT 
			if( status == USB_INT_SUCCESS ){
				Port_Status_Bits = buf[0];
				Port_Status_Bits |= ((uint16_t)buf[1]<<8);
				printf("HS_Port_Status=%02x\n",Port_Status_Bits);
				if( Port_Status_Bits &  PORT_CONNECT_ST )	
				{
					HS_HUB_SaveData.HUB_Info.portD[i].Addr = USB_HSDEVICE_ADDR+1+i;				
					HS_HUB_SaveData.HUB_Info.portD[i].Status = HUB_DEVICE_CONNECT;				// DEVICE CONNECT
					U20HOST_HUB_DEV_Enumerate( i );
				}
				else if( (Port_Status_Bits &  PORT_CONNECT_ST) == 0x00 ){						// DEVICE DISCONNECT
					U20HOST_CLRPortpChangeField( i );
					HS_HUB_SaveData.HUB_Info.portD[i].Status = HUB_DEVICE_DISCONNECT;			// DEVICE DISCONNECT
					HS_HUB_SaveData.HUB_Info.portD[i].Addr = 0;
				}
			}
			else{
				 return status;	
			}	 		
		}
	}
	return status;
}

/*********************************************************************
 * @fn      U20HOST_CheckDeviceStatus
 *
 * @brief   USB2.0 Host Check Device Status on specific hub port.
 *
 * @param   port  - Hub port number.
 *
 * @return  Status.
 */
uint8_t  U20HOST_CheckDeviceStatus( uint8_t port )
{
	uint8_t status,buf[ 4 ];
	uint16_t Port_Change_Field,Port_Status_Bits;
	U20HOST_SetSelfAddress( HS_HUB_SaveData.HUB_Info.DevAddr );	
	status = U20HOST_GetHubPortStatus( port+1,buf );									// GET PORT STATUS
	if( status == USB_INT_SUCCESS ){
		Port_Change_Field = buf[2];
		Port_Change_Field |= ((uint16_t)buf[3]<<8);
		HS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField = Port_Change_Field;
		if( HS_HUB_SaveData.HUB_Info.portD[port].PortpChangeField ){
			U20HOST_CLRPortpChangeField( port );
			Delay_Ms( 5 );		
			status = U20HOST_GetHubPortStatus( port+1,buf );							// GET PORT STATUS, ENSURE DEVICE CONNECT
			if( status == USB_INT_SUCCESS ){
				Port_Status_Bits = buf[0];
				Port_Status_Bits |= ((uint16_t)buf[1]<<8);
				printf("USBHS_Port_Status=%02x\n",Port_Status_Bits);
				if( Port_Status_Bits &  PORT_CONNECT_ST )	
				{
					HS_HUB_SaveData.HUB_Info.portD[port].Addr = USB_HSDEVICE_ADDR+1+port;			
					HS_HUB_SaveData.HUB_Info.portD[port].Status = HUB_DEVICE_CONNECT;				// DEVICE CONNECT
					U20HOST_HUB_DEV_Enumerate( port );
				}
				else if( (Port_Status_Bits &  PORT_CONNECT_ST) == 0x00 ){							// DEVICE DISCONNECT
					HS_HUB_SaveData.HUB_Info.portD[port].Status = HUB_DEVICE_DISCONNECT;			 
					HS_HUB_SaveData.HUB_Info.portD[port].Addr = 0;
				}
			}
			else{
				return status;	
			}	
		}
	}
	else{
		return status;		
	} 
	return status;
}

/*********************************************************************
 * @fn      U20HOST_HUB_DEV_MainProcess
 *
 * @brief   USB2.0 Host Hub Device Main Process. Process UDISK devices on all hub ports.
 *
 * @return  none
 */
void U20HOST_HUB_DEV_MainProcess( void )
{
	uint8_t port, s, i;
	uint8_t *pd = DATA_BUFFER;
	for( port=0; port!= HS_HUB_SaveData.HUB_Info.Numofport; port++){
		if( ( HS_HUB_SaveData.HUB_Info.portD[port].Status ==HUB_DEVICE_CONFIG ) && 			// Initialization Successful
			( HS_HUB_SaveData.HUB_Info.portD[port].DeviceType == UDISK_DEVICE ) ){			// The Device is a USB Flash Drive.
			printf("USB2.0_FAT_PROCESS\n");
			strcpy( (char *)&mCmdParam.Create.mPathName,( const char * )"\\NEWFILE.TXT");  	// create NEWFILE.TXT
			i = CHRV3FileOpen();
			printf("i=%x\n",i);
			if(i == ERR_FAT_ERROR){ 
				printf("File System Not Supported! Require FAT12/FAT16/FAT32\n");
			}else{
				if((i == ERR_MISS_DIR)||(i == ERR_MISS_FILE))
				{
					printf( "Find No File And Create\r\n" );
					i = CHRV3FileCreate();
				}
				mCmdParam.Close.mUpdateLen=1;
				i = CHRV3FileClose();                                   				    // Modify file information completed

				printf("Write DATA\n");
				strcpy( (char *)&mCmdParam.Open.mPathName,( const char * )"\\NEWFILE.TXT");
				i = CHRV3FileOpen();
				memset(FAT_BUFFER, 0x43, 32);
				mCmdParam.Write.mDataBuffer = FAT_BUFFER;
				mCmdParam.Write.mSectorCount = 16;
				i=CHRV3FileWrite( );                               						   // Write data to NEWFILE.txt
				for(s=0;s!=5;s++)  printf("%02x ",FAT_BUFFER[s]);
				printf("\n");
				mCmdParam.Close.mUpdateLen = 1;
				i= CHRV3FileClose();                                 					   // Write to NEWFILE.txt completed

				printf("Read DATA\n");
				strcpy( (char *)&mCmdParam.Open.mPathName,( const char * )"\\NEWFILE.TXT");    // Open File
				i = CHRV3FileOpen();
				mCmdParam.Read.mDataBuffer = pd;
				mCmdParam.Read.mSectorCount = 16;
				i=CHRV3FileRead( );
				for( s=0; s != 5; s++ )	printf("%02x ",*(pd+s));							   // Read File DATA
				printf("\n");				
				if( mCmdParam.Read.mSectorCount < 16 )
				{
					printf("Err Number of Sectors Read Only: %d\n",mCmdParam.Read.mSectorCount);
				}
				mCmdParam.Close.mUpdateLen = 1;
				i= CHRV3FileClose();
			}
			H417_SaveVariable( port );	
			HS_HUB_SaveData.HUB_Info.portD[port].Status = HUB_DEVICE_CONFIG_SUCESS;			   // Save DATA	
		}			
	}
}

/*********************************************************************
 * @fn      FAT_Init
 *
 * @brief   Initialize FAT file system for specified USB port.
 *
 * @param	port: USB HUB port index (0 ~ 3)
 *
 * @return  0x01: File system initialization completed
 */
uint8_t FAT_Init( uint8_t port )
{
	uint8_t i;
	uint32_t temp32;

    H417_RSaveVariable( port );          // Restore disk related variables
	i = CHRV3LibInit();
	printf("LIB_Init=%02x\n",i);

    CHRV3vSectorSize = gDiskPerSecSize;
	CHRV3DiskStatus = DISK_MOUNTED;		 // Disk initialized successfully, filesystem not parsed or unsupported
	CHRV3vSectorSizeB = 0;

	temp32 = CHRV3vSectorSize;
	for( i = 0; i <= 12; i++ )
	{
		temp32 = temp32 >> 1;
		CHRV3vSectorSizeB++;
		if( ( temp32 & 0x01 )== 0x01 )
		{
			break;
		}
	}

	/* Set USB transfer packet size according to device speed */
	if( gDeviceUsbType == USB_U30_SPEED ){
		CHRV3vPacketSize = 1024;
	}
	else{
		CHRV3vPacketSize = 512;
	}

	CHRV3vSectorSize = gDiskPerSecSize;  // Physical disk sector size, normally 512 Bytes
    H417_SaveVariable( port );           // Save current disk related variables

#if 0
	/* Debug test code: open file and continuous read test */
	strcpy( (char *)&mCmdParam.Open.mPathName,( const char * )"\\123.C" );
	i = CHRV3FileOpen();
	printf("Open=%02x\n",i);
	printf("CHRV3vFileSize=%d,%d\n",CHRV3vFileSize,CHRV3vSectorSize);
    CHRV3vFileSize+=CHRV3vSectorSize-1;
	while(1){
	    mCmdParam.Read.mDataBuffer=pd;
		mCmdParam.Read.mSectorCount=16;
		i=CHRV3FileRead( );	
		for(s=0;s!=5;s++)printf("%02x ",*(pd+s));
		printf("\n");
		if( i!=0 )printf("error=%02x\n",i);
        if( mCmdParam.Read.mSectorCount<16 )break;
	}
	printf("read_end\n");
    H417_SaveVariable( port );           // Save variables after test reading
#endif
	return 0x01;
}

/* Each port has a 100-byte buffer set up to store disk information. */
uint8_t  Disk_Infor0[100];		   /* Buffer for port 0, store disk information (100 bytes) */
uint8_t  Disk_Infor1[100];		   /* Buffer for port 1, store disk information (100 bytes) */
uint8_t  Disk_Infor2[100];		   /* Buffer for port 2, store disk information (100 bytes) */
uint8_t  Disk_Infor3[100];		   /* Buffer for port 3, store disk information (100 bytes) */

/*********************************************************************
 * @fn      H417_RSaveVariable
 *
 * @brief   Restore disk variable parameters from corresponding port buffer
 *
 * @param   port: USB HUB port index, range 0~3
 *
 * @return  none
 */
void H417_RSaveVariable( uint8_t port )
{
    /* Assign corresponding disk info buffer according to port number */
	if( port==0 ){mCmdParam.SaveVariable.mBuffer = Disk_Infor0;}
	else if( port==1 ){ mCmdParam.SaveVariable.mBuffer = Disk_Infor1; }
	else if( port==2 ){ mCmdParam.SaveVariable.mBuffer = Disk_Infor2; }
	else if( port==3 ){ mCmdParam.SaveVariable.mBuffer = Disk_Infor3; }

	mCmdParam.SaveVariable.mSaveVariable = 0x80;							/* Variable operation mode: 0x80 = restore variables */
	CHRV3SaveVariable( );                               					/* Execute variable restore operation for CHRV3 filesystem */
	CHRV3DirtyBuffer( );                                					/* Clear dirty disk buffer to avoid old data interference */

	/* Check SS HUB: device configured successfully and device type is USB flash disk */
	if( ( SS_HUB_SaveData.HUB_Info.portD[port].Status == HUB_DEVICE_CONFIG ) &&
	    ( SS_HUB_SaveData.HUB_Info.portD[port].DeviceType == UDISK_DEVICE ) ){
			U30HOST_HUBSaveVari( port,0 );									/* Param 0: Restore USB host variables for U-disk */
	}

	/* Check HS HUB: device configured successfully and device type is USB flash disk */
	if( ( HS_HUB_SaveData.HUB_Info.portD[port].Status == HUB_DEVICE_CONFIG ) &&
		( HS_HUB_SaveData.HUB_Info.portD[port].DeviceType == UDISK_DEVICE ) ){
			U20HOST_HUBSaveVari( port,0 );									/* Param 0: Restore USB host variables for U-disk */
	}
}

/*********************************************************************
 * @fn      H417_SaveVariable
 *
 * @brief   Save current disk variable parameters into corresponding port buffer
 *
 * @param   port: USB HUB port index, range 0~3
 *
 * @return  none
 */
void H417_SaveVariable( uint8_t port )
{
    /* Assign corresponding disk info buffer according to port number */
	if( port==0 ){mCmdParam.SaveVariable.mBuffer = Disk_Infor0;}
	else if( port==1 ){ mCmdParam.SaveVariable.mBuffer = Disk_Infor1; }
	else if( port==2 ){ mCmdParam.SaveVariable.mBuffer = Disk_Infor2; }
	else if( port==3 ){ mCmdParam.SaveVariable.mBuffer = Disk_Infor3; }

	mCmdParam.SaveVariable.mSaveVariable = 0x01;							/* Variable operation mode: 0x01 = save variables */
	CHRV3SaveVariable( );                               					/* Execute variable save operation for CHRV3 filesystem */

	/* Check SS HUB: device configured successfully and device type is USB flash disk */
	if( ( SS_HUB_SaveData.HUB_Info.portD[port].Status == HUB_DEVICE_CONFIG ) &&
	    ( SS_HUB_SaveData.HUB_Info.portD[port].DeviceType == UDISK_DEVICE ) ){
			U30HOST_HUBSaveVari( port,1 );									/* Param 1: Save USB host variables for U-disk */
	}

	/* Check HS HUB: device configured successfully and device type is USB flash disk */
	if( ( HS_HUB_SaveData.HUB_Info.portD[port].Status == HUB_DEVICE_CONFIG ) &&
		( HS_HUB_SaveData.HUB_Info.portD[port].DeviceType == UDISK_DEVICE ) ){
			U20HOST_HUBSaveVari( port,1 );									/* Param 1: Save USB host variables for U-disk */
	}
}