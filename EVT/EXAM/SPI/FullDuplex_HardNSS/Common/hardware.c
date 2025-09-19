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
#include "string.h"

/* SPI Mode Definition */
#define HOST_MODE   0
#define SLAVE_MODE   1

/* SPI Communication Mode Selection */
// #define SPI_MODE   HOST_MODE
#define SPI_MODE   SLAVE_MODE

/* Global define */
#define Size 18

/* Global Variable */

u16 TxData[Size] = { 0x0101, 0x0202, 0x0303, 0x0404, 0x0505, 0x0606,
                     0x1111, 0x1212, 0x1313, 0x1414, 0x1515, 0x1616,
                     0x2121, 0x2222, 0x2323, 0x2424, 0x2525, 0x2626 };
u16 RxData[Size];

/*********************************************************************
 * @fn      SPI_FullDuplex_Init
 *
 * @brief   Configuring the SPI for full-duplex communication.
 *
 * @return  none
 */
void SPI_FullDuplex_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	SPI_InitTypeDef SPI_InitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC , ENABLE );
	RCC_HB1PeriphClockCmd( RCC_HB1Periph_SPI2, ENABLE );	

#if (SPI_MODE == HOST_MODE)
	// NSS PB12(AF5)
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// SCK PB13(AF5)
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// MOSI PC1(AF5)
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// MISO PC2(AF5)
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
#elif (SPI_MODE == SLAVE_MODE)
	// NSS PB12(AF5)
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// SCK PB13(AF5)
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// MOSI PC1(AF5)
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// MISO PC2(AF5)
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

#endif

#if (SPI_MODE == HOST_MODE)
	SPI_SSOutputCmd( SPI2, ENABLE );

#endif

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;

#if (SPI_MODE == HOST_MODE)
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

#elif (SPI_MODE == SLAVE_MODE)
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;

#endif

	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode6;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init( SPI2, &SPI_InitStructure );

	SPI_Cmd( SPI2, ENABLE );
}


/*********************************************************************
 * @fn      Hardware
 *
 * @brief   main.
 *
 * @return  none
 */
void Hardware(void)
{
	uint32_t i = 0,j = 0;
	uint32_t value;
#if (SPI_MODE == SLAVE_MODE)
  printf("Slave Mode\r\n");
	Delay_Ms(1000);

#endif

	SPI_FullDuplex_Init();

#if (SPI_MODE == HOST_MODE)
    printf("Host Mode\r\n");
	Delay_Ms(2000);

#endif

	while(1)
	{
		while( ( i<18 ) || ( j<18 ))
		{
			if( i<18 )
			{
				if( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_TXE ) != RESET )
				{
					SPI_I2S_SendData( SPI2, TxData[i] );
					i++;
				}
			}

			if( j<18 )
			{
				if( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_RXNE ) != RESET )
				{
					RxData[j] = SPI_I2S_ReceiveData( SPI2 );
					j++;
				}
			}
		}
		for (uint32_t t = 0; t < Size; t++)
		{
			printf("tx %04x rx %04x\n", TxData[t], RxData[t]);
		}

		value = memcmp( TxData, RxData, Size );

		if( value == 0 )
		{
			printf( "Same\r\n" );
		}
		else
		{
			printf( "Different\r\n" );
		}

		while(1);
	}

	while (1)
		;
}
