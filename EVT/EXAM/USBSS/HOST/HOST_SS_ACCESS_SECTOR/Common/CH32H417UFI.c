/* 2026.09.09
*****************************************
**   Copyright  (C)  W.ch  1999-2019   **
**   Web:        https://wch.cn         **
*****************************************
**  USB-flash File Interface for CHRV3 **
**  KEIL423, gcc 8.20                   **
*****************************************
*/
/* CHRV3 U‑disk host file system interface, support: FAT12/FAT16/FAT32 */
//#define DISK_BASE_BUF_LEN     512 /* Default disk data buffer size is 512 bytes (can be set to 2048 or even 4096 to support U disks with large sectors). Set to 0 to disable buffer definition in this file and specify the buffer address via pDISK_BASE_BUF in application before CHRV3LibInit */
/* If you want to reuse the disk data buffer to save RAM, define DISK_BASE_BUF_LEN as 0 to skip buffer definition here, then assign the shared buffer start address to variable pDISK_BASE_BUF before calling CHRV3LibInit */
//#define NO_DEFAULT_ACCESS_SECTOR      1       /* Disable default disk sector read/write routine, replace it with self‑implemented code below */
//#define NO_DEFAULT_DISK_CONNECT       1       /* Disable default disk connection check routine, replace it with self‑implemented code below */
//#define NO_DEFAULT_FILE_ENUMER        1       /* Disable default file name enumeration callback, replace it with self‑implemented code below */
//#include "CHRV3SFR.H"
#include "ch32h417_conf.h"
// #include "usb_host_config.h"
#include "CHRV3UFI.h"
#include "ch32h417_enum.h"
#include "ch32h417_udisk.h"
#if DEF_USB_PORT_FS_EN
uint8_t USBHostTransact( uint8_t endp_pid, uint8_t tog, uint32_t timeout )
{
    uint8_t  r, trans_rerty;
    uint16_t i;
    USBFSD->HOST_TX_CTRL = USBFSD->HOST_RX_CTRL = 0;
    if( tog & 0x80 )
    {
        USBFSD->HOST_RX_CTRL = 1<<2;
    }
    if( tog & 0x40 )
    {
        USBFSD->HOST_TX_CTRL = 1<<2;
    }
    trans_rerty = 0;
    do
    {
        USBFSD->HOST_EP_PID = endp_pid;         // Specify token PID and endpoint number
        USBFSD->INT_FG = USBFS_UIF_TRANSFER;    // Enable transfer
        for( i = DEF_WAIT_USB_TOUT_200US; ( i != 0 ) && ( ( USBFSD->INT_FG & USBFS_UIF_TRANSFER ) == 0 ); i-- )
        {
            Delay_Us( 1 );
        }
        USBFSD->HOST_EP_PID = 0x00;  // Stop USB transfer
        if( ( USBFSD->INT_FG & USBFS_UIF_TRANSFER ) == 0 )
        {
            return ERR_USB_UNKNOWN;
        }
        else
        {
            /* Transfer completed */
            if( USBFSD->INT_ST & USBFS_UIS_TOG_OK )
            {
                return ERR_SUCCESS;
            }
            r = USBFSD->INT_ST & USBFS_UIS_H_RES_MASK;  // USB device response status
            if( r == USB_PID_STALL )
            {
                return ( r | ERR_USB_TRANSFER );
            }
            if( r == USB_PID_NAK )
            {
                if( timeout == 0 )
                {
                    return ( r | ERR_USB_TRANSFER );
                }
                if( timeout < 0xFFFF )
                {
                    timeout--;
                }
                --trans_rerty;
            }
            else switch ( endp_pid >> 4 )
            {
                case USB_PID_SETUP:
                case USB_PID_OUT:
                    if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                case USB_PID_IN:
                    if( ( r == USB_PID_DATA0 ) || ( r == USB_PID_DATA1 ) )
                    {
                        ;
                    }
                    else if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                default:
                    return ERR_USB_UNKNOWN;
            }
        }
        Delay_Us( 15 );
        if( USBFSD->INT_FG & USBFS_UIF_DETECT )
        {
            Delay_Us( 200 );
            if( USBFSH_CheckRootHubPortEnable( ) == 0 )
            {
                return ERR_USB_DISCON;  // USB device disconnect event
            }
        }
    }while( ++trans_rerty < 10 );
    return ERR_USB_TRANSFER; // Response timeout
#elif DEF_USB_PORT_HS_EN
    return (USBHSH_Transact( endp_pid<<4 | endp_pid>>4 , tog, timeout ));
}
#endif
#if 0
uint8_t HostCtrlTransfer( uint8_t *DataBuf, uint8_t *RetLen )
{
    uint8_t  ret;
    uint16_t retlen;
    retlen = (uint16_t)(*RetLen);
    ret = USBHSH_CtrlTransfer( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, DataBuf, &retlen );
    return ret;
}
uint8_t CtrlGetDeviceDescrTB( void )
{
    uint8_t ret;
    ret = USBHSH_GetDeviceDescr( &RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, TxBuffer );
    return ret;
}
uint8_t CtrlGetConfigDescrTB( void )
{
    uint16_t len;
    uint8_t  ret;
    ret = USBHSH_GetConfigDescr( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, TxBuffer, 256, &len );
    return ret;
}
uint8_t CtrlSetUsbConfig( uint8_t cfg )
{
    uint8_t ret;
    ret = USBHSH_SetUsbConfig( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, cfg );
    return ret;
}
uint8_t CtrlSetUsbAddress( uint8_t addr )
{
    uint8_t ret;
    ret = USBHSH_SetUsbAddress( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, addr );
    return ret;
}
uint8_t CtrlClearEndpStall( uint8_t endp )
{
    uint8_t ret;
    ret = USBHSH_ClearEndpStall( RootHubDev[ DEF_USB_PORT_HS ].bEp0MaxPks, endp );
    return ret;
}
#endif
#ifndef FOR_ROOT_UDISK_ONLY
uint8_t CtrlGetHubDescr( void )
{
}
uint8_t HubGetPortStatus( uint8_t HubPortIndex )
{
}
uint8_t HubSetPortFeature( uint8_t HubPortIndex, uint8_t FeatureSelt )
{
}
uint8_t HubClearPortFeature( uint8_t HubPortIndex, uint8_t FeatureSelt )
{
}
#endif
CMD_PARAM_I mCmdParam;                       /* Command parameters */
#if     DISK_BASE_BUF_LEN > 0
//uint8_t   DISK_BASE_BUF[ DISK_BASE_BUF_LEN ] __attribute__((at(BA_RAM+SZ_RAM/2)));    /* Disk data buffer in external RAM, buffer length equals one sector size */
uint8_t DISK_BASE_BUF[ DISK_BASE_BUF_LEN ] __attribute__((aligned (4)));                /* Disk data buffer in external RAM, buffer length equals one sector size */
//uint8_t   DISK_FAT_BUF[ DISK_BASE_BUF_LEN ] __attribute__((aligned (4)));             /* Disk FAT data buffer in external RAM, buffer length equals one sector size */
#endif
/* The following functions can be modified as needed */
#ifndef NO_DEFAULT_ACCESS_SECTOR                                                        /* Define NO_DEFAULT_ACCESS_SECTOR in application to disable default disk sector read/write routine and implement your own */
//if ( use_external_interface ) {  // Replace low‑level U‑disk sector read/write routine
//    CHRV3vSectorSize=512;        // Set actual physical sector size, must be multiple of 512
//    CHRV3vSectorSizeB=9;         // Bit shift value for sector size: 9 for 512, 10 for 1024, 11 for 2048
//    CHRV3DiskStatus=DISK_MOUNTED;  // Force block device connected (waiting for file system analysis)
//}
uint8_t CHRV3ReadSector( uint8_t SectCount, uint8_t *DataBuf )  /* Read multiple sectors from disk into data buffer */
{
    uint8_t retry;
    retry = MS_ReadSector( CHRV3vLbaCurrent,(uint16_t)SectCount,DataBuf );
    return retry;
//  if ( use_external_interface ) return( extReadSector( CHRV3vLbaCurrent, SectCount, DataBuf ) );  /* External interface */
#if 0
    for( retry = 0; retry < 3; retry ++ ) {  /* Retry on error */
        pCBW -> mCBW_DataLen = (uint32_t)SectCount << CHRV3vSectorSizeB;  /* Total data transfer length */
        pCBW -> mCBW_Flag = 0x80;
        pCBW -> mCBW_LUN = CHRV3vCurrentLun;
        pCBW -> mCBW_CB_Len = 10;
        pCBW -> mCBW_CB_Buf[ 0 ] = SPC_CMD_READ10;
        pCBW -> mCBW_CB_Buf[ 1 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 2 ] = (uint8_t)( CHRV3vLbaCurrent >> 24 );
        pCBW -> mCBW_CB_Buf[ 3 ] = (uint8_t)( CHRV3vLbaCurrent >> 16 );
        pCBW -> mCBW_CB_Buf[ 4 ] = (uint8_t)( CHRV3vLbaCurrent >> 8 );
        pCBW -> mCBW_CB_Buf[ 5 ] = (uint8_t)( CHRV3vLbaCurrent );
        pCBW -> mCBW_CB_Buf[ 6 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 7 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 8 ] = SectCount;
        pCBW -> mCBW_CB_Buf[ 9 ] = 0x00;
        CHRV3BulkOnlyCmd( DataBuf );  /* Execute Bulk‑Only protocol command */
        if ( CHRV3IntStatus == ERR_SUCCESS ) {
            return( ERR_SUCCESS );
        }
        CHRV3IntStatus = CHRV3AnalyzeError( retry );
        if ( CHRV3IntStatus != ERR_SUCCESS ) {
            return( CHRV3IntStatus );
        }
    }
    return( CHRV3IntStatus = ERR_USB_DISK_ERR );  /* Disk operation failed */
    #endif
}
#ifdef  EN_DISK_WRITE
uint8_t CHRV3WriteSector( uint8_t SectCount, uint8_t *DataBuf )  /* Write multiple sectors from buffer to disk */
{
    uint8_t retry;
        retry = MS_WriteSector( CHRV3vLbaCurrent,(uint16_t)SectCount,DataBuf );
    return retry;
    #if 0
//  if ( use_external_interface ) return( extWriteSector( CHRV3vLbaCurrent, SectCount, DataBuf ) );  /* External interface */
    for( retry = 0; retry < 3; retry ++ ) {  /* Retry on error */
        pCBW -> mCBW_DataLen = (uint32_t)SectCount << CHRV3vSectorSizeB;  /* Total data transfer length */
        pCBW -> mCBW_Flag = 0x00;
        pCBW -> mCBW_LUN = CHRV3vCurrentLun;
        pCBW -> mCBW_CB_Len = 10;
        pCBW -> mCBW_CB_Buf[ 0 ] = SPC_CMD_WRITE10;
        pCBW -> mCBW_CB_Buf[ 1 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 2 ] = (uint8_t)( CHRV3vLbaCurrent >> 24 );
        pCBW -> mCBW_CB_Buf[ 3 ] = (uint8_t)( CHRV3vLbaCurrent >> 16 );
        pCBW -> mCBW_CB_Buf[ 4 ] = (uint8_t)( CHRV3vLbaCurrent >> 8 );
        pCBW -> mCBW_CB_Buf[ 5 ] = (uint8_t)( CHRV3vLbaCurrent );
        pCBW -> mCBW_CB_Buf[ 6 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 7 ] = 0x00;
        pCBW -> mCBW_CB_Buf[ 8 ] = SectCount;
        pCBW -> mCBW_CB_Buf[ 9 ] = 0x00;
        CHRV3BulkOnlyCmd( DataBuf );  /* Execute Bulk‑Only protocol command */
        if ( CHRV3IntStatus == ERR_SUCCESS ) {
            Delay_Us( 200 );  /* Short delay after write operation */
            return( ERR_SUCCESS );
        }
        CHRV3IntStatus = CHRV3AnalyzeError( retry );
        if ( CHRV3IntStatus != ERR_SUCCESS ) {
            return( CHRV3IntStatus );
        }
    }
    return( CHRV3IntStatus = ERR_USB_DISK_ERR );  /* Disk operation failed */
    #endif
}
#endif
#endif  // NO_DEFAULT_ACCESS_SECTOR
#ifndef NO_DEFAULT_DISK_CONNECT         /* Define NO_DEFAULT_DISK_CONNECT in application to disable default disk detection routine and implement your own */
/*
Agreement: USB device address allocation rules (refer to USB_DEVICE_ADDR)
Address   Device location
0x02      USB device or external HUB under built‑in Root‑HUB0
0x03      USB device or external HUB under built‑in Root‑HUB1
0x1x      USB device on port x of external HUB connected to Root‑HUB0, x=1~n
0x2x      USB device on port x of external HUB connected to Root‑HUB1, x=1~n
*/
#define     UHUB_DEV_ADDR   (USBHSH->DEV_ADDR)
#define     UHPORT_STATUS   (USBHSH->PORT_STATUS)
#define     bUMS_CONNECT    (1<<0)
#define     bUMS_SUSPEND    (1<<2)
/* Check whether the disk is connected */
uint8_t CHRV3DiskConnect( void )
{
    // uint8_t  ums, devaddr;
    uint8_t ums;
    UHUB_DEV_ADDR = UHUB_DEV_ADDR & 0x7F;
    ums = UHPORT_STATUS;
    // devaddr = UHUB_DEV_ADDR;
    //if ( devaddr == USB_DEVICE_ADDR+1 )
    {
        /* USB device under built‑in Root‑HUB */
        if ( ums & bUMS_CONNECT )
        {
            /* USB device exists under built‑in Root‑HUB */
            if ( ( ums & bUMS_SUSPEND ) == 0 )
            {
                /* USB device exists and no hot‑plug event detected */
                return( ERR_SUCCESS );  /* USB device connected without re‑plugging */
            }
            else
            {
                /* USB device was re‑attached after disconnection */
                CHRV3DiskStatus = DISK_CONNECT;  /* Previously disconnected */
                return( ERR_SUCCESS );  /* External HUB or USB device re‑connected */
            }
        }
        else
        {
            /* USB device disconnected */
            CHRV3DiskStatus = DISK_DISCONNECT;
            return( ERR_USB_DISCON );
        }
    }
}
#endif  // NO_DEFAULT_DISK_CONNECT
#ifndef NO_DEFAULT_FILE_ENUMER          /* Define NO_DEFAULT_FILE_ENUMER in application to disable default filename enumeration callback and implement your own */
void xFileNameEnumer( void )            /* File name enumeration callback function */
{
/* If you set the enumeration index CHRV3vFileSize to 0xFFFFFFFF before calling FileOpen,
FileOpen will invoke this callback each time a file entry is found.
After the callback returns, FileOpen decrements CHRV3vFileSize and continues enumeration
until no more entries are found or aborted early. Recommended usage:
Define a global counter variable before calling FileOpen. When this callback is triggered,
obtain the FAT directory entry via CHRV3vFdtOffset from pDISK_BASE_BUF,
check DIR_Attr and DIR_Name to filter target files or directories, record required info,
and increment the counter. After FileOpen returns, treat ERR_MISS_FILE or ERR_FOUND_NAME
as successful enumeration, and use the counter as the total matched file count.
Set CHRV3vFileSize = 1 inside this callback to terminate enumeration in advance.
The code below is an example implementation */
#if     0
    uint8_t         i;
    uint16_t        FileCount;
    PX_FAT_DIR_INFO pFileDir;
    uint8_t         *NameBuf;
    pFileDir = (PX_FAT_DIR_INFO)( pDISK_BASE_BUF + CHRV3vFdtOffset );  /* Start address of current FDT entry */
    FileCount = (UINT16)( 0xFFFFFFFF - CHRV3vFileSize );  /* Current file enumeration index, initial CHRV3vFileSize = 0xFFFFFFFF and decreases on each match */
    if ( FileCount < sizeof( FILE_DATA_BUF ) / 12 ) {  /* Verify buffer capacity, assume each filename occupies 12 bytes */
        NameBuf = & FILE_DATA_BUF[ FileCount * 12 ];  /* Calculate target buffer address for current filename */
        for ( i = 0; i < 11; i ++ ) NameBuf[ i ] = pFileDir -> DIR_Name[ i ];  /* Copy 11‑character short filename, trailing spaces not stripped */
//      if ( pFileDir -> DIR_Attr & ATTR_DIRECTORY ) NameBuf[ i ] = 1;  /* Mark entry as directory */
        NameBuf[ i ] = 0;  /* String terminator */
    }
#endif
}
#endif  // NO_DEFAULT_FILE_ENUMER
uint8_t CHRV3LibInit( void )                        /* Initialize CHRV3 library, return 0 on success */
{
    uint8_t s;
    s = CHRV3GetVer( );
    if( s < CHRV3_LIB_VER )
    {
        return( 0xFF );                             /* Query library version, return error if version is too old */
    }
    printf( "lib vision:%02x\r\n",s );
#if     DISK_BASE_BUF_LEN > 0
    pDISK_BASE_BUF = & DISK_BASE_BUF[0];            /* Assign disk data buffer in external RAM */
    pDISK_FAT_BUF = & DISK_BASE_BUF[0];             /* Assign FAT buffer, can share the same memory with pDISK_BASE_BUF to save RAM */
//  pDISK_FAT_BUF = & DISK_FAT_BUF[0];              /* Independent FAT buffer separated from pDISK_BASE_BUF for better performance */
                                                    /* For better file access performance, re‑assign pDISK_FAT_BUF to another independent buffer
                                                       with the same size as pDISK_BASE_BUF after calling CHRV3LibInit in main program */
#endif
    CHRV3DiskStatus = DISK_UNKNOWN;                 /* Unknown initial state */
    CHRV3vSectorSizeB = 9;                          /* Default physical disk sector: 512 Bytes */
    CHRV3vSectorSize = 512;                         /* Default physical disk sector: 512 Bytes */
    CHRV3vStartLba = 0;                             /* Auto‑detect FDD and HDD partitions by default */
    CHRV3vPacketSize = 512;                         /* Max packet length for USB mass‑storage device: 64 for FS, 512 for HS/SS.
                                                       Updated after U‑disk enumeration for high‑speed or super‑speed devices */
    pTX_DMA_A_REG = (uint32_t *)&(USBHSH->TX_DMA);  /* Point to transmit DMA address register, initialized by application */
    pRX_DMA_A_REG = (uint32_t *)&(USBHSH->RX_DMA);  /* Point to receive DMA address register, initialized by application */
    pTX_LEN_REG = (uint16_t *)&(USBHSH->TX_LEN);    /* Point to transmit length register, initialized by application */
    pRX_LEN_REG = (uint16_t *)&(USBHSH->RX_LEN);    /* Point to receive length register, initialized by application */
    return( ERR_SUCCESS );
}
void mDelaymS( uint16_t n )
{
    Delay_Ms(n);
}
void mDelayuS( uint16_t n )
{
    Delay_Us(n);
}
