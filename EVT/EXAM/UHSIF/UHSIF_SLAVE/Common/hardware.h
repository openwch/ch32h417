/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file contains all the functions prototypes for the 
*                      hardware firmware library.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __HARDWARE_H
#define __HARDWARE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32h417.h"
#include "debug.h"


#define DEF_TX_CHAIN_TIMEOUT              1000000

#define DEF_UHSIF_TXBUF_CNT               8
#define DEF_UHSIF_RXBUF_CNT               8
typedef struct
{
    volatile uint32_t adr;
    volatile uint32_t len;
}pack_t;

typedef struct
{
    volatile uint8_t load;
    volatile uint8_t deal;
    volatile uint8_t total;
    volatile uint8_t stop;
}comm_t;
extern pack_t UHSIF_WR_Pack[ DEF_UHSIF_TXBUF_CNT ];
extern comm_t UHSIF_WR_COMM;
extern pack_t UHSIF_RD_Pack[ DEF_UHSIF_RXBUF_CNT ];
extern comm_t UHSIF_RD_COMM;

extern void UHSIF_Init( void ); 
extern void UHSIF_Para_Init( void ); 
extern void Hardware( void );
extern void UART_Proc( void );
extern void USART1_Init(uint32_t baudrate);
#ifdef __cplusplus
}
#endif

#endif 





