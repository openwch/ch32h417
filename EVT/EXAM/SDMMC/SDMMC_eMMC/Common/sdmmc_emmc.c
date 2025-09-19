/********************************** (C) COPYRIGHT *******************************
* File Name          : sdmmc_emmc.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/09/12
* Description        : This file contains the headers of the SDMMC.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "debug.h"
#include "ch32h417.h"
#include "sdmmc_emmc.h"
#include "string.h"

SDMMC_CMDInitTypeDef SDMMC_CmdInitStructure={0};
SDMMC_TranModeTypeDef SDMMC_TranModeStructure = {0};
static u8 CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1;
static u32 CSD_Tab[4], CID_Tab[4], RCA = 0x01;
#define SDMMC_DMADDR_ALIGN_SIZE            16
uint8_t     *SDIO_Write_Buf;
uint8_t     *SDIO_Read_Buf;
volatile uint32_t    SDIO_Write_Buf_Len;
volatile uint32_t    SDIO_Read_Buf_Len;
volatile uint8_t     SDMMC_ErrorTimes = 0x00;                                          
eMMC_CardInfo eMMCCardInfo;
eMMC_CardInfo eMMCCardInfo1;
uint8_t     *SDIO_Read_Buf;

/*********************************************************************
 * @fn      CmdRespError
 *
 * @brief   Errer command.
 *
 * @return  errorstatus: Err code
 */
SD_Error CmdRespError( void )
{
    SD_Error errorstatus = SD_ERROR_NOTDEF;
    uint32_t timeout = 0;
    uint32_t intFlag = 0;
    while( errorstatus == SD_ERROR_NOTDEF )
    {
        intFlag = SDMMC->INT_FG & (SDMMC_FLAG_CMDDONE | SDMMC_FLAG_REIDX_ER
                                | SDMMC_FLAG_RECRC_WR | SDMMC_FLAG_RE_TMOUT);
        if( intFlag )
        {
            if( ( intFlag & SDMMC_FLAG_CMDDONE ) != RESET )
            {
                errorstatus = SD_OK;
            }
            if( ( intFlag & SDMMC_FLAG_REIDX_ER ) != RESET )
            {
                errorstatus = SD_ILLEGAL_CMD;
            }
            if( ( intFlag & SDMMC_FLAG_RECRC_WR ) != RESET )
            {
                errorstatus = SD_CMD_CRC_FAIL;
            }
            if( ( intFlag & SDMMC_FLAG_RE_TMOUT ) != RESET )
            {
                errorstatus = SD_CMD_RSP_TIMEOUT;
            }
            SDMMC->INT_FG = (0x1FF & (SDMMC_FLAG_CMDDONE | SDMMC_FLAG_REIDX_ER
                                    | SDMMC_FLAG_RECRC_WR | SDMMC_FLAG_RE_TMOUT))
                            | 0x600;
        }
        if( ++ timeout > SDMMC_TOUT_TIMES )
        {
            errorstatus = SD_CMD_RSP_TIMEOUT;
            printf( "sdmmc resp tout\n\r" );
        }
    }
    if( ( SDMMC->CMD_SET & SDMMC_CMDIDX_MASK ) == 18 )
    {
        return errorstatus;
    }
    if( ( SDMMC->CMD_SET & SDMMC_CMDIDX_MASK ) == 25 )
    {
        return errorstatus;
    }

    return errorstatus;
}

 /*********************************************************************
 * @fn      eMMC_GPIO_Init
 *
 * @brief   Init SDMMC GPIO.
 *
 * @return  None.
 */ 
void eMMC_GPIO_Init()
{
    u32 ChipID;
    GPIO_InitTypeDef GPIO_Initure={0};
    ChipID=DBGMCU_GetCHIPID();
    if((ChipID&0xF0)==0){
	RCC_HB1PeriphClockCmd(RCC_HB1Periph_SWPMI, ENABLE);
    SWPMI->OR|=(1<<0); 
    }
    RCC_HBPeriphClockCmd(RCC_HBPeriph_SDMMC|RCC_HBPeriph_DMA1, ENABLE);
    //Data4、Data5
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA|RCC_HB2Periph_AFIO,ENABLE);
    GPIO_Initure.GPIO_Pin=GPIO_Pin_14|GPIO_Pin_15; 
    GPIO_Initure.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_Initure.GPIO_Speed=GPIO_Speed_Very_High;
    GPIO_Init(GPIOA,&GPIO_Initure); 
    
    //Data0、Data1、Data2、Data3、Data6、Data7、clk
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC|RCC_HB2Periph_AFIO,ENABLE);
    GPIO_Initure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12; 
    GPIO_Initure.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_Initure.GPIO_Speed=GPIO_Speed_Very_High;
    GPIO_Init(GPIOC,&GPIO_Initure); 

    //CMD、STR
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOD|RCC_HB2Periph_AFIO,ENABLE);
    GPIO_Initure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3; 
    GPIO_Initure.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_Initure.GPIO_Speed=GPIO_Speed_Very_High;
    GPIO_Init(GPIOD,&GPIO_Initure); 
}

/*********************************************************************
 * @fn      SDMMC_SetCommand_Polling
 *
 * @brief   eMMC set commmand.
 *
 * @return  SD_Error - ERR code
 */
SD_Error SDMMC_SetCommand_Polling( uint8_t command, uint32_t arg, uint16_t respexpect )
{
	SDMMC_CmdInitStructure.SDMMC_Argument = arg;
	SDMMC_CmdInitStructure.SDMMC_CMDIdx = command;
	SDMMC_CmdInitStructure.SDMMC_RespExpect = respexpect;
	SDMMC_CommandConfig(&SDMMC_CmdInitStructure);
	return  CmdRespError();
}

/*********************************************************************
 * @fn      MMC_AccessEXTCSD_Polling
 *
 * @brief   eMMC set EXTCSD data.
 * 
 * @param   access : 0 - change CMD set; 1 - set bits; 2 - clear bits; 3 - write byte
 *          index:  location
 *          value:  data
 *
 * @return  SD_Error - ERR code
 */
SD_Error MMC_AccessEXTCSD_Polling( uint8_t access, uint8_t index, uint8_t value )
{
    return SDMMC_SetCommand_Polling( SD_CMD_HS_SWITCH, ((access ) << 24) | (index << 16) | (value << 8), SDMMC_Resp_R1b );
}

/*********************************************************************
 * @fn      eMMC_Init
 *
 * @brief   Init eMMC.
 *
 * @return  errorstatus: Err code
 */
SD_Error eMMC_Init()
{
    SD_Error errorstatus = SD_OK;
    eMMC_GPIO_Init();
    SDMMC_DeInit();
    errorstatus = SD_PowerON();
    if( errorstatus == SD_OK )
    {
    errorstatus = eMMC_Read_CID_CSD();
    }
    if( errorstatus == SD_OK )
    {
       errorstatus = SD_SelectDeselect( ( u32 )(eMMCCardInfo.RCA << 16 ) );  
    }
      
    if( errorstatus == SD_OK )
    { 
        PWR_VIO18LevelCfg(PWR_VIO18Level_MODE1);  
        errorstatus = eMMC_ReadExtCsd(&eMMCCardInfo);    
    } 

    if (errorstatus ==SD_OK )
    {
        errorstatus = eMMC_GetCardInfo(&eMMCCardInfo, &CSD_Tab[0], &CID_Tab[0], RCA);  
    }

    if( errorstatus == SD_OK )
    {
        errorstatus = SD_EnableWideBusOperation( 2 ); 
    }
        
    if( errorstatus == SD_OK )
    { 
        errorstatus = MMC_AccessEXTCSD_Polling( 3, 185,2);   
    }

      if( errorstatus == SD_OK )
    {
        SDMMC_SetClockSpeed(SDMMC_ClockSpeed_High,3);
    }

    return errorstatus;
}

/*********************************************************************
 * @fn      eMMC_PowerON
 *
 * @brief   eMMC Power on
 *
 * @return  SD_Error - ERR code
 */
SD_Error SD_PowerON()
{    
    u8 i = 0;
    SDMMC_InitTypeDef SDMMC_InitStructure = { 0 };
    u32 response = 0, count = 0, validvoltage = 0;
    SD_Error errorstatus = SD_OK;
    SDMMC_ClockCmd( ENABLE ) ;
    SDMMC_InitStructure.SDMMC_Mode = SDMMC_Mode_Host;
    SDMMC_InitStructure.SDMMC_ClockEdge = SDMMC_SampleClock_Rising;
    SDMMC_InitStructure.SDMMC_DMA_EN=1;
    SDMMC_InitStructure.SDMMC_BusWidth = SDMMC_BusWidth_1;
    SDMMC_InitStructure.SDMMC_PhaseInv = SDMMC_Phase_No_Inverse;
    SDMMC_InitStructure.SDMMC_ClockSpeed = SDMMC_ClockSpeed_Low;
    SDMMC_InitStructure.SDMMC_ClockDiv = RB_SDMMC_DIV_MASK;
    SDMMC_InitStructure.SDMMC_TimeOut = 0x0C;
    SDMMC_Init( &SDMMC_InitStructure );
    SDMMC_ClockCmd( ENABLE );
    for( i = 0; i < 74; i++ ){
        SDMMC_CmdInitStructure.SDMMC_Argument= 0x0;
        SDMMC_CmdInitStructure.SDMMC_CMDIdx=SD_CMD_GO_IDLE_STATE;
        SDMMC_CmdInitStructure.SDMMC_RespExpect=SDMMC_Resp_NONE;
        SDMMC_CommandConfig(&SDMMC_CmdInitStructure);
        errorstatus = CmdRespError();
        if( errorstatus == SD_OK )
        {
            break;
        }
    }
    
	while( ( !validvoltage ) && ( count < SD_MAX_VOLT_TRIAL ) )
    {
        SDMMC_CmdInitStructure.SDMMC_Argument = 0x40FF8080;
        SDMMC_CmdInitStructure.SDMMC_CMDIdx = SD_CMD_SEND_OP_COND;
        SDMMC_CmdInitStructure.SDMMC_RespExpect = SDMMC_Resp_48;  //r3
        SDMMC_CommandConfig(&SDMMC_CmdInitStructure);
        errorstatus = CmdRespError();
        if( errorstatus != SD_OK )
        {
            return errorstatus;
        }
        response = SDMMC->RESPONSE3;
        validvoltage = ( ( ( response >> 31 ) == 1 ) ? 1 : 0 );
        count++;
    }
    if( count >= SD_MAX_VOLT_TRIAL )
    {
        errorstatus = SD_INVALID_VOLTRANGE;
        return errorstatus;
    }
    CardType = SDIO_HIGH_CAPACITY_MMC_CARD;
     return errorstatus;
}

/*********************************************************************
 * @fn      SD_EnableWideBusOperation
 *
 * @brief   Enable SDIO Wide bus
 *
 * @param   wmode: 0-1bit 1-4bit 2-8bit
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_EnableWideBusOperation( u32 wmode )
{
    SD_Error errorstatus = SD_OK;
    if( wmode > 2 )
    {
        return SD_UNSUPPORTED_FEATURE;
    }
    else
    {
        switch(wmode)
        {
            case 0:
                SDMMC_CmdInitStructure.SDMMC_Argument = 0x03B70000;
                SDMMC_CmdInitStructure.SDMMC_CMDIdx = SD_CMD_HS_SWITCH;
                SDMMC_CmdInitStructure.SDMMC_RespExpect = SDMMC_Resp_R1b;
                SDMMC_CommandConfig(&SDMMC_CmdInitStructure);
                errorstatus = CmdRespError();
            break;
            case 1:
                SDMMC_CmdInitStructure.SDMMC_Argument = 0x03B70100;
                SDMMC_CmdInitStructure.SDMMC_CMDIdx = SD_CMD_HS_SWITCH;
                SDMMC_CmdInitStructure.SDMMC_RespExpect = SDMMC_Resp_R1b;
                SDMMC_CommandConfig(&SDMMC_CmdInitStructure);
                errorstatus = CmdRespError();
            break;
            case 2:
                SDMMC_CmdInitStructure.SDMMC_Argument = 0x03B70200;
                SDMMC_CmdInitStructure.SDMMC_CMDIdx = SD_CMD_HS_SWITCH;
                SDMMC_CmdInitStructure.SDMMC_RespExpect = SDMMC_Resp_R1b;
                SDMMC_CommandConfig(&SDMMC_CmdInitStructure);
                errorstatus = CmdRespError();
            break;
        }
        if( SD_OK == errorstatus )
        {
            SDMMC->CONTROL&=~( 3 << 0 );
            SDMMC->CONTROL |= ( u16 )wmode << 0;
        }
    }
    return errorstatus;
}

/*********************************************************************
 * @fn      SDMMC_SetCommand
 *
 * @brief   eMMC set command
 *
 * @return  SD_Error - ERR code
 */
SD_Error SDMMC_SetCommand( uint8_t command, uint32_t arg, uint16_t respexpect )
{
	SDMMC_AutoClockControl_ClockEnable();
	SDMMC_CmdInitStructure.SDMMC_Argument = arg;
	SDMMC_CmdInitStructure.SDMMC_CMDIdx = command;
	SDMMC_CmdInitStructure.SDMMC_RespExpect = respexpect;
	SDMMC_CommandConfig(&SDMMC_CmdInitStructure);
	return CmdRespError( );
}

/*********************************************************************
 * @fn      EMMC_SetRCA
 *
 * @brief   eMMC set RCA
 *
 * @return  SD_Error - ERR code
 */
SD_Error EMMC_SetRCA( void )
{
    SD_Error errorstatus = SD_OK;
    errorstatus = SDMMC_SetCommand( SD_CMD_SET_REL_ADDR, 0xAAAA0000, SDMMC_Resp_48);
    if( errorstatus != SD_OK )
    {
        return( errorstatus );
    }
    RCA = 0xAAAA;     
    return( SD_OK );
}

/*********************************************************************
 * @fn      eMMC_Read_CID_CSD
 *
 * @brief   eMMC Read CID Register and CSD Register
 *
 * @return  SD_Error - ERR code
 */
SD_Error eMMC_Read_CID_CSD( void )
{
    SD_Error errorstatus = SD_OK;
    SDMMC_CmdInitStructure.SDMMC_Argument = 0x0;
    SDMMC_CmdInitStructure.SDMMC_CMDIdx = SD_CMD_ALL_SEND_CID;
    SDMMC_CmdInitStructure.SDMMC_RespExpect = SDMMC_Resp_136;
    SDMMC_CommandConfig(&SDMMC_CmdInitStructure);
    errorstatus = CmdRespError();
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }
	CID_Tab[ 0 ] = SDMMC_GetResponse(Response3);
	CID_Tab[ 1 ] = SDMMC_GetResponse(Response2);
	CID_Tab[ 2 ] = SDMMC_GetResponse(Response1);
	CID_Tab[ 3 ] = SDMMC_GetResponse(Response0);

	SDMMC_CmdInitStructure.SDMMC_Argument = 0x1A860000;
	SDMMC_CmdInitStructure.SDMMC_CMDIdx = SD_CMD_SET_REL_ADDR;
	SDMMC_CmdInitStructure.SDMMC_RespExpect = SDMMC_Resp_48;
	SDMMC_CommandConfig(&SDMMC_CmdInitStructure);
	errorstatus = CmdRespError();
  
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }
    if( errorstatus == SD_OK )
    {
       RCA = 0x1A86;
       eMMCCardInfo.RCA=0x1A86;
       }
    SDMMC_CmdInitStructure.SDMMC_Argument = ( uint32_t )( RCA << 16 );
    SDMMC_CmdInitStructure.SDMMC_CMDIdx = SD_CMD_SEND_CSD;
    SDMMC_CmdInitStructure.SDMMC_RespExpect = SDMMC_Resp_136;
    SDMMC_CommandConfig( &SDMMC_CmdInitStructure );
    errorstatus = CmdRespError();
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }
    CSD_Tab[ 0 ] = SDMMC_GetResponse(Response3);
    CSD_Tab[ 1 ] = SDMMC_GetResponse(Response2);
    CSD_Tab[ 2 ] = SDMMC_GetResponse(Response1);
    CSD_Tab[ 3 ] = SDMMC_GetResponse(Response0);
    return SD_OK;
}

/*********************************************************************
 * @fn      SD_SelectDeselect
 *
 * @brief   Select eMMC
 *
 * @param   addr - RCA address
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_SelectDeselect( u32 addr )
{
    SD_Error errorstatus = SD_OK;
    SDMMC_CmdInitStructure.SDMMC_Argument = addr;;
    SDMMC_CmdInitStructure.SDMMC_CMDIdx = SD_CMD_SEL_DESEL_CARD;
    SDMMC_CmdInitStructure.SDMMC_RespExpect = SDMMC_Resp_48;
    SDMMC_CommandConfig( &SDMMC_CmdInitStructure );
    errorstatus = CmdRespError();
    return errorstatus;
}

/*********************************************************************
 * @fn      DataTransErr
 *
 * @brief   Return data transtrforming errs
 *
 * @param   None
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error DataTransErr( void )
{
    SD_Error errorstatus = SD_ERROR_NOTDEF;
    uint32_t timeout = 0;
    uint32_t intFlag = 0;
    while( errorstatus == SD_ERROR_NOTDEF )
    {
        intFlag = SDMMC->INT_FG & (SDMMC_FLAG_TRANDONE | SDMMC_FLAG_BKGAP
                                | SDMMC_FLAG_TRANERR | SDMMC_FLAG_FIFO_OV
                                | SDMMC_FLAG_DATTMO);
        if( intFlag )
        {
            if( ( SDMMC->INT_FG & SDMMC_FLAG_TRANDONE ) != RESET )
            {
                errorstatus = SD_OK;
            }
            if( ( SDMMC->INT_FG & SDMMC_FLAG_BKGAP ) != RESET )
            {
                errorstatus = SD_OK;
            }
            if( ( SDMMC->INT_FG & SDMMC_FLAG_TRANERR ) != RESET )
            {
                errorstatus = SD_DATA_CRC_FAIL;
                if( ( SDMMC->CMD_SET & SDMMC_CMDIDX_MASK ) != 14 )
                {
                    SDMMC_ErrorTimes ++;
                }
            }
            if( ( SDMMC->INT_FG & SDMMC_FLAG_FIFO_OV ) != RESET )
            {
                errorstatus = SD_RX_OVERRUN;
            }
            if( ( SDMMC->INT_FG & SDMMC_FLAG_DATTMO ) != RESET )
            {
                errorstatus = SD_DATA_TIMEOUT;
            }
            SDMMC->INT_FG = (0x1FF & (SDMMC_FLAG_TRANDONE | SDMMC_FLAG_BKGAP
                                    | SDMMC_FLAG_TRANERR | SDMMC_FLAG_FIFO_OV
                                    | SDMMC_FLAG_DATTMO))
                            | 0x600;

        }
        if( ++ timeout > SDMMC_TOUT_TIMES )
        {
            errorstatus = SD_DATA_TIMEOUT;
    
            printf( "sdmmc data tout\r\n" );
        }
    }
    return errorstatus;
}

/*********************************************************************
 * @fn      SDMMC_WaitData0
 *
 * @brief   wait data0 line signal
 *
 * @param   None
 *              
 * @return  0 - sucess
 *          1 - error
 */ 
uint8_t SDMMC_WaitData0( void )
{
    uint32_t retrytimes;

    retrytimes = 0;
    while( ( ++ retrytimes < 500000 ) && ( SDMMC_GetStatus_LineData0( ) == 0x00 ) )
    {
        Delay_Us( 1 );
    }

    if( retrytimes >= 500000 )
    {
        return 1;
    }
    return 0;
}

/*********************************************************************
 * @fn      eMMC_ReadExtCsd
 *
 * @brief   eMMC Read Extended CSD Register
 *
 * @return  SD_Error - ERR code
 */
SD_Error eMMC_ReadExtCsd(eMMC_CardInfo *eMMC)
{
	SD_Error errorstatus = SD_OK;
	__attribute__((aligned(32)))u32 *ExtCsdBuf;
	ExtCsdBuf = (u32 *)(&(eMMC->eMMC_ExtCsd.CsdBuf[0]));

    SDMMC_BlockConfig( 0, 0 );
    SDMMC_TranModeStructure.TranMode_DualDMA_TNCnt = 0x00;
    SDMMC_TranModeStructure.TranMode_DualDMA = DISABLE;
    SDMMC_TranModeStructure.TranMode_AutoGapStop = DISABLE;
    SDMMC_TranModeStructure.TranMode_Boot = DISABLE;
    SDMMC_TranModeStructure.TranMode_Direction = SDMMC_TranDir_Receive;
    SDMMC_TranMode_Init( &SDMMC_TranModeStructure );
    SDMMC_SetDMAAddr1( (uint32_t)ExtCsdBuf );
    SDMMC_BlockConfig( 512, 1 );


    SDMMC_WaitData0( );

    SDMMC_CmdInitStructure.SDMMC_Argument = 0;
    SDMMC_CmdInitStructure.SDMMC_CMDIdx = SD_CMD_HS_SEND_EXT_CSD;
    SDMMC_CmdInitStructure.SDMMC_RespExpect = SDMMC_Resp_48;  //r3
    SDMMC_CommandConfig(&SDMMC_CmdInitStructure);
    errorstatus = CmdRespError();
  
    if( errorstatus != SD_OK )
    {
        SDMMC_BlockConfig( 0, 0 );
        return( errorstatus );
    }


    errorstatus=DataTransErr( );
    return errorstatus;
}

/*********************************************************************
 * @fn       eMMC_GetCardInfo
 *
 * @brief    Get eMMC information
 *
 * @return  SD_Error -  ERR code
 */
SD_Error eMMC_GetCardInfo(eMMC_CardInfo *eMMC, u32 *CSD_Tab, u32 *CID_Tab, u16 Rca)
{
    SD_Error errorstatus = SD_OK;
    u8 tmp = 0;
    eMMC->CardType = (u8)CardType;
    eMMC->RCA = (u16)Rca;
    //CSD
    /* Byte 0 */
    tmp = (u8)((CSD_Tab[0] & 0xFF000000) >> 24);
    eMMC->eMMC_csd.CSDStruct = (tmp & 0xC0) >> 6;
    eMMC->eMMC_csd.SysSpecVersion = (tmp & 0x3C) >> 2;
    eMMC->eMMC_csd.Reserved1 = tmp & 0x03;

    /* Byte 1 */
    tmp = (u8)((CSD_Tab[0] & 0x00FF0000) >> 16);
    eMMC->eMMC_csd.TAAC = tmp;

    /* Byte 2 */
    tmp = (u8)((CSD_Tab[0] & 0x0000FF00) >> 8);
    eMMC->eMMC_csd.NSAC = tmp;

    /* Byte 3 */
    tmp = (u8)(CSD_Tab[0] & 0x000000FF);
    eMMC->eMMC_csd.MaxBusClkFrec = tmp;

    /* Byte 4 */
    tmp = (u8)((CSD_Tab[1] & 0xFF000000) >> 24);
    eMMC->eMMC_csd.CardComdClasses = tmp << 4;

    /* Byte 5 */
    tmp = (u8)((CSD_Tab[1] & 0x00FF0000) >> 16);
    eMMC->eMMC_csd.CardComdClasses |= (tmp & 0xF0) >> 4;
    eMMC->eMMC_csd.RdBlockLen = tmp & 0x0F;

    /* Byte 6 */
    tmp = (u8)((CSD_Tab[1] & 0x0000FF00) >> 8);
    eMMC->eMMC_csd.PartBlockRead = (tmp & 0x80) >> 7;
    eMMC->eMMC_csd.WrBlockMisalign = (tmp & 0x40) >> 6;
    eMMC->eMMC_csd.RdBlockMisalign = (tmp & 0x20) >> 5;
    eMMC->eMMC_csd.DSRImpl = (tmp & 0x10) >> 4;
    eMMC->eMMC_csd.Reserved2 = 0; /*!< Reserved */
    eMMC->eMMC_csd.DeviceSize = (tmp & 0x03) << 10;

    /* Byte 7 */
    tmp = (u8)(CSD_Tab[1] & 0x000000FF);
    eMMC->eMMC_csd.DeviceSize |= (tmp) << 2;

    /* Byte 8 */
    tmp = (u8)((CSD_Tab[2] & 0xFF000000) >> 24);
    eMMC->eMMC_csd.DeviceSize |= (tmp & 0xC0) >> 6;
    eMMC->eMMC_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
    eMMC->eMMC_csd.MaxRdCurrentVDDMax = (tmp & 0x07);

    /* Byte 9 */
    tmp = (u8)((CSD_Tab[2] & 0x00FF0000) >> 16);
    eMMC->eMMC_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
    eMMC->eMMC_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
    eMMC->eMMC_csd.DeviceSizeMul = (tmp & 0x03) << 1;

    /* Byte 10 */
    tmp = (u8)((CSD_Tab[2] & 0x0000FF00) >> 8);
    eMMC->eMMC_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;
    eMMC->CardBlockSize = 1 << (eMMC->eMMC_csd.RdBlockLen);
    eMMC->SectorNums = (u32)(eMMC->eMMC_ExtCsd.EXT_CSD.SEC_COUNT[3] << 24) | (u32)(eMMC->eMMC_ExtCsd.EXT_CSD.SEC_COUNT[2] << 16) |
            (u32)(eMMC->eMMC_ExtCsd.EXT_CSD.SEC_COUNT[1] << 8) | (u32)(eMMC->eMMC_ExtCsd.EXT_CSD.SEC_COUNT[0]);
    eMMC->eMMC_csd.EraseGrSize = (tmp & 0x40) >> 6;
    eMMC->eMMC_csd.EraseGrMul = (tmp & 0x3F) << 1;

    /* Byte 11 */
    tmp = (u8)(CSD_Tab[2] & 0x000000FF);
    eMMC->eMMC_csd.EraseGrMul |= (tmp & 0x80) >> 7;
    eMMC->eMMC_csd.WrProtectGrSize = (tmp & 0x7F);

    /* Byte 12 */
    tmp = (u8)((CSD_Tab[3] & 0xFF000000) >> 24);
    eMMC->eMMC_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
    eMMC->eMMC_csd.ManDeflECC = (tmp & 0x60) >> 5;
    eMMC->eMMC_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
    eMMC->eMMC_csd.MaxWrBlockLen = (tmp & 0x03) << 2;

    /* Byte 13 */
    tmp = (u8)((CSD_Tab[3] & 0x00FF0000) >> 16);
    eMMC->eMMC_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
    eMMC->eMMC_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
    eMMC->eMMC_csd.Reserved3 = 0;
    eMMC->eMMC_csd.ContentProtectAppli = (tmp & 0x01);

    /* Byte 14 */
    tmp = (u8)((CSD_Tab[3] & 0x0000FF00) >> 8);
    eMMC->eMMC_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
    eMMC->eMMC_csd.CopyFlag = (tmp & 0x40) >> 6;
    eMMC->eMMC_csd.PermWrProtect = (tmp & 0x20) >> 5;
    eMMC->eMMC_csd.TempWrProtect = (tmp & 0x10) >> 4;
    eMMC->eMMC_csd.FileFormat = (tmp & 0x0C) >> 2;
    eMMC->eMMC_csd.ECC = (tmp & 0x03);

    /* Byte 15 */
    tmp = (u8)(CSD_Tab[3] & 0x000000FF);
    eMMC->eMMC_csd.CSD_CRC = (tmp & 0xFE) >> 1;
    eMMC->eMMC_csd.Reserved4 = 1;

    //CID
    /* Byte 0 */
    tmp = (u8)((CID_Tab[0] & 0xFF000000) >> 24);
    eMMC->eMMC_cid.ManufacturerID = tmp;

    /* Byte 1 */
    tmp = (u8)((CID_Tab[0] & 0x00FF0000) >> 16);
    eMMC->eMMC_cid.OEM_AppliID = tmp << 8;

    /* Byte 2 */
    tmp = (u8)((CID_Tab[0] & 0x000000FF00) >> 8);
    eMMC->eMMC_cid.OEM_AppliID |= tmp;

    /* Byte 3 */
    tmp = (u8)(CID_Tab[0] & 0x000000FF);
    eMMC->eMMC_cid.ProdName1 = tmp << 24;

    /* Byte 4 */
    tmp = (u8)((CID_Tab[1] & 0xFF000000) >> 24);
    eMMC->eMMC_cid.ProdName1 |= tmp << 16;

    /* Byte 5 */
    tmp = (u8)((CID_Tab[1] & 0x00FF0000) >> 16);
    eMMC->eMMC_cid.ProdName1 |= tmp << 8;

    /* Byte 6 */
    tmp = (u8)((CID_Tab[1] & 0x0000FF00) >> 8);
    eMMC->eMMC_cid.ProdName1 |= tmp;

    /* Byte 7 */
    tmp = (u8)(CID_Tab[1] & 0x000000FF);
    eMMC->eMMC_cid.ProdName2 = tmp;

    /* Byte 8 */
    tmp = (u8)((CID_Tab[2] & 0xFF000000) >> 24);
    eMMC->eMMC_cid.ProdRev = tmp;

    /* Byte 9 */
    tmp = (u8)((CID_Tab[2] & 0x00FF0000) >> 16);
    eMMC->eMMC_cid.ProdSN = tmp << 24;

    /* Byte 10 */
    tmp = (u8)((CID_Tab[2] & 0x0000FF00) >> 8);
    eMMC->eMMC_cid.ProdSN |= tmp << 16;

    /* Byte 11 */
    tmp = (u8)(CID_Tab[2] & 0x000000FF);
    eMMC->eMMC_cid.ProdSN |= tmp << 8;

    /* Byte 12 */
    tmp = (u8)((CID_Tab[3] & 0xFF000000) >> 24);
    eMMC->eMMC_cid.ProdSN |= tmp;

    /* Byte 13 */
    tmp = (u8)((CID_Tab[3] & 0x00FF0000) >> 16);
    eMMC->eMMC_cid.Reserved1 |= (tmp & 0xF0) >> 4;
    eMMC->eMMC_cid.ManufactDate = (tmp & 0x0F) << 8;

    /* Byte 14 */
    tmp = (u8)((CID_Tab[3] & 0x0000FF00) >> 8);
    eMMC->eMMC_cid.ManufactDate |= tmp;

    /* Byte 15 */
    tmp = (u8)(CID_Tab[3] & 0x000000FF);
    eMMC->eMMC_cid.CID_CRC = (tmp & 0xFE) >> 1;
    eMMC->eMMC_cid.Reserved2 = 1;

    return(errorstatus);
}

/*********************************************************************
 * @fn       SD_WriteBlock
 *
 * @brief   Write block
 *
 * @param   *buf - data
 *                 addr - RCA address
 *                 blksize - block size
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_WriteBlock( uint8_t *buf, long long addr,  uint16_t blksize )
{
    SD_Error errorstatus;

    if( ( ( uint32_t )buf % 16 ) != 0 )
    {
        return SD_INVALID_PARAMETER;
    }

    if( (CardType == SDIO_HIGH_CAPACITY_SD_CARD) ||
        (CardType == SDIO_HIGH_CAPACITY_MMC_CARD) )
    {
        blksize = 512;
        addr >>= 9;
    }

    SDMMC_WaitData0( );

    errorstatus = SDMMC_SetCommand( SD_CMD_WRITE_SINGLE_BLOCK, addr,
                                    SDMMC_Resp_48);
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }
    SDMMC_BlockConfig( 0, 0 );
    SDMMC_TranModeStructure.TranMode_DualDMA_TNCnt = 0x00;
    SDMMC_TranModeStructure.TranMode_DualDMA = DISABLE;
    SDMMC_TranModeStructure.TranMode_AutoGapStop = DISABLE;
    SDMMC_TranModeStructure.TranMode_Boot = DISABLE;
    SDMMC_TranModeStructure.TranMode_Direction = SDMMC_TranDir_Send;
    SDMMC_TranMode_Init( &SDMMC_TranModeStructure );
    SDMMC_SetDMAAddr1( (uint32_t)buf );
    SDMMC_BlockConfig( blksize, 1 );

    return DataTransErr( );
}

/*********************************************************************
 * @fn       SD_WriteBlock
 *
 * @brief   Write multiple block
 *
 * @param   *buf - data
 *                 addr - RCA address
 *                 blksize - block size
 *                 nblks - block number
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_WriteMultiBlocks( uint8_t *buf, long long addr,
                                uint16_t blksize, uint32_t nblks )
{
    SD_Error errorstatus;

    if( ( ( uint32_t )buf % 16 ) != 0 )
    {
        return SD_INVALID_PARAMETER;
    }

    if( (CardType == SDIO_HIGH_CAPACITY_SD_CARD) ||
        (CardType == SDIO_HIGH_CAPACITY_MMC_CARD) )
    {
        blksize = 512;
        addr >>= 9;
    }

    SDMMC_WaitData0( );

    errorstatus = SDMMC_SetCommand( SD_CMD_WRITE_MULT_BLOCK, addr,
                                    SDMMC_Resp_48 );
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }

    SDMMC_BlockConfig( 0, 0 );
    SDMMC_TranModeStructure.TranMode_DualDMA_TNCnt = 0x00;
    SDMMC_TranModeStructure.TranMode_DualDMA = DISABLE;
    SDMMC_TranModeStructure.TranMode_AutoGapStop = DISABLE;
    SDMMC_TranModeStructure.TranMode_Boot = DISABLE;
    SDMMC_TranModeStructure.TranMode_Direction = SDMMC_TranDir_Send;
    SDMMC_TranMode_Init( &SDMMC_TranModeStructure );
    SDMMC_SetDMAAddr1( (uint32_t)buf );
    SDMMC_BlockConfig( blksize, nblks );

    while( SDMMC_GetBlockNumSuccess( ) );

    while( nblks > SDMMC_GetBlockNumSuccess( ) )
    {
        errorstatus = DataTransErr( );
        if( errorstatus != SD_OK )
        {
            return errorstatus;
        }
        if( nblks <= SDMMC_GetBlockNumSuccess( ) )
        {
            SDMMC_BlockConfig( 0, 0 );
        }
        else
        {
            SDMMC_DMAContinueWrite( );
        }
    }

    return SDMMC_SetCommand( SD_CMD_STOP_TRANSMISSION, 0x00, SDMMC_Resp_48);
}

/*********************************************************************
 * @fn       SD_ReadBlock
 *
 * @brief   Read block
 *
 * @param   *buf - data
 *                 addr - RCA address
 *                 blksize - block size
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_ReadBlock( uint8_t *buf, long long addr, uint16_t blksize )
{
    SD_Error errorstatus;

    if( ( ( uint32_t )buf % 16 ) != 0 )
    {
        return SD_INVALID_PARAMETER;
    }

    if( (CardType == SDIO_HIGH_CAPACITY_SD_CARD) ||
        (CardType == SDIO_HIGH_CAPACITY_MMC_CARD) )
    {
        blksize = 512;
        addr >>= 9;
    }

    /* wait untill DATA0 pull up */
    SDMMC_WaitData0( );

    SDMMC_BlockConfig( 0, 0 );                  /* MUST Config to 0 first */
    SDMMC_TranModeStructure.TranMode_DualDMA_TNCnt = 0x00;
    SDMMC_TranModeStructure.TranMode_DualDMA = DISABLE;
    SDMMC_TranModeStructure.TranMode_AutoGapStop = DISABLE;
    SDMMC_TranModeStructure.TranMode_Boot = DISABLE;
    SDMMC_TranModeStructure.TranMode_Direction = SDMMC_TranDir_Receive;
    SDMMC_TranMode_Init( &SDMMC_TranModeStructure );
    SDMMC_BlockConfig( blksize, 1 );
    SDMMC_SetDMAAddr1( (uint32_t)buf );

    errorstatus = SDMMC_SetCommand( SD_CMD_READ_SINGLE_BLOCK, addr,
                                    SDMMC_Resp_48 );
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }

    return DataTransErr( );
}

/*********************************************************************
 * @fn       SD_ReadMultiBlocks
 *
 * @brief   Read multiple block
 *
 * @param   *buf - data
 *                 addr - RCA address
 *                 blksize - block size
 *                 nblks - block number
 *              
 * @return  SD_Error -  ERR code
 */
SD_Error SD_ReadMultiBlocks( uint8_t *buf, long long addr,
                                uint16_t blksize, uint32_t nblks )
{
    SD_Error errorstatus;

    if( ( ( uint32_t )buf % 16 ) != 0 )
    {
        return SD_INVALID_PARAMETER;
    }

    if( (CardType == SDIO_HIGH_CAPACITY_SD_CARD) ||
        (CardType == SDIO_HIGH_CAPACITY_MMC_CARD) )
    {
        blksize = 512;
        addr >>= 9;
    }

    SDMMC_WaitData0( );

    SDMMC_BlockConfig( 0, 0 );
    SDMMC_TranModeStructure.TranMode_DualDMA_TNCnt = 0x00;
    SDMMC_TranModeStructure.TranMode_DualDMA = DISABLE;
    SDMMC_TranModeStructure.TranMode_AutoGapStop = ENABLE;
    SDMMC_TranModeStructure.TranMode_Boot = DISABLE;
    SDMMC_TranModeStructure.TranMode_Direction = SDMMC_TranDir_Receive;
    SDMMC_TranMode_Init( &SDMMC_TranModeStructure );
    SDMMC_SetDMAAddr1( (uint32_t)buf );
    SDMMC_BlockConfig( blksize, nblks );

    errorstatus = SDMMC_SetCommand( SD_CMD_READ_MULT_BLOCK, addr,
                                    SDMMC_Resp_48 );
    if( errorstatus != SD_OK )
    {
        return errorstatus;
    }

    while( nblks > SDMMC_GetBlockNumSuccess( ) )
    {
        errorstatus = DataTransErr( );
        if( errorstatus != SD_OK )
        {
            SDMMC_ClearGAPStop( );
            return errorstatus;
        }

        SDMMC_ClearGAPStop( );
        SDMMC_SetGAPStop( );
    }
    SDMMC_ClearGAPStop( );
    return SDMMC_SetCommand( SD_CMD_STOP_TRANSMISSION, 0x00, SDMMC_Resp_48);
}

/*********************************************************************
 * @fn      SD_ReadDisk
 *
 * @brief   Read SD card
 *
 * @param   buf - data
 *            sector - sector addr
 *            cnt - sector number
 *
 * @return  Sta
 */
SD_Error SD_ReadDisk( uint8_t *buf, uint32_t sector, uint8_t cnt )
{
    SD_Error  sta = SD_OK;
    long long lsector = sector;
    uint8_t  n;

    lsector <<= 9;
    if( ( uint32_t )buf % SDMMC_DMADDR_ALIGN_SIZE != 0 )
    {
        for( n = 0; n < cnt; n++ )
        {
            sta = SD_ReadBlock( SDIO_Write_Buf, lsector + 512 * n, 512 );
            memcpy( buf, SDIO_Write_Buf, 512 );
            buf += 512;
        }
    }
    else
    {
        if( cnt == 1 )
        {
            sta = SD_ReadBlock( buf, lsector, 512 );
        }
        else
        {
            sta = SD_ReadMultiBlocks( buf, lsector, 512, cnt );
        }
    }
    return( sta );
}

/*********************************************************************
 * @fn      SD_WriteDisk
 *
 * @brief   Write SD card
 *
 * @param   buf - data
 *            sector - sector addr
 *            cnt - sector number
 *
 * @return  Sta
 */
SD_Error SD_WriteDisk( uint8_t *buf, uint32_t sector, uint8_t cnt )
{
    SD_Error sta = SD_OK;
    uint8_t n;
    long long lsector = sector;

    lsector <<= 9;
    if( ( uint32_t )buf % SDMMC_DMADDR_ALIGN_SIZE != 0 )
    {
        for( n = 0; n < cnt; n++ ){
            memcpy( SDIO_Write_Buf, buf, 512 );
            sta = SD_WriteBlock( SDIO_Write_Buf, lsector + 512 * n, 512 );
            buf += 512;
        }
    }
    else
    {
        if( cnt == 1 )
        {
            sta = SD_WriteBlock( buf, lsector, 512 );
        }
        else
        {
            sta = SD_WriteMultiBlocks( buf, lsector, 512, cnt );
        }
    }
    return sta;
}
