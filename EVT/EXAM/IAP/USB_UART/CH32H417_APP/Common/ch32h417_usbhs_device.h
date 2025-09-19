/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32h417_usbhs_device.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/03/05
 * Description        : USBHS2.0
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32H417_USBHS_DEVICE_H__
#define __CH32H417_USBHS_DEVICE_H__

#include "debug.h"
#include "string.h"
#include "ch32h417_usb.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL                    0
#endif

#ifndef VOID
#define VOID                    void
#endif
#ifndef CONST
#define CONST                   const
#endif
#ifndef BOOL
typedef unsigned char           BOOL;
#endif
#ifndef BOOLEAN
typedef unsigned char           BOOLEAN;
#endif
#ifndef CHAR
typedef char                    CHAR;
#endif
#ifndef INT8
typedef char                    INT8;
#endif
#ifndef INT16
typedef short                   INT16;
#endif
#ifndef INT32
typedef long                    INT32;
#endif
#ifndef UINT8
typedef unsigned char           UINT8;
#endif
#ifndef UINT16
typedef unsigned short          UINT16;
#endif
#ifndef UINT32
typedef unsigned long           UINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile  UINT8V;
#endif
#ifndef UINT16V
typedef unsigned short volatile UINT16V;
#endif
#ifndef UINT32V
typedef unsigned long volatile  UINT32V;
#endif

#ifndef PVOID
typedef void                    *PVOID;
#endif
#ifndef PCHAR
typedef char                    *PCHAR;
#endif
#ifndef PCHAR
typedef const char              *PCCHAR;
#endif
#ifndef PINT8
typedef char                    *PINT8;
#endif
#ifndef PINT16
typedef short                   *PINT16;
#endif
#ifndef PINT32
typedef long                    *PINT32;
#endif
#ifndef PUINT8
typedef unsigned char           *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short          *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long           *PUINT32;
#endif
#ifndef PUINT8V
typedef volatile unsigned char  *PUINT8V;
#endif
#ifndef PUINT16V
typedef volatile unsigned short *PUINT16V;
#endif
#ifndef PUINT32V
typedef volatile unsigned long  *PUINT32V;
#endif


/* Global define */
#define IF_FULL_SPEED       1

#define DEF_USBD_UEP0_SIZE     64
#if IF_FULL_SPEED
#define USBHS_MAX_PACK_SIZE 64
#else
#define USBHS_MAX_PACK_SIZE 64
#endif
#define pUSBHS_SetupReqPak            ((PUSB_SETUP_REQ)USBHS_EP0_Buf)

#define DEF_UEP_IN                    0x80
#define DEF_UEP_OUT                   0x00
/* Endpoint Number */
#define DEF_UEP_BUSY                  0x01
#define DEF_UEP_FREE                  0x00
#define DEF_UEP_NUM                   16
#define DEF_UEP0                      0x00
#define DEF_UEP1                      0x01
#define DEF_UEP2                      0x02
#define DEF_UEP3                      0x03
#define DEF_UEP4                      0x04
#define DEF_UEP5                      0x05
#define DEF_UEP6                      0x06
#define DEF_UEP7                      0x07
#define DEF_UEP8                      0x08
#define DEF_UEP9                      0x09
#define DEF_UEP10                     0x0A
#define DEF_UEP11                     0x0B
#define DEF_UEP12                     0x0C
#define DEF_UEP13                     0x0D
#define DEF_UEP14                     0x0E
#define DEF_UEP15                     0x0F


#define TEST_ENABLE                   0x01
#define TEST_MASK                     0x0F

#define USBHSD_UEP_RXDMA_BASE         0x40030024
#define USBHSD_UEP_TXDMA_BASE         0x40030040 
#define USBHSD_UEP_TXLEN_BASE         0x400300A0
#define USBHSD_UEP_TXCTL_BASE         0x400300A2
#define USBHSD_UEP_TX_EN( N )         ( (uint16_t)( 0x01 << N ) )
#define USBHSD_UEP_RX_EN( N )         ( (uint16_t)( 0x01 << N ) )

#define DEF_UEP_DMA_LOAD              0 /* Direct the DMA address to the data to be processed */
#define DEF_UEP_CPY_LOAD              1 /* Use memcpy to move data to a buffer */
#define USBHSD_UEP_RXDMA( N )         ( *((volatile uint32_t *)( USBHSD_UEP_RXDMA_BASE + ( N - 1 ) * 0x04 ) ) )
#define USBHSD_UEP_RXBUF( N )         ( (uint8_t *)(*((volatile uint32_t *)( USBHSD_UEP_RXDMA_BASE + ( N - 1 ) * 0x04 ) ) ) + 0x20000000 )
#define USBHSD_UEP_TXCTRL( N )        ( *((volatile uint8_t *)( USBHSD_UEP_TXCTL_BASE + ( N - 1 ) * 0x04 ) ) )
#define USBHSD_UEP_TXDMA( N )         ( *((volatile uint32_t *)( USBHSD_UEP_TXDMA_BASE + ( N - 1 ) * 0x04 ) ) )
#define USBHSD_UEP_TXBUF( N )         ( (uint8_t *)(*((volatile uint32_t *)( USBHSD_UEP_TXDMA_BASE + ( N - 1 ) * 0x04 ) ) ) + 0x20000000 )
#define USBHSD_UEP_TLEN( N )          ( *((volatile uint16_t *)( USBHSD_UEP_TXLEN_BASE + ( N - 1 ) * 0x04 ) ) )


/* USB SPEED TYPE */
#define USBHS_SPEED_TYPE_MASK         ((uint8_t)(0x03))
#define USBHS_SPEED_LOW               ((uint8_t)(0x02))
#define USBHS_SPEED_FULL              ((uint8_t)(0x00))
#define USBHS_SPEED_HIGH              ((uint8_t)(0x01))

extern const UINT8 *pDescr;
extern volatile UINT8  USBHS_Dev_SetupReqCode;                                  /* USBHS device Setup package command code */
extern volatile UINT16 USBHS_Dev_SetupReqLen;                                   /* USBHS device Setup packet length */
extern volatile UINT8  USBHS_Dev_SetupReqValueH;                                /* USBHS device Setup package Value high byte */
extern volatile UINT8  USBHS_Dev_Config;                                        /* USBHS device configuration values */
extern volatile UINT8  USBHS_Dev_Address;                                       /* USBHS device address value */
extern volatile UINT8  USBHS_Dev_SleepStatus;                                   /* USBHS device sleep state */
extern volatile UINT8  USBHS_Dev_EnumStatus;                                    /* USBHS device enumeration status */
extern volatile UINT8  USBHS_Dev_Endp0_Tog;                                     /* USBHS device endpoint 0 sync flag */

extern const uint8_t *pUSBHS_Descr;

/********************************************************************************/
extern void USBHS_RCC_Init(FunctionalState sta);                                             /* USBHS device RCC initialization */
extern void USBHS_Device_Endp_Init ( void );                                    /* USBHS device endpoint initialization */
extern void USBHS_Device_Init ( FunctionalState sta );                          /* USBHS device initialization */
void DevEPhs_IN_Deal(UINT8 l);
void DevEPhs_OUT_Deal(UINT8 l);
extern void USBHS_Device_SetAddress( UINT32 address );                          /* USBHS device set device address */
extern void USBHS_IRQHandler( void );                                           /* USBHS device interrupt service routine */
extern void USBHS_Sleep_WakeUp_Cfg( void );                                     /* USBHS device sleep and wake configuration */

#ifdef __cplusplus
}
#endif

#endif

