/********************************** (C) COPYRIGHT  *******************************
* File Name          : i2c_host.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/08/24
* Description        : SCCB host over I2C2 (8/16-bit sub-address).
*********************************************************************************/
#ifndef __I2C_HOST_H
#define __I2C_HOST_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

void i2c_host_init(void);

int SCCB_Read (uint8_t slv_addr, uint8_t  reg, uint8_t *out);
int SCCB_Write(uint8_t slv_addr, uint8_t  reg, uint8_t data);

int SCCB_Read16 (uint8_t slv_addr, uint16_t reg, uint8_t *out);
int SCCB_Write16(uint8_t slv_addr, uint16_t reg, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_HOST_H */
