/********************************** (C) COPYRIGHT *******************************
 * File Name          : eeprom.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/09/17
 * Description        : eeprom functions.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "debug.h"
#include "eeprom.h"

#pragma GCC push_options
#pragma GCC optimize("no-jump-tables")

uint16_t VirtAddVarTab[NB_VAR] = {0x5a00, 0x5a01, 0x5a02};
uint16_t VarDataTab[NB_VAR] = {0, 0, 0};
uint16_t VarValue, VarDataTmp = 0;
uint16_t DataVar = 0;

/* Private functions ---------------------------------------------------------*/
static FLASH_Status EEPROM_Format (void);
static uint16_t EEPROM_FindValidPage (uint8_t Operation);
static uint16_t EEPROM_VerifyPageFullWriteVariable (uint16_t VirtAddress, uint16_t Data);
static uint16_t EEPROM_PageTransfer (uint16_t VirtAddress, uint16_t Data);
static uint16_t EEPROM_VerifyPageFullyErased (uint32_t Address);


/*********************************************************************
 * @fn      EEPROM_FindValidPage
 *
 * @brief   Find valid Page for write or read operation.
 *
 * @param   Operation - READ_VALID or WRITE_VALID.
 *
 * @return  uint16_t - Valid page number or NO_VALID_PAGE.
 */
static uint16_t EEPROM_FindValidPage (uint8_t Operation) {
    uint16_t PageStatus0 = 0xFFFF, PageStatus1 = 0xFFFF;

    /* Get Page0 status */
    PageStatus0 = (*(__IO uint16_t *)EEPROM_PART0_BASE_ADDRESS);

    /* Get Page1 status */
    PageStatus1 = (*(__IO uint16_t *)EEPROM_PART1_BASE_ADDRESS);

    /* Write or read operation */
    switch (Operation) {
    case WRITE_VALID: /* Write operation */
        if (PageStatus1 == PAGE_STATE_VALID) {
            /* Page0 receiving data */
            if (PageStatus0 == PAGE_STATE_RECEIVE) {
                return PAGE0; /* Page0 valid */
            } else {
                return PAGE1; /* Page1 valid */
            }
        } else if (PageStatus0 == PAGE_STATE_VALID) {
            /* Page1 receiving data */
            if (PageStatus1 == PAGE_STATE_RECEIVE) {
                return PAGE1; /* Page1 valid */
            } else {
                return PAGE0; /* Page0 valid */
            }
        } else {
            return NO_VALID_PAGE; /* No valid Page */
        }

    case READ_VALID:              /* Read operation */
        if (PageStatus0 == PAGE_STATE_VALID) {
            return PAGE0;         /* Page0 valid */
        } else if (PageStatus1 == PAGE_STATE_VALID) {
            return PAGE1;         /* Page1 valid */
        } else {
            return NO_VALID_PAGE; /* No valid Page */
        }

    default:
        return PAGE0; /* Page0 valid */
    }
}


/*********************************************************************
 * @fn      EEPROM_VerifyPageFullWriteVariable
 *
 * @brief   Verify if active page is full and writes variable in EEPROM.
 *
 * @param   VirtAddress - Variable virtual address.
 *          Data - Variable data to be written.
 *
 * @return  uint16_t - Flash operation status.
 */
static uint16_t EEPROM_VerifyPageFullWriteVariable (uint16_t VirtAddress, uint16_t Data) {
    FLASH_Status FlashStatus = FLASH_COMPLETE;
    uint16_t ValidPage = PAGE0;
    uint32_t Address = EEPROM_START_ADDRESS, PageEndAddress = EEPROM_START_ADDRESS + EEPROM_PAGE_SIZE;

    uint32_t data32 = Data;
    uint32_t VirtAddress1 = VirtAddress;

    /* Get valid Page for write operation */
    ValidPage = EEPROM_FindValidPage (WRITE_VALID);

    /* Check if there is no valid page */
    if (ValidPage == NO_VALID_PAGE) {
        return NO_VALID_PAGE;
    }

    /* Get the valid Page start Address */
    Address = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * EEPROM_PAGE_SIZE));

    /* Get the valid Page end Address */
    PageEndAddress = (uint32_t)((EEPROM_START_ADDRESS - 1) + (uint32_t)((ValidPage + 1) * EEPROM_PAGE_SIZE));

    /* Check each active page address starting from beginning */
    while (Address < PageEndAddress) {
        /* Verify if Address and Address+4 contents are 0xe339e339 */
        if ((*(__IO uint32_t *)Address) == ((((uint32_t)PAGE_STATE_EMPTY) <<16) | (uint32_t)PAGE_STATE_EMPTY)) {
            /* Set variable data */

            FLASH_Unlock();
            FlashStatus = FLASH_ProgramHalfWord (Address, (uint16_t)data32);

            /* If program operation was failed, a Flash error code is returned */
            if (FlashStatus != FLASH_COMPLETE) {
                return FlashStatus;
            }
            /* Set variable virtual address */

            FlashStatus = FLASH_ProgramHalfWord (Address + GAP_STEP, (uint16_t)VirtAddress1);
            FLASH_Lock();

            if (FlashStatus != FLASH_COMPLETE) {
                return FlashStatus;
            }
            /* Return program operation status */
            return FlashStatus;
        } else {
            /* Next address location */
            Address = Address + GAP_STEP * 2;
        }
    }

    /* Return PAGE_FULL in case the valid page is full */
    return PAGE_FULL;
}


/*********************************************************************
 * @fn      EEPROM_VerifyPageFullyErased
 *
 * @brief   Verify if specified page is fully erased.
 *
 * @param   Address - Page base address to be verified.
 *
 * @return  uint16_t - (0: Page not erased, 1: Page erased).
 */
static uint16_t EEPROM_VerifyPageFullyErased (uint32_t Address) {
    uint32_t EndAddress;
    uint32_t ReadStatus = 1;
    uint32_t AddressValue = 0x55555555;

    /* end-address */
    EndAddress = (uint32_t)(Address + (EEPROM_PAGE_SIZE - 4U));

    /* Check each active page address starting from end */
    while (Address <= EndAddress) {
        /* Get the current location content to be compared with virtual address */
        AddressValue = (*(__IO uint32_t *)Address);

        /* Compare the read address with the virtual address */
        if (AddressValue != (((uint32_t)PAGE_STATE_EMPTY) | (((uint32_t)PAGE_STATE_EMPTY) << 16))) {

            /* In case variable value is read, reset ReadStatus flag */
            ReadStatus = 0;

            break;
        }
        /* Next address location */
        Address = Address + 4;
    }

    /* Return ReadStatus value: (0: Page not erased, 1: Sector erased) */
    return ReadStatus;
}


/*********************************************************************
 * @fn      EEPROM_Format
 *
 * @brief   Erases PAGE0 and PAGE1 and writes VALID_PAGE header to PAGE0.
 *
 * @return  FLASH_Status - Flash operation status.
 */
static FLASH_Status EEPROM_Format (void) {
    FLASH_Status FlashStatus = FLASH_COMPLETE;

    uint32_t valid = 0x0000;

    /* Erase Page0 */
    if (!EEPROM_VerifyPageFullyErased (EEPROM_PART0_BASE_ADDRESS)) {
        FlashStatus = FLASH_ROM_ERASE (EEPROM_PART0_BASE_ADDRESS, EEPROM_PAGE_SIZE);

        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE) {
            return FlashStatus;
        }
    }

    /* Set Page0 as valid page: Write VALID_PAGE at Page0 base address */
    FLASH_Unlock();
    FlashStatus = FLASH_ProgramHalfWord (EEPROM_PART0_BASE_ADDRESS, (uint16_t)valid);
    FLASH_Lock();

    /* If program operation was failed, a Flash error code is returned */
    if (FlashStatus != FLASH_COMPLETE) {
        return FlashStatus;
    }

    /* Erase Page1 */
    if (!EEPROM_VerifyPageFullyErased (EEPROM_PART1_BASE_ADDRESS)) {

        FlashStatus = FLASH_ROM_ERASE (EEPROM_PART1_BASE_ADDRESS, EEPROM_PAGE_SIZE);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE) {
            return FlashStatus;
        }
    }

    return FLASH_COMPLETE;
}


/*********************************************************************
 * @fn      EEPROM_PageTransfer
 *
 * @brief   Transfers last updated variables data from the full Page to an empty one.
 *
 * @param   VirtAddress - Variable virtual address.
 *          Data - Variable data to be written.
 *
 * @return  uint16_t - Flash operation status.
 */
static uint16_t EEPROM_PageTransfer (uint16_t VirtAddress, uint16_t Data) {
    FLASH_Status FlashStatus = FLASH_COMPLETE;
    uint32_t NewPageAddress = EEPROM_START_ADDRESS;
    uint16_t OldPageId = 0;
    uint16_t ValidPage = PAGE0, VarIdx = 0;
    uint16_t EepromStatus = 0, ReadStatus = 0;

    uint32_t valid = 0x0000;
    uint32_t receive = PAGE_STATE_RECEIVE;

    /* Get active Page for read operation */
    ValidPage = EEPROM_FindValidPage (READ_VALID);

    if (ValidPage == PAGE1) /* Page1 valid */
    {
        /* New page address where variable will be moved to */
        NewPageAddress = EEPROM_PART0_BASE_ADDRESS;

        /* Old page ID where variable will be taken from */
        OldPageId = PAGE1;
    } else if (ValidPage == PAGE0) /* Page0 valid */
    {
        /* New page address  where variable will be moved to */
        NewPageAddress = EEPROM_PART1_BASE_ADDRESS;

        /* Old page ID where variable will be taken from */
        OldPageId = PAGE0;
    } else {
        return NO_VALID_PAGE; /* No valid Page */
    }

    /* Set the new Page status to RECEIVE_DATA status */

    FLASH_Unlock();
    FlashStatus = FLASH_ProgramHalfWord (NewPageAddress, (uint16_t)receive);
    FLASH_Lock();

    /* If program operation was failed, a Flash error code is returned */
    if (FlashStatus != FLASH_COMPLETE) {
        return FlashStatus;
    }

    /* Write the variable passed as parameter in the new active page */
    EepromStatus = EEPROM_VerifyPageFullWriteVariable (VirtAddress, Data);
    /* If program operation was failed, a Flash error code is returned */
    if (EepromStatus != FLASH_COMPLETE) {
        return EepromStatus;
    }

    /* Transfer process: transfer variables from old to the new active page */
    for (VarIdx = 0; VarIdx < NB_VAR; VarIdx++) {
        if (VirtAddVarTab[VarIdx] != VirtAddress) /* Check each variable except the one passed as parameter */
        {
            /* Read the other last variable updates */
            ReadStatus = EEPROM_Read (VirtAddVarTab[VarIdx], &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (ReadStatus != 0x1) {
                /* Transfer the variable to the new active page */
                EepromStatus = EEPROM_VerifyPageFullWriteVariable (VirtAddVarTab[VarIdx], DataVar);
                /* If program operation was failed, a Flash error code is returned */
                if (EepromStatus != FLASH_COMPLETE) {
                    return EepromStatus;
                }
            }
        }
    }

    /* Erase the old Page: Set old Page status to ERASED status */


    if (OldPageId == PAGE0) {
        FlashStatus = FLASH_ROM_ERASE (EEPROM_PART0_BASE_ADDRESS, EEPROM_PAGE_SIZE);
    } else if (OldPageId == PAGE1) {
        FlashStatus = FLASH_ROM_ERASE (EEPROM_PART1_BASE_ADDRESS, EEPROM_PAGE_SIZE);
    }

    /* If erase operation was failed, a Flash error code is returned */
    if (FlashStatus != FLASH_COMPLETE) {
        return FlashStatus;
    }

    /* Set new Page status to VALID_PAGE status */

    FLASH_Unlock();
    FlashStatus = FLASH_ProgramHalfWord (NewPageAddress, (uint16_t)valid);
    FLASH_Lock();

    /* If program operation was failed, a Flash error code is returned */
    if (FlashStatus != FLASH_COMPLETE) {
        return FlashStatus;
    }

    /* Return last operation flash status */
    return FlashStatus;
}


/*********************************************************************
 * @fn      EEPROM_Init
 *
 * @brief   Restore the pages to a known good state in case of page's status corruption after a power loss.
 *
 * @return  uint16_t - Flash operation status.
 */
uint16_t EEPROM_Init (void) {
    uint16_t PageStatus0 = 6, PageStatus1 = 6;
    uint16_t VarIdx = 0;
    uint16_t EepromStatus = 0, ReadStatus = 0;
    int16_t x = -1;
    FLASH_Status FlashStatus;

    uint32_t valid = 0x0000;

    /* Get Page0 status */
    PageStatus0 = (*(__IO uint16_t *)EEPROM_PART0_BASE_ADDRESS);
    /* Get Page1 status */
    PageStatus1 = (*(__IO uint16_t *)EEPROM_PART1_BASE_ADDRESS);

    /* Check for invalid header states and repair if necessary */
    switch (PageStatus0) {
    case PAGE_STATE_EMPTY:
        if (PageStatus1 == PAGE_STATE_VALID) /* Page0 erased, Page1 valid */
        {
            /* Erase Page0 */
            if (!EEPROM_VerifyPageFullyErased (EEPROM_PART0_BASE_ADDRESS)) {

                FlashStatus = FLASH_ROM_ERASE (EEPROM_PART0_BASE_ADDRESS, EEPROM_PAGE_SIZE);

                /* If erase operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE) {
                    return FlashStatus;
                }
            }
        } else if (PageStatus1 == PAGE_STATE_RECEIVE) /* Page0 erased, Page1 receive */
        {
            /* Erase Page0 */
            if (!EEPROM_VerifyPageFullyErased (EEPROM_PART0_BASE_ADDRESS)) {

                FlashStatus = FLASH_ROM_ERASE (EEPROM_PART0_BASE_ADDRESS, EEPROM_PAGE_SIZE);
                /* If erase operation was failed, a Flash error code is returned */

                if (FlashStatus != FLASH_COMPLETE) {
                    return FlashStatus;
                }
            }
            /* Mark Page1 as valid */

            FLASH_Unlock();
            FlashStatus = FLASH_ProgramHalfWord (EEPROM_PART1_BASE_ADDRESS, (uint16_t)valid);
            FLASH_Lock();

            /* If program operation was failed, a Flash error code is returned */
            if (FlashStatus != FLASH_COMPLETE) {
                return FlashStatus;
            }
        } else /* First EEPROM access (Page0&1 are erased) or invalid state -> format EEPROM */
        {
            /* Erase both Page0 and Page1 and set Page0 as valid page */
            FlashStatus = EEPROM_Format();
            /* If erase/program operation was failed, a Flash error code is returned */
            if (FlashStatus != FLASH_COMPLETE) {
                return FlashStatus;
            }
        }
        break;

    case PAGE_STATE_RECEIVE:
        if (PageStatus1 == PAGE_STATE_VALID) /* Page0 receive, Page1 valid */
        {
            /* Transfer data from Page1 to Page0 */
            for (VarIdx = 0; VarIdx < NB_VAR; VarIdx++) {
                if ((*(__IO uint16_t *)(EEPROM_PART0_BASE_ADDRESS + 6)) == VirtAddVarTab[VarIdx]) {
                    x = VarIdx;
                }
                if (VarIdx != x) {
                    /* Read the last variables' updates */
                    ReadStatus = EEPROM_Read (VirtAddVarTab[VarIdx], &DataVar);
                    /* In case variable corresponding to the virtual address was found */
                    if (ReadStatus != 0x1) {
                        /* Transfer the variable to the Page0 */
                        EepromStatus = EEPROM_VerifyPageFullWriteVariable (VirtAddVarTab[VarIdx], DataVar);
                        /* If program operation was failed, a Flash error code is returned */
                        if (EepromStatus != FLASH_COMPLETE) {
                            return EepromStatus;
                        }
                    }
                }
            }
            /* Mark Page0 as valid */

            FLASH_Unlock();
            FlashStatus = FLASH_ProgramHalfWord (EEPROM_PART0_BASE_ADDRESS, (uint16_t)valid);
            FLASH_Lock();

            /* If program operation was failed, a Flash error code is returned */
            if (FlashStatus != FLASH_COMPLETE) {
                return FlashStatus;
            }

            /* Erase Page1 */
            if (!EEPROM_VerifyPageFullyErased (EEPROM_PART1_BASE_ADDRESS)) {

                FlashStatus = FLASH_ROM_ERASE (EEPROM_PART1_BASE_ADDRESS, EEPROM_PAGE_SIZE);
                /* If erase operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE) {
                    return FlashStatus;
                }
            }
        } else if (PageStatus1 == PAGE_STATE_EMPTY) /* Page0 receive, Page1 erased */
        {
            /* Erase Page1 */
            if (!EEPROM_VerifyPageFullyErased (EEPROM_PART1_BASE_ADDRESS)) {

                FlashStatus = FLASH_ROM_ERASE (EEPROM_PART1_BASE_ADDRESS, EEPROM_PAGE_SIZE);

                /* If erase operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE) {
                    return FlashStatus;
                }
            }
            /* Mark Page0 as valid */

            FLASH_Unlock();
            FlashStatus = FLASH_ProgramHalfWord (EEPROM_PART0_BASE_ADDRESS, (uint16_t)valid);
            FLASH_Lock();

            /* If program operation was failed, a Flash error code is returned */
            if (FlashStatus != FLASH_COMPLETE) {
                return FlashStatus;
            }
        } else /* Invalid state -> format eeprom */
        {
            /* Erase both Page0 and Page1 and set Page0 as valid page */
            FlashStatus = EEPROM_Format();
            /* If erase/program operation was failed, a Flash error code is returned */
            if (FlashStatus != FLASH_COMPLETE) {
                return FlashStatus;
            }
        }
        break;

    case PAGE_STATE_VALID:
        if (PageStatus1 == PAGE_STATE_VALID) /* Invalid state -> format eeprom */
        {
            /* Erase both Page0 and Page1 and set Page0 as valid page */
            FlashStatus = EEPROM_Format();
            /* If erase/program operation was failed, a Flash error code is returned */
            if (FlashStatus != FLASH_COMPLETE) {
                return FlashStatus;
            }
        } else if (PageStatus1 == PAGE_STATE_EMPTY) /* Page0 valid, Page1 erased */
        {
            /* Erase Page1 */
            if (!EEPROM_VerifyPageFullyErased (EEPROM_PART1_BASE_ADDRESS)) {

                FlashStatus = FLASH_ROM_ERASE (EEPROM_PART1_BASE_ADDRESS, EEPROM_PAGE_SIZE);
                /* If erase operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE) {
                    return FlashStatus;
                }
            }
        } else /* Page0 valid, Page1 receive */
        {
            /* Transfer data from Page0 to Page1 */
            for (VarIdx = 0; VarIdx < NB_VAR; VarIdx++) {
                if ((*(__IO uint16_t *)(EEPROM_PART1_BASE_ADDRESS + 6)) == VirtAddVarTab[VarIdx]) {
                    x = VarIdx;
                }
                if (VarIdx != x) {
                    /* Read the last variables' updates */
                    ReadStatus = EEPROM_Read (VirtAddVarTab[VarIdx], &DataVar);
                    /* In case variable corresponding to the virtual address was found */
                    if (ReadStatus != 0x1) {
                        /* Transfer the variable to the Page1 */
                        EepromStatus = EEPROM_VerifyPageFullWriteVariable (VirtAddVarTab[VarIdx], DataVar);
                        /* If program operation was failed, a Flash error code is returned */
                        if (EepromStatus != FLASH_COMPLETE) {
                            return EepromStatus;
                        }
                    }
                }
            }
            /* Mark Page1 as valid */

            FLASH_Unlock();
            FlashStatus = FLASH_ProgramHalfWord (EEPROM_PART1_BASE_ADDRESS, (uint16_t)valid);
            FLASH_Lock();

            /* If program operation was failed, a Flash error code is returned */
            if (FlashStatus != FLASH_COMPLETE) {
                return FlashStatus;
            }
            /* Erase Page0 */
            if (!EEPROM_VerifyPageFullyErased (EEPROM_PART0_BASE_ADDRESS)) {

                FlashStatus = FLASH_ROM_ERASE (EEPROM_PART0_BASE_ADDRESS, EEPROM_PAGE_SIZE);

                /* If erase operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE) {
                    return FlashStatus;
                }
            }
        }
        break;

    default: /* Any other state -> format eeprom */
        /* Erase both Page0 and Page1 and set Page0 as valid page */
        FlashStatus = EEPROM_Format();
        /* If erase/program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE) {
            return FlashStatus;
        }
        break;
    }

    return FLASH_COMPLETE;
}


/*********************************************************************
 * @fn      EEPROM_Read
 *
 * @brief   Reads the last stored variable data, if found, which correspond to the passed virtual address.
 *
 * @param   VirtAddress - Variable virtual address.
 *          Data - Pointer to the variable data.
 *
 * @return  uint16_t - (0: variable exist, 1: variable doesn't exist).
 */
uint16_t EEPROM_Read (uint16_t VirtAddress, uint16_t *Data) {
    uint16_t ValidPage = PAGE0;
    uint16_t AddressValue = 0x5555, ReadStatus = 1;
    uint32_t Address = EEPROM_START_ADDRESS, PageStartAddress = EEPROM_START_ADDRESS;

    /* Get active Page for read operation */
    ValidPage = EEPROM_FindValidPage (READ_VALID);

    /* Check if there is no valid page */
    if (ValidPage == NO_VALID_PAGE) {
        return NO_VALID_PAGE;
    }

    /* Get the valid Page start Address */
    PageStartAddress = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * EEPROM_PAGE_SIZE));

    /* Get the valid Page end Address */
    Address = (uint32_t)((EEPROM_START_ADDRESS - GAP_STEP) + (uint32_t)((1 + ValidPage) * EEPROM_PAGE_SIZE));

    /* Check each active page address starting from end */
    while (Address > (PageStartAddress + GAP_STEP)) {
        /* Get the current location content to be compared with virtual address */
        AddressValue = (*(__IO uint16_t *)Address);

        /* Compare the read address with the virtual address */
        if (AddressValue == VirtAddress) {
            /* Get content of Address-2 which is variable value */
            *Data = (*(__IO uint16_t *)(Address - GAP_STEP));

            /* In case variable value is read, reset ReadStatus flag */
            ReadStatus = 0;

            break;
        } else {
            /* Next address location */
            Address = Address - GAP_STEP * 2;
        }
    }

    /* Return ReadStatus value: (0: variable exist, 1: variable doesn't exist) */
    return ReadStatus;
}


/*********************************************************************
 * @fn      EEPROM_Write
 *
 * @brief   Writes the variable data to the emulated EEPROM.
 *
 * @param   VirtAddress - Variable virtual address.
 *          Data - Variable data to be written.
 *
 * @return  uint16_t - Flash operation status.
 */
uint16_t EEPROM_Write (uint16_t VirtAddress, uint16_t Data) {
    uint16_t Status = 0;

    /* Write the variable virtual address and value in the EEPROM */
    Status = EEPROM_VerifyPageFullWriteVariable (VirtAddress, Data);

    /* In case the EEPROM active page is full */
    if (Status == PAGE_FULL) {
        /* Perform Page transfer */
        Status = EEPROM_PageTransfer (VirtAddress, Data);
    }

    /* Return last operation status */
    return Status;
}

#pragma GCC pop_options