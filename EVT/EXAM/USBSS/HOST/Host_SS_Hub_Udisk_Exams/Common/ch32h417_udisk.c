/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_udisk.c
* Author             : WCH
* Version            : V1.0
* Date               : 2025/05/30
* Description        : This file provides the function of the host operating the 
*					   udisk.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "string.h"
#include "stdio.h"
#include "ch32h417_usbss_host.h"
#include "ch32h417_usbhs_host.h"
#include "ch32h417_enum.h"
#include "ch32h417_udisk.h"
#include "hub.h"
//#define		MY_DEBUG_PRINTF			1
#define	U30_MAX_PACKSIZE			1024
__attribute__ ( ( aligned( 16 ) ) ) BULK_ONLY_CMD	mBOC;						/* CBW package, 16-byte aligned */

/* Constants, variable definitions */
uint8_t  gDiskMaxLun;				    										/* The maximum logical unit number of the disk */
uint8_t  gDiskCurLun;	    													/* The current operation logic unit number of the disk */
UINT32 gDiskCapability;		    												/* Total disk capacity */
UINT32 gDiskPerSecSize;	    													/* Disk sector size */
uint8_t  gDiskBulkInEp;															/* The IN endpoint address of the USB large-capacity storage device */
uint8_t  gDiskBulkInTog;														/* USB high-capacity transmission synchronization logo */
uint8_t  gDiskBulkInBrust;														/* IN brust quantity */
uint8_t  gDiskBulkOutEp;														/* The OUT endpoint address of the USB large-capacity storage device */
uint8_t  gDiskBulkOutTog;														/* USB high-capacity transmission synchronization logo */
uint8_t  gDiskBulkOutBrust;														/* OUT burst quantity */
uint16_t gDiskBulkInEpSize;  	    											/* The maximum packet size of the IN endpoint of a USB large-capacity storage device */
uint16_t gDiskBulkOutEpSize;  													/* The maximum packet size of the OUT endpoint of a USB large-capacity storage device */
uint8_t  gDiskInterfNumber;														/* The port number of the USB large-capacity storage device */
uint16_t U20_ENDP_SIZE;
uint8_t  test_flag;
uint8_t MS_Init_Hotrst(  UINT8 *pbuf );
uint8_t disk_max_lun;

/*********************************************************************
 * @fn      MS_GetMaxLun
 *
 * @brief   The USB host acquires the maximum logical unit number
 *
 * @return  Current  Status 
 */
uint8_t MS_GetMaxLun( void )
{
	uint8_t  status;
	uint8_t  buf[ 1 ];
	uint8_t  Setup_buf[8];

	Setup_buf[0] = 0xA1;
	Setup_buf[1] = 0xFE;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = 0x00;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x01;
	Setup_buf[7] = 0x00;

	/* Obtain the maximum logical unit number of the current USB flash drive */
	gDiskMaxLun = 0x00;
	if( gDeviceUsbType == USB_U30_SPEED ){
		status = U30HostCtrlTransfer(Setup_buf, buf, NULL );				/* Execution control transmission */
	}
	else{
		status = U20HostCtrlTransfer(Setup_buf, buf, NULL );				/* Execution control transmission */
	}
	if( status == USB_INT_SUCCESS )
	{
		gDiskMaxLun = buf[ 0 ];												/* Save the maximum logical unit number of the current USB flash drive */
		disk_max_lun = gDiskMaxLun;
	}
	else if( status == USB_INT_DISK_ERR )
	{
		if( gDeviceUsbType == USB_U30_SPEED ){
			status = U30HOST_ClearEndpStall(  0x00 );						/* Clear endpoint 0 */
		}
		else{
			status = U20HOST_ClearEndpStall(  0x00 );						/* Clear endpoint 0 */
		}
	}
	if( status == USB_INT_SUCCESS )return 0x00;

	return status;
}

/*********************************************************************
 * @fn      MS_ResetErrorBOC
 *
 * @brief   USB host reset USB flash drive 
 *
 * @return  Current  Status 
 */
uint8_t MS_ResetErrorBOC( void )
{
	uint8_t status;
	uint8_t Setup_buf[8];

	Setup_buf[0] = 0x21;
	Setup_buf[1] = 0xFF;
	Setup_buf[2] = 0x00;
	Setup_buf[3] = 0x00;
	Setup_buf[4] = gDiskInterfNumber;
	Setup_buf[5] = 0x00;
	Setup_buf[6] = 0x00;
	Setup_buf[7] = 0x00;

	/* USB host reset USB flash drive */
	if( gDeviceUsbType == USB_U30_SPEED ){
		status = U30HostCtrlTransfer(  Setup_buf, NULL, NULL  );	    	/* Execution control transmission */
	}
	else{
		status = U20HostCtrlTransfer(  Setup_buf, NULL, NULL  );	    	/* Execution control transmission */
	}
	if( status == USB_INT_SUCCESS )
	{
		/* Clear the batch upload and download endpoints */
		if( gDeviceUsbType == USB_U30_SPEED ){
			status = U30HOST_ClearEndpStall( 0x80 | gDiskBulkInEp );		/* Clear the batch upload endpoint */
		}
		else{
			status = U20HOST_ClearEndpStall( 0x80 | gDiskBulkInEp );		/* Clear the batch upload endpoint */
		}
		if( status != USB_INT_SUCCESS )
		{
			return( status );
		}
		if( gDeviceUsbType == USB_U30_SPEED ){
			status = U30HOST_ClearEndpStall( gDiskBulkOutEp );				/* Clear the batch download endpoint */
		}
		else{
			status = U20HOST_ClearEndpStall( gDiskBulkOutEp );				/* Clear the batch download endpoint */
		}
	}

	return( status );
}

/*********************************************************************
 * @fn      U30HOST_MS_CofDescrAnalyse
 *
 * @brief   USB3.0 host analyzes the Mass Storage Device configuration descriptor
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status 
 */
uint8_t U30HOST_MS_CofDescrAnalyse( uint8_t *pbuf,uint8_t port )
{
	uint16_t i;

	gDiskInterfNumber = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> itf_descr.bInterfaceNumber;  		/* Save the Interface number */
	gDiskBulkInEp  = 0;
	gDiskBulkOutEp = 0;
	if( (SS_HUB_SaveData.Device_Type == HUB_DEVICE) && (SS_HUB_SaveData.Depth == 0)){			/* Level 1 Hub */
		if( ( ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ 0 ].bLength == 0x07)
			&&( ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ 0 ].bDescriptorType == 0x05)
			&& ( (((PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ 0 ].bmAttributes&0x0f) == 0x03) ){			/* The interrupt endpoint of the HUB */
				SS_HUB_SaveData.HUB_Info.IntEndpnum = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ 0 ].bEndpointAddress & 0x0F;			/* Interrupt endpoint number */
				return( USB_OPERATE_SUCCESS );
			}
	}
	else{
		SS_HUB_SaveData.HUB_Info.portD[port].EndpNum = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> itf_descr.bNumEndpoints;
		printf("num=%02x\n",SS_HUB_SaveData.HUB_Info.portD[port].EndpNum);
		for( i = 0; i < SS_HUB_SaveData.HUB_Info.portD[port].EndpNum; i ++ )   																/* Only analyze the first two endpoints */
		{
			if( ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bLength == 0x07
				&& ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bDescriptorType == 0x05
				&& ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bmAttributes == 2 )
			{
				if( ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x80 )
				{
					/* IN */
					gDiskBulkInEp = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;   /* IN */
					gDiskBulkInEpSize = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].wMaxPacketSizeH;
					gDiskBulkInEpSize = gDiskBulkInEpSize << 8;
					gDiskBulkInEpSize += ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].wMaxPacketSizeL;
					gDiskBulkInBrust = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bMaxBurst1+1;
					gDiskBulkInTog = 0;		
					SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Endp_Size = gDiskBulkInEpSize;	
					SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].EndpType = ((PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bmAttributes;
					SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bEndpointAddress;
					SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog = 0;
					SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Brust = gDiskBulkInBrust;
					printf("in_para:%02x,%02x,%02x,%02x,%02x,%02x\n",i,
									SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Endp_Size,
									SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].EndpType,
									SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num,
									SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog,
									SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Brust
					);
				}
				else
				{
					/* OUT */
					gDiskBulkOutEp = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;  /* OUT */
					gDiskBulkOutEpSize = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].wMaxPacketSizeH;
					gDiskBulkOutEpSize = gDiskBulkOutEpSize << 8;
					gDiskBulkOutEpSize += ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].wMaxPacketSizeL;
					gDiskBulkOutBrust = ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bMaxBurst1+1;
					gDiskBulkOutTog = 0;	
					SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Endp_Size = gDiskBulkOutEpSize;	
					SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].EndpType = ((PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bmAttributes;
					SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num =  ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> endp_descr[ i ].bEndpointAddress;
					SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog = 0;
					SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Brust = gDiskBulkOutBrust;
										printf("out_para:%02x,%02x,%02x,%02x,%02x,%02x\n",i,
									SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Endp_Size,
									SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].EndpType,
									SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num,
									SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog,
									SS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Brust
					);
				}
			}
		}
	}
	if( ( (PUSB_CFG_DESCR_LONG_U30)pbuf ) -> itf_descr.bInterfaceClass != 0x08 || gDiskBulkInEp == 0 || gDiskBulkOutEp == 0 )
	{  															 				
		return( USB_OPERATE_ERROR ); 	  									    							    /* Terminate the operation and return directly */
	}
#if 1
//#ifdef  MY_DEBUG_PRINTF
	printf("gDiskBulkInEp:%02x\n ",gDiskBulkInEp);
	printf("gDiskBulkOutEp:%02x\n ",gDiskBulkOutEp);
	printf("gDiskBulkInBrust:%02x\n ",gDiskBulkInBrust);
	printf("gDiskBulkOutBrust:%02x\n ",gDiskBulkOutBrust);
	printf("gDiskBulkInEpSize:%02x\n ",gDiskBulkInEpSize);
	printf("gDiskBulkOutEpSize:%02x\n ",gDiskBulkOutEpSize);	

#endif
	gDiskCurLun = 0x00;														
	return( USB_OPERATE_SUCCESS );
}

/*********************************************************************
 * @fn      U20HOST_MS_CofDescrAnalyse
 *
 * @brief   USB2.0 host analyzes the Mass Storage Device configuration descriptor
 *
 * @param   pbuf  - Data buffer
 *
 * @return  Status 
 */
uint8_t U20HOST_MS_CofDescrAnalyse( uint8_t *pbuf ,uint8_t port)
{
	uint16_t i;
	gDiskInterfNumber = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceNumber;  	/* Save the Interface number */
	gDiskBulkInEp  = 0;
	gDiskBulkOutEp = 0;
	if( (HS_HUB_SaveData.Device_Type == HUB_DEVICE) && (HS_HUB_SaveData.Depth == 0)){					/* Level 1 Hub */
		printf("Endp_descr=%02x,%02x,%02x\n",( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ 0 ].bLength,
										( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ 0 ].bDescriptorType,
										(((PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ 0 ].bmAttributes&0x0f)
		);
		if( ( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ 0 ].bLength == 0x07)
			&&( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ 0 ].bDescriptorType == 0x05)
			&& ( (((PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ 0 ].bmAttributes&0x0f) == 0x03) ){		/* The interrupt endpoint of the HUB */
			HS_HUB_SaveData.HUB_Info.IntEndpnum = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ 0 ].bEndpointAddress & 0x0F;		/* Interrupt endpoint number */
			printf("HS_HUB_SaveData.HUB_Info.IntEndpnum=%02x\n",HS_HUB_SaveData.HUB_Info.IntEndpnum);
			return( USB_OPERATE_SUCCESS );
		}
	}
	else{
		HS_HUB_SaveData.HUB_Info.portD[port].EndpNum = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bNumEndpoints;
		printf("num=%02x\n",HS_HUB_SaveData.HUB_Info.portD[port].EndpNum);
		for( i = 0; i < HS_HUB_SaveData.HUB_Info.portD[port].EndpNum; i ++ )   													/* Only analyze the first two endpoints */
		{
			if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bLength == 0x07
				&& ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bDescriptorType == 0x05
				&& ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bmAttributes == 2 )
			{
				if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x80 )
				{
					/* IN */
					gDiskBulkInEp = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;   /* IN */
					gDiskBulkInEpSize = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeH;
					gDiskBulkInEpSize = gDiskBulkInEpSize << 8;
					gDiskBulkInEpSize += ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeL;
					gDiskBulkInTog = 0;	
					HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Endp_Size = gDiskBulkInEpSize;	
					HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].EndpType = ((PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bmAttributes;
					HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress;
					HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog = 0;
					printf("hs_in_para:%02x,%02x,%02x,%02x,%02x\n",i,
								HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Endp_Size,
								HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].EndpType,
								HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num,
								HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog
					);	
				}
				else
				{
					gDiskBulkOutEp = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;  /* OUT */
					gDiskBulkOutEpSize = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeH;
					gDiskBulkOutEpSize = gDiskBulkOutEpSize << 8;
					gDiskBulkOutEpSize += ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeL;
					gDiskBulkOutTog = 0;	
					HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Endp_Size = gDiskBulkOutEpSize;	
					HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].EndpType = ((PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bmAttributes;
					HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num =  ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress;
					HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog = 0;
					printf("hs_out_para:%02x,%02x,%02x,%02x,%02x\n",i,
									HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Endp_Size,
									HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].EndpType,
									HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].Num,
									HS_HUB_SaveData.HUB_Info.portD[port].portEndp[i].tog
					);
				}
			}
		}
	}
	if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass != 0x08 || gDiskBulkInEp == 0 || gDiskBulkOutEp == 0 )
	{  															 				
		return( USB_OPERATE_ERROR ); 	  									   
	}

#ifdef  MY_DEBUG_PRINTF
	printf("gDiskBulkInEp:%02x\n ",gDiskBulkInEp);
	printf("gDiskBulkOutEp:%02x\n ",gDiskBulkOutEp);
	printf("gDiskBulkInEpSize:%02x\n ",gDiskBulkInEpSize);
	printf("gDiskBulkOutEpSize:%02x\n ",gDiskBulkOutEpSize);
#endif

	U20_ENDP_SIZE = gDiskBulkOutEpSize;
	gDiskCurLun = 0x00;															
	return( USB_OPERATE_SUCCESS );																			/* Terminate the operation and return directly */
}

/*********************************************************************
 * @fn      U30HOST_Issue_Bulk
 *
 * @brief   The USB host performs batch transfer
 *
 * @param   EndpNum  - Endpoint number
 *			*SeqNum	 - Package number, 0-31
 *			PacketNum  - Packets of data are being transmitted
 *			*pDatBuf  - Data buffer	
 *			*pSize  - Data size
 *			Type  - Transmission type (IN/OUT)
 *			BrustNum  - Number of bursts ranges from 1 to 16
 *
 * @return  Status 
 */
#define  TIME_OUT_VALUE         1000
uint8_t U30HOST_Issue_Bulk( uint8_t EndpNum, uint8_t *SeqNum, uint8_t PacketNum, uint8_t *pDatBuf, UINT32 *pSize, uint8_t Type, uint8_t BrustNum )
{
	uint16_t temp;
	uint32_t  timeout = 0;
	BrustNum = 1;
	*SeqNum &= 0x1F;

	if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
	if( Type == U30_PID_OUT ){																			/* OUT data */
		USBSSH->UH_TX_DMA = (UINT32)pDatBuf; 															/* Set tx dma address */
		USBSSH->HOST_TX_NUMP = PacketNum;		
		
		USBSSH->UH_TX_CTRL =  *SeqNum << 16  | BrustNum<<24 | *pSize | EndpNum <<12;
		USBSSH->UH_TX_CTRL |= UH_RTX_VALID ;
		while(1){
			timeout++;
			if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
			if( USBSSH->UH_TX_CTRL & UH_INT_FLAG ) 														/* OUT Over */
			{
				USBSSH->UH_TX_CTRL &= ~UH_INT_FLAG;
				if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_ACK )								/* ACK-OK */
				{
					break;
				}
				else if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_NRDY )						/* wait for the device ACK or E-RDY */
				{

				}
				else if((USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_STALL )						/* Return directly */
				{
					return USB_INT_DISK_ERR;
				}
				else if( (USBSSH->USB_STATUS & USB_TX_RES_MASK) == TX_RES_FAILED )
				{
					USBSSH->UH_TX_CTRL |= UH_RTX_VALID;
				}
				else
				{
					USBSSH->USB_STATUS = USB_ACT_FLAG;
				}
			}
			else if( USBSSH->USB_STATUS & USB_ERDY_FLAG )
			{
				if( !(USBSSH->UH_TX_CTRL & UH_RTX_VALID )){		
					USBSSH->UH_TX_CTRL |=  UH_RTX_VALID;
				}
				USBSSH->USB_STATUS = USB_ERDY_FLAG;
			}
			if( timeout >= USB_CH417USBTIMEOUTVALUE ){
				printf("out_time\n");
				return USB_CH417USBTIMEOUT;
			}	
		}
	}
	else if( Type == U30_PID_IN ){																		/* Input data */
		if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
		USBSSH->UH_RX_DMA = (UINT32)pDatBuf;
		USBSSH->HOST_RX_NUMP = PacketNum;

		BrustNum = 1;
		USBSSH->UH_RX_CTRL = *SeqNum << 16  | BrustNum<<24 | EndpNum <<12; 								/* ACK-TP(IN) */
		USBSSH->UH_RX_CTRL |= UH_RTX_VALID;

		while(1){
			timeout++;
			if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
			if((USBSSH->UH_RX_CTRL & UH_INT_FLAG) || ((USBSSH->HOST_RX_NUMP & 0xFF) == 0))
			{
				USBSSH->UH_RX_CTRL &= ~UH_INT_FLAG;
				if( (USBSSH->USB_STATUS & USB_RX_RES_MASK) == RX_RES_ACK )
				{					
					if(USBSSH->HOST_RX_NUMP & 0xff ){													/* If the data is not fully retrieved, it needs to be retrieved again */	
						USBSSH->UH_RX_CTRL |= UH_RTX_VALID;				
					}
					else{
				 		temp = USBSSH->UH_RX_CTRL & 0x7ff;
						temp +=((PacketNum-1)*1024);													/* Output data */		
						*pSize = temp;
						break;
					}
				}
				else if((USBSSH->USB_STATUS & USB_RX_RES_MASK) == RX_RES_NRDY )
				{

				}
				else
				{
					return USB_INT_DISK_ERR;
				}
			}
			else if( USBSSH->USB_STATUS & USB_ERDY_FLAG )
			{	
				USBSSH->UH_RX_CTRL |= UH_RTX_VALID;	
				USBSSH->USB_STATUS = USB_ERDY_FLAG;
			}
			if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
			if( timeout >= USB_CH417USBTIMEOUTVALUE ){
				printf("in_time\n");
				return USB_CH417USBTIMEOUT;
			}	
		}
	}
	return USB_INT_SUCCESS;
}

/*********************************************************************
 * @fn      U30HOST_Issue_BulkIn
 *
 * @brief   The USB host performs bulk transfer
 *
 * @param   *pDatBuf  - Data buffer
 *			*pSize  - Data size
 *
 * @return  Status
 */
uint8_t U30HOST_Issue_BulkIn( uint8_t *pDatBuf, UINT32 *pSize )
{
	uint8_t status;
	UINT32 len, PacketNum;

	if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
	len = *pSize;

	PacketNum = (len+U30_MAX_PACKSIZE-1)/U30_MAX_PACKSIZE;

#ifdef  MY_DEBUG_PRINTF
	printf("in_data=%02x,%02x,%08lx,%02x\n",gDiskBulkInEp,gDiskBulkInTog,pDatBuf,len);
#endif
	status = U30HOST_Issue_Bulk( gDiskBulkInEp,&gDiskBulkInTog,PacketNum,pDatBuf,&len,U30_PID_IN,gDiskBulkInBrust);

#ifdef  MY_DEBUG_PRINTF
	printf("status=%02x,%d\n",status,len);
#endif
	if( status != USB_INT_SUCCESS )return status;		
	gDiskBulkInTog+=PacketNum;
	*pSize = len;

	if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
	return USB_INT_SUCCESS;
}

/*********************************************************************
 * @fn      U30HOST_Issue_BulkOut
 *
 * @brief   The USB host performs bulk transfer
 *
 * @param   *pDatBuf  - Data buffer
 *			*pSize  - Data size
 *
 * @return  Status
 */
uint8_t U30HOST_Issue_BulkOut( uint8_t *pDatBuf, UINT32 *pSize )
{
	uint8_t status;
	UINT32 len, PacketNum;

	PacketNum = (*pSize+U30_MAX_PACKSIZE-1) / U30_MAX_PACKSIZE;
	if( *pSize >= U30_MAX_PACKSIZE ){
		len = U30_MAX_PACKSIZE;
		*pSize -= U30_MAX_PACKSIZE;
	}
	else{
		len = *pSize;
	}
	while(1){
		if( gDeviceConnectstatus == USB_INT_DISCONNECT ) return USB_INT_DISCONNECT;
		status = U30HOST_Issue_Bulk( gDiskBulkOutEp, &gDiskBulkOutTog, 1, pDatBuf, &len, U30_PID_OUT, gDiskBulkOutBrust );	
		if( status != USB_INT_SUCCESS ){
			return status;		
		}	
		gDiskBulkOutTog++;
		PacketNum--;
		pDatBuf+=U30_MAX_PACKSIZE;
		if( PacketNum == 0 )break;
		if( *pSize >= U30_MAX_PACKSIZE ){
			len = U30_MAX_PACKSIZE;
			*pSize -= U30_MAX_PACKSIZE;
		}
		else{
			len = *pSize;
		}
	}
	return USB_INT_SUCCESS;
}

/*********************************************************************
 * @fn      MS_ScsiCmd_Process
 *
 * @brief   Execute command processing based on the BulkOnly protocol
 *			Since up to 20K of data can be transmitted at a time, no 
 *			cyclic processing was carried out
 *
 * @param   *DataBuf  - Data buffer
 *
 * @return  Status
 */
uint8_t MS_ScsiCmd_Process( uint8_t *DataBuf )
{
	UINT8  status;
	UINT8  *p,s;
	UINT32 len;
	p = DataBuf;
	mBOC.mCBW.mCBW_Sig = USB_BO_CBW_SIG;
	mBOC.mCBW.mCBW_Tag = 0x05630563;
	mBOC.mCBW.mCBW_LUN = gDiskCurLun;			   							   
	len = USB_BO_CBW_SIZE;
	if( gDeviceUsbType == USB_U30_SPEED ){
		status = U30HOST_Issue_BulkOut( (UINT8 *)&mBOC.mCBW, &len );
	}
	else{
		status = U20HOST_Issue_BulkOut( (UINT8 *)&mBOC.mCBW, &len );
	}

#ifdef  MY_DEBUG_PRINTF
	printf("CBW_status=%02x\n",status);
#endif

	if( status == USB_INT_DISCONNECT )											/* If the device is disconnected, return directly */
	{
		return( status );
	}
	if( status != USB_INT_SUCCESS )
	{
        if( gDeviceUsbType == USB_U30_SPEED ){                  				/* In the case of modifying USB3.0, a timeout will be returned directly */
            if( status!= USB_INT_SUCCESS )return status;                        /* Return directly */
        }
		if( status == USB_CH417USBTIMEOUT )return USB_CH417USBTIMEOUT;
		status = MS_ResetErrorBOC( );
		if( gDeviceUsbType == USB_U30_SPEED ){                  				/* In the case of modifying USB3.0, a timeout will be returned directly */
		    if(gDeviceConnectstatus == USB_INT_DISCONNECT)return USB_INT_DISCONNECT;                           
		}
		if( status == USB_INT_DISCONNECT )										/* Return directly */
		{
			return( status );
		}
		else if( status != 0x14 )return ( status );
		/* Send the CBW packet again */
		len = USB_BO_CBW_SIZE;
		if( gDeviceUsbType == USB_U30_SPEED ){
			status = U30HOST_Issue_BulkOut( (UINT8 *)&mBOC.mCBW, &len );
		}
		else{
			status = U20HOST_Issue_BulkOut( (UINT8 *)&mBOC.mCBW, &len );
		}
		if( status == USB_INT_DISCONNECT )										/* Return directly */
		{
			return( status );
		}
		if( status != USB_INT_SUCCESS )
		{
	        if( gDeviceUsbType == USB_U30_SPEED ){
	            if( status!= USB_INT_SUCCESS )return status;                            
	        }
			s = MS_ResetErrorBOC( );
	        printf("boc_status1=%02x\n",status);
			return( s );
		}
	}
	if( ( mBOC.mCBW.mCBW_DataLen > 0 ) && ( mBOC.mCBW.mCBW_Flag == USB_BO_DATA_IN ) )
	{
		/* If there is data to be uploaded, send an IN token for data reading */
		/* Send batch upload IN */
		len = mBOC.mCBW.mCBW_DataLen;
		if( gDeviceUsbType == USB_U30_SPEED ){
			status = U30HOST_Issue_BulkIn( p, &len );
		}
		else{
			status = U20HOST_Issue_BulkIn( p, &len );
		}
#ifdef  MY_DEBUG_PRINTF
printf("status2=%02x\n",status);
#endif
		if( status == USB_INT_DISCONNECT )										/* Return directly */
		{
			return( status );
		}
		if( status != USB_INT_SUCCESS )
		{
			if( status == USB_INT_DISK_ERR )									/* Return the STALL Error */
			{
                if( gDeviceUsbType == USB_U30_SPEED ){
                    status = U30HOST_ClearEndpStall( 0x80 | gDiskBulkInEp );	/* Clear the Up Endpoint */
                }
                else{
                    status = U20HOST_ClearEndpStall( 0x80 | gDiskBulkInEp );	/* Clear the Up Endpoint */
                }
                if( status == USB_INT_DISCONNECT )								/* Return directly */
                {
                    return( status );
                }
                else if( status == 0x14 ){
                    gDiskBulkInTog = 0;
                }
                else{
                    gDiskBulkInTog = 0;
                }
			}
			else return status;
		}
	}
	else if( ( mBOC.mCBW.mCBW_DataLen > 0 ) && ( mBOC.mCBW.mCBW_Flag == USB_BO_DATA_OUT ) )
	{
#ifdef  MY_DEBUG_PRINTF
		printf("Out\n");
#endif
		/* Send batch downhaul OUT */
		len = mBOC.mCBW.mCBW_DataLen;
		if( gDeviceUsbType == USB_U30_SPEED ){
			status = U30HOST_Issue_BulkOut( p, &len );
		}
		else{
			status = U20HOST_Issue_BulkOut( p, &len );
		}
		if( status == USB_INT_DISCONNECT )									/* Return directly */
		{
			return( status );
		}
		if( status != USB_INT_SUCCESS )
		{
			if( status == USB_INT_DISK_ERR )								/* Return the STALL error */
			{
				if( gDeviceUsbType == USB_U30_SPEED ){
					status = U30HOST_ClearEndpStall( gDiskBulkOutEp );		/* Clear the Down Endpoint */
				}
				else{
					status = U20HOST_ClearEndpStall( gDiskBulkOutEp );		/* Clear the Down Endpoint */
				}
				if( status == USB_INT_DISCONNECT )							/* Return directly */
				{
					return( status );
				}
			}
		}
	}
#ifdef  MY_DEBUG_PRINTF

#endif
	len = USB_BO_CSW_SIZE;

	if( gDeviceUsbType == USB_U30_SPEED ){
	    status = U30HOST_Issue_BulkIn( (UINT8 *)&mBOC.mCSW, &len );
	}
	else{
		status = U20HOST_Issue_BulkIn( (UINT8 *)&mBOC.mCSW, &len );
	}

	p = (UINT8 *)&mBOC.mCSW;
#ifdef  MY_DEBUG_PRINTF
	for( i=0;i!=len;i++ ){
		printf("%02x ",*p++);
	}
	printf("\n");
	printf("len=%d\n",len);
#endif
	if( status == USB_INT_SUCCESS )
	{
		if( len != USB_BO_CSW_SIZE )											/* Determine whether the length is 13 bytes */
//		if( ( len != 0x00 ) || ( mBOC.mCSW.mCSW_Sig != USB_BO_CSW_SIG ) )		/* Some USB flash drives may not be processed according to this mark */
		{
			return( USB_INT_DISK_ERR );
		}
		if( mBOC.mCSW.mCSW_Status == 0 )
		{
			return( USB_OPERATE_SUCCESS );
		}
		else if( mBOC.mCSW.mCSW_Status >= 2 )
		{
			return( USB_INT_DISK_ERR );
		}
		else
		{
#ifdef  MY_DEBUG_PRINTF
		    printf("mBOC.mCSW.mCSW_Status=%02x\n",mBOC.mCSW.mCSW_Status);
		    for( i=0;i!=len;i++ ){
		        printf("%02x ",*p++);
		    }
		    printf("\n");
#endif
			return( USB_INT_DISK_ERR1 );  										/* Disk operation error */
		}
	}
	else if( status == USB_INT_DISCONNECT )										/* Return directly */
	{
		return( status );
	}
	else
	{
		/* Determine which step made the mistake */
		if( (status == USB_INT_DISK_ERR))										/* Return the STALL error */
		{
			if( gDeviceUsbType == USB_U30_SPEED ){
				status = U30HOST_ClearEndpStall(  0x80 | gDiskBulkInEp );
				printf("status4=%02x\n",status);
				gDiskBulkInTog = 0;
			}
			else{
				status = U20HOST_ClearEndpStall(  0x80 | gDiskBulkInEp );
			}
			if( status == USB_INT_DISCONNECT )									/* Return directly */
			{
				return( status );
			}
		}
		return status;
	}
	status = USB_OPERATE_SUCCESS;
	return( status );
}

/*********************************************************************
 * @fn      MS_RequestSense
 *
 * @brief   The USB host checks the disk error status
 *
 * @param   *pbuf  - Data buffer
 *
 * @return  Status
 */
uint8_t MS_RequestSense( uint8_t *pbuf )
{

#ifdef  MY_DEBUG_PRINTF
		printf( "MS_RequestSense:...\n" );
#endif

	mDelaymS( 10 );  														  	
	if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
	mBOC.mCBW.mCBW_DataLen 	   = 0x00000012;
	mBOC.mCBW.mCBW_Flag 	   = 0x80;
	mBOC.mCBW.mCBW_CB_Len 	   = 0x06;
	mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x03;  										
	mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x12;
	mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;
	return( MS_ScsiCmd_Process( pbuf ) );  							
}

/*********************************************************************
 * @fn      MS_DiskInquiry
 *
 * @brief   The USB host acquires disk characteristics
 *
 * @param   *pbuf  - Data buffer
 *
 * @return  Status
 */
uint8_t MS_DiskInquiry( uint8_t *pbuf )
{
	uint8_t s, retry;
	for( retry = 0; retry!=3; retry++ ){
	    mDelaymS( 100 * retry );
        mBOC.mCBW.mCBW_DataLen 	   = 0x00000024;
        mBOC.mCBW.mCBW_Flag 	   = 0x80;
        mBOC.mCBW.mCBW_CB_Len 	   = 0x06;
        mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x12;  										
        mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x24;
        mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;

        s = MS_ScsiCmd_Process( pbuf );
        if( s == USB_OPERATE_SUCCESS )break;
	}
	return( s );  							
}

/*********************************************************************
 * @fn      MS_DiskInquiry_1
 *
 * @brief   The USB host acquires disk characteristics1
 *
 * @param   *pbuf  - Data buffer
 *
 * @return  Status
 */
uint8_t MS_DiskInquiry_1( uint8_t *pbuf )
{
    uint8_t s;
    mBOC.mCBW.mCBW_DataLen     = 0x00000024;
    mBOC.mCBW.mCBW_Flag        = 0x80;
    mBOC.mCBW.mCBW_CB_Len      = 0x06;
    mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x12;                                       
    mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x24;
    mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;

    s = MS_ScsiCmd_Process( pbuf );

    return( s );                            
}

/*********************************************************************
 * @fn      MS_DiskCapacity
 *
 * @brief   The USB host acquires disk capacity
 *
 * @param   *pbuf  - Data buffer
 *
 * @return  Status
 */
uint8_t MS_DiskCapacity( uint8_t *pbuf )
{
	mBOC.mCBW.mCBW_DataLen     = 0x00000008;
	mBOC.mCBW.mCBW_Flag 	   = 0x80;
	mBOC.mCBW.mCBW_CB_Len      = 10;
	mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x25;  										
	mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 6 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 7 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 8 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 9 ] = 0x00;
	return( MS_ScsiCmd_Process( pbuf ) );  							
}

/*********************************************************************
 * @fn      MS_DiskTestReady
 *
 * @brief   The USB host tests whether the disk is ready
 *
 * @param   *pbuf  - Data buffer
 *
 * @return  Status
 */
uint8_t MS_DiskTestReady( uint8_t *pbuf )
{
	mBOC.mCBW.mCBW_DataLen 	   = 0x00;
	mBOC.mCBW.mCBW_Flag 	   = 0x00;
	mBOC.mCBW.mCBW_CB_Len      = 0x06;
	mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x00;  										
	mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x00;
	mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;
	return( MS_ScsiCmd_Process( pbuf ) );  										
}

/*********************************************************************
* @fn      recover_init
*
* @brief   Recover init
*
* @param   none
*
* @return  none
*/
void recover_init( void )
{
    USBSSH->LINK_CTRL |= (1<<6) ;
}

UINT8V Hot_ret_flag;

/*********************************************************************
* @fn      Hot_Reset
*
* @brief   USB30 Hot Reset
*
* @param   *pdata  - Data
*
* @return  Status 
*/
uint8_t Hot_Reset( uint8_t *pdata )
{
    UINT8 s,count;
    UINT32 time_count = 0;
    count = 0;
    if( gUdisk_flag ){
hot_reset_ag:
        printf("hot_reset\n");
        Hot_ret_flag = 1;
        USBSSH->LINK_CTRL = POWER_MODE_2;

        USBSSH->LINK_CTRL |= (1<<8) ;
        mDelaymS( 80 );
        USBSSH->LINK_CTRL &= ~(1<<8) ;

        while( Hot_ret_flag == 1 ){
            if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
            time_count++;
            if( time_count>=0x1ffff ){
                break;
           }
        }

        mDelaymS( 20*(count+1) );

        Hot_ret_flag = 0;
        s = U30HSOT_Enumerate_Hotrst( pdata );
        if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
        if( s == USB_INT_DISCONNECT )return s;
        s = MS_Init_Hotrst( pdata );

        if( s == USB_INT_DISCONNECT )return s;
        if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
        if( s!= 0x00 ){
            count++;
            if( count<3 ){
                mDelaymS(count*10);
                goto hot_reset_ag;
            }
        }
    }
    else{
        hot_reset_ag1:
                printf("hot_reset1\n");
                Hot_ret_flag = 1;
                USBSSH->LINK_CTRL = POWER_MODE_2;
                printf("warn_rst\n");
                USBSSH->LINK_CTRL |= (1<<8) ;
                mDelaymS( 80 );
                USBSSH->LINK_CTRL &= ~(1<<8) ;
                while( Hot_ret_flag == 1 ){
                    time_count++;
                    if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
                    if( time_count>=0x1ffff ){
                        break;
                   }
                }
                mDelaymS( 20*(count+1) );
                Hot_ret_flag = 0;
                if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
                if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
                if( s == USB_INT_DISCONNECT )return s;

                s = MS_Init( pdata );
                printf("hot001=%02x\n",s);
                if( gDeviceConnectstatus == USB_INT_DISCONNECT )return USB_INT_DISCONNECT;
                if( s == USB_INT_DISCONNECT )return s;
                if( s!= 0x00 ){
                    count++;
                    if( count<3 ){
                        mDelaymS(count*10);
                        goto hot_reset_ag1;
                    }
                }
                mDelaymS(500);
    }
    return s;
}

/*********************************************************************
* @fn      MS_ReadSector
*
* @brief   Read data to the disk in units of sectors
*
* @param   StartLba  - Starting sector number
*          SectCount  - The number of sectors that need to be read
*          DataBuf  - Data buffer
*
* @return  Status 
*/
uint8_t MS_ReadSector( UINT32 StartLba, uint16_t SectCount, PUINT8 DataBuf )
{
	uint8_t  err, s, retry_count;
	UINT32 len;

#ifdef  MY_DEBUG_PRINTF
	printf( "MS_ReadSector:...\n" );
 	printf("%08lx,%d,%d\n",StartLba,gDiskPerSecSize,SectCount);
 	printf("DataBuf=%08lx\n",(UINT32)DataBuf);
#endif
	retry_count = 0;

retry:
	len = SectCount * gDiskPerSecSize; 											// Calculate the total length of the read

	for( err = 0; err < 5; err ++ ) 											// Error retry
	{
	    mBOC.mCBW.mCBW_DataLen = len;
		mBOC.mCBW.mCBW_Flag = 0x80;
		mBOC.mCBW.mCBW_CB_Len = 10;
		mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x28;  									// CMD
		mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 2 ] = (UINT8)( StartLba >> 24 );
		mBOC.mCBW.mCBW_CB_Buf[ 3 ] = (UINT8)( StartLba >> 16 );
		mBOC.mCBW.mCBW_CB_Buf[ 4 ] = (UINT8)( StartLba >> 8 );
		mBOC.mCBW.mCBW_CB_Buf[ 5 ] = (UINT8)( StartLba );
		mBOC.mCBW.mCBW_CB_Buf[ 6 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 7 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 8 ] = SectCount;
		mBOC.mCBW.mCBW_CB_Buf[ 9 ] = 0x00;
		s = MS_ScsiCmd_Process( DataBuf );  							 		// BulkOnly CMD
		if( s == USB_OPERATE_SUCCESS )
		{
			return( USB_OPERATE_SUCCESS );  									// SUCCESS
		}
		if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT  )
		{
			return( s );  														// The disk has either been disconnected or just replugged in
		}
		if( s == USB_CH417USBTIMEOUT ){
			printf("TIMEOUT\n");
			return s;
		}	
		if( gDeviceUsbType == USB_U30_SPEED ){
            if(  (s == 0x1f) || (s == 0x14) ){                	// Reading sector recognition requires a retry
                s = Hot_Reset( DataBuf );
                if( s == 0x00 ){
                    retry_count++;
                    if( retry_count>=5 )return s;
                    goto retry;
                }
                else return s;
            }
		}
		s = MS_RequestSense( DataBuf );
		if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT  )
		{
			return( s );
		}
	}
	return( s );  																
}

#if 1
/*********************************************************************
* @fn      MS_WriteSector
*
* @brief   Write data to the disk in units of sectors
*
* @param   StartLba  - Starting sector number
*          SectCount  - The number of sectors that need to be written
*          DataBuf  - Data buffer
*
* @return  Status 
*/
uint8_t MS_WriteSector( UINT32 StartLba, uint8_t SectCount, PUINT8 DataBuf )
{
	UINT8  err, s;
	UINT32 len;

#ifdef  MY_DEBUG_PRINTF
	printf( "MS_WriteSector:...\n" );
#endif

	len = SectCount * gDiskPerSecSize;		  									// Calculate the total length of the read
	for( err = 0; err < 3; err++ ) 												// Error retry
	{
		mBOC.mCBW.mCBW_DataLen = len;
		mBOC.mCBW.mCBW_Flag = 0x00;
		mBOC.mCBW.mCBW_CB_Len = 10;
		mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x2A;   									// CMD
		mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 2 ] = (UINT8)( StartLba >> 24 );
		mBOC.mCBW.mCBW_CB_Buf[ 3 ] = (UINT8)( StartLba >> 16 );
		mBOC.mCBW.mCBW_CB_Buf[ 4 ] = (UINT8)( StartLba >> 8 );
		mBOC.mCBW.mCBW_CB_Buf[ 5 ] = (UINT8)( StartLba );
		mBOC.mCBW.mCBW_CB_Buf[ 6 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 7 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 8 ] = SectCount;
		mBOC.mCBW.mCBW_CB_Buf[ 9 ] = 0x00;
		s = MS_ScsiCmd_Process( DataBuf );  									// BulkOnly CMD
		if( s == USB_OPERATE_SUCCESS )
		{
			return( USB_OPERATE_SUCCESS );  									// SUCCESS
		}
		if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT )
		{
			return( s );  														// The disk has either been disconnected or just replugged in
		}
		s = MS_RequestSense( DataBuf );
		if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT  )
		{
			return( s );
		}
	}
	return( s );  																// Return Status
}
#endif

/*********************************************************************
 * @fn      MS_Init
 *
 * @brief   Initialization of USB large-capacity storage devices
 *
 * @param   *pbuf  - Data buffer
 *
 * @return  Status.
 */
uint8_t MS_Init(  uint8_t *pbuf )
{
	uint8_t count, status;

	/*******************************Get the maximum logical unit number********************************/
#ifdef  MY_DEBUG_PRINTF
	printf( "MS_GetMaxLun...\n" );
#endif
	status = MS_GetMaxLun(  );

	if ( status != USB_OPERATE_SUCCESS )
	{
#ifdef  MY_DEBUG_PRINTF
		printf( "ERROR = %02X\n", (UINT16)status );
#endif
		gDiskMaxLun = 0x00;											 			/* Some USB flash drives may not be supported, if an error occurs, take the default value */
	}
	printf("gDiskMaxLun=%02x\n",gDiskMaxLun);
	mDelaymS( 10 );

	/*******************************Get information on the USB flash drive(INQUIRY)******************************/
	gDiskCurLun = 0x00;															/* Determine whether the current logic unit is a CD-ROM */

	for( count = 0; count < gDiskMaxLun + 1; count++ )
	{
		status = MS_DiskInquiry( pbuf );  										/* Get disk characteristics */
		printf( "Disk Inquiry:...%02x\n",status );
		if ( status != USB_OPERATE_SUCCESS )
		{
#ifdef  MY_DEBUG_PRINTF
			printf( "ERROR = %02X\n", (UINT16)status );
#endif
			if( status == USB_INT_DISCONNECT )									// If the device is disconnected, return directly
			{
				return( status );
			}
			else if( (status&0x20) != USB_PID_NAK ){
			    return( USB_OPERATE_ERROR ); 	  								// Terminate the operation and return directly
			}
		}

#ifdef  MY_DEBUG_PRINTF
		for ( uint8_t i = 8; i < 36; i ++ )
		{
			printf( "%02x ", *( pbuf + i ) );
		}
		printf( "\n" );
#endif
		if( ( gDiskMaxLun == 0x00 ) && ( *( pbuf + 0 ) == 0x05 ) )
		{
			return( USB_OPERATE_ERROR );
		}
		if( *( pbuf + 0 ) == 0x05 )
		{
			gDiskCurLun++;
			mDelaymS( 3 );
			continue;															// Continue
		}
		else
		{
			break;
		}
	}

	gDiskCurLun = 0;
	/*******************************Get the capacity of the USB flash drive***************************************/
ready_rety:
	for( count = 0; count < 5; count++ )
	{

		mDelaymS( 200 );
		mDelaymS( 100 * count );

#ifdef  MY_DEBUG_PRINTF
		printf( "Disk Capacity:...\n" );
#endif

		printf( "Disk Capacity:...=%02x\n",status );
		status = MS_DiskCapacity( pbuf );  									  // Get disk capacity
		printf( "Disk Capacity:...=%02x\n",status );
		if ( status != USB_OPERATE_SUCCESS )
		{
#ifdef  MY_DEBUG_PRINTF
			printf( "ERROR = %02x\n", (UINT16)status );
#endif
			if( status == USB_INT_DISCONNECT )								  // If the device is disconnected, return directly
			{
				return( status );
			}
			MS_RequestSense( pbuf );										  // If an error occurs, low-level error handling will be carried out
		}
		else
		{
			/* Save the current sector size (Calculate separately to prevent compiler errors) */
			gDiskPerSecSize = ( ( (UINT32)( *( pbuf + 4 ) ) ) << 24 );
			gDiskPerSecSize |= ( ( (UINT32)( *( pbuf + 5 ) ) ) << 16 );
			gDiskPerSecSize |= ( ( (UINT32)( *( pbuf + 6 ) ) ) << 8 ) + ( *( pbuf + 7 ) );

			/* Save the current number of sectors (Calculate them separately to prevent compiler errors) */
			gDiskCapability = ( ( (UINT32)( *( pbuf + 0 ) ) ) << 24 );
			gDiskCapability |= ( ( (UINT32)( *( pbuf + 1 ) ) ) << 16 );
			gDiskCapability |= ( ( (UINT32)( *( pbuf + 2 ) ) ) << 8 ) + ( *( pbuf + 3 ) );

			if( gDiskPerSecSize <= 512 ){
			    gDiskPerSecSize = 512;
			}
			if( gDiskPerSecSize >2048 ){        							  // Support large sectors
			    gDiskPerSecSize = 512;
			}
			printf("gDiskPerSecSize: %08lx\n",(UINT32)gDiskPerSecSize);
			printf("gDiskCapability: %08lx\n",gDiskCapability);

			break;
		}
	}
	if((status != 0) && gDiskMaxLun){
	    gDiskCurLun++;
	    if( gDiskCurLun< (gDiskMaxLun+1))goto ready_rety;
	    else return USB_CH417USBTIMEOUT;
	}

	/*******************************Test whether the USB flash drive is ready*********************************/
	for( count = 0; count < 5; count ++ )
	{
		mDelaymS( 50 );

#ifdef  MY_DEBUG_PRINTF
		printf( "Disk Ready:...\n" );
#endif
		
		status = MS_DiskTestReady( pbuf );  								  // Test whether the disk is readyTest whether the disk is ready
		printf( "Disk Ready:...=%02x\n",status );
		if ( status != USB_OPERATE_SUCCESS )
		{

#ifdef  MY_DEBUG_PRINTF
			printf( "ERROR = %02X\n", (UINT16)status );
#endif
			if( status == USB_INT_DISCONNECT )								  // Return directly
			{
				return( status );
			}
			MS_RequestSense( pbuf );										  // If an error occurs, low-level error handling will be carried out
		}
		else
		{
			break;
		}
	}	
	return( USB_OPERATE_SUCCESS );
}

/*********************************************************************
 * @fn      MS_Init_Hotrst
 *
 * @brief   Initialization of USB large-capacity storage devices
 *
 * @param   *pbuf  - Data buffer
 *
 * @return  Status.
 */
uint8_t MS_Init_Hotrst( uint8_t *pbuf )
{
    uint8_t  count, status;

#ifdef  MY_DEBUG_PRINTF
    printf( "MS_GetMaxLun...\n" );
#endif

    gDiskCurLun = 0x00;														// gDiskMaxLun = 0;

    for( count = 0; count < gDiskMaxLun + 1; count++ )
    {
        status = MS_DiskInquiry_1( pbuf );                                  // Get Disk characteristics
        if ( status != USB_OPERATE_SUCCESS )
        {
#ifdef  MY_DEBUG_PRINTF
            printf( "ERROR = %02X\n", (UINT16)status );
#endif
            if( status == USB_INT_DISCONNECT )                              // If the device is disconnected, return directly
            {
                return( status );
            }
            else if( (status&0x20) != USB_PID_NAK ){
                return( USB_OPERATE_ERROR );                                // Return directly
            }
            else return status;
        }

#ifdef  MY_DEBUG_PRINTF
       for ( i = 8; i < 36; i ++ )
       {
           printf( "%02x ", *( pbuf + i ) );
       }
       printf( "\n" );
#endif

        if( ( gDiskMaxLun == 0x00 ) && ( *( pbuf + 0 ) == 0x05 ) )
        {
            /* If there is only one logical disk on the current USB drive and it is a CD-ROM, 
			   then terminate the operation and return directly */
            return( USB_OPERATE_ERROR );
        }
        if( *( pbuf + 0 ) == 0x05 )
        {
            gDiskCurLun++;
            mDelaymS( 3 );
            continue;                                                           
        }
        else
        {
            break;
        }
    }

    return ( USB_OPERATE_SUCCESS );
}


/* The following is the processing of USB2.0 and USB1.1 */
#if 1
/*********************************************************************
 * @fn      U20HOST_Issue_BulkOut
 *
 * @brief   The USB host performs Bulk transfer
 *
 * @param   EndpNum  - Endpoint Number
 *			tog  - Toggle
 *			*pDatBuf  - Data buffer
 *			*pSize  - Data size
 *			Pid  - Pid Number
 *
 * @return  Status.
 */
uint8_t U20HOST_Issue_Bulk( uint8_t EndpNum, uint8_t tog, uint8_t *pDatBuf, uint16_t *pSize, uint8_t Pid )
{
	uint8_t s = 0;

	if( Pid == USB_PID_OUT ){			// OUT
		USBHSH->TX_DMA = (UINT32)pDatBuf;
		USBHSH->TX_LEN = *pSize;
	    s = USBHSH_Transact( USB_PID_OUT | EndpNum<<4, tog, 1000000 );          // OUT DATA, 200ms timeout
	}
	else if( Pid == USB_PID_IN ){		// IN
		USBHSH->RX_DMA = (UINT32)pDatBuf;
	    s = USBHSH_Transact( USB_PID_IN  | EndpNum<<4, tog, 1000000 );          // IN DATA, 200ms timeout
	    if( s == ERR_SUCCESS ){
	    	*pSize = USBHSH->RX_LEN;
	    }
	    else if( s == 0x2e )return USB_INT_DISK_ERR;
	}
	return s;
}

/*********************************************************************
 * @fn      U20HOST_Issue_BulkOut
 *
 * @brief   The USB host performs Bulk transfer
 *
 * @param   *pDatBuf  - Data buffer, Buffer address must be 4-byte aligned
 *			*pSize  - Data size, Maximum 1024
 *
 * @return  Status.
 */
uint8_t U20HOST_Issue_BulkOut( uint8_t *pDatBuf, UINT32 *pSize )
{
	uint8_t status;
	uint16_t len;

	while(1){
		if( *pSize >= U20_ENDP_SIZE ){
			len = U20_ENDP_SIZE;
		}
		else{
			len = *pSize;
		}
		status = U20HOST_Issue_Bulk( gDiskBulkOutEp, gDiskBulkOutTog, pDatBuf, &len, USB_PID_OUT );
		if( status != USB_INT_SUCCESS )return status;													// Further processing is still needed
		*pSize -= len;
		pDatBuf += len;
		gDiskBulkOutTog ^= 0x40;
		if( *pSize == 0 )break;
	}
	return USB_INT_SUCCESS;
}

/*********************************************************************
 * @fn      U20HOST_Issue_BulkIn
 *
 * @brief   The USB host performs Bulk transfer
 *
 * @param   *pDatBuf  - Data buffer, Buffer address must be 4-byte aligned
 *			*pSize  - Data size, Maximum 1024
 *
 * @return  Status.
 */
uint8_t U20HOST_Issue_BulkIn( uint8_t *pDatBuf, UINT32 *pSize )
{
	uint8_t status;
	uint16_t len, total_len;
	total_len = 0;
	while(1){
		if( *pSize >= U20_ENDP_SIZE ){
			len = U20_ENDP_SIZE;
		}
		else{
			len = *pSize;
		}
		status = U20HOST_Issue_Bulk( gDiskBulkInEp, gDiskBulkInTog, pDatBuf, &len, USB_PID_IN );
		if( status != USB_INT_SUCCESS )return status;													// Further processing is still needed
		gDiskBulkInTog ^= 0x80;
		*pSize -= len;
		pDatBuf += len;
		total_len += len;
		if( (*pSize == 0) || (len < U20_ENDP_SIZE)){													// Not full package
			*pSize = total_len;
			break;
		}
	}
	return USB_INT_SUCCESS;
}
#endif