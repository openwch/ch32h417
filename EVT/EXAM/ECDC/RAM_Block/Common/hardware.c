/********************************** (C) COPYRIGHT  *******************************
 * File Name          : hardware.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/05/25
 * Description        : This document contains example routines related to ECDC.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "hardware.h"

/* Global define */

/* Global Variable */

__attribute__((aligned(16))) volatile uint32_t plain_text[128] = {0};
__attribute__((aligned(16))) volatile uint32_t encrypted_text[sizeof(plain_text) / sizeof(*plain_text)] = {0};
__attribute__((aligned(16))) volatile uint32_t decrypted_text[sizeof(plain_text) / sizeof(*plain_text)] = {0};

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   RAM memory block encryption and decryption.
 *
 * @return  none
 */
void Hardware(void)
{
    for (int i = 0; i < sizeof(plain_text) / sizeof(*plain_text); i++, plain_text[i] = i)
        ;
    ECDC_IV_TypeDef IV;
    IV.IV_31T0 = 0x00000001;
    IV.IV_63T32 = 0x00000002;
    IV.IV_95T64 = 0x00000003;
    IV.IV_127T96 = 0x00000300;

    ECDC_KEY_TypeDef key;
    key.KEY_255T224 = 0x9642cadd;
    key.KEY_223T192 = 0xadec514f;
    key.KEY_191T160 = 0x51af5d2e;
    key.KEY_159T128 = 0x84af48fd;
    key.KEY_127T96 = 0xe4a0556b;
    key.KEY_95T64 = 0xb89464ba;
    key.KEY_63T32 = 0x97d4570e;
    key.KEY_31T0 = 0x55acd4c5;

    ECDC_InitTypeDef ecdc_initstruct;
    ecdc_initstruct.Algorithm = ECDCAlgorithm_AES;
    ecdc_initstruct.BlockCipherMode = ECDCBlockCipherMode_ECB;
    ecdc_initstruct.ExcuteMode = ECDC_RAM_Encrypt;
    ecdc_initstruct.ExcuteEndian = ECDCExcuteEndian_Big;
    ecdc_initstruct.KeyLen = ECDCKeyLen_128b;
    ecdc_initstruct.IV = &IV;
    ecdc_initstruct.Key = &key;

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_ECDC, ENABLE);
    ECDC_HardwareClockCmd(ENABLE);
    ECDC_ClockConfig(ECDC_ClockSource_PLLCLK_Div1);

    ECDC_Init(&ecdc_initstruct);

    ECDC_SetDST_BaseAddr((uint32_t)encrypted_text);
    ECDC_SetSRC_BaseAddr((uint32_t)plain_text, sizeof(plain_text) * 8 / 128);

    while (!ECDC_GetFlagStatus(ECDC_FLAG_RAM2RAM_END))
        ;
    ECDC_ClearFlag(ECDC_FLAG_RAM2RAM_END);
    printf("encrypted:");
    for (int i = 0; i < sizeof(encrypted_text) / sizeof(*encrypted_text); i++)
    {
        if (i % 8 == 0)
        {
            printf("\n");
        }
        printf("%#010x ", encrypted_text[i]);
    }
    printf("\n");

    ecdc_initstruct.ExcuteMode = ECDC_RAM_Decrypt;
    ECDC_Init(&ecdc_initstruct);

    ECDC_SetDST_BaseAddr((uint32_t)decrypted_text);
    ECDC_SetSRC_BaseAddr((uint32_t)encrypted_text, sizeof(encrypted_text) * 8 / 128);

    while (!ECDC_GetFlagStatus(ECDC_FLAG_RAM2RAM_END))
        ;

    ECDC_ClearFlag(ECDC_FLAG_RAM2RAM_END);
    printf("decrypted:");
    for (int i = 0; i < sizeof(decrypted_text) / sizeof(*decrypted_text); i++)
    {
        if (i % 8 == 0)
        {
            printf("\n");
        }
        printf("%#010x ", decrypted_text[i]);
    }
    printf("\n");

    printf("plain_text:");
    for (int i = 0; i < sizeof(plain_text) / sizeof(*plain_text); i++)
    {
        if (i % 8 == 0)
        {
            printf("\n");
        }
        printf("%#010x ", plain_text[i]);
    }
    printf("\n");
    while (1)
        ;
}
