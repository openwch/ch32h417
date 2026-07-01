/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/03/01
 * Description        : Main program body for V3F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 * SDI-Print routine (V3F core):
 * printf() output is streamed over the debug link (read by the WCH-Link and
 * forwarded to its USB-CDC serial port) instead of a USART peripheral, so no
 * extra UART wiring is required.
 *
 * SDI print is selected by SDI_PRINT == SDI_PR_OPEN (enabled in ch32h417_conf.h
 * for this example). The character transport lives in SRC/Debug/debug.c, which
 * writes to the memory-mapped Debug Module data registers DATA0/DATA1. Their
 * address is chip-specific: (0xE0000000 + hartinfo.dataaddr). On the CH32H417
 * hartinfo = 0x00212340, so DATA0/DATA1 are at 0xE0000340/0xE0000344 (the
 * CH32V-series value 0xE0000380 does not apply here).
 *
 * How to run:
 *   1. Build and download this project.
 *   2. Enable SDI print on the probe:  wlink sdi-print enable --chip CH32H41X
 *   3. Open the WCH-Link serial port (e.g. 115200 8N1) to read the output.
 */

#include "debug.h"

int main(void)
{
    uint8_t i = 0;

    SystemInit();
    SystemAndCoreClockUpdate();
    Delay_Init();

#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif

    printf("SystemClk:%d\r\n", SystemClock);
    printf("V3F SystemCoreClk:%d\r\n", SystemCoreClock);

    while(1)
    {
        Delay_Ms(1000);
        printf("SDI print test %d\r\n", i++);
    }
}
