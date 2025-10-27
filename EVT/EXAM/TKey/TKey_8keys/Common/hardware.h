/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file contains all the functions prototypes for the 
*                      hardware.
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
#define TKY_FILTER_MODE     			            3
#define TKY_FILTER_GRADE     			            2
#define TKY_BASE_REFRESH_ON_PRESS     			    0
#define TKY_BASE_UP_REFRESH_DOUBLE     			    0
#define TKY_BASE_DOWN_REFRESH_SLOW     			    0
#define TKY_BASE_REFRESH_SAMPLE_NUM     			100
#define TKY_SHIELD_EN     			                1
#define TKY_SINGLE_PRESS_MODE     			        0
#define TKY_MAX_QUEUE_NUM     			            8  

typedef enum _TKY_QUEUE_ID {
    TKY_QUEUE_0 = 0,
    TKY_QUEUE_1,
    TKY_QUEUE_2,
    TKY_QUEUE_3,
    TKY_QUEUE_4,
    TKY_QUEUE_5,
    TKY_QUEUE_6,
    TKY_QUEUE_7,
    TKY_QUEUE_8,
    TKY_QUEUE_9,
    TKY_QUEUE_10,
    TKY_QUEUE_11,
    TKY_QUEUE_12,
    TKY_QUEUE_13,
    TKY_QUEUE_14,
    TKY_QUEUE_15,
    /* Add new above this */
    TKY_QUEUE_END
} TKY_QUEUE_ID;

#define GEN_TKY_CH_INIT(qNum,chNum,chTime,disChTime,slpmode,chBaseline,trs,trs2) \
    {\
     .queueNum=qNum,.channelNum=chNum,\
     .chargeTime=chTime,\
     .disChargeTime=disChTime,\
     .sleepStatus=slpmode,\
     .baseLine = chBaseline,\
     .threshold=trs,\
     .threshold2=trs2\
    }

#define TKY_CHS_INIT                                                 \
        GEN_TKY_CH_INIT (TKY_QUEUE_0,  0,  3, 0x1ff, 0, 2621, 120, 110), \
        GEN_TKY_CH_INIT (TKY_QUEUE_1,  1,  3, 0x1ff, 0, 2600, 120, 110), \
        GEN_TKY_CH_INIT (TKY_QUEUE_2,  2,   3, 0x1ff, 0, 2360, 120, 110), \
        GEN_TKY_CH_INIT (TKY_QUEUE_3,  3,   3, 0x1ff, 0, 2233, 120, 110), \
        GEN_TKY_CH_INIT (TKY_QUEUE_4,  4,  3, 0x1ff, 0, 2604, 120, 110), \
        GEN_TKY_CH_INIT (TKY_QUEUE_5,  5,  3, 0x1ff, 0, 2604, 120, 110), \
        GEN_TKY_CH_INIT (TKY_QUEUE_6,  6,   3, 0x1ff, 0, 2604, 110, 100), \
        GEN_TKY_CH_INIT (TKY_QUEUE_7,  7,   3, 0x1ff, 0, 2604, 100, 90)


void Hardware(void);

#ifdef __cplusplus
}
#endif

#endif 





