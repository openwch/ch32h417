/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_usbfs_device.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/08/20
* Description        : This file contains all the functions prototypes for the
*                      USBFS firmware library.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32H417_USBFS_DEVICE_H_
#define __CH32H417_USBFS_DEVICE_H_

#include "debug.h"
#include "string.h"
#include "usb_desc.h"
#include "ch32h417_usb.h"

/*******************************************************************************/
/* Macro Definition */

/* General */
#define pUSBFS_SetupReqPak          ((PUSB_SETUP_REQ)USBFS_EP0_Buf)

/* end-point number */
#define DEF_UEP_IN                  0x80
#define DEF_UEP_OUT                 0x00
#define DEF_UEP_BUSY                0x01
#define DEF_UEP_FREE                0x00
#define DEF_UEP_NUM                 0x08
#define DEF_UEP0                    0x00
#define DEF_UEP1                    0x01
#define DEF_UEP2                    0x02
#define DEF_UEP3                    0x03
#define DEF_UEP4                    0x04
#define DEF_UEP5                    0x05
#define DEF_UEP6                    0x06
#define DEF_UEP7                    0x07

#define USBFSD_UEP_MOD_BASE         0x4002340C
#define USBFSD_UEP_DMA_BASE         0x40023410
#define USBFSD_UEP_LEN_BASE         0x40023430
#define USBFSD_UEP_CTL_BASE         0x40023432
#define USBFSD_UEP_RX_EN            0x08
#define USBFSD_UEP_TX_EN            0x04
#define USBFSD_UEP_BUF_MOD          0x01
#define DEF_UEP_DMA_LOAD            0 /* Direct the DMA address to the data to be processed */
#define DEF_UEP_CPY_LOAD            1 /* Use memcpy to move data to a buffer */
#define USBFSD_UEP_MOD( N )         (*((volatile uint8_t *)( USBFSD_UEP_MOD_BASE + N )))
#define USBFSD_UEP_TX_CTRL( N )     (*((volatile uint8_t *)( USBFSD_UEP_CTL_BASE + N * 0x04 )))
#define USBFSD_UEP_RX_CTRL( N )     (*((volatile uint8_t *)( USBFSD_UEP_CTL_BASE + N * 0x04 + 1 )))
#define USBFSD_UEP_DMA( N )         (*((volatile uint32_t *)( USBFSD_UEP_DMA_BASE + N * 0x04 )))
#define USBFSD_UEP_BUF( N )         ((uint8_t *)(*((volatile uint32_t *)( USBFSD_UEP_DMA_BASE + N * 0x04 ))) + 0x20000000)
#define USBFSD_UEP_TLEN( N )        (*((volatile uint16_t *)( USBFSD_UEP_LEN_BASE + N * 0x04 )))

/******************************************************************************/
/* Variable Declaration */

/* Global */
extern const    uint8_t  *pUSBFS_Descr;

/* Setup Request */
extern volatile uint8_t  USBFS_SetupReqCode;
extern volatile uint8_t  USBFS_SetupReqType;
extern volatile uint16_t USBFS_SetupReqValue;
extern volatile uint16_t USBFS_SetupReqIndex;
extern volatile uint16_t USBFS_SetupReqLen;

/* USB Device Status */
extern volatile uint8_t  USBFS_DevConfig;
extern volatile uint8_t  USBFS_DevAddr;
extern volatile uint8_t  USBFS_DevSleepStatus;
extern volatile uint8_t  USBFS_DevEnumStatus;

/* HID Class Command */
extern volatile uint8_t  USBFS_HidIdle[ 2 ];
extern volatile uint8_t  USBFS_HidProtocol[ 2 ];

/* Endpoint Buffer */
extern __attribute__ ((aligned(4))) uint8_t USBFS_EP0_Buf[ DEF_USBD_UEP0_SIZE ];     //ep0(64)
extern __attribute__ ((aligned(4))) uint8_t USBFS_EP1_Buf[ DEF_USB_EP1_FS_SIZE ];    //ep1_in(64)
extern __attribute__ ((aligned(4))) uint8_t USBFS_EP2_Buf[ DEF_USB_EP2_FS_SIZE ];    //ep2_in(64)

/******************************************************************************/
/* Function Declaration */
extern void USBFS_Device_Init( FunctionalState sta );
extern void USBFS_Device_Endp_Init( void );
extern void USBFS_RCC_Init( void );
extern void USBFS_Send_Resume( void );

#endif
