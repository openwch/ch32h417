/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32h417_uhsif.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/07/23
* Description        : This file contains the headers of the interrupt handlers.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32H417_UHSIF_H
#define __CH32H417_UHSIF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32h417.h"
#include "string.h"
#include "debug.h"
#include "ch32h417_usbss_device.h"


/* UHSIF mode definition: 0 is for configuring slave FPGA mode, 1 is for configuring slave SOC mode,
                          and 2 is for configuring host mode. */
#define DEF_UHSIF_SLAVE_FPGA_MODE         0
#define DEF_UHSIF_SLAVE_SOC_MODE          1
#define DEF_UHSIF_MASTER_MODE             2

/* Buff size configuration */
#define DEF_BUFF_SIZE                     16384
#define DEF_RX_BUFF_SIZE                  DEF_BUFF_SIZE
#define DEF_TX_BUFF_SIZE                  DEF_BUFF_SIZE

/* UHSIF pin remapping configuration */
#define DEF_UHSIF_PINREMAP0               0
#define DEF_UHSIF_PINREMAP1               1
#define DEF_UHSIF_PINREMAP2               2
#define DEF_UHSIF_CLKREMAP0               0
#define DEF_UHSIF_CLKREMAP1               1
#define DEF_UHSIF_CLKREMAP2               2
#define DEF_UHSIF_CLKREMAP3               3

#define DEF_UHSIF_DATA_BIT8               0
#define DEF_UHSIF_DATA_BIT16              1
#define DEF_UHSIF_DATA_BIT24              2
#define DEF_UHSIF_DATA_BIT32              3

/* Line number macro definition */
#define DEF_LINE_COUNT_MAX                4
#define DEF_LINE_COUNT                    4
#define DEF_LINE0                         0
#define DEF_LINE1                         1
#define DEF_LINE2                         2
#define DEF_LINE3                         3

/* Line direction macro definition: 
   0 indicates that the current line is configured as input data, 
   and 1 indicates that the current line is configured as output data. */
#define DEF_LINE_DIR_IN                   0
#define DEF_LINE_DIR_OUT                  1

/* Macro definition of the number of Buff */
#define DEF_BUFF_COUNT_MAX                8
#define DEF_BUFF_COUNT_1                  1
#define DEF_BUFF_COUNT_2                  2
#define DEF_BUFF_COUNT_3                  3
#define DEF_BUFF_COUNT_4                  4
#define DEF_BUFF_COUNT_5                  5
#define DEF_BUFF_COUNT_6                  6
#define DEF_BUFF_COUNT_7                  7
#define DEF_BUFF_COUNT_8                  8


/* Definition and setting of data output polarity in slave mode: 
   0 represents the output data at the falling edge of the clock, 
   and 1 represents the output data at the rising edge of the clock. */
#define DEF_SLAVE_DOUT_POLARITY_NEGEDGE   0
#define DEF_SLAVE_DOUT_POLARITY_POSEDGE   1
#define DEF_SLAVE_DOUT_POLARITY           DEF_SLAVE_DOUT_POLARITY_POSEDGE

/* Definition and setting of clock output polarity in device mode: 
   0 indicates that the clock output edge is not inverting, 
   and 1 indicates that the clock output edge is inverting. */
#define DEF_SLAVE_CLK_POLARITY_NEGEDGE    0
#define DEF_SLAVE_CLK_POLARITY_POSEDGE    1
#define DEF_SLAVE_CLK_POLARITY            DEF_SLAVE_CLK_POLARITY_NEGEDGE

/* Definition and setting of data output polarity in host mode: 
   0 represents the output data at the falling edge of the clock, 
   and 1 represents the output data at the rising edge of the clock. */
#define DEF_MASTER_DOUT_POLARITY_NEGEDGE  0
#define DEF_MASTER_DOUT_POLARITY_POSEDGE  1
#define DEF_MASTER_DOUT_POLARITY          DEF_MASTER_DOUT_POLARITY_POSEDGE

/* Definition and setting of clock output polarity in host mode: 
   0 indicates that the clock output edge is not inverting, 
   and 1 indicates that the clock output edge is inverting. */
#define DEF_MASTER_CLK_POLARITY_NEGEDGE   0
#define DEF_MASTER_CLK_POLARITY_POSEDGE   1
#define DEF_MASTER_CLK_POLARITY           DEF_MASTER_CLK_POLARITY_POSEDGE

/* Error message prompt */
#define DEF_UHSIF_SUCESS                  0x00        /* The initialization related to UHSIF was successful */
#define DEF_UHSIF_GPIO_CFG_ERR            0xA1        /* The UHSIF pin's reflection parameter exceeds the limit */
#define DEF_UHSIF_DIV_ERR                 0xA8      
#define DEF_UHSIF_DATA_OVER_ERR           0xA9      

#define DEF_UHSIF_REG_ADDR_ERR            0xA2        /* The input address is empty */
#define DEF_UHSIF_REG_CFG_ERR             0xA3        /* The modification of the UHSIF control register failed */
#define DEF_UHSIF_REG_GET_ERR             0xA4        /* The UHSIF control register failed to be read */
#define DEF_UHSIF_MODE_ERR                0xA5        /* The master-slave mode selection is incorrect */

#define DEF_UHSIF_WR_LIMIT_ERR            0xA7        /* The UHSIF read and write registers have exceeded the limit */
#define DEF_LINE_ENABLE_ERR               0xB1        /* The line is not enabled */
#define DEF_LINE_OVER_ERR                 0xB2        /* The number of lines exceeds the limit */
#define DEF_LINE_DIR_ERR                  0xB3        /* The line direction is set incorrectly */
#define DEF_BUFF_OVER_ERR                 0xC1        /* The number of buff exceeds the limit */
#define DEF_DESC_OVER_ERR                 0xD1        /* The number of descriptors exceeds the limit */

/*******************************************************************************
 * @fn      UHSIF_Get_Ver
 *
 * @brief   Obtain version information
 *
 * @param   None
 *
 * @return  Current UHSIF library version information
 */
uint8_t UHSIF_Get_Ver( void );

/*******************************************************************************
 * @fn      UHSIF_Cfg
 *
 * @brief   UHSIF register configuration
 *
 * @param   None
 *
 * @return  None
 */
void UHSIF_Cfg( void );

/*******************************************************************************
 * @fn      UHSIF_GPIO_Init
 *
 * @brief   UHSIF GPIO initialization
 *
 * @param   mode_select:    0 ~ 2 (0:DEF_UHSIF_SLAVE_FPGA_MODE; 1:DEF_UHSIF_SLAVE_SOC_MODE; 2:DEF_UHSIF_MASTER_MODE)
 *          uhsif_port_rm:  0 ~ 2 (UHSIF pin mapping selection)
 *          uhsif_clk_rmm:  0 ~ 3 (UHSIF clock pin mapping selection)
 *          clk_div:        0 ~ 0x3f (UHISF clock frequency division selection)
 *          width_bit:      Choose to use an 8/16/24/32-bit data cable.
 *
 * @return  Whether the UHISF configuration was successful
 */
uint8_t UHSIF_GPIO_Init( uint8_t mode_select, uint8_t uhsif_port_rm, uint8_t uhsif_clk_rmm, uint8_t clk_div,  uint8_t width_bit );

/*******************************************************************************
 * @fn      UHSIF_Line_Cfg
 *
 * @brief   UHISF line configuration
 *
 * @param   line_num:    0 ~ 3 (line number)
 *          line_dir:    0 ~ 1 (0:IN; 1:OUT)
 *          buff_count:  0 ~ 8 (Selection of buff quantity)
 *          *pbuf:         Configure the send and receive addresses of the current line
 *          water_mark_value£º 0 ~ 0x3fff(When sending, it indicates the number of remaining words to be sent in the buffer; 
                               when receiving, it indicates the number of remaining words to be received in the buffer)
 *
 * @return  Whether the UHISF configuration was successful
 */
uint8_t UHSIF_Line_Cfg( uint8_t line_num, uint8_t line_dir, uint8_t buff_count, uint8_t *pbuf, uint16_t water_mark_value );

/*******************************************************************************
 * @fn      UHSIF_Start
 *
 * @brief   Enable or disable UHSIF function.
 *
 * @param   None
 *
 * @return  None
 */
void UHSIF_Start( FunctionalState sta );

/*******************************************************************************
 * @fn      UHSIF_Line0_IN_Callback
 *
 * @brief   The interrupt callback function for line 0 configuration input
 *
 * @param   rcv_count: The length of the data received by line 0
 *
 * @return  None
 */
void UHSIF_Line0_IN_Callback( uint32_t rcv_count );

/*******************************************************************************
 * @fn      UHSIF_Line1_IN_Callback
 *
 * @brief   The interrupt callback function for line 1 configuration input
 *
 * @param   rcv_count: The length of the data received by line 1
 *
 * @return  None
 */
void UHSIF_Line1_IN_Callback( uint32_t rcv_count );

/*******************************************************************************
 * @fn      UHSIF_Line2_IN_Callback
 *
 * @brief   The interrupt callback function for line 2 configuration input
 *
 * @param   rcv_count: The length of the data received by line 2
 *
 * @return  None
 */
void UHSIF_Line2_IN_Callback( uint32_t rcv_count );

/*******************************************************************************
 * @fn      UHSIF_Line3_IN_Callback
 *
 * @brief   The interrupt callback function for line 3 configuration input
 *
 * @param   rcv_count: The length of the data received by line 3
 *
 * @return  None
 */
void UHSIF_Line3_IN_Callback( uint32_t rcv_count );

/*******************************************************************************
 * @fn      UHSIF_Line0_OUT_Callback
 *
 * @brief   The interrupt callback function for line 0 configuration output
 *
 * @param   None
 *
 * @return  None
 */
void UHSIF_Line0_OUT_Callback( void );

/*******************************************************************************
 * @fn      UHSIF_Line1_OUT_Callback
 *
 * @brief   The interrupt callback function for line 1 configuration output
 *
 * @param   None
 *
 * @return  None
 */
void UHSIF_Line1_OUT_Callback( void );

/*******************************************************************************
 * @fn      UHSIF_Line2_OUT_Callback
 *
 * @brief   The interrupt callback function for line 2 configuration output
 *
 * @param   None
 *
 * @return  None
 */
void UHSIF_Line2_OUT_Callback( void );

/*******************************************************************************
 * @fn      UHSIF_Line3_OUT_Callback
 *
 * @brief   The interrupt callback function for line 3 configuration output
 *
 * @param   None
 *
 * @return  None
 */
void UHSIF_Line3_OUT_Callback( void );

/*******************************************************************************
 * @fn      UHSIF_Trans_Cfg
 *
 * @brief   UHSIF data transmission configuration
 *
 * @param   line_num:    0 ~ 3 (The current line number in use)
 *          buff_cur:    0 ~ 7 (The current buff number in use)
 *          trans_size:  The length of the sent data
 *
 * @return  Whether the UHISF configuration was successful
 */
uint8_t UHSIF_Trans_Cfg( uint8_t line_num, uint8_t buff_cur, uint32_t trans_size );

/*******************************************************************************
 * @fn      UHSIF_Reg_Cfg
 *
 * @brief   UHISF device register modification.
 *
 * @param   mode:       0 ~ 2 (0:DEF_UHSIF_SLAVE_FPGA_MODE; 1:DEF_UHSIF_SLAVE_SOC_MODE; 2:DEF_UHSIF_MASTER_MODE)
 *          reg_addr:   0 ~ 75 (The address of the register that UHSIF needs to operate on)
 *          value:      Values that need to be modified
 *
 * @return  Whether the configuration is successful
 */
uint8_t UHSIF_Reg_Cfg( uint8_t mode, uint8_t reg_addr, uint32_t value );

#ifdef __cplusplus
}
#endif


#endif 

