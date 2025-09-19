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
/*
 *@Note
  With HOST_IAP, with as IAP's supporting APP use. The generated Hex file needs to be converted to a bin file.

  If the V5F core is used, the BIN files need to be merged. The merging method can be found in the documentation 
  provided in the IAP\USB_UART example.
*/

#include "debug.h"
#include "stdio.h"
#include "string.h"
/*******************************************************************************/
/* Macro Definitions */

/* Flash page size */
#define DEF_FLASH_SECTOR_SIZE             8192

/* APP CODE ADDR Setting */
#define DEF_APP_CODE_START_ADDR           0x0800C000                                                /* IAP Flash Operation start address, user code start address */
#define DEF_APP_CODE_END_ADDR             0x08077FFF                                                /* IAP Flash Operation end address, user code end address */
                                                                                                    /* Please refer to link.ld file for accuracy flash size */
#define DEF_APP_CODE_MAXLEN               (DEF_APP_CODE_END_ADDR - DEF_APP_CODE_START_ADDR + 1)     /* IAP Flash Operation size, user code max size */

/* Verify CODE ADDR Setting */
#define DEF_VERIFY_CODE_START_ADDR        (DEF_APP_CODE_START_ADDR - DEF_FLASH_SECTOR_SIZE)         /* IAP Flash verify-code start address */
#define DEF_VERIFY_CODE_END_ADDR          (DEF_APP_CODE_START_ADDR - 1)                             /* IAP Flash verify-code end address */
#define DEF_VERIFY_CODE_MAXLEN            (DEF_VERIFY_CODE_END_ADDR - DEF_VERIFY_CODE_START_ADDR)   /* IAP Flash verify-code max size */
#define DEF_VERIFY_CODE_LEN               0x10                                                      /* IAP Flash verify-code actual length, be careful not to exceed the DEF_VERIFY_CODE_MAXLEN */

/* Flash Operation Key Setting */
#define DEF_FLASH_OPERATION_KEY_CODE_0    0x1A86FF00                                                /* IAP Flash operation Key-code 0 */
#define DEF_FLASH_OPERATION_KEY_CODE_1    0x55AA55AA                                                /* IAP Flash operation Key-code 1 */

/*******************************************************************************/
/* Flash Operation Key */
volatile uint32_t Flash_Operation_Key0;
volatile uint32_t Flash_Operation_Key1;

/*********************************************************************
 * @fn      GPIO_Cfg_init
 *
 * @brief   GPIO init
 *
 * @return  none
 */
void GPIO_Cfg_init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;

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
 * @return  0 - not Press Down
 *          1 - Press Down
 */
uint8_t PA0_Check( void )
{
    u8 i;
    i = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
    if (i == 0)
    {
        Delay_Ms(200);
        i = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
        if (i == 0)
        {
            return 1;
        }
    }
    return 0;
}

/*********************************************************************
 * @fn      IAP_Flash_Erase
 *
 * @brief   Erase Flash In page(256 bytes),Specify length & address,
 *          Based On Fast Flash Operation,
 *          With address protection and program runaway protection.
 *
 * @return  0: Operation Success
 *          See notes for other errors
 */
uint8_t IAP_Flash_Erase( uint32_t address, uint32_t length )
{
    /* Verify Keys, No flash operation if keys are not correct */
    if( (Flash_Operation_Key0 != DEF_FLASH_OPERATION_KEY_CODE_0) || (Flash_Operation_Key1 != DEF_FLASH_OPERATION_KEY_CODE_1) )
    {
        /* ERR: Risk of code running away */
        return 0xFF;
    }
    /* Verify Address, No flash operation if the address is out of range */
    if((address >= DEF_VERIFY_CODE_START_ADDR) && ((address + length - 1) <= DEF_VERIFY_CODE_END_ADDR))
    {
        FLASH_ROM_ERASE(DEF_VERIFY_CODE_START_ADDR,DEF_FLASH_SECTOR_SIZE);
    }

    return 0;
}

/*********************************************************************
 * @fn      IAP_VerifyCode_Erase
 *
 * @brief   Erase IAP VerifyCode, Based On Fast Flash Operation.
 *          With address protection and program runaway protection.
 *
 * @return  ret : The meaning of 'ret' can be found in the notes of the
 *          corresponding function.
 */
uint8_t  IAP_VerifyCode_Erase( void )
{
    uint8_t ret;

    /* Verify Code Erase */
    Flash_Operation_Key1 = DEF_FLASH_OPERATION_KEY_CODE_1;
    ret = IAP_Flash_Erase( DEF_VERIFY_CODE_START_ADDR, DEF_FLASH_SECTOR_SIZE );
    Flash_Operation_Key1 = 0;
    if( ret != 0 )
    {
        return ret;
    }

    return 0;
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
    uint8_t ret;
    printf("Build Time: %s %s\n", __DATE__, __TIME__);
    printf("GCC Version: %d.%d.%d\n",__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
    printf("IAP\n");
    GPIO_Cfg_init();
    while(1)
    {
        ret = PA0_Check( );
        if( ret )
        {
            Flash_Operation_Key0 = DEF_FLASH_OPERATION_KEY_CODE_0;
            printf( "Reset Chip, prepare to Jump To IAP\r\n" );
            printf( "Erase Verify-Code\r\n" );
            IAP_VerifyCode_Erase( );
            NVIC_SystemReset( );
        }
        Delay_Ms( 200 );
        printf( "USER Code\r\n" );
    }
}
