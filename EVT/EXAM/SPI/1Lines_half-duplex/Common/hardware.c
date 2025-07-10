/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the CRC firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"




/* Global define */

/* SPI Mode Definition */
#define HOST_MODE   0
#define SLAVE_MODE   1

/* SPI Communication Mode Selection */
#define SPI_MODE   HOST_MODE
// #define SPI_MODE   SLAVE_MODE

#define Size 18


/* Global Variable */
volatile u8 Txval=0, Rxval=0;

u16 TxData[Size] = { 0x0101, 0x0202, 0x0303, 0x0404, 0x0505, 0x0606,
                     0x1111, 0x1212, 0x1313, 0x1414, 0x1515, 0x1616,
                     0x2121, 0x2222, 0x2323, 0x2424, 0x2525, 0x2626 };
u16 RxData[Size];

/*********************************************************************
 * @fn      SPI_1Lines_HalfDuplex_Init
 *
 * @brief   Configuring the SPI for half-duplex communication.
 *
 * @return  none
 */
void SPI_1Lines_HalfDuplex_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	SPI_InitTypeDef SPI_InitStructure={0};

	RCC_HB2PeriphClockCmd( RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC , ENABLE );
	RCC_HB1PeriphClockCmd( RCC_HB1Periph_SPI2, ENABLE );	

#if (SPI_MODE == HOST_MODE)
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

#elif (SPI_MODE == SLAVE_MODE)
	// SCK PB13(AF5)
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// MISO PC2(AF5)
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF5);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif


#if (SPI_MODE == HOST_MODE)
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

#elif (SPI_MODE == SLAVE_MODE)
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Rx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;

#endif

	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_Mode6;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init( SPI2, &SPI_InitStructure );


	NVIC_EnableIRQ(SPI2_IRQn);

#if (SPI_MODE == SLAVE_MODE)
	SPI_I2S_ITConfig( SPI2, SPI_I2S_IT_RXNE , ENABLE );

#endif

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
	u8 i;



#if (SPI_MODE == SLAVE_MODE)
	printf("SLAVE Mode\r\n");
	Delay_Ms(1000);

#endif
	SPI_1Lines_HalfDuplex_Init();

#if (SPI_MODE == HOST_MODE)
	printf("HOST Mode\r\n");
	Delay_Ms(2000);
	SPI_I2S_ITConfig( SPI2, SPI_I2S_IT_TXE , ENABLE );

#endif

	while(1)
	{
#if (SPI_MODE == HOST_MODE)
		while( Txval<18 );

		while( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_TXE ) != RESET )
		{
			if(SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_BSY ) == RESET)
			{
				SPI_Cmd( SPI2, DISABLE );
				while(1);
			}
		}

#elif (SPI_MODE == SLAVE_MODE)
		while( Rxval<18 );

		for( i=0; i<18; i++ )
		{
			printf( "Rxdata:%04x\r\n", RxData[i] );
		}

		while(1);

#endif
	}
	while (1)
		;
}


void SPI2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      SPI2_IRQHandler
 *
 * @brief   This function handles SPI2 exception.
 *
 * @return  none
 */
void SPI2_IRQHandler(void)
{
#if (SPI_MODE == HOST_MODE)
	if( SPI_I2S_GetITStatus( SPI2, SPI_I2S_IT_TXE ) != RESET )
	{
		SPI_I2S_SendData( SPI2, TxData[Txval++] );
		if( Txval == 18 )
		{
			SPI_I2S_ITConfig( SPI2, SPI_I2S_IT_TXE , DISABLE );
		}
	}

#elif (SPI_MODE == SLAVE_MODE)
	RxData[Rxval++] = SPI_I2S_ReceiveData( SPI2 );

#endif
}