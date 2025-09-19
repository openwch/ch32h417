/********************************** (C) COPYRIGHT *******************************
 * File Name          : usbd_compatibility_hid.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/07/31
 * Description        : header file of usbd_compatibility_hid.c
*********************************************************************************
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

extern uint8_t HID_Report_Buffer[DEF_USBD_HS_PACK_SIZE + 1];               // HID Report Buffer
extern volatile uint16_t Data_Pack_Max_Len ;                            // UART data packet length in hid packet
extern volatile uint16_t Head_Pack_Len;                                 // UART head packet( valid data length ) length in hid packet
extern volatile uint8_t HID_Set_Report_Flag;
extern void UART_Tx_Service( void );
extern void UART_Rx_Service( void );
extern void UART_DMA_Init( void );
extern void UART_Init( void );
extern void TIM2_Init( void );
extern void HID_Set_Report_Deal( void );

#endif /* USER_USBD_COMPATIBILITY_HID_H_ */
