/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_usbss_device.h"
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/05/23
* Description        : header file of ch32h417_usbss_device.c
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __CH32H417_USBSS_DEVICE_H__
#define __CH32H417_USBSS_DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32h417.h"

/******************************************************************************/
/* USB Device Requests */
#define     GET_DEVICE                0x80
#define     GET_ENDPOINT              0x82
#define     GET_HUB                   0xA0
#define     GET_PORT                  0xA3
#define     HUB_FEATURE               0x20
#define     PORT_FEATURE              0x23
#define     SET_DEVICE                0x00
#define     SET_INTERFACE             0x01
#define     SET_ENDPOINT              0x02
#define     GET_DESCRIPTOR            0x06
#define     SET_DESCRIPTOR            0x07
#define     CLEAR_TT_BUF              0x08
#define     RESET_TT                  0x09
#define     GET_TT_STATE              0x0A
#define     STOP_TT                   0x0B
#define     SET_AND_TEST              0x0C
#define     SET_HUB_DEPTH             0x0C

/* Standard Request Codes */
#define     GET_STATUS                0x00
#define     CLEAR_FEATURE             0x01
// #define  reserved                  0x02
#define     SET_FEATURE               0x03
// #define  reserved                  0x04
#define     SET_DEV_ADDRESS           0x05
#define     GET_DEV_CONFIG            0x08
#define     USB_SET_CONFIGURATION            0x09
#define     GET_DEV_INTERFACE         0x0A
#define     SET_DEV_INTERFACE         0x0B
#define     SYNCH_FRAME               0x0C
#define     SET_FUNC_SUSP             0x0
#define     SET_SEL                   0x30
#define     SET_ISOCH_DLY             0x31

/* Descriptor Types */
#define     DES_TYPE_DEVICE             0x01
#define     DES_TYPE_CONFIG             0x02
#define     DES_TYPE_STRING             0x03
#define     DES_TYPE_INTERFACE          0x04
#define     DES_TYPE_ENDP               0x05
#define     DES_TYPE_DEV_QUALIFIER      0x06
#define     DES_TYPE_OTHER_SPD_CONFIG   0x07
#define     DES_TYPE_INTERFACE_PWR      0x08
#define     DES_TYPE_BOS                0x0F // 15
#define     DES_TYPE_CAP                0x10 // 16

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

#define DEF_USBSSD_UEP0_SIZE           512
#define DEF_USB_EP1_SS_SIZE            1024
#define DEF_USB_EP2_SS_SIZE            1024
#define DEF_USB_EP3_SS_SIZE            1024
#define DEF_USB_EP4_SS_SIZE            1024

#define DEF_ENDP1_OUT_BURST_LEVEL       16
#define DEF_ENDP1_IN_BURST_LEVEL        16
#define DEF_ENDP2_OUT_BURST_LEVEL       16
#define DEF_ENDP2_IN_BURST_LEVEL        16
#define DEF_ENDP3_OUT_BURST_LEVEL       16
#define DEF_ENDP3_IN_BURST_LEVEL        16
#define DEF_ENDP4_OUT_BURST_LEVEL       16
#define DEF_ENDP4_IN_BURST_LEVEL        16

#define pUSBSS_SetupReqPak            ((PUSB_SETUP_REQ)USBSS_EP0_Buf)



#define USBSS_PHY_CFG_CR              (*((__IO uint32_t *)0x400341f8))          
#define USBSS_PHY_CFG_DAT             (*((__IO uint32_t *)0x400341fc))      

extern __attribute__ ((aligned(4))) uint8_t USBSS_EP0_Buf[ DEF_USBSSD_UEP0_SIZE ];
extern __attribute__ ((aligned(4))) uint8_t USBSS_EP1_Rx_Buf[ DEF_USB_EP1_SS_SIZE * 32 ];
extern __attribute__ ((aligned(4))) uint8_t USBSS_EP2_Rx_Buf[ DEF_USB_EP2_SS_SIZE * 32 ];
extern __attribute__ ((aligned(4))) uint8_t USBSS_EP3_Tx_Buf[ DEF_USB_EP2_SS_SIZE * 32 ];
extern __attribute__ ((aligned(4))) uint8_t USBSS_EP3_Rx_Buf[ DEF_USB_EP3_SS_SIZE * 32 ];
extern __attribute__ ((aligned(4))) uint8_t USBSS_EP4_Tx_Buf[ DEF_USB_EP4_SS_SIZE * 32 ];
extern __attribute__ ((aligned(4))) uint8_t USBSS_EP4_Rx_Buf[ DEF_USB_EP4_SS_SIZE * 32 ];
extern vu8 EP1_Chain_Sel;
extern vu8 EP2_Chain_Sel;
extern vu8 U3_UP_Flag;
extern vu32 U3_Time_Out_Count;
/******************************************************************************/
typedef enum {  Power_off = 0,
                Disconnected =1,
                DISABLED = 2,
                Training = 3,
                Enabled = 4,
                Resetting = 5,
                Error = 6,
                Loopback = 7,
                Compliance = 8
} PortState;

typedef struct  __attribute__((packed))USBSS_Port_Info {
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
} USBSS_Port_Info;

typedef struct  __attribute__((packed)) USBSS_DEV_Info{
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
    USBSS_Port_Info portD[ 1 ];
} USBSS_DEV_Info;


extern USBSS_DEV_Info ss_dev_info ;

/* SetUp Request Values */
extern const uint8_t *pUSBHS_Descr;

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
extern void USBSS_Device_Endp_Deinit(void);
extern void USBSS_Device_Init( FunctionalState sta );
extern void USBSS_LINK_Handle( USBSSH_TypeDef *USBSSHx,uint8_t port_num );
extern void SET_Devic_Address( uint32_t address,USBSSH_TypeDef *USBSSx );
extern uint32_t USBSS_PHY_Cfg( uint8_t port_num, uint8_t addr, uint16_t data );
extern void USBSS_PHY_Init( uint8_t port_num);
extern void USBSS_CFG_MOD( );
extern void USBSS_PLL_Init( FunctionalState sta );
/*---------------------------------------------*/
/* LINK_CFG */
#define U3_LINK_RESET               0x80000000
#define LINK_FORCE_RXTERM           0x00800000
#define LINK_FORCE_POLLING          0x00400000
#define LINK_TOUT_MODE              0x00200000
#define LINK_U2_ALLOW               0x00020000
#define LINK_U1_ALLOW               0x00010000
#define LINK_LTSSM_MODE             0x00008000
#define LINK_LOOPBACK_ACT           0x00004000
#define LINK_LOOPBACK_EN            0x00002000
#define LINK_U2_RXDET               0x00001000
#define LINK_CP78_SEL_MASK          0x00000C00
 #define LINK_CP78_SEL_190BITS      0x00000000
 #define LINK_CP78_SEL_120BITS      0x00000400
 #define LINK_CP78_SEL_50BITS       0x00000800
 #define LINK_CP78_SEL_250BITS      0x00000C00
#define LINK_TX_DEEMPH_MASK         0x00000300
 #define LINK_TX_DEEMPH_6DB         0x00000000
 #define LINK_TX_DEEMPH_3_5DB       0x00000100
#define LINK_TX_SWING               0x00000080
#define LINK_RX_EQ_EN               0x00000040
#define LINK_LFPS_RX_PD             0x00000020
#define LINK_COMPLIANCE_EN          0x00000010
#define LINK_PHY_RESET              0x00000008
#define LINK_SS_PLR_SWAP            0x00000004
#define LINK_RX_TERM_EN             0x00000002
#define LINK_DOWN_MODE              0x00000001

/* LINK_CTRL */
#define LINK_RX_TS_CFG_MASK         0xFF000000
#define LINK_TX_TS_CFG_MASK         0x00FF0000
 #define LINK_HOT_RESET             0x00010000
#define LINK_TX_LGO_U3              0x00008000
#define LINK_TX_LGO_U2              0x00004000
#define LINK_TX_LGO_U1              0x00002000
#define LINK_POLLING_EN             0x00001000
#define LINK_REG_ROUT_EN            0x00000800
#define LINK_LUP_LDN_EN             0x00000400
#define LINK_TX_UX_EXIT             0x00000200
#define LINK_TX_WARM_RESET          0x00000100
#define LINK_GO_RX_DET              0x00000080
#define LINK_GO_RECOVERY            0x00000040
#define LINK_GO_INACTIVE            0x00000020
#define LINK_GO_DISABLED            0x00000010
#define LINK_PD_MODE_MASK           0x00000003

/* LINK_INT_CTRL */
#define LINK_IE_STATE_CHG           0x80000000
#define LINK_IE_U1_TOUT             0x40000000
#define LINK_IE_U2_TOUT             0x20000000
#define LINK_IE_UX_FAIL             0x10000000
#define LINK_IE_TX_WARMRST          0x08000000
#define LINK_IE_UX_EXIT_FAIL        0x04000000
#define LINK_IE_RX_LMP_TOUT         0x00800000
#define LINK_IE_TX_LMP              0x00400000
#define LINK_IE_RX_LMP              0x00200000
#define LINK_IE_RX_DET              0x00100000
#define LINK_IE_LOOPBACK            0x00080000
#define LINK_IE_COMPLIANCE          0x00040000
#define LINK_IE_HPBUF_FULL          0x00020000
#define LINK_IE_HPBUF_EMPTY         0x00010000
#define LINK_IE_HOT_RST             0x00008000
#define LINK_IE_WAKEUP              0x00004000
#define LINK_IE_WARM_RST            0x00002000
#define LINK_IE_UX_EXIT             0x00001000
#define LINK_IE_TXEQ                0x00000800
#define LINK_IE_TERM_PRES           0x00000400
#define LINK_IE_UX_REJ              0x00000200
#define LINK_IE_U3_WK_TOUT          0x00000100
#define LINK_IE_GO_U0               0x00000080
#define LINK_IE_GO_U1               0x00000040
#define LINK_IE_GO_U2               0x00000020
#define LINK_IE_GO_U3               0x00000010
#define LINK_IE_DISABLE             0x00000008
#define LINK_IE_INACTIVE            0x00000004
#define LINK_IE_RECOVERY            0x00000002
#define LINK_IE_READY               0x00000001

/* LINK_INT_FLAG */
#define LINK_IF_STATE_CHG           0x80000000
#define LINK_IF_U1_TOUT             0x40000000
#define LINK_IF_U2_TOUT             0x20000000
#define LINK_IF_UX_FAIL             0x10000000
#define LINK_IF_TX_WARMRST          0x08000000
#define LINK_IF_UX_EXIT_FAIL        0x04000000
#define LINK_IF_RX_LMP_TOUT         0x00800000
#define LINK_IF_TX_LMP              0x00400000
#define LINK_IF_RX_LMP              0x00200000
#define LINK_IF_RX_DET              0x00100000
#define LINK_IF_LOOPBACK            0x00080000
#define LINK_IF_COMPLIANCE          0x00040000
#define LINK_IF_HPBUF_FULL          0x00020000
#define LINK_IF_HPBUF_EMPTY         0x00010000
#define LINK_IF_HOT_RST             0x00008000
#define LINK_IF_WAKEUP              0x00004000
#define LINK_IF_WARM_RST            0x00002000
#define LINK_IF_UX_EXIT             0x00001000
#define LINK_IF_TXEQ                0x00000800
#define LINK_IF_TERM_PRES           0x00000400
#define LINK_IF_UX_REJ              0x00000200
#define LINK_IF_U3_WK_TOUT          0x00000100
#define LINK_IF_GO_U0               0x00000080
#define LINK_IF_GO_U1               0x00000040
#define LINK_IF_GO_U2               0x00000020
#define LINK_IF_GO_U3               0x00000010
#define LINK_IF_DISABLE             0x00000008
#define LINK_IF_INACTIVE            0x00000004
#define LINK_IF_RECOVERY            0x00000002
#define LINK_IF_READY               0x00000001

/* LINK_STATUS */
#define LINK_HPBUF_EMPTY            0x80000000
#define LINK_HPBUF_FULL             0x40000000
#define LINK_HPBUF_IDLE             0x20000000
#define LINK_U3_SLEEP_ALLOW         0x00400000
#define LINK_U2_SLEEP_ALLOW         0x00200000
#define LINK_RXDET_SLEEP_ALLOW      0x00100000
#define LINK_WAKUP                  0x00080000
#define LINK_RX_LFPS                0x00040000
#define LINK_RX_DETECT              0x00020000
#define LINK_RX_UX_EXIT_REQ         0x00010000
#define LINK_STATE_MASK             0x00000F00
 #define LINK_STATE_U0              0x00000000
 #define LINK_STATE_U1              0x00000100
 #define LINK_STATE_U2              0x00000200
 #define LINK_STATE_U3              0x00000300
 #define LINK_STATE_DISABLE         0x00000400
 #define LINK_STATE_RXDET           0x00000500
 #define LINK_STATE_INACTIVE        0x00000600
 #define LINK_STATE_POLLING         0x00000700
 #define LINK_STATE_RECOVERY        0x00000800
 #define LINK_STATE_HOTRST          0x00000900
 #define LINK_STATE_COMPLIANCE      0x00000A00
 #define LINK_STATE_LOOPBACK        0x00000B00
#define LINK_TXEQ                   0x00000040
#define LINK_PD_MODE_ST_MASK        0x00000030
 #define LINK_PD_MODE_ST_P0         0x00000000
 #define LINK_PD_MODE_ST_P1         0x00000010
 #define LINK_PD_MODE_ST_P2         0x00000020
 #define LINK_PD_MODE_ST_P3         0x00000030
#define LINK_READY                  0x00000008
#define LINK_BUSY                   0x00000004
#define LINK_RX_WARM_RST            0x00000002
#define LINK_RX_TERM_PRES           0x00000001

/* LINK_LPM_CR */
#define LINK_LPM_TERM_PRESENT       0x00000800
#define LINK_LPM_TERM_CHG           0x00000400
#define LINK_LPM_EN                 0x00000200
#define LINK_LPM_RST                0x00000100

/* LINK_PORT_CAP */
#define LINK_LMP_RX_CAP_VLD         0x80000000
#define LINK_LMP_TX_CAP_VLD         0x40000000
#define LINK_SPEED_MASK             0x3F000000
#define LINK_PORT_CAP_MASK          0x00FFFFFF

/* USB_CONTROL */
#define USBSS_DEV_ADDR_MASK         0x7F000000
#define USBSS_UIE_FIFO_RXOV         0x00800000           
#define USBSS_UIE_FIFO_TXOV         0x00400000           
#define USBSS_UIE_ITP               0x00100000       
#define USBSS_UIE_RX_PING           0x00080000         
#define USBSS_UDIE_STATUS           0x00040000
#define USBSS_UHIE_NOTIF            0x00040000  
#define USBSS_UDIE_SETUP            0x00020000       
#define USBSS_UHIE_ERDY             0x00020000     
#define USBSS_UIE_TRANSFER          0x00010000           
#define USBSS_CHAIN_CONFLICT        0x00008000               
#define USBSS_TX_ERDY_MODE          0x00004000           
#define USBSS_HP_PEND_MASK          0x00000300    
 #define USBSS_HP_PENDING           0x00000200
#define USBSS_HOST_MODE             0x00000080       
#define USBSS_ITP_EN                0x00000040       
#define USBSS_SETUP_FLOW            0x00000020           
#define USBSS_DIR_ABORT             0x00000010       
#define USBSS_DMA_MODE              0x00000008       
#define USBSS_FORCE_RST             0x00000004       
#define USBSS_USB_CLR_ALL           0x00000002           
#define USBSS_DMA_EN                0x00000001  

/* USB_STATUS */
#define USBSS_HRX_RES_MASK          0xC0000000
#define USBSS_HTX_RES_MASK          0x00C00000
#define USBSS_EP_DIR_MASK           0x00001000
#define USBSS_EP_ID_MASK            0x00000700
#define USBSS_UIF_FIFO_RXOV         0x00000080  
#define USBSS_UIF_FIFO_TXOV         0x00000040           
#define USBSS_UIF_ITP               0x00000010       
#define USBSS_UIF_RX_PING           0x00000008           
#define USBSS_UDIF_STATUS           0x00000004           
#define USBSS_UHIF_NOTIF            0x00000004           
#define USBSS_UDIF_SETUP            0x00000002           
#define USBSS_UHIF_ERDY             0x00000002       
#define USBSS_UIF_TRANSFER          0x00000001   

/* USB_ITP */
#define USBSS_ITP_INTERVAL_MASK     0x00003FFF

/* USB_ITP_ADJ */
#define USBSS_ITP_DELTA             0x001FFF00
#define USBSS_ITP_DELAYED           0x00000080
#define USBSS_ITP_ADJ_CR_MASK       0x0000007F

/* UEP_TX_EN */
#define USBSS_EP15_TX_EN            0x00008000
#define USBSS_EP14_TX_EN            0x00004000
#define USBSS_EP13_TX_EN            0x00002000
#define USBSS_EP12_TX_EN            0x00001000
#define USBSS_EP11_TX_EN            0x00000800
#define USBSS_EP10_TX_EN            0x00000400
#define USBSS_EP9_TX_EN             0x00000200
#define USBSS_EP8_TX_EN             0x00000100
#define USBSS_EP7_TX_EN             0x00000080
#define USBSS_EP6_TX_EN             0x00000040
#define USBSS_EP5_TX_EN             0x00000020
#define USBSS_EP4_TX_EN             0x00000010
#define USBSS_EP3_TX_EN             0x00000008
#define USBSS_EP2_TX_EN             0x00000004
#define USBSS_EP1_TX_EN             0x00000002
#define USBSS_UH_TX_EN              0x00000002

/* UEP_RX_EN */
#define USBSS_EP15_RX_EN            0x00008000
#define USBSS_EP14_RX_EN            0x00004000
#define USBSS_EP13_RX_EN            0x00002000
#define USBSS_EP12_RX_EN            0x00001000
#define USBSS_EP11_RX_EN            0x00000800
#define USBSS_EP10_RX_EN            0x00000400
#define USBSS_EP9_RX_EN             0x00000200
#define USBSS_EP8_RX_EN             0x00000100
#define USBSS_EP7_RX_EN             0x00000080
#define USBSS_EP6_RX_EN             0x00000040
#define USBSS_EP5_RX_EN             0x00000020
#define USBSS_EP4_RX_EN             0x00000010
#define USBSS_EP3_RX_EN             0x00000008
#define USBSS_EP2_RX_EN             0x00000004
#define USBSS_EP1_RX_EN             0x00000002
#define USBSS_UH_RX_EN              0x00000002

/* UEP0_TX_CTRL */
#define USBSS_UIF_EP0_TX_ACT        0x80000000
#define USBSS_EP0_TX_FLOW           0x02000000
#define USBSS_EP0_TX_PP             0x01000000
#define USBSS_EP0_TX_ERDY           0x00800000
#define USBSS_EP0_TX_RES            0x00600000
 #define USBSS_EP0_TX_NRDY          0x00000000
 #define USBSS_EP0_TX_DPH           0x00200000
 #define USBSS_EP0_TX_STALL         0x00400000
#define USBSS_EP0_TX_LEN_MASK       0x000007FF

/* UEP0_RX_CTRL */
#define USBSS_UIF_EP0_RX_ACT        0x80000000
#define USBSS_EP0_RX_PP             0x01000000
#define USBSS_EP0_RX_ERDY           0x00800000
#define USBSS_EP0_RX_RES            0x00600000
 #define USBSS_EP0_RX_NRDY          0x00000000
 #define USBSS_EP0_RX_ACK           0x00200000
 #define USBSS_EP0_RX_STALL         0x00400000
#define USBSS_EP0_RX_SEQ_MASK       0x001F0000
#define USBSS_EP0_RX_LEN_MASK       0x000007FF

/* R8_UEPn_TX_CFG */
#define USBSS_EP_TX_CHAIN_AUTO      0x80
#define USBSS_EP_TX_FIFO_MODE       0x40
#define USBSS_EP_TX_FIFO_CFG        0x20
#define USBSS_EP_TX_EOB_MODE        0x08
#define USBSS_EP_TX_ERDY_AUTO       0x04
#define USBSS_EP_TX_SEQ_AUTO        0x02
#define USBSS_EP_TX_ISO_MODE        0x01

/* R8_UEPn_TX_CR */
#define USBSS_EP_TX_HALT            0x80
#define USBSS_EP_TX_CLR             0x40
#define USBSS_EP_TX_CHAIN_CLR       0x20
#define USBSS_EP_TX_ERDY_NUMP_MASK  0x1F


/* R8_UEPn_TX_SEQ */
#define USBSS_EP_TX_SEQ_NUM_MASK    0x1F

/* R8_UEPn_TX_ST */
#define USBSS_EP_TX_INT_FLAG        0x80
#define USBSS_EP_TX_FC_ST           0x40
#define USBSS_EP_TX_ERDY_REQ        0x20
#define USBSS_EP_TX_CHAIN_RES       0x10
#define USBSS_EP_TX_CHAIN_EN_MASK   0x0F

/* R8_UEPn_TX_CHAIN_CR */
#define USBSS_EP_TX_CUR_USE         0xC0               
#define USBSS_EP_TX_CUR_CFG         0x30               
#define USBSS_EP_TX_FORCE_RET       0x04             
#define USBSS_EP_TX_RET_SEL         0x03               

/* R8_UEPn_TX_CHAIN_ST */
#define USBSS_EP_TX_CHAIN_EN        0x80
#define USBSS_EP_TX_CHAIN_IF        0x40
#define USBSS_EP_TX_EOB_LPF         0x20
#define USBSS_EP_TX_NUMP_EMPTY      0x08
#define USBSS_EP_TX_DPH_PP          0x04
#define USBSS_EP_TX_CHAIN_NO_MASK   0x03

/* R8_UEPn_RX_CFG */
#define USBSS_EP_RX_CHAIN_AUTO      0x80
#define USBSS_EP_RX_FIFO_MODE       0x40
#define USBSS_EP_RX_FIFO_CFG        0x20
#define USBSS_EP_RX_EOB_MODE        0x08
#define USBSS_EP_RX_ERDY_AUTO       0x04
#define USBSS_EP_RX_SEQ_AUTO        0x02
#define USBSS_EP_RX_ISO_MODE        0x01

/* R8_UEPn_RX_CR */
#define USBSS_EP_RX_HALT            0x80
#define USBSS_EP_RX_CLR             0x40
#define USBSS_EP_RX_CHAIN_CLR       0x20
#define USBSS_EP_RX_ERDY_NUMP_MASK  0x1F

/* R8_UEPn_RX_SEQ */
#define USBSS_EP_RX_SEQ_NUM_MASK    0x1F

/* R8_UEPn_RX_ST */
#define USBSS_EP_RX_INT_FLAG        0x80
#define USBSS_EP_RX_FC_ST           0x40
#define USBSS_EP_RX_ERDY_REQ        0x20
#define USBSS_EP_RX_CHAIN_RES       0x10
#define USBSS_EP_RX_CHAIN_EN_MASK   0x0F

/* R8_UEPn_RX_CHAIN_CR */
#define USBSS_EP_RX_CUR_USE         0xC0               
#define USBSS_EP_RX_CUR_CFG         0x30               
#define USBSS_EP_RX_FORCE_RET       0x04             
#define USBSS_EP_RX_RET_SEL         0x03               

/* R8_UEPn_RX_CHAIN_ST */
#define USBSS_EP_RX_CHAIN_EN        0x80
#define USBSS_EP_RX_CHAIN_IF        0x40
#define USBSS_EP_RX_LPF_FLAG        0x20
#define USBSS_EP_RX_ISO_PKT_ERR     0x10
#define USBSS_EP_RX_NUMP_EMPTY      0x08
#define USBSS_EP_RX_DPH_PP          0x04
#define USBSS_EP_RX_CHAIN_NO_MASK   0x03

/* R32_UH_TX_CTRL */
#define USBSS_UH_TX_ACT             0x80000000
#define USBSS_UH_TX_ISO             0x40000000
#define USBSS_UH_TX_SETUP           0x20000000
#define USBSS_UH_TX_STATUS          0x10000000
#define USBSS_UH_TX_LPF             0x00800000
#define USBSS_UH_TX_RES             0x00600000
 #define USBSS_UH_TX_NRDY           0x00000000
 #define USBSS_UH_TX_ACK            0x00200000
 #define USBSS_UH_TX_STALL          0x00400000
#define USBSS_UH_TX_SEQ             0x001F0000
#define USBSS_UH_TX_EP              0x0000F000
#define USBSS_UH_TX_LEN_MASK        0x000007FF  

/* R32_UH_RX_CTRL */
#define USBSS_UH_RX_ACT             0x80000000
#define USBSS_UH_RX_ISO             0x40000000
#define USBSS_UH_RX_NUMP            0x1F000000
#define USBSS_UH_RX_RES             0x00600000
 #define USBSS_UH_RX_NRDY           0x00000000
 #define USBSS_UH_RX_ACK            0x00200000
 #define USBSS_UH_RX_STALL          0x00400000
#define USBSS_UH_RX_SEQ             0x001F0000
#define USBSS_UH_RX_EP              0x0000F000
#define USBSS_UH_RX_LEN_MASK        0x000007FF   

/* R32_HOST_STATUS */
#define USBSS_UH_ITP_PRESAGE        0x000C0000
#define USBSS_UH_RX_ISO_PKT_ERR     0x00020000
#define USBSS_UH_RX_EOB_LPF         0x00010000
#define USBSS_UH_RX_ERDY_DIR        0x00008000
#define USBSS_UH_RX_ERDY_NUMP       0x00001F00
#define USBSS_UH_RX_ERDY_EP         0x000000F0


#define TOUT_MODE       (1<<21)

#define HUB_DEPTH_MASK      (7<<24)
#define HUB_DEPTH_INVLD     (7<<24)

#define PM_MASK         ((uint32_t)0x00000003)
#define POWER_MODE_0    ((uint32_t)0x00000004)
#define POWER_MODE_1    ((uint32_t)0x00000005)
#define POWER_MODE_2    ((uint32_t)0x00000006)
#define POWER_MODE_3    ((uint32_t)0x00000007)

// #define GO_DISABLED     (1<<4)
// #define GO_INACTIVE     (1<<5)
// #define GO_RECOVERY     (1<<6)
// #define GO_RX_DET       (1<<7)

//#define POLLING_EN          ((uint32_t)1<<12)

// link int en

// link int flag

// USB CONTROL









#define USB_INT_EP_MASK     (0x7<<8)
//#define USB_INT_RES_MASK    (0x3<<14)
//#define USB_RES_ACK         (0x0<<14)
//#define USB_RES_ERDY        (0x1<<14)
//#define USB_RES_NRDY        (0x2<<14)
//#define USB_RES_STALL       (0x3<<14)

#define USB_TX_RES_MASK     (0x3<<22)
#define TX_RES_ACK          (0x0<<22)
#define TX_RES_FAILED       (0x1<<22)
#define TX_RES_NRDY         (0x2<<22)
#define TX_RES_STALL        (0x3<<22)

#define USB_RX_RES_MASK     (0x3<<30)
#define RX_RES_ACK          (0x0<<30)
#define RX_RES_FAILED       (0x1<<30)
#define RX_RES_NRDY         (0x2<<30)
#define RX_RES_STALL        (0x3<<30)

#define USB_SEQ_MASK        (0x1f<<21)

// #define USB_INT_ST_MASK  0xf<<16
// #define USB_SEQ_MATCH        0x1<<16

#define USB_TX_LPF          (0x1<<28)

#define USB_STATUS_IS       (0x1<<29)
#define USB_SETUP_IS        (0x1<<30)
#define USB_TX_DIR          (0x1<<12)


#define USB_ERDY_ENDP_MASK  (0xf<<4)
#define USB_ERDY_NUMP_MASK  (0x1f<<8)
// #define USB_ITP_PREAGE       (0x1<<31)


#define EP0_EN            (1<<0)
#define EP1_EN            (1<<1)
#define EP2_EN            (1<<2)
#define EP3_EN            (1<<3)
#define EP4_EN            (1<<4)
//#define EP5_EN            (1<<5)
//#define EP6_EN            (1<<6)
//#define EP7_EN            (1<<7)

#define EP1_ISO           (1<<17)
#define EP2_ISO           (1<<18)
#define EP3_ISO           (1<<19)
#define EP4_ISO           (1<<20)
#define EP5_ISO           (1<<21)
#define EP6_ISO           (1<<22)
#define EP7_ISO           (1<<23)

#define TX_ERDY_ACT         (1<<0)
#define TX_ERDY_DIR         (1<<1)
#define TX_ERDY_ENDP_0      (0<<2)
#define TX_ERDY_ENDP_1      (1<<2)
#define TX_ERDY_ENDP_2      (2<<2)
#define TX_ERDY_ENDP_3      (3<<2)
#define TX_ERDY_ENDP_4      (4<<2)
#define TX_ERDY_ENDP_5      (5<<2)
#define TX_ERDY_ENDP_6      (6<<2)
#define TX_ERDY_ENDP_7      (7<<2)

#define TX_ERDY_NUMP_0      (0<<6)
#define TX_ERDY_NUMP_1      (1<<6)
#define TX_ERDY_NUMP_2      (2<<6)
#define TX_ERDY_NUMP_3      (3<<6)
#define TX_ERDY_NUMP_4      (4<<6)
#define TX_ERDY_NUMP_5      (5<<6)
#define TX_ERDY_NUMP_6      (6<<6)
#define TX_ERDY_NUMP_7      (7<<6)
#define TX_ERDY_NUMP_8      (8<<6)


// LMP
#define LMP_HP          0
// LMP subtype
#define LMP_SUBTYPE_MASK        (0xf<<5)
#define LMP_SET_LINK_FUNC       (0x1<<5)
#define LMP_U2_INACT_TOUT       (0x2<<5)
#define LMP_VENDOR_TEST         (0x3<<5)
#define LMP_PORT_CAP            (0x4<<5)
#define LMP_PORT_CFG            (0x5<<5)
#define LMP_PORT_CFG_RES        (0x6<<5)

#define LMP_LINK_SPEED          (1<<9)

#define NUM_HP_BUF      (4<<0)
#define DOWN_STREAM     (1<<16)
#define UP_STREAM       (2<<16)
#define TIE_BRK         (1<<20)



#ifdef __cplusplus
}
#endif


#endif /* __ch32h417_USBSS_H */

