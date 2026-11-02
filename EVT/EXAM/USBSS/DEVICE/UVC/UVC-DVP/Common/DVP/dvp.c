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
#include "i2c_host.h"
#include "ch32h417_dvp.h"

void DVP_IRQHandler (void) __attribute__((interrupt("WCH-Interrupt-fast")));

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
	DVP_InitTypeDef DVP_InitStructure = {0};

	RCC_HBPeriphClockCmd( RCC_HBPeriph_DVP, ENABLE );
	DVP_DeInit();

	DVP_StructInit( &DVP_InitStructure );
	DVP_InitStructure.DVP_DataSize       = DVP_DataSize_8b;
	DVP_InitStructure.DVP_HCLK_P         = DVP_Hclk_P_Rising;
	DVP_InitStructure.DVP_HSYNC_P        = DVP_Hsync_P_High;
	DVP_InitStructure.DVP_VSYNC_P        = DVP_Vsync_P_High;
	DVP_InitStructure.DVP_CaptureMode    = DVP_CaptureMode_Continuous;
	DVP_InitStructure.DVP_FrameCapRate   = DVP_FrameCapRate_100P;
	DVP_InitStructure.DVP_JPEGMode       = DISABLE;
	DVP_InitStructure.DVP_COL_NUM        = 1280;
	DVP_InitStructure.DVP_ROW_NUM        = 480;
	DVP_InitStructure.DVP_DMA_BUF0_Addr  = (uint32_t)Dvp_Recaddr;
	DVP_InitStructure.DVP_DMA_BUF1_Addr  = (uint32_t)Dvp_Recaddr;
	DVP_Init( &DVP_InitStructure );

	DVP_ReceiveCircuitResetCmd( DISABLE );
	DVP_FIFO_ResetCmd( DISABLE );

	DVP_ITConfig( DVP_IT_STR_FRM | DVP_IT_ROW_DONE | DVP_IT_FRM_DONE |
	              DVP_IT_FIFO_OV | DVP_IT_STP_FRM, ENABLE );
	DVP_DMACmd( ENABLE );
	NVIC_SetPriority( DVP_IRQn, 0x00 );   /* highest: must not miss ROW_DONE */
	NVIC_EnableIRQ( DVP_IRQn );
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
    DVP->ROW_NUM = res_height;
    DVP->COL_NUM = res_width * 2;
    Dvp_DataSize = res_width * 2;
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
	i2c_host_init();
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
	return (uint8_t)(SCCB_Write(0x30, reg, data) != 0);
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
	SCCB_Read(0x30, reg, &val);
	return val;
}

/*******************************************************************************
 * @fn        SCCB_WR_Reg16
 *
 * @brief     Write OV5640 Register (16-bit sub-address, high byte first)
 *
 * @param     reg  - 16-bit register address
 *            data - register value
 *
 * @return    0 - success
 *            other - fail
 *******************************************************************************/
uint8_t SCCB_WR_Reg16( uint16_t reg, uint8_t data )
{
	return (uint8_t)(SCCB_Write16(0x3C, reg, data) != 0);
}

/*******************************************************************************
 * @fn         SCCB_RD_Reg16
 *
 * @brief      Read OV5640 Register (16-bit sub-address, high byte first)
 *
 * @return     Camera Register value
 *******************************************************************************/
uint8_t SCCB_RD_Reg16( uint16_t reg )
{
	uint8_t val = 0;
	SCCB_Read16(0x3C, reg, &val);
	return val;
}


