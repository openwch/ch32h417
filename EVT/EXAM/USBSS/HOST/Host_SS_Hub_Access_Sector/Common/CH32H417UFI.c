/* 2014.09.09
*****************************************
**   Copyright  (C)  W.ch  1999-2019   **
**   Web:      http://wch.cn           **
*****************************************
**  USB-flash File Interface for CHRV3 **
**  KEIL423, gcc 8.20          **
*****************************************
*/
/* CHRV3 U‑Disk Host File System Interface, Support: FAT12/FAT16/FAT32 */
//#define DISK_BASE_BUF_LEN     512 /* Default disk data buffer size is 512 bytes(can be set to 2048 or even 4096 to support U‑disks with large sectors). Set to 0 to disable buffer definition in this file and assign buffer address via pDISK_BASE_BUF before CHRV3LibInit */
/* To save RAM by reusing the disk data buffer, define DISK_BASE_BUF_LEN as 0 to skip local buffer allocation. Assign the shared buffer address to variable pDISK_BASE_BUF in application before calling CHRV3LibInit */
//#define NO_DEFAULT_ACCESS_SECTOR      1       /* Disable default disk sector read/write functions, replace with custom implementation below */
//#define NO_DEFAULT_DISK_CONNECT       1       /* Disable default disk connection check function, replace with custom implementation below */
//#define NO_DEFAULT_FILE_ENUMER        1       /* Disable default filename enumeration callback function, replace with custom implementation below */
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
        USBFSD->HOST_EP_PID = endp_pid;       // Specify token PID and endpoint number
        USBFSD->INT_FG = USBFS_UIF_TRANSFER;  // Allow transmission
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
            /* Complete transfer */
            if( USBFSD->INT_ST & USBFS_UIS_TOG_OK )
            {
                return ERR_SUCCESS;
            }
            r = USBFSD->INT_ST & USBFS_UIS_H_RES_MASK;  // USB device answer status
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

    return ERR_USB_TRANSFER; // Reply timeout
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

CMD_PARAM_I mCmdParam;                    /* Command parameters */
#if     DISK_BASE_BUF_LEN > 0
//uint8_t   DISK_BASE_BUF[ DISK_BASE_BUF_LEN ] __attribute__((at(BA_RAM+SZ_RAM/2)));    /* Disk data buffer in external RAM, buffer size equals one sector */
uint8_t DISK_BASE_BUF[ DISK_BASE_BUF_LEN ] __attribute__((aligned (4)));                /* Disk data buffer in external RAM, buffer size equals one sector */
//uint8_t   DISK_FAT_BUF[ DISK_BASE_BUF_LEN ] __attribute__((aligned (4)));             /* FAT table buffer in external RAM, buffer size equals one sector */
#endif
/* The following functions can be modified as required */
#ifndef NO_DEFAULT_ACCESS_SECTOR        /* Define NO_DEFAULT_ACCESS_SECTOR in application to disable default sector read/write functions for custom implementation */
//if ( use_external_interface ) {  // Replace low‑level U‑disk sector read/write handler
//    CHRV3vSectorSize=512;  // Set actual physical sector size, must be a multiple of 512
//    CHRV3vSectorSizeB=9;   // Bit shift for sector size: 9 for 512B, 10 for 1024B, 11 for 2048B
//    CHRV3DiskStatus=DISK_MOUNTED;  // Force block device ready state, waiting for file system parsing
//}
uint8_t CHRV3ReadSector( uint8_t SectCount, uint8_t *DataBuf )                          /* Read multiple consecutive sectors from disk into buffer */
{
    uint8_t retry;
    retry = MS_ReadSector( CHRV3vLbaCurrent,(uint16_t)SectCount,DataBuf );
    return retry;
//  if ( use_external_interface ) return( extReadSector( CHRV3vLbaCurrent, SectCount, DataBuf ) );  /* External driver interface */
#if 0
    for( retry = 0; retry < 3; retry ++ ) {  /* Retry operation on failure */
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
    return( CHRV3IntStatus = ERR_USB_DISK_ERR );  /* Disk access error */
    #endif
}
#ifdef  EN_DISK_WRITE
uint8_t CHRV3WriteSector( uint8_t SectCount, uint8_t *DataBuf )                         /* Write multiple consecutive sectors from buffer to disk */
{
    uint8_t retry;
        retry = MS_WriteSector( CHRV3vLbaCurrent,(uint16_t)SectCount,DataBuf );
    return retry;
    #if 0
//  if ( use_external_interface ) return( extWriteSector( CHRV3vLbaCurrent, SectCount, DataBuf ) );  /* External driver interface */
    for( retry = 0; retry < 3; retry ++ ) {  /* Retry operation on failure */
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
    return( CHRV3IntStatus = ERR_USB_DISK_ERR );  /* Disk access error */
    #endif
}
#endif
#endif  // NO_DEFAULT_ACCESS_SECTOR
#ifndef NO_DEFAULT_DISK_CONNECT         /* Define NO_DEFAULT_DISK_CONNECT in application to disable default detection for custom connection check */
/*
Rule for USB device address assignment (refer to USB_DEVICE_ADDR)
Address   Device location
0x02      USB device or external HUB under on‑chip Root‑HUB0
0x03      USB device or external HUB under on‑chip Root‑HUB1
0x1x      USB device on port x of external HUB behind Root‑HUB0, x=1~n
0x2x      USB device on port x of external HUB behind Root‑HUB1, x=1~n
*/
#define     UHUB_DEV_ADDR   (USBHSH->DEV_ADDR)
#define     UHPORT_STATUS   (USBHSH->PORT_STATUS)
#define     bUMS_CONNECT    (1<<0)
#define     bUMS_SUSPEND    (1<<2)
/* Check whether the storage disk is connected */
uint8_t CHRV3DiskConnect( void )
{
    // uint8_t  ums, devaddr;
    uint8_t ums;
    UHUB_DEV_ADDR = UHUB_DEV_ADDR & 0x7F;
    ums = UHPORT_STATUS;
    // devaddr = UHUB_DEV_ADDR;
    //if ( devaddr == USB_DEVICE_ADDR+1 )
    {
        /* USB device attached to on‑chip Root‑HUB */
        if ( ums & bUMS_CONNECT )
        {
            /* Valid device detected under Root‑HUB */
            if ( ( ums & bUMS_SUSPEND ) == 0 )
            {
                /* Device stays connected without hot‑plug event */
                return( ERR_SUCCESS );                              /* USB device is stable and connected */
            }
            else
            {
                /* Device re‑attached after disconnection */
                CHRV3DiskStatus = DISK_CONNECT;                     /* Mark re‑connection state */
                return( ERR_SUCCESS );                              /* External HUB or mass‑storage device re‑connected */
            }
        }
        else
        {
            /* USB device has been disconnected */
// mDiskDisconn:
            CHRV3DiskStatus = DISK_DISCONNECT;
            return( ERR_USB_DISCON );
        }
    }
    // else
    // {
    //  goto mDiskDisconn;
    // }
}
#endif  // NO_DEFAULT_DISK_CONNECT
#ifndef NO_DEFAULT_FILE_ENUMER          /* Define NO_DEFAULT_FILE_ENUMER in application to disable default enumeration callback for custom logic */
void xFileNameEnumer( void )            /* Filename enumeration callback function */
{
/* When FileOpen is called with CHRV3vFileSize = 0xFFFFFFFF, this callback will be triggered for each matched file entry.
After callback returns, FileOpen decrements CHRV3vFileSize and continues scanning until no more entries or termination.
Recommended workflow:
Declare a global counter variable before calling FileOpen. Inside this callback, retrieve FAT_DIR_INFO via CHRV3vFdtOffset,
parse DIR_Attr and DIR_Name to filter target files or folders, store required information and increment the counter.
After FileOpen returns, treat ERR_MISS_FILE or ERR_FOUND_NAME as normal completion, the counter holds the total matched files.
Set CHRV3vFileSize = 1 inside this callback to terminate enumeration early. Below is a template example */
#if     0
    uint8_t         i;
    uint16_t        FileCount;
    PX_FAT_DIR_INFO pFileDir;
    uint8_t         *NameBuf;
    pFileDir = (PX_FAT_DIR_INFO)( pDISK_BASE_BUF + CHRV3vFdtOffset );  /* Start address of current FDT entry */
    FileCount = (UINT16)( 0xFFFFFFFF - CHRV3vFileSize );  /* Index of current entry. CHRV3vFileSize starts at 0xFFFFFFFF and decreases on each hit */
    if ( FileCount < sizeof( FILE_DATA_BUF ) / 12 ) {  /* Validate buffer capacity, reserve 12 bytes for each 8.3 filename */
        NameBuf = & FILE_DATA_BUF[ FileCount * 12 ];  /* Calculate target buffer address for current filename */
        for ( i = 0; i < 11; i ++ ) NameBuf[ i ] = pFileDir -> DIR_Name[ i ];  /* Copy raw 11‑character filename, spaces reserved */
//      if ( pFileDir -> DIR_Attr & ATTR_DIRECTORY ) NameBuf[ i ] = 1;  /* Mark entry as a directory */
        NameBuf[ i ] = 0;  /* Append string terminator */
    }
#endif
}
#endif  // NO_DEFAULT_FILE_ENUMER
uint8_t CHRV3LibInit( void )  /* Initialize CHRV3 library, return ERR_SUCCESS on success */
{
    uint8_t s;
    s = CHRV3GetVer( );
    if( s < CHRV3_LIB_VER )
    {
        return( 0xFF );  /* Fetch library version, return error if runtime version is too old */
    }
    printf( "lib vision:%02x\r\n",s );
#if     DISK_BASE_BUF_LEN > 0
    pDISK_BASE_BUF = & DISK_BASE_BUF[0];            /* Assign disk sector buffer pointer */
    pDISK_FAT_BUF = & DISK_BASE_BUF[0];             /* Assign FAT table buffer pointer, share RAM with sector buffer for memory saving */
//  pDISK_FAT_BUF = & DISK_FAT_BUF[0];              /* Use independent FAT buffer to improve file access performance */
/* To speed up file operations, re‑assign pDISK_FAT_BUF to a separate aligned buffer of identical size after CHRV3LibInit */
#endif
    CHRV3DiskStatus = DISK_UNKNOWN;                 /* Reset disk status to unknown */
    CHRV3vSectorSizeB = 9;                          /* Default physical sector shift value for 512‑byte sectors */
    CHRV3vSectorSize = 512;                         /* Default physical sector size: 512 Bytes */
    CHRV3vStartLba = 0;                             /* Auto‑detect FDD / HDD partition offset by default */
    CHRV3vPacketSize = 512;                         /* Max bulk packet size for USB mass‑storage:64 for FS,512 for HS/SS. Update after device enumeration for high‑speed devices */
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
