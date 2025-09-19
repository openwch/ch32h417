/********************************** (C) COPYRIGHT  *******************************
 * File Name          : iap.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/01/09
 * Description        : IAP
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "iap.h"
#include "string.h"
#include "flash.h"
#include "core_riscv.h"

/******************************************************************************/

iapfun jump2app;
u32 Program_addr = FLASH_Base;
u32 Verify_addr = FLASH_Base;
u32 User_APP_Addr_offset = 0x6000;
u8 Verify_Star_flag = 0;
u8 Fast_Program_Buf[1024*10];
u32 CodeLen = 0;
u8 End_Flag = 0;
u8 EP2_Rx_Buffer[USBD_DATA_SIZE+4];
#define  isp_cmd_t   ((isp_cmd  *)EP2_Rx_Buffer)
#define  Size_256B         0x100
#define  Size_4KB          0x1000
#define  Size_8KB          0x2000

uint32_t Flash_Erase_Page_Size = Size_8KB;
/*********************************************************************
 * @fn      RecData_Deal
 *
 * @brief   USB deal data
 *
 * @return  ERR_ERROR - ERROR
 *          ERR_SUCCESS - SUCCESS
 *          ERR_End - End
 */
u8 RecData_Deal(void)
{
    uint32_t  i, s, Lenth,temp;
    u8 *PBuf = NULL;
    Lenth = isp_cmd_t->other.buf[1];

    switch ( isp_cmd_t->other.buf[0]) {
    case CMD_IAP_ERASE:
        FLASH_Unlock_Fast();
        if(((*(vu32*)FLASH_CFGR0_BASE) & (1<<28)) != 0)
        {
            Flash_Erase_Page_Size = Size_8KB;
        }
        else 
        {
            Flash_Erase_Page_Size = Size_4KB;
        }
        s = ERR_SUCCESS;
        break;

    case CMD_IAP_PROM:
        for (i = 0; i < Lenth; i++) 
        {
            Fast_Program_Buf[CodeLen + i] = isp_cmd_t->program.data[i];
        }
        CodeLen += Lenth;
        if (CodeLen >= Flash_Erase_Page_Size) {
            FLASH_Unlock_Fast();

            FLASH_ErasePage(Program_addr);
            PBuf = Fast_Program_Buf;
            for (uint32_t j = 0; j < (Flash_Erase_Page_Size/256);j++)
            {
                CH32_IAP_Program(Program_addr, (u32*) PBuf);
                CodeLen -= Size_256B;
                PBuf += Size_256B;

                Program_addr += Size_256B;
            }
            for (int i = 0; i < CodeLen; i++) 
            {
                Fast_Program_Buf[i] = *(PBuf);
                PBuf++;
            }
        }
        s = ERR_SUCCESS;
        break;

    case CMD_IAP_VERIFY:

        if (Verify_Star_flag == 0) 
        {
        Verify_Star_flag = 1;
          if(CodeLen != 0)
          {
            for(i = 0; ((CodeLen+i) % Size_256B) != 0;i++)
            {
                Fast_Program_Buf[CodeLen + i] = 0xff;
            }
            temp = CodeLen;
            PBuf = Fast_Program_Buf;
            FLASH_ErasePage(Program_addr& (~(Flash_Erase_Page_Size-1)));

            for(i = 0;i < ((temp+Size_256B-1)/Size_256B);i++)
            {
                CH32_IAP_Program(Program_addr, (u32*) PBuf);
                PBuf += Size_256B;
                Program_addr += Size_256B;
            }
            CodeLen = 0;
          }
        }  

        s = ERR_SUCCESS;
        for (i = 0; i < Lenth; i++) {
            if (isp_cmd_t->verify.data[i] != *(u8*) (Verify_addr + i)) {
                s = ERR_ERROR;
                break;
            }
        }

        Verify_addr += Lenth;
        break;

    case CMD_IAP_END:
        Verify_Star_flag = 0;
        End_Flag = 1;
        Program_addr = FLASH_Base;
        Verify_addr = FLASH_Base;
        s = ERR_End;

        FLASH_ErasePage(CalAddr & (~(Flash_Erase_Page_Size-1)));
        
        FLASH->CTLR |= ((uint32_t)0x00008000);
        FLASH->CTLR |= ((uint32_t)0x00000080);

        break;
    case CMD_JUMP_IAP:

        s = ERR_SUCCESS;
        break;
    default:
        s = ERR_ERROR;
        break;
    }

    return s;
}

/*********************************************************************
 * @fn      UART_RecData_Deal
 *
 * @brief   UART deal data
 *
 * @return  ERR_ERROR - ERROR
 *          ERR_SUCCESS - SUCCESS
 *          ERR_End - End
 */
u8 UART_RecData_Deal(void)
{
    uint32_t  i, s, Lenth,temp;
    u8 *PBuf = NULL;

    Lenth = isp_cmd_t->UART.Len;
    switch ( isp_cmd_t->UART.Cmd) {
    case CMD_IAP_ERASE:

        FLASH_Unlock_Fast();
        if(((*(vu32*)FLASH_CFGR0_BASE) & (1<<28)) != 0)
        {
            Flash_Erase_Page_Size = Size_8KB;
        }
        else 
        {
            Flash_Erase_Page_Size = Size_4KB;
        }
        s = ERR_SUCCESS;
        break;

    case CMD_IAP_PROM:
        for (i = 0; i < Lenth; i++) 
        {
            Fast_Program_Buf[CodeLen + i] = isp_cmd_t->UART.data[i];
        }
        CodeLen += Lenth;
        if (CodeLen >= Flash_Erase_Page_Size) 
        {
            FLASH_Unlock_Fast();
            FLASH_ErasePage(Program_addr);
            PBuf = Fast_Program_Buf;
            for (uint32_t j = 0; j < (Flash_Erase_Page_Size/256);j++)
            {
                CH32_IAP_Program(Program_addr, (u32*) PBuf);
                CodeLen -= Size_256B;
                PBuf += Size_256B;

                Program_addr += Size_256B;
            }
            for (int i = 0; i < CodeLen; i++) 
            {
                Fast_Program_Buf[i] = *(PBuf);
                PBuf++;
            }
        }

        s = ERR_SUCCESS;
        break;

    case CMD_IAP_VERIFY:

        if (Verify_Star_flag == 0) 
        {
            Verify_Star_flag = 1;
            if(CodeLen != 0)
            {
                for(i = 0; ((CodeLen+i) % Size_256B) != 0;i++)
                {
                    Fast_Program_Buf[CodeLen + i] = 0xff;
                }
                temp = CodeLen;
                PBuf = Fast_Program_Buf;
                FLASH_ErasePage(Program_addr& (~(Flash_Erase_Page_Size-1)));

                for(i = 0;i < ((temp+Size_256B-1)/Size_256B);i++)
                {
                    CH32_IAP_Program(Program_addr, (u32*) PBuf);
                    PBuf += Size_256B;
                    Program_addr += Size_256B;
                }
                CodeLen = 0;
            }
        }  

        s = ERR_SUCCESS;
        for (i = 0; i < Lenth; i++) 
        {
            if (isp_cmd_t->UART.data[i] != *(u8*) (Verify_addr + i)) 
            {
                s = ERR_ERROR;
                break;
            }
        }

        Verify_addr += Lenth;
        break;

    case CMD_IAP_END:
        Verify_Star_flag = 0;
        End_Flag = 1;
        Program_addr = FLASH_Base;
        Verify_addr = FLASH_Base;
        s = ERR_End;
        FLASH_ErasePage(CalAddr & (~(Flash_Erase_Page_Size-1)));
        FLASH->CTLR |= ((uint32_t)0x00008000);
        FLASH->CTLR |= ((uint32_t)0x00000080);

        break;

    case CMD_JUMP_IAP:

        s = ERR_SUCCESS;
        break;
    default:
        s = ERR_ERROR;
        break;
    }

    return s;
}

/*********************************************************************
 * @fn      GPIO_Cfg_init
 *
 * @brief   GPIO init
 *
 * @return  none
 */
void GPIO_Cfg_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      PA0_Check
 *
 * @brief   Check PA0 state
 *
 * @return  1 - IAP
 *          0 - APP
 */
u8 PA0_Check(void)
{
    u8 i, cnt=0;

    GPIO_Cfg_init();

    for(i=0; i<10; i++){
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==0) cnt++;
        Delay_Ms(5);
    }

    if(cnt>6) return 0;
    else return 1;
}

/*********************************************************************
 * @fn      USART1_CFG
 *
 * @brief   baudrate:USART1 baudrate
 *
 * @return  none
 */
void USART1_CFG(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB|RCC_HB2Periph_AFIO,ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_USART1,ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF7);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF7);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
    USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init( USART1, &USART_InitStructure );
    USART_Cmd( USART1, ENABLE );
}

/*********************************************************************
 * @fn      UART1_SendMultiyData
 *
 * @brief   Deal device Endpoint 1 OUT.
 *
 * @param   l: Data length.
 *
 * @return  none
 */
void UART1_SendMultiyData(u8* pbuf, u8 num)
{
    u8 i = 0;

    while(i<num)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART_SendData(USART1, pbuf[i]);
        i++;
    }
}

/*********************************************************************
 * @fn      UART1_SendMultiyData
 *
 * @brief   USART1 send date
 *
 * @param   pbuf - Packet to be sent
 *          num - the number of date
 *
 * @return  none
 */
void UART1_SendData(u8 data)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, data);
}

/*********************************************************************
 * @fn      UART1_Rx
 *
 * @brief   UART1 receive date
 *
 * @return  none
 */
u8 UART1_Rx(void)
{
    while( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData( USART1);
}

/*********************************************************************
 * @fn      UART_Rx_Deal
 *
 * @brief   UART Rx data deal
 *
 * @return  none
 */
void UART_Rx_Deal(void)
{
    u8 i, s;
    u16 Data_add = 0;

    if (UART1_Rx() == Uart_Sync_Head1)
    {
        if (UART1_Rx() == Uart_Sync_Head2)
        {
            isp_cmd_t->UART.Cmd = UART1_Rx();
            Data_add += isp_cmd_t->UART.Cmd;
            isp_cmd_t->UART.Len = UART1_Rx();
            Data_add += isp_cmd_t->UART.Len;

            if(isp_cmd_t->UART.Cmd == CMD_IAP_ERASE ||isp_cmd_t->UART.Cmd == CMD_IAP_VERIFY)
            {
                isp_cmd_t->other.buf[2] = UART1_Rx();
                Data_add += isp_cmd_t->other.buf[2];
                isp_cmd_t->other.buf[3] = UART1_Rx();
                Data_add += isp_cmd_t->other.buf[3];
                isp_cmd_t->other.buf[4] = UART1_Rx();
                Data_add += isp_cmd_t->other.buf[4];
                isp_cmd_t->other.buf[5] = UART1_Rx();
                Data_add += isp_cmd_t->other.buf[5];
            }
            if ((isp_cmd_t->other.buf[0] == CMD_IAP_PROM) || (isp_cmd_t->other.buf[0] == CMD_IAP_VERIFY))
            {
                for (i = 0; i < isp_cmd_t->UART.Len; i++) {
                    isp_cmd_t->UART.data[i] = UART1_Rx();
                    Data_add += isp_cmd_t->UART.data[i];
                }
            }
            if (UART1_Rx() == (uint8_t)(Data_add & 0xFF))
            {
                if(UART1_Rx() == (uint8_t)(Data_add >>8))
                {
                    if (UART1_Rx() == Uart_Sync_Head2)
                    {
                        if (UART1_Rx() == Uart_Sync_Head1)
                        {
                            s = UART_RecData_Deal();

                            if (s != ERR_End)
                            {
                                UART1_SendData(Uart_Sync_Head1);
                                UART1_SendData(Uart_Sync_Head2);
                                UART1_SendData(0x00);
                                if (s == ERR_ERROR)
                                {
                                    UART1_SendData(0x01);
                                }
                                else
                                {
                                    UART1_SendData(0x00);
                                }
                                UART1_SendData(Uart_Sync_Head2);
                                UART1_SendData(Uart_Sync_Head1);
                            }
                        }
                    }
                }
            }
        }
    }
}
