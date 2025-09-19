/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "shared.h"
/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      GPIO_Toggle_INIT
 *
 * @brief   Initializes GPIOA.0.
 *
 * @return  none
 */
void GPIO_Toggle_INIT(void)
{	
    GPIO_InitTypeDef GPIO_InitStructure = {0};
	//PA0
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
}

/*********************************************************************
 * @fn      Reset_Sharing_Data
 *
 * @brief   Initializes Sharing_Data.
 *
 * @return  none
 */
void Reset_Sharing_Data(void)
{
	for(int i = 0;i<4;i++)
	{
		Buffer_Sharing[i] = i;
	}
	Data_Sharing = 0xFFFF0000;
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
	printf("HSEM_Core data sharing TEST\r\n");
	printf("V3 TEST\r\n");
	GPIO_Toggle_INIT();

#ifdef Core_V3F
	Reset_Sharing_Data();

	while(1)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_0);
		if(HSEM_FastTake(HSEM_ID0) == READY){
		for(u8 t=0; t<4; t++){
			printf("Buffer_Sharing[%d] - %d\r\n",t,Buffer_Sharing[t]);
			Buffer_Sharing[t] += 1;
			Delay_Ms(100);
		}
		printf("Data_Sharing - %x\r\n",Data_Sharing);
		Data_Sharing += 1;
		HSEM_ReleaseOneSem(HSEM_ID0, 0);
		Delay_Ms(10);

		GPIO_ResetBits(GPIOA,GPIO_Pin_0);
		Delay_Ms(100);
		
		}
	}

#else
	printf("V5 TEST\r\n");

	while(1)
	{
		if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_0) == RESET)
		{
			if(HSEM_FastTake(HSEM_ID0) == READY){
			for(u8 t=0; t<4; t++){
				printf("Buffer_Sharing[%d] - %d\r\n",t,Buffer_Sharing[t]);
				Buffer_Sharing[t] += 1;
				Delay_Ms(100);
			}
			printf("Data_Sharing - %x\r\n",Data_Sharing);
			Data_Sharing += 1;
			HSEM_ReleaseOneSem(HSEM_ID0, 0);
			Delay_Ms(10);
			}
		}
	}

#endif

}
