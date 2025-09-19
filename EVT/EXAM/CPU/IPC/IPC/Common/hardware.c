/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/08/14
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

/* Global define */

/* Global Variable */
vu32 cnt = 0;


void IPC_CH0_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      IPC_Config
 *
 * @brief   IPC Configure.
 *
 * @return  none
 */
void IPC_Config(IPC_Channel_TypeDef IPC_CHx, IPC_TxCID_TypeDef IPC_TxCIDx, IPC_RxCID_TypeDef IPC_RxCIDx)
{
	IPC_InitTypeDef  IPC_InitStructure = {0};

    IPC_InitStructure.IPC_CH = IPC_CHx;
    IPC_InitStructure.TxCID = IPC_TxCIDx;
    IPC_InitStructure.RxCID = IPC_RxCIDx;
    IPC_InitStructure.TxIER = ENABLE;
    IPC_InitStructure.RxIER = ENABLE;
    IPC_InitStructure.AutoEN = ENABLE;
	IPC_Init(&IPC_InitStructure);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{
	printf("IPC TEST\r\n");
#ifdef Core_V3F
	IPC_DeInit();
	IPC_Config(IPC_CH0,IPC_TxCID1,IPC_RxCID0);
	IPC_CH0_Lock();
	IPC_WriteMSG(IPC_MSG0,0);
	IPC_SetFlagStatus(IPC_CH0,IPC_CH_Sta_Bit0);
	IPC_ClearFlagStatus(IPC_CH0,IPC_CH_Sta_Bit1);
	NVIC_SetPriority(IPC_CH0_IRQn,0<<7); 
	NVIC_EnableIRQ(IPC_CH0_IRQn);
    Delay_Ms(100);
	IPC_ITConfig(IPC_CH0,IPC_CH_Sta_Bit1,ENABLE);

#else
	NVIC_SetPriority(IPC_CH0_IRQn,0<<5); //V3 interrupt config
	NVIC_EnableIRQ(IPC_CH0_IRQn);

#endif

	while (1){};
}



/*********************************************************************
 * @fn      IPC_CH0_Handler
 *
 * @brief   This function handles IPC_CH0 exception.
 *
 * @return  none
 */
void IPC_CH0_Handler(void)
{
#ifdef Core_V3F
	if (IPC_GetITStatus(IPC_CH0,IPC_CH_Sta_Bit0) != RESET) 
	{
		uint32_t temp = 0;
		temp = IPC_ReadMSG(IPC_MSG0);
		printf("temp - %08x\r\n",temp);
		temp++;
		cnt++;
		IPC_WriteMSG(IPC_MSG0,temp);
	}
	IPC_ITConfig(IPC_CH0,IPC_CH_Sta_Bit0,DISABLE);

	if(cnt < 10)
	{
		IPC_ITConfig(IPC_CH0,IPC_CH_Sta_Bit1,ENABLE);
	}
	else
    {
		printf("IPC test end\r\n");
	}

#else
	if(IPC_GetITStatus(IPC_CH0,IPC_CH_Sta_Bit1) != RESET)
	{
		uint32_t temp = 0;
		temp = IPC_ReadMSG(IPC_MSG0);
		printf("temp - %08x\r\n",temp);
		temp++;
		IPC_WriteMSG(IPC_MSG0,temp);
	}
	IPC_ITConfig(IPC_CH0,IPC_CH_Sta_Bit1,DISABLE);

	IPC_ITConfig(IPC_CH0,IPC_CH_Sta_Bit0,ENABLE);

#endif
}
