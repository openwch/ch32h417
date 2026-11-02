/********************************** (C) COPYRIGHT *******************************
* File Name          : dvp.h
* Author             : WCH
* Version            : V1.2
* Date               : 2025/07/28
* Description        :
*********************************************************************************
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __DVP_H__
#define __DVP_H__

#include "ch32h417.h"
#include "ov2640.h"

/* Global define */

/* Function declaration */
void SCCB_Init(void);
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data);
uint8_t SCCB_RD_Reg(uint8_t reg);
uint8_t SCCB_WR_Reg16(uint16_t reg, uint8_t data);   // 16-bit sub-address (OV5640)
uint8_t SCCB_RD_Reg16(uint16_t reg);
void dvp_Init(void);
void Dvp_Row_Col_Set( uint16_t res_width , uint16_t res_height );
void DVP_Stream_Reconfig(void);
#endif /* DVP_DVP_H_ */
