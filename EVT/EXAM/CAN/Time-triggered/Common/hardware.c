/********************************** (C) COPYRIGHT  *******************************
 * File Name          : hardware.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/09/16
 * Description        : This document demonstrates the use of TTCAN.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "hardware.h"

/* CAN Mode Definition */
#define TX_MODE 0
#define RX_MODE 1

/* CAN Communication Mode Selection */
// #define CAN_MODE   TX_MODE
#define CAN_MODE RX_MODE

#define USE_SOFT_FILTER

#ifndef USE_SOFT_FILTER
#warning "The chips of version-A are not support hardware filter."
#endif

#define CANSOFTFILTER_MAX_GROUP_NUM                                                                                    \
    2 // The maximum recommended configuration is 14.
      // Configure only what you need to prevent excessive RAM usage or an increase in the software's filtering time.

#define CANSOFTFILER_PREDEF_CTRLBYTE_MASK32 ((CAN_FilterScale_32bit << 5) | (CAN_FilterMode_IdMask << 1))
#define CANSOFTFILER_PREDEF_CTRLBYTE_ID32 ((CAN_FilterScale_32bit << 5) | (CAN_FilterMode_IdList << 1))

#define GET_PIN_BIT(x) ((uint16_t)0x1 << x)

/**
 * @struct CANFilterStruct_t
 * @note   This is the structure of the software filtering table. It can be configured through the CAN_SoftFilterInit
 * function, or you can directly set the configuration values. The configured values can be modified directly during
 * runtime. However, when using the interrupt mode for reception, you need to be aware that if the modification is
 * interrupted, it may affect the filtering results during this period.
 */
struct CANFilterStruct_t
{
    union {
        union {
            struct
            {
                uint32_t : 1;
                uint32_t RTR : 1;
                uint32_t IDE : 1;
                uint32_t ExID : 29;
            } Access_Ex;
            struct
            {
                uint32_t : 1;
                uint32_t RTR : 1;
                uint32_t IDE : 1;
                uint32_t : 18;
                uint32_t StID : 11;
            } Access_St;
        };
        union {
            struct
            {
                uint16_t FR_16_L;
                uint16_t FR_16_H;
            };
            uint32_t FR_32;
        };
    } FR[2];
    union {
        struct
        {
            uint16_t en : 1;
            uint16_t mode : 4;
            uint16_t scale : 3;
        };
        uint16_t ctrl_byte;
    };
} CANFilterStruct[CANSOFTFILTER_MAX_GROUP_NUM];

/**
 * @struct CANPinTable_t
 * @note   CAN pin table structure
 */
typedef struct
{
    CAN_TypeDef *CANx;
    GPIO_TypeDef *PORT;
    uint8_t Pin;
    uint8_t AF;
} CANPinTable_t;

/**
 * @brief  CAN Tx pin table
 * @note   CAN1: PA12, PB7, PB9, PD1, PA14
 *         CAN2: PB13, PB6
 *         CAN3: PD13, PF7, PF3, PC5
 */
const CANPinTable_t CANTXPinTable[] = {
    {CAN1, GPIOA, 12, GPIO_AF9}, {CAN1, GPIOB, 7, GPIO_AF3}, {CAN1, GPIOB, 9, GPIO_AF9}, {CAN1, GPIOD, 1, GPIO_AF9},
    {CAN1, GPIOA, 14, GPIO_AF5},

    {CAN2, GPIOB, 13, GPIO_AF9}, {CAN2, GPIOB, 6, GPIO_AF9},

    {CAN3, GPIOD, 13, GPIO_AF5}, {CAN3, GPIOF, 7, GPIO_AF2}, {CAN3, GPIOF, 3, GPIO_AF2}, {CAN3, GPIOC, 5, GPIO_AF6},
};
/**
 * @brief  CAN Rx pin table
 * @note   CAN1: PA11, PB6, PB8, PD0, PA13
 *         CAN2: PB12, PB5
 *         CAN3: PD12, PF6, PF4, PC4
 */
const CANPinTable_t CANRXPinTable[] = {
    {CAN1, GPIOA, 11, GPIO_AF9}, {CAN1, GPIOB, 6, GPIO_AF3}, {CAN1, GPIOB, 8, GPIO_AF9}, {CAN1, GPIOD, 0, GPIO_AF9},
    {CAN1, GPIOA, 13, GPIO_AF5},

    {CAN2, GPIOB, 12, GPIO_AF9}, {CAN2, GPIOB, 5, GPIO_AF9},

    {CAN3, GPIOD, 12, GPIO_AF5}, {CAN3, GPIOF, 6, GPIO_AF2}, {CAN3, GPIOF, 4, GPIO_AF2}, {CAN3, GPIOC, 4, GPIO_AF6},
};

volatile int CAN2FilterStartBank = 14, CAN3FilterStartBank = 28;

volatile uint8_t TxBuff[8], RxBuff[8];

uint8_t mbox;

/*********************************************************************
 * @fn      CAN_Mode_Init
 *
 * @brief   CAN mode initialization
 *
 * @param   CANx: CAN1 or CAN2
 *          RxTab: CAN Rx pin table
 *          TxTab: CAN Tx pin table
 *          CAN_InitSturcture: CAN initialization structure
 *
 * @return  ErrorStatus: READY or NoREADY
 */
ErrorStatus CAN_Mode_Init(CAN_TypeDef *CANx, const CANPinTable_t *RxTab, const CANPinTable_t *TxTab,
                          CAN_InitTypeDef *CAN_InitSturcture)
{
    GPIO_InitTypeDef GPIO_InitSturcture = {0};

    if (CANx != RxTab->CANx || CANx != TxTab->CANx)
    {
        return NoREADY;
    }

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO | (RCC_HB2Periph_GPIOA << (((void *)(RxTab->PORT) - (void *)GPIOA) / sizeof(GPIO_TypeDef))) |
                              (RCC_HB2Periph_GPIOA << ((TxTab->PORT - GPIOA) / sizeof(GPIO_TypeDef))),
                          ENABLE);

    RCC_HB1PeriphClockCmd(((CANx == CAN1)   ? RCC_HB1Periph_CAN1
                           : (CANx == CAN2) ? RCC_HB1Periph_CAN2
                                            : RCC_HB1Periph_CAN3) |
                              RCC_HB1Periph_CAN1,
                          ENABLE);

    GPIO_PinAFConfig(RxTab->PORT, RxTab->Pin, RxTab->AF);
    GPIO_PinAFConfig(TxTab->PORT, TxTab->Pin, TxTab->AF);

    GPIO_InitSturcture.GPIO_Pin = GET_PIN_BIT(TxTab->Pin);
    GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(TxTab->PORT, &GPIO_InitSturcture);

    GPIO_InitSturcture.GPIO_Pin = GET_PIN_BIT(RxTab->Pin);
    GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(RxTab->PORT, &GPIO_InitSturcture);

    CAN_Init(CANx, CAN_InitSturcture);

    CAN_TTComModeCmd(CAN1, ENABLE);

    return READY;
}

/*********************************************************************
 * @fn      CAN_SoftFilterInit
 *
 * @brief   CAN soft filter initialization
 *
 * @param   CAN_FilterInitStruct: CAN filter initialization structure
 *
 * @return  None
 *
 */
void CAN_SoftFilterInit(CAN_FilterInitTypeDef *CAN_FilterInitStruct)
{
    if (CAN_FilterInitStruct->CAN_FilterNumber > CANSOFTFILTER_MAX_GROUP_NUM)
    {
        return;
    }
    if (CAN_FilterInitStruct->CAN_FilterActivation)
    {
        CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].en = 1;
    }
    else
    {
        CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].en = 0;
    }

    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[0].FR_16_H = CAN_FilterInitStruct->CAN_FilterIdHigh;
    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[0].FR_16_L = CAN_FilterInitStruct->CAN_FilterIdLow;
    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[1].FR_16_H = CAN_FilterInitStruct->CAN_FilterMaskIdHigh;
    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].FR[1].FR_16_L = CAN_FilterInitStruct->CAN_FilterMaskIdLow;
    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].mode = CAN_FilterInitStruct->CAN_FilterMode;
    CANFilterStruct[CAN_FilterInitStruct->CAN_FilterNumber].scale = CAN_FilterInitStruct->CAN_FilterScale;
}

/*********************************************************************
 * @fn     	CAN_Send_Msg
 *
 * @brief   	CAN Transmit function.
 *
 * @param   	CANx - where x can be 1...3 to select the CAN peripheral.
 *		   	IdType - The target ID type for sending the message.
 *				- CAN_Id_Standard
 *				- CAN_Id_Extended
 *		   	Identifier - The target ID for sending the message.
 *		   	msg - Transmit data buffer.
 *          	len - Data length.
 *
 * @return  	0 - Send successful.
 *          	1 - Send failed.
 */
uint8_t CAN_Send_Msg(CAN_TypeDef *CANx, uint32_t IdType, uint32_t Identifier, uint8_t *msg, uint8_t len)
{

    uint16_t i = 0;

    CanTxMsg CanTxStructure = {0};

    IdType == CAN_Id_Extended ? (CanTxStructure.ExtId = Identifier) : (CanTxStructure.StdId = Identifier);
    CanTxStructure.IDE = IdType;
    CanTxStructure.RTR = CAN_RTR_Data;
    CanTxStructure.DLC = len;

    for (i = 0; i < len; i++)
    {
        CanTxStructure.Data[i] = msg[i];
    }

    mbox = CAN_Transmit(CANx, &CanTxStructure);
    i = 0;

    while ((CAN_TransmitStatus(CANx, mbox) != CAN_TxStatus_Ok) && (i < 0xFFF))
    {
        i++;
    }

    if (i == 0xFFF)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*********************************************************************
 * @fn      	CAN_SoftSlaveStartBank
 *
 * @brief   	This function applies only to CH32 Connectivity line devices.
 *
 * @param		CANx - where x can be 1 to select the CAN peripheral.
 *				CAN_BankNumber - Select the start slave bank filter from 1...size of CANFilterStruct
 *
 * @return  	none
 */
void CAN_SoftSlaveStartBank(CAN_TypeDef *CANx, uint8_t CAN_BankNumber)
{
    if (CANx == CAN2)
    {
        CAN2FilterStartBank = CAN_BankNumber;
    }
    else if (CANx == CAN3)
    {
        CAN3FilterStartBank = CAN_BankNumber;
    }
}

/*********************************************************************
 * @fn      	CAN_ReceiveViaSoftFilter
 *
 * @brief   	Receives a message via soft filter.
 *
 * @param   	CANx - where x can be 1 to select the CAN peripheral.
 *          	FIFONumber - Receive FIFO number.
 *        			- CAN_FIFO0.
 *          	RxMessage -  pointer to a structure receive message which contains
 *        CAN Id, CAN DLC, CAN datas and FMI number.
 *
 * @return  	none
 */
void CAN_ReceiveViaSoftFilter(CAN_TypeDef *CANx, uint8_t FIFONumber, CanRxMsg *RxMessage)
{
    int group, start_bank, end_bank;
    if (CANx == CAN1)
    {
        start_bank = 0;
        end_bank = CAN2FilterStartBank;
    }
    else if (CANx == CAN2)
    {
        start_bank = CAN2FilterStartBank;
        end_bank = CAN3FilterStartBank;
    }
    else if (CANx == CAN3)
    {
        start_bank = CAN3FilterStartBank;
        end_bank = CANSOFTFILTER_MAX_GROUP_NUM;
    }
    else
    {
        return;
    }

    for (group = start_bank; group < end_bank; group++)
    {
        if (CANFilterStruct[group].en)
        {
            uint32_t temp = CANx->sFIFOMailBox[0].RXMIR & (~0x1);
            switch ((uint8_t)CANFilterStruct[group].ctrl_byte & ~0x1)
            {

            case CANSOFTFILER_PREDEF_CTRLBYTE_ID32:
                if ((CANFilterStruct[group].FR[0].FR_32 != temp) && (CANFilterStruct[group].FR[1].FR_32 != temp))
                {
                    continue;
                }
                else
                {
                    CAN_Receive(CANx, CAN_FIFO0, RxMessage);
                    return;
                }
                break;

            case CANSOFTFILER_PREDEF_CTRLBYTE_MASK32:
                if ((CANFilterStruct[group].FR[0].FR_32 & CANFilterStruct[group].FR[1].FR_32) ^
                    (temp & CANFilterStruct[group].FR[1].FR_32))
                {
                    continue;
                }
                else
                {
                    CAN_Receive(CANx, CAN_FIFO0, RxMessage);
                    return;
                }
                break;

            default:
                return;
                break;
            }
        }
    }
    CAN_FIFORelease(CANx, CAN_FIFO0);
}

/*********************************************************************
 * @fn      CAN_Receive_Msg
 *
 * @brief   CAN Receive function.
 *
 * @param   CANx - where x can be 1...3 to select the CAN peripheral.
 *			RxBuff - Pointer to the memory used to store received data.
 *
 * @return  CanRxStructure.DLC - Receive data length.
 */
uint8_t CAN_Receive_Msg(CAN_TypeDef *CANx, uint8_t *RxBuff)
{
    u8 i;

    CanRxMsg CanRxStructure = {0};

    if (CAN_MessagePending(CANx, CAN_FIFO0) == 0)
    {
        return 0;
    }
#ifdef USE_SOFT_FILTER
    CAN_ReceiveViaSoftFilter(CANx, CAN_FIFO0, &CanRxStructure);
#else
    CAN_Receive(CANx, CAN_FIFO0, &CanRxStructure);
#endif
    for (i = 0; i < 8; i++)
    {
        RxBuff[i] = CanRxStructure.Data[i];
    }

    return CanRxStructure.DLC;
}

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Test the Time triggerd function
 *
 * @return  none
 */
void Hardware(void)
{
    __attribute__((unused)) uint32_t Circle_CNT = 0;
    CAN_FilterInitTypeDef CAN_FilterInitSturcture = {0};
    CAN_FilterInitSturcture.CAN_FilterNumber = 0;
#if (Frame_Format == Standard_Frame)
    /* identifier/mask mode, One 32-bit filter, StdId: 0x317 */
    CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x62E0;
    CAN_FilterInitSturcture.CAN_FilterIdLow = 0;
    CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0xFFE0;
    CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0x0006;
    /* identifier list mode, One 32-bit filter, StdId: 0x317,0x316 */
//	CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdList;
//	CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;
//	CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x62E0;
//	CAN_FilterInitSturcture.CAN_FilterIdLow = 0;
//	CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0x62C0;
//	CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0;
#elif (Frame_Format == Extended_Frame)
    /* identifier/mask mode, One 32-bit filter, ExtId: 0x12124567 */
    CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x9092;
    CAN_FilterInitSturcture.CAN_FilterIdLow = 0x2B3C;
    CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0xFFFF;
    CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0xFFFE;
#endif

    CAN_FilterInitSturcture.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitSturcture.CAN_FilterActivation = ENABLE;

    CAN_InitTypeDef CAN_InitSturcture = {0};
    CAN_InitSturcture.CAN_TTCM = ENABLE;
    CAN_InitSturcture.CAN_ABOM = DISABLE;
    CAN_InitSturcture.CAN_AWUM = DISABLE;
    CAN_InitSturcture.CAN_NART = ENABLE;
    CAN_InitSturcture.CAN_RFLM = DISABLE;
    CAN_InitSturcture.CAN_TXFP = DISABLE;
    CAN_InitSturcture.CAN_Mode = CAN_Mode_LoopBack;
    CAN_InitSturcture.CAN_SJW = CAN_SJW_1tq;
    CAN_InitSturcture.CAN_BS1 = CAN_BS1_6tq;
    CAN_InitSturcture.CAN_BS2 = CAN_BS2_5tq;
    CAN_InitSturcture.CAN_Prescaler = 16;
    CAN_Mode_Init(CAN1, CANRXPinTable, CANTXPinTable, &CAN_InitSturcture);

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
    NVIC_EnableIRQ(CAN1_RX0_IRQn);
    CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
    NVIC_EnableIRQ(CAN1_TX_IRQn);

#ifdef USE_SOFT_FILTER
    CAN_SoftFilterInit(&CAN_FilterInitSturcture);
#else
    CAN_FilterInit(&CAN_FilterInitSturcture);
#endif
    while (1)
    {
#if (CAN_MODE == TX_MODE)
        for (uint8_t i = 0; i < 8; i++)
        {
            TxBuff[i] = (Circle_CNT + i) % 256;
        }
        CAN_Send_Msg(CAN1, CAN_ID_STD, 0x317, (uint8_t *)TxBuff, 8);
        Circle_CNT++;
        Delay_Ms(500);
#endif
    }
}

__attribute__((interrupt("WCH-Interrupt-fast"))) void CAN1_RX0_IRQHandler()
{
    uint8_t px;
    if (CAN_GetITStatus(CAN1, CAN_IT_FMP0))
    {
        px = CAN_Receive_Msg(CAN1, (uint8_t *)RxBuff);
        printf("Receive data: ");
        for (int i = 0; i < px; i++)
        {
            printf("%#04x ", RxBuff[i]);
        }
        printf("\n");
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    }
}

__attribute__((interrupt("WCH-Interrupt-fast"))) void CAN1_TX_IRQHandler()
{
    if (CAN_GetITStatus(CAN1, CAN_IT_TME))
    {
        printf("Sent data: ");
        for (int i = 0; i < (CAN1->sTxMailBox[mbox].TXMDTR & 0xff); i++)
        {
            printf("%#04x ", TxBuff[i]);
        }
        printf("\n");
        CAN_ClearITPendingBit(CAN1, CAN_IT_TME);
    }
}
