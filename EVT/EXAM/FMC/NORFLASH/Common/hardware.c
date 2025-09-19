/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/17
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "FMC_NOR.h"
/* Global define */
/* Global Variable */
uint16_t TxBuffer[BUFFER_SIZE];
uint16_t RxBuffer[BUFFER_SIZE]={1,11,2,22,3,33,4,77};
volatile uint32_t WriteReadStatus = 0, Index = 0;
/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
    FMC_NorFlash_Init();
    NOR_ReadID();
    NOR_ReturnToReadMode();
    NOR_EraseBlock(WRITE_READ_ADDR);
    Fill_Buffer(TxBuffer, BUFFER_SIZE, 0x05);
    NOR_WriteBuffer(TxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    NOR_ReadBuffer(RxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);
    for (Index = 0x00; (Index < BUFFER_SIZE) && (WriteReadStatus == 0); Index++)
    {
        if (RxBuffer[Index] != TxBuffer[Index])
        {
            WriteReadStatus = Index + 1;
            printf("RxBuffer[%d]=%x\r\n",Index,RxBuffer[Index]);
            printf("TxBuffer[%d]=%x\r\n",Index,TxBuffer[Index]);
        }
    }

    if (WriteReadStatus == 0)
    {
        /* OK */
        printf("sucess\r\n");
    }
    else
    {
        /* KO */
        printf("error\r\n");
    }
    while(1);
}
