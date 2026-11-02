/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/09/20
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"
#include "eeprom.h"
/*
 *@Note
 *The routine implements FLASH emulation IEEPROM to store data. It is suitable for chips 
 *with erased data bits of 0xe339e339, and the chip supports half-word programming (16 bits)
 *Tips:You can choose the size of a single flash memory chip,by the definetion if "EEPROM_PAGE_SIZE".
 *     The value of FLASH_CFGR0_BASE bit 28 directly determines the size of the smallest sector  
 *     You can choose the starting address of FLASH,by the definetion if "EEPROM_START_ADDRESS".
 *     You can choose the number of EEPROM caches,by the definetion if "NB_VAR".
 *     EEPROM virtual address, meaning the label can be customized, corresponds to the initial value of the array -"VirtAddVarTab".
 */

/* Global Variable */

extern uint16_t VirtAddVarTab[NB_VAR];
extern uint16_t VarDataTab[NB_VAR];
extern uint16_t VarValue, VarDataTmp;

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   EEPROM emulation test.
 *
 * @return  none
 */
void Hardware(void)
{

    if (EEPROM_Init() != FLASH_COMPLETE) {
        printf ("init error \r\n");
        while(1);
    }

    for (int i = 0; i < NB_VAR; i++) {
        EEPROM_Read (VirtAddVarTab[i], &VarDataTab[i]);
        printf ("%04x - %04x\r\n", VirtAddVarTab[i], VarDataTab[i]);
    }

    for (VarValue = 1; VarValue <= 0x400; VarValue++) {

        EEPROM_Write (VirtAddVarTab[0], VarValue);
        EEPROM_Read (VirtAddVarTab[0], &VarDataTab[0]);
        if (VarValue != VarDataTab[0]) {
            printf ("error 0 - %04x \r\n", VarValue);
        }

        EEPROM_Write (VirtAddVarTab[1], VarValue);
        EEPROM_Read (VirtAddVarTab[1], &VarDataTab[1]);
        if (VarValue != VarDataTab[1]) {
            printf ("error 1 - %04x\r\n", VarValue);
        }

        EEPROM_Write (VirtAddVarTab[2], VarValue);
        EEPROM_Read (VirtAddVarTab[2], &VarDataTab[2]);
        if (VarValue != VarDataTab[2]) {
            printf ("error 2 - %04x\r\n", VarValue);
        }
        printf ("check OK - %04x\r\n", VarValue);
    }

	while(1)
	{

	}
}
