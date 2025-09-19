/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
  Example routine to emulate a custom USB device (CH372 device).
  This routine demonstrates the use of a USBHS Device to emulate a custom device, the CH372,
  with endpoints 1/3/5 downlinking data and uploading via endpoints 2/4/6 respectively
  Endpoint 1 uploads and downlinks via a ring buffer with no bitwise inversion. Endpoints 3/4 
  and 5/6 copy the data with a bitwise inversion before uploading.

  [Ep1 OUT] ------ Not Invert ------> [Ep2 IN]
  [Ep3 OUT] -------- Invert --------> [Ep4 IN]
  [Ep5 OUT] -------- Invert --------> [Ep6 IN ]
  
  Note: This routine needs to be demonstrated in conjunction with the host software.
  
  Tool: https://www.wch.cn/downloads/USBEndpDebug_ZIP.html
*/
#include "hardware.h"
#include "ch32h417_usbfs_device.h"

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
    printf("Build Time: %s %s\n", __DATE__, __TIME__);
    printf("GCC Version: %d.%d.%d\n",__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
    printf("CH372Device Running On USBFS Controller\n");

	/* USBFSD device init */
	USBFS_RCC_Init( );
	USBFS_Device_Init( ENABLE );

    while(1)
    {
        /* Determine if enumeration is complete, perform data transfer if completed */
        if(USBFS_DevEnumStatus)
        {
            /* Data Transfer */
            if(RingBuffer_Comm.RemainPack)
            {
                if((USBFSD->UEP2_TX_CTRL & USBFS_UEP_T_RES_MASK) == USBFS_UEP_T_RES_NAK)
                {
                    memcpy(USBFSD_UEP_BUF(DEF_UEP2),&Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_USBD_FS_PACK_SIZE], RingBuffer_Comm.PackLen[RingBuffer_Comm.DealPtr]);
                    USBFSD->UEP2_TX_LEN = RingBuffer_Comm.PackLen[RingBuffer_Comm.DealPtr];
                    USBFSD->UEP2_TX_CTRL = (USBFSD->UEP2_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;
                    
                    NVIC_DisableIRQ(USBFS_IRQn);
                    RingBuffer_Comm.RemainPack--;
                    RingBuffer_Comm.DealPtr++;
                    if(RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks)
                    {
                        RingBuffer_Comm.DealPtr = 0;
                    }
                    NVIC_EnableIRQ(USBFS_IRQn);
                }
            }

            /* Monitor whether the remaining space is available for further downloads */
            if(RingBuffer_Comm.RemainPack < (DEF_Ring_Buffer_Max_Blks - DEF_RING_BUFFER_RESTART))
            {
                if(RingBuffer_Comm.StopFlag)
                {
                    RingBuffer_Comm.StopFlag = 0;
                    USBFSD->UEP1_RX_CTRL = (USBFSD->UEP1_RX_CTRL & ~USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_ACK;
                }
            }
        }
	}
}
