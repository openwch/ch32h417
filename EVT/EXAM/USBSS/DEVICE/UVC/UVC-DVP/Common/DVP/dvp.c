/********************************** (C) COPYRIGHT *******************************
* File Name          : dvp.c
* Author             : WCH
* Version            : V1.2
* Date               : 2025/07/28
* Description        :
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "uvclib.h"
#include "dvp.h"
#include "ch32h417_dvp.h"

#if 1
#define RGB565_ROW_NUM   320
#define RGB565_COL_NUM   480   //Col * 2
#define OV2640_RGB565_HEIGHT   320
#define OV2640_RGB565_WIDTH	   240

//JPEG PIXEL 1024 * 768
#define OV2640_JPEG_HEIGHT	768
#define OV2640_JPEG_WIDTH	1024

UINT32  JPEG_DVPDMAaddr0 = 0x2011A000;
UINT32  JPEG_DVPDMAaddr1 = 0x2011A000 + OV2640_JPEG_WIDTH;

UINT32  RGB565_DVPDMAaddr0 = 0x2011A000;
UINT32  RGB565_DVPDMAaddr1 = 0x2011A000 + RGB565_COL_NUM;

#endif

void DVP_IRQHandler (void) __attribute__((interrupt("WCH-Interrupt-fast")));
__attribute__ ((aligned(32))) uint8_t	UVC_DMABuffer[UVC_DMA_SIZE] __attribute__((section(".DMADATA")));

/*******************************************************************************
 * @fn     DVP_IRQHandler
 *
 * @brief  RGB565 - Take the data as: frame start to frame reception completion R16_DVP_ROW_NUM * R16_DVP_COL_NUM.
 *         JPEG - The data taken is: 0xff and 0xd8 are taken from the data of one frame from the start of the frame
 *                to the end of the frame; 0xff, 0xd9 end
 *
 * @return   None
 */
void DVP_IRQHandler(void)
{
	DVP_Hander( );
}

/*******************************************************************************
 * @fn     dvp_Init
 *
 * @brief
 *
 * @return   None
 */
void dvp_Init(void)
{
	RCC_HBPeriphClockCmd( RCC_HBPeriph_DVP, ENABLE );

    DVP->CR0 &= ~RB_DVP_MSK_DAT_MOD;						
    DVP->CR0 |= RB_DVP_D8_MOD | RB_DVP_V_POLAR;				
    DVP->CR1 &= ~( RB_DVP_ALL_CLR | RB_DVP_RCV_CLR );

    DVP->DMA_BUF0 = (UINT32)(uint8_t *)Dvp_Recaddr;		            //DMA addr0
	DVP->DMA_BUF1 = (UINT32)(uint8_t *)Dvp_Recaddr+Dvp_DataSize;    //DMA addr1
 
    /* Interupt Enable */     
	DVP->IER |= RB_DVP_IE_STP_FRM;                 					// Frame end interrupt enable
	DVP->IER |= RB_DVP_IE_FRM_DONE;                					// Frame receiving completion interrupt enable
	DVP->IER |= RB_DVP_IE_ROW_DONE;                					// Line end interrupt enable
	DVP->IER |= RB_DVP_IE_STR_FRM;                 					// New frame start interrupt enable
    DVP->IER |= RB_DVP_IE_FIFO_OV; 
 
	DVP->CR1 |= RB_DVP_DMA_EN;  									// enable DMA
	DVP->CR0 |= RB_DVP_ENABLE;  									// enable DVP

}

/*******************************************************************************
 * @fn       DVP_ROW_COL_Set
 *
 * @brief    Set the size of one row and one column
 *           res_width: row
 *           res_height col
 *
 * @return   None
 */
void Dvp_Row_Col_Set( uint16_t res_width , uint16_t res_height )
{
    switch(Formatchange_flag)
    {
        case FORMAT_MJPEG:        
            DVP->ROW_NUM = 0;
            DVP->COL_NUM = 512;
            Dvp_DataSize    = 512;
            break;
        case FORMAT_YUV2:
            DVP->ROW_NUM = res_height;
            DVP->COL_NUM = res_width *2;
            Dvp_DataSize = res_width *2;
            break;
    }
}

/*******************************************************************************
 * @fn     SCCB_Init
 *
 * @brief  Initialize SCCB
 *
 * @return   None
 */
void SCCB_Init( void )
{
	IIC_SCL_OUT;  
	IIC_SDA_OUT; 

	IIC_SCL_SET; 
	IIC_SDA_SET;  
}

/*******************************************************************************
 * @fn       SCCB_Start
 *
 * @brief    Start Signal
 *
 * @return   None
 */

void SCCB_Start( void )
{
	IIC_SDA_SET; 
	IIC_SCL_SET;  
	Delay_Us( 50 );
	IIC_SDA_CLR;  
	Delay_Us( 50 );
	IIC_SCL_CLR; 
}

/*******************************************************************************
 * @fn        SCCB_Stop
 *
 * @brief     Stop Signal
 *
 * @return    None
 */

void SCCB_Stop( void )
{
	IIC_SDA_CLR;  
	Delay_Us( 50 );
	IIC_SCL_SET;  
	Delay_Us( 50 );
	IIC_SDA_SET;  
	Delay_Us( 50 );
}

/*******************************************************************************
 * @fn      SCCB_No_Ack
 *
 * @brief   NAK Signal
 *
 * @return  None
 */

void SCCB_No_Ack( void )
{
	Delay_Us( 50 );
	IIC_SDA_SET;  
	IIC_SCL_SET;  
	Delay_Us( 50 );
	IIC_SCL_CLR; 
	Delay_Us( 50 );
	IIC_SDA_CLR;  
	Delay_Us( 50 );
}

/*******************************************************************************
 * @fn        SCCB_WR_Byte
 * @brief     Write One Byte
 * @param     data
 * @return    0 - success
 *            other - fail
 */

uint8_t SCCB_WR_Byte(uint8_t dat)
{
	uint8_t j,res;

	for( j = 0; j < 8; j++ ) 
	{
		if( dat & 0x80 )
		{
			IIC_SDA_SET;
		}
		else{
		    IIC_SDA_CLR;
		}

		dat <<= 1;
		Delay_Us( 50 );
		IIC_SCL_SET;
		Delay_Us( 50 );
		IIC_SCL_CLR;
	}

	IIC_SDA_IN;		

	Delay_Us( 50 );
	IIC_SCL_SET;			
	Delay_Us( 50 );
	if( SDA_IN_R )
	{
		res = 1;  
	}
	else 
	{
		res = 0;      
	}
   
	IIC_SCL_CLR;

	IIC_SDA_OUT;
	return res;

}

/*******************************************************************************
 * @fn        SCCB_RD_Byte
 *
 * @brief     Read One Byte
 *
 * @return    Read one byte data
 */

uint8_t SCCB_RD_Byte( void )
{
	uint8_t temp = 0, j;

	IIC_SDA_IN;		

	for( j = 8; j > 0; j-- ) 
	{
		Delay_Us( 50 );
		IIC_SCL_SET;
		temp = temp << 1;
		if( SDA_IN_R )
		{
			temp++;
		}
		Delay_Us( 50 );
		IIC_SCL_CLR;
	}

	IIC_SDA_OUT;	

	return temp;
}

/*******************************************************************************
 * @fn        SCCB_WR_Reg
 *
 * @brief     Write camera Register
 *
 * @param     Reg_Adr - Register address
 *            Reg_Val - Register value
 *
 * @return    0 - success
 *            other - fail
 */

uint8_t SCCB_WR_Reg( uint8_t reg, uint8_t data )
{
	uint8_t res = 0;

	SCCB_Start( ); 				
	if( SCCB_WR_Byte( SCCB_ID ))
	{
		res = 1;	
	}

	Delay_Us(100);
  	if( SCCB_WR_Byte( reg ))	
	{
		res = 1;	
	}
  	Delay_Us(100);
  	if( SCCB_WR_Byte( data ))	
	{
		res = 1; 
	}
  	SCCB_Stop( );
  	return	res;
}

/*******************************************************************************
 * @fn         SCCB_RD_Reg
 *
 * @brief      Read camera Register
 *
 * @return     Camera Register value
 */

uint8_t SCCB_RD_Reg( uint8_t reg )
{
	uint8_t val = 0;
	SCCB_Start( ); 			
	SCCB_WR_Byte( SCCB_ID );		
	Delay_Us( 100 );
  	SCCB_WR_Byte( reg );		
  	Delay_Us( 100 );
	SCCB_Stop( );
	Delay_Us( 100 );

	SCCB_Start( );
	SCCB_WR_Byte( SCCB_ID | 0x01 );	
	Delay_Us( 100 );
  	val=SCCB_RD_Byte( );		 	
  	SCCB_No_Ack( );
  	SCCB_Stop( );
  	return val;
}


