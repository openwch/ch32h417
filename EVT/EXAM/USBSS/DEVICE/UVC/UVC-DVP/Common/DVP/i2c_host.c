/********************************** (C) COPYRIGHT  *******************************
* File Name          : i2c_host.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/08/24
* Description        : SCCB host over I2C2 (SCL=PB10, SDA=PB11, AF4, 100kHz).
*********************************************************************************/
#include "ch32h417.h"
#include "debug.h"
#include "i2c_host.h"

#define TIMEOUT_DELAY   500

/*******************************************************************************
 * @fn      i2c_host_init
 *
 * @brief   Initialize I2C2 as SCCB host
 *
 * @return  none
 */
void i2c_host_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    I2C_InitTypeDef   I2C_InitStructure  = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB | RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_I2C2, ENABLE);

    /* SCL-PB10  SDA-PB11 (AF4) */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF4);   /* SCL - PB10 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF4);   /* SDA - PB11 */

    I2C_InitStructure.I2C_ClockSpeed          = 100000;
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_16_9;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &I2C_InitStructure);

    I2C_Cmd(I2C2, ENABLE);
    I2C_AcknowledgeConfig(I2C2, ENABLE);
}

/*******************************************************************************
 * @fn      SCCB_Read
 *
 * @brief   Read a register (8-bit sub-address)
 *
 * @return  0 - Success, -1 - Error
 */
int SCCB_Read(uint8_t slv_addr, uint8_t reg, uint8_t *out)
{
    I2C_TypeDef *I2Cx = I2C2;
    uint32_t timeout;
    uint8_t data;

    timeout = 0;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_GenerateSTOP(I2Cx, DISABLE);
    I2C_GenerateSTART(I2Cx, ENABLE);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2Cx->DATAR = slv_addr << 1;
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_SendData(I2Cx, reg);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_GenerateSTOP(I2Cx, ENABLE);

    I2C_GenerateSTART(I2Cx, ENABLE);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_Send7bitAddress(I2Cx, slv_addr << 1, I2C_Direction_Receiver);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    timeout = 0;
    while(!I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }
    data = I2C_ReceiveData(I2Cx);

    I2C_GenerateSTOP(I2Cx, ENABLE);
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    *out = data;
    return 0;

timeout:
    I2C_GenerateSTOP(I2Cx, ENABLE);
    return -1;
}

/*******************************************************************************
 * @fn      SCCB_Write
 *
 * @brief   Write a register (8-bit sub-address)
 *
 * @return  0 - Success, -1 - Error
 */
int SCCB_Write(uint8_t slv_addr, uint8_t reg, uint8_t data)
{
    I2C_TypeDef *I2Cx = I2C2;
    uint32_t timeout;

    timeout = 0;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_GenerateSTOP(I2Cx, DISABLE);
    I2C_GenerateSTART(I2Cx, ENABLE);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2Cx->DATAR = slv_addr << 1;
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_SendData(I2Cx, reg);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_SendData(I2Cx, data);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_GenerateSTOP(I2Cx, ENABLE);
    return 0;

timeout:
    I2C_GenerateSTOP(I2Cx, ENABLE);
    return -1;
}

/*******************************************************************************
 * @fn      SCCB_Read16
 *
 * @brief   Read a register (16-bit sub-address)
 *
 * @return  0 - Success, -1 - Error
 */
int SCCB_Read16(uint8_t slv_addr, uint16_t reg, uint8_t *out)
{
    I2C_TypeDef *I2Cx = I2C2;
    uint32_t timeout;
    uint8_t data;

    timeout = 0;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_GenerateSTOP(I2Cx, DISABLE);
    I2C_GenerateSTART(I2Cx, ENABLE);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2Cx->DATAR = slv_addr << 1;
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_SendData(I2Cx, reg >> 8);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_SendData(I2Cx, reg & 0xFF);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_GenerateSTOP(I2Cx, ENABLE);

    I2C_GenerateSTART(I2Cx, ENABLE);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_Send7bitAddress(I2Cx, slv_addr << 1, I2C_Direction_Receiver);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    timeout = 0;
    while(!I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }
    data = I2C_ReceiveData(I2Cx);

    I2C_GenerateSTOP(I2Cx, ENABLE);
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    *out = data;
    return 0;

timeout:
    I2C_GenerateSTOP(I2Cx, ENABLE);
    return -1;
}

/*******************************************************************************
 * @fn      SCCB_Write16
 *
 * @brief   Write a register (16-bit sub-address)
 *
 * @return  0 - Success, -1 - Error
 */
int SCCB_Write16(uint8_t slv_addr, uint16_t reg, uint8_t data)
{
    I2C_TypeDef *I2Cx = I2C2;
    uint32_t timeout;

    timeout = 0;
    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_GenerateSTOP(I2Cx, DISABLE);
    I2C_GenerateSTART(I2Cx, ENABLE);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2Cx->DATAR = slv_addr << 1;
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_SendData(I2Cx, reg >> 8);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_SendData(I2Cx, reg & 0xFF);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_SendData(I2Cx, data);
    timeout = 0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        Delay_Us(1);
        if(timeout++ > TIMEOUT_DELAY) goto timeout;
    }

    I2C_GenerateSTOP(I2Cx, ENABLE);
    return 0;

timeout:
    I2C_GenerateSTOP(I2Cx, ENABLE);
    return -1;
}
