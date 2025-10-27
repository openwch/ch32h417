/*
 * app_net.h
 *
 *  Created on: Sep 9, 2022
 *      Author: TECH66
 */

#ifndef USER_APP_NET_H_
#define USER_APP_NET_H_

#include "wchnet.h"
#include "eth_driver.h"

extern u8 dhcpflag;
void WCHNET_HandleGlobalInt(void);
void WCHNET_CreatTcpSocket (uint32_t srcport, uint32_t desport, uint8_t *DESIP, uint8_t *SocketId);
void WCHNET_CreatUdpSocket (uint32_t srcport, uint8_t *SocketId);
uint8_t TCP_Send (uint8_t socketid, uint8_t *buf, uint32_t len);
uint8_t UDP_Send (uint8_t socketid, uint8_t *buf, uint32_t slen, uint8_t *dstIP, uint16_t port);
void NET_Init (void);

static inline void NET_Process (void) {
    /*Ethernet library main task function,
     * which needs to be called cyclically*/
    WCHNET_MainTask();
    /*Query the Ethernet global interrupt,
     * if there is an interrupt, call the global interrupt handler*/
    if (WCHNET_QueryGlobalInt()) {
        WCHNET_HandleGlobalInt();
    }
}

#endif /* USER_APP_NET_H_ */
