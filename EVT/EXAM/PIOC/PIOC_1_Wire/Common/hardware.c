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
#include "RGB1W.h"

#define RGB    0
#define DS1820 1

// #define Mode   DS1820
#define Mode   RGB_WS2812

__IO	uint16_t	temper;

u8 RGBpbuf[] = {
    0xFF, 0x00, 0x00,
    0x00, 0xFF, 0x00,
    0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00,
    0x00, 0xFF, 0x00,
    0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00,
    0x00, 0xFF, 0x00,
    0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00,
};

u8 RGBpbuf1[] = {
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0xFF,
     0xFF, 0x00, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0xFF,
     0xFF, 0x00, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0xFF,
     0xFF, 0x00, 0x00,
     0x00, 0xFF, 0x00,
};

u8 RGBpbuf2[] = {
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0xFF,
     0xFF, 0x00, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0xFF,
     0xFF, 0x00, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0xFF,
     0xFF, 0x00, 0x00,
     0x00, 0xFF, 0x00,
};

/* Global define */
#define     rgb_source_addr         ((uint8_t *)RGBpbuf2)
#define     rgb_data_bytes          sizeof(RGBpbuf2)  // 10 RGB LEDs, 30 bytes data
#define     timer_to_run            1               // start by timer

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   1_wire function
 *
 * @return  none
 */
void Hardware(void)
{
    printf("1 wire ...\r\n");
#if (Mode==RGB_WS2812)  //RGB
    uint16_t    total_bytes;
	uint8_t     t1=0;
	u8* RGB_RAM;
#elif(Mode==DS1820)  //DS1820
    int16_t     abs_tmp;
#endif

	RGB1W_Init( );
	stat = 0x80;    //free
	while (1)
	{
#if (Mode==RGB_WS2812)  //RGB
		total_bytes = rgb_data_bytes;

#if 0 //short data
        stat = RGB1W_SendSFR_Wait( total_bytes, rgb_source_addr ,0);//SFR mode for 1~32 bytes data
        total_bytes = 0;
        if ( stat == RGB1W_ERR_OK ) printf("1-wire finished\r\n");
        else printf("1-wire error %02x\r\n", stat);
        Delay_Ms(1000);
#else
// long data
        Delay_Ms(200);
        stat = 0x80;//free
        if ( stat != 0xFF ) {//RGB1W completed
            if ( stat < 0x80 ) {//got status in interrupt
                if ( stat == RGB1W_ERR_OK ) printf("1-wire finished\r\n")/**/;
                else printf("1-wire error %02x\r\n", stat)/* */;
                stat = 0x80;//free
            }
            if ( stat == 0x80 && total_bytes && timer_to_run ) {//RAM mode for 1~3072 bytes data
                stat = 0xFF;//wait
                if(t1==0){
                    t1 = 1;
                    RGB_RAM = RGBpbuf;
                }
                else if(t1==1){
                    t1 = 2;
                    RGB_RAM = RGBpbuf1;
                }
                else if(t1==2){
                    t1 = 0;
                    RGB_RAM = RGBpbuf2;
                }
                 RGB1W_SendRAM( total_bytes, RGB_RAM ,0);

                total_bytes = 0;
            }
        }
        if ( PIOC->D8_SYS_CFG & RB_INT_REQ ) {//query if disable interrupt
            stat = PIOC->D8_CTRL_RD;//auto remove interrupt request after reading
        }
#endif

#elif(Mode==DS1820)  //DS1820
		if ( stat != 0xFF ) {//DS1W completed
			if ( stat < 0x80 ) {//got status in interrupt
				if ( stat == RGB1W_ERR_OK ) printf("1-wire finished\r\n");
				else printf("1-wire error %02x\r\n", stat);
				stat = 0x80;//free
			}
			if ( stat == 0x80 && timer_to_run ) {
				stat = 0xFF;//wait
// for external power
                PIOC->D8_SYS_CFG = RB_MST_CLK_GATE | RB_MST_IO_EN0;//run PIOC before write SFR
// set RB_MST_CFG_B4 for parasite power
//				PIOC->D8_SYS_CFG = RB_MST_CLK_GATE | RB_MST_IO_EN0 | RB_MST_CFG_B4;//run PIOC before write SFR
				RGB1W_COMMAND = CMD_T_STA_GET;//start convert and get result
				RGB1W_COMMAND = CMD_T_GET;
				Delay_Ms(1000);
			}
		}
		if ( PIOC->D8_SYS_CFG & RB_INT_REQ ) {//query if disable interrupt
			stat = PIOC->D8_CTRL_RD;//auto remove interrupt request after reading
			printf("1-sta-%08x\r\n", stat);
			temper = PIOC->D16_DATA_REG0_1;//result data
// DS18S20
//          abs_tmp = ( temper + 1 >> 1 ) & 0x7F;// absolute temperature
// DS18B20
            abs_tmp = ( temper + 8 >> 4 ) & 0x7F;// absolute temperature
            if ( temper & 0x8000 ) abs_tmp = 0 - abs_tmp;
        }

#endif
    }
}




