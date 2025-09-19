/********************************** (C) COPYRIGHT *******************************
 * File Name  :usbd_compatibility_hid.h
 * Author     :OWNER
 * Version    : v0.01
 * Date       : 2022Äê7ÔÂ8ÈÕ
 * Description:
*******************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef USER_USBD_COMPATIBILITY_HID_H_
#define USER_USBD_COMPATIBILITY_HID_H_

#include "usb_desc.h"
#define DEF_UART_BUF_SIZE               8192
#define DEF_UART_TOUT_TIME              30             // NOTE: the timeout time should be set according to the actual baud rate.

#define SET_REPORT_DEAL_OVER            0x00
#define SET_REPORT_WAIT_DEAL            0x01


extern uint8_t  HID_Report_Buffer[64];               // HID Report Buffer
extern volatile uint8_t HID_Set_Report_Flag;

extern void UART_Tx_Service( void );
extern void UART_Rx_Service( void );
extern void UART_DMA_Init( void );
extern void UART_Init( void );
extern void TIM2_Init( void );
extern void HID_Set_Report_Deal( void );

#endif /* USER_USBD_COMPATIBILITY_HID_H_ */
