#ifndef __APP_IOCHUB_H
#define __APP_IOCHUB_H

#include "wchiochub.h"
#include "iochub_config.h"

#ifdef CH32V30X
#include "ch32v30x.h"
#endif
#ifdef CH32V20X
#include "ch32v20x.h"
#endif

#define USART_REC_LEN 128


extern uint8_t localDeviceID[8];
extern uint8_t logindeviceSecret[16];
extern uint8_t wchIochubSerIp[4];
extern uint16_t wchIochubSerPort; 
extern uint16_t wchIochubScrPort;

extern uint8_t IocHub_VER[3];
extern uint8_t IocHub_DeviceID[8];
extern uint8_t IocHub_NATType;
extern uint8_t IocHub_NETType;
extern volatile uint8_t sessionopenflag;

#if LAN_SCAN_FUNC_ENABLE
void WCHIOCHUB_lanScanCallBackP(uint8_t num,IOCHUB_LAN_DEVICE_INFO*lanDeviceList);
#endif

void WCHIOCHUB_regCallback(uint16_t state);
void AT_Process(void);
void WCHIOCHUB_TimeIsr(void);
void IoCHub_Init (void);
void WCHIOCHUB_StartEn (void);
void WCHIOCHUB_DataLoopbackInit (void);
void WCHIOCHUB_DataLoopback (void);
#endif
