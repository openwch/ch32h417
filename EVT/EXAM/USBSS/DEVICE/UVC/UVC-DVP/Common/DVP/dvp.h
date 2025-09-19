/********************************** (C) COPYRIGHT *******************************
* File Name          : dvp.h
* Author             : WCH
* Version            : V1.2
* Date               : 2025/07/28
* Description        :
*********************************************************************************
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __DVP_H__
#define __DVP_H__

#include "ch32h417.h"
#include "ov2640.h"

/* Global define */
/*IIC*/

#define IIC_GPIO GPIOB



#define IIC_SCL_OUT    { IIC_GPIO->CFGHR = (( IIC_GPIO->CFGHR & 0xFFFFF0FF) | (uint32_t)3 << 8 );}  //direction: 0=in, 1=out     
#define IIC_SDA_IN     { IIC_GPIO->CFGHR = (( IIC_GPIO->CFGHR & 0xFFFF0FFF) | (uint32_t)8 << 12 );}
#define IIC_SDA_OUT    { IIC_GPIO->CFGHR = (( IIC_GPIO->CFGHR & 0xFFFF0FFF) | (uint32_t)3 << 12 );}
#define IIC_SCL_SET    { IIC_GPIO->BSHR = GPIO_Pin_10; }     //Output high
#define IIC_SCL_CLR    { IIC_GPIO->BCR = GPIO_Pin_10; }      //Output low
#define IIC_SDA_SET    { IIC_GPIO->BSHR = GPIO_Pin_11; }     //Output high
#define IIC_SDA_CLR    { IIC_GPIO->BCR = GPIO_Pin_11; }      //Output low
/*SDA Input level status*/
#define SDA_IN_R	   ( IIC_GPIO->INDR & GPIO_Pin_11 )
/*ID*/
#define SCCB_ID   	   0X60  	//OV2640 ID

#define UVC_DMA_SIZE   1024*80

/* Global Variable */
extern __attribute__ ((aligned(32))) uint8_t  UVC_DMABuffer[UVC_DMA_SIZE] __attribute__((section(".DMADATA")));

/* Function declaration */
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
uint8_t SCCB_WR_Byte(uint8_t dat);
uint8_t SCCB_RD_Byte(void);
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data);
uint8_t SCCB_RD_Reg(uint8_t reg);
void dvp_Init(void);
void Dvp_Row_Col_Set( uint16_t res_width , uint16_t res_height );
#endif /* DVP_DVP_H_ */
