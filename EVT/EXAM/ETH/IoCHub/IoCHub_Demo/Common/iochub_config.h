/********************************** (C) COPYRIGHT *******************************
* File Name          : iochub_config.h
* Author             : WCH
* Version            : V1.2.1
* Date               : 2025/05/27
* Description        : This file contains the configurations of 
*                      Ethernet protocol stack library
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __IOCHUB_CONFIG_H__
#define __IOCHUB_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CONNECT_NUM							(1UL)    	/*The number of sessions configured by the user themselves*/
#define IOCHUB_WND_TIMEOUT						(20)		/* uint:ms */
#define IOCHUB_WND_SIZE							(20UL)
#define IOCHUB_FRAME_SIZE						(WCHNET_TCP_MSS)
#define IOCHUB_POOL_BUF_SIZE					(IOCHUB_FRAME_SIZE*6)
#define IOCHUB_CMD_BUF_SIZE						(IOCHUB_FRAME_NUM*IOCHUB_FRAME_SIZE)
#define IOCHUB_PKTLEN_SIZE						(IOCHUB_PKTLEN_NUM*IOCHUB_WND_SIZE)
#define IOCHUB_SEND_BUF_SIZE					(IOCHUB_FRAME_SIZE*(1+IOCHUB_WND_SIZE))
#define IOCHUB_RECV_BUF_SIZE					(IOCHUB_FRAME_SIZE*10)
#define	IOCHUB_HDLTABLE_SIZE					(MAX_CONNECT_NUM*(IOCHUB_HDLSIZE+IOCHUB_PKTLEN_SIZE+IOCHUB_SEND_BUF_SIZE+IOCHUB_RECV_BUF_SIZE))
#define IOCHUB_MEMP_SIZE						(IOCHUB_HDLTABLE_SIZE+IOCHUB_POOL_BUF_SIZE+IOCHUB_CMD_BUF_SIZE)

#define IOC_LAN_DEVICE_NUM						(3UL)

#ifdef __cplusplus
}
#endif
#endif