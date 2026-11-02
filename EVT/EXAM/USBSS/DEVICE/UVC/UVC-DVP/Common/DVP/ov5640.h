/********************************** (C) COPYRIGHT  *******************************
* File Name          : ov5640.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/10/26
* Description        : OV5640 DVP sensor driver (YUY2 only).
*********************************************************************************/
#ifndef __OV5640_H
#define __OV5640_H

#include "ch32h417.h"

/* Identity */
#define OV5640_MID             0x7FA3
#define OV5640_PID             0x5640

/* VCO is 960MHz; only the SYSCLK divider differs. */
#define OV5640_PLL_MULT_HS     0x21   /* sysdiv=2 -> PCLK 48MHz (USB2) */
#define OV5640_PLL_MULT_SS     0x11   /* sysdiv=1 -> PCLK 96MHz (USB3) */

uint8_t OV5640_Detect(void);        /* 1 = OV5640 present */
uint8_t OV5640_Init(void);          /* 0 = ok */
void    OV5640_Set_Pclk_Tier(uint8_t is_hs);
void    OV5640_Format_Mode(uint8_t choice);
void    OV5640_Change_Resolution(uint8_t formatindex, uint8_t frameindex);

#endif /* __OV5640_H */
