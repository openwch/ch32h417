/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_usbss_device.h"
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/05/23
* Description        : header file of ch32h417_usbss_device.c
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32H417_USBSS_DEVICE_H__
#define __CH32H417_USBSS_DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32h417.h"
#include "hardware.h"    


/******************************************************************************/
/* Endpoint Number */
#define DEF_UEP_IN                    0x80
#define DEF_UEP_OUT                   0x00
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

#define DEF_USBSSD_UEP0_SIZE          512
#define DEF_USB_EP1_SS_SIZE           1024
#define DEF_USB_EP2_SS_SIZE           1024
#define DEF_USB_EP3_SS_SIZE           1024
#define DEF_USB_EP4_SS_SIZE           1024

#define DEF_ENDP1_OUT_BURST_LEVEL     16
#define DEF_ENDP1_IN_BURST_LEVEL      16
#define DEF_ENDP2_OUT_BURST_LEVEL     16
#define DEF_ENDP2_IN_BURST_LEVEL      16
#define DEF_ENDP3_OUT_BURST_LEVEL     16
#define DEF_ENDP3_IN_BURST_LEVEL      16
#define DEF_ENDP4_OUT_BURST_LEVEL     16
#define DEF_ENDP4_IN_BURST_LEVEL      16

#define DEF_ENDP1_OUT_BUFF_SIZE       2
#define DEF_ENDP1_IN_BUFF_SIZE        2
#define DEF_ENDP2_OUT_BUFF_SIZE       2
#define DEF_ENDP2_IN_BUFF_SIZE        2
#define DEF_ENDP3_OUT_BUFF_SIZE       2
#define DEF_ENDP3_IN_BUFF_SIZE        2
#define DEF_ENDP4_OUT_BUFF_SIZE       2
#define DEF_ENDP4_IN_BUFF_SIZE        2

#define pUSBSS_SetupReqPak            ((PUSB_SETUP_REQ)USBSS_EP0_Buf)

#define USBSS_PHY_CFG_CR              (*((__IO uint32_t *)0x400341f8))          
#define USBSS_PHY_CFG_DAT             (*((__IO uint32_t *)0x400341fc))      

#define LMP_HP                        0
#define LMP_SUBTYPE_MASK              ( 0xf << 5 )
#define LMP_SET_LINK_FUNC             ( 0x1 << 5 )
#define LMP_U2_INACT_TOUT             ( 0x2 << 5 )
#define LMP_VENDOR_TEST               ( 0x3 << 5 )
#define LMP_PORT_CAP                  ( 0x4 << 5 )
#define LMP_PORT_CFG                  ( 0x5 << 5 )
#define LMP_PORT_CFG_RES              ( 0x6 << 5 )
#define LMP_LINK_SPEED                ( 1 << 9 )
#define NUM_HP_BUF                    ( 4 << 0 )
#define DOWN_STREAM                   ( 1 << 16 )
#define UP_STREAM                     ( 2 << 16 )

      
/******************************************************************************/
typedef enum 
{  
    UNINIT = 0,
    U3_INI_FRIST = 1,
    U3_INIT_SECOND = 2,
    U2_INIT = 3,
    U2U3_SUCC = 4,
} LINK_State_t;

typedef enum 
{   
    Power_off = 0,
    Disconnected =1,
    DISABLED = 2,
    Training = 3,
    Enabled = 4,
    Resetting = 5,
    Error = 6,
    Loopback = 7,
    Compliance = 8
} PortState;

typedef struct  __attribute__((packed))USBSS_Port_Info 
{
    uint16_t port_change;
    uint16_t port_status;
    uint8_t set_u3_exit;
    uint8_t set_u3_enter;
    uint8_t set_u2_exit;
    uint8_t set_u2_enter;
    uint8_t set_u1_exit;
    uint8_t set_u1_enter;
    uint8_t set_warm_reset;
    uint8_t set_hot_reset;
    uint8_t port_rmt_wkup;
    uint8_t lp_mode;
    uint8_t DSPORT;
} USBSS_Port_Info_t;

typedef struct  __attribute__((packed)) USBSS_Dev_Info
{
    uint8_t u1_enable;
    uint8_t u2_enable;
    uint8_t set_devaddr;
    uint8_t devaddr;
    uint8_t set_depth_req;
    uint8_t set_depth_value;
    uint8_t set_isoch_delay;
    uint8_t set_isoch_value;
    uint8_t set_rmt_wkup;
    uint8_t set_func_susp;
    uint8_t endp1_flow_ctrl;
    uint8_t port_lpwr;
    uint8_t port_u1_st;
    USBSS_Port_Info_t portD[ 1 ];
} USBSS_Dev_Info_t;


extern __attribute__ ((aligned(4))) uint8_t USBSS_EP0_Buf[ DEF_USBSSD_UEP0_SIZE ];
extern __attribute__ ((aligned(4))) uint8_t USBSS_EP1_Rx_Buf[ DEF_USB_EP1_SS_SIZE * DEF_ENDP1_OUT_BURST_LEVEL * DEF_ENDP1_OUT_BUFF_SIZE ];
extern __attribute__ ((aligned(4))) uint8_t USBSS_EP2_Rx_Buf[ DEF_USB_EP2_SS_SIZE * DEF_ENDP2_OUT_BURST_LEVEL * DEF_ENDP2_OUT_BUFF_SIZE ];
extern __attribute__ ((aligned(4))) uint8_t USBSS_EP3_Rx_Buf[ DEF_USB_EP3_SS_SIZE * DEF_ENDP3_OUT_BURST_LEVEL * DEF_ENDP3_OUT_BUFF_SIZE ];


extern USBSS_Dev_Info_t USBSS_Dev_Info;
extern volatile uint8_t  EP1_Chain_Sel;
extern volatile uint8_t  EP2_Chain_Sel;
extern volatile uint8_t  EP3_T_Chain_Sel;
extern volatile uint8_t  EP3_R_Chain_Sel;
extern volatile uint8_t  USB_Enum_Status;
/* SetUp Request Values */
extern const uint8_t *pUSBSS_Descr;

/* Setup Request */
extern volatile uint8_t  USBSS_SetupReqCode;
extern volatile uint8_t  USBSS_SetupReqType;
extern volatile uint16_t USBSS_SetupReqValue;
extern volatile uint16_t USBSS_SetupReqIndex;
extern volatile uint16_t USBSS_SetupReqLen;

/* USB Device Status */
extern volatile uint8_t  USBSS_DevConfig;
extern volatile uint8_t  USBSS_DevAddr;
extern volatile uint8_t  USBSS_DevSleepStatus;
extern volatile uint8_t  USBSS_DevEnumStatus;
extern volatile uint16_t USBSS_DevMaxPackLen;

extern void USBSS_Device_Endp_Init(void );
extern void USBSS_Device_Endp_Deinit( void );
extern void USBSS_Device_Init( FunctionalState sta );
extern void USBSS_LINK_Handle( USBSSH_TypeDef *USBSSHx );
extern void SET_Device_Address( uint32_t address, USBSSH_TypeDef *USBSSx );
extern uint32_t USBSS_PHY_Cfg( uint8_t port_num, uint8_t addr, uint16_t data );
extern void USBSS_PHY_Init( uint8_t port_num );
extern void USBSS_CFG_MOD( void );
extern void USBSS_PLL_Init( FunctionalState sta );
extern void USB_Timer_Start( FunctionalState sta );
extern void USB_Timer_Init( void );

#ifdef __cplusplus
}
#endif


#endif /* __ch32h417_USBSS_H */

