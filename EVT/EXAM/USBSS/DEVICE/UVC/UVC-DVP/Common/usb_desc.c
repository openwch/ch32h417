/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/05/26
 * Description        : usb device descriptor,configuration descriptor,
 *                      string descriptors and other descriptors.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "ch32h417.h"
#include "usb_desc.h"
#include "ch32h417_usbss_device.h"

/* Device Descriptor */
const uint8_t  MyDevDescr[ ] =
{
    0x12,                                                   // bLength
    0x01,                                                   // bDescriptorType
    0x00, 0x02,                                             // bcdUSB
    0xEF,                                                   // bDeviceClass
    0x02,                                                   // bDeviceSubClass
    0x01,                                                   // bDeviceProtocol
    DEF_USBD_UEP0_SIZE,                                     // bMaxPacketSize0
    (uint8_t)DEF_USB_VID, (uint8_t)( DEF_USB_VID >> 8 ),    // idVendor
    (uint8_t)DEF_USB_PID, (uint8_t)( DEF_USB_PID >> 8 ),    // idProduct
    DEF_IC_PRG_VER, 0x00,                                   // bcdDevice
    0x01,                                                   // iManufacturer
    0x02,                                                   // iProduct
    0x03,                                                   // iSerialNumber
    0x01,                                                   // bNumConfigurations
};

/* Configuration Descriptor(high speed)  */
const uint8_t  MyCfgDescr_HS[ ] =
{
    0x09,                                                   // bLength
    0x02,                                                   // bDescriptorType (Configuration)
    0xEC, 0x01,                                             // wTotalLength 
    0x02,                                                   // bNumInterfaces 2
    0x01,                                                   // bConfigurationValue
    0x00,                                                   // iConfiguration (String Index)
    0xA0,                                                   // bmAttributes
    0x4B,                                                   // bMaxPower 100mA

    /* Interface Association Descriptor */
    0x08,                                                   // bLength
    0x0b,                                                   // bDescriptorType - INTERFACE ASSOCIATION Descriptor
    0x00,                                                   // bFirstInterface
    0x02,                                                   // bInterfaceCount
    0x0e,                                                   // bFunctionClass - CC_VIDEO
    0x03,                                                   // bFunctionSubClass - SC_VIDEO_INTERFACE_COLLECTION
    0x00,                                                   // bFunctionProtocol - PC_PROTOCOL_UNDEFINED
    0x08,                                                   // iFunction

    /* Standard VC Interface Descriptor */
    0x09,                                                   // bLength
    0x04,                                                   // bDescriptorType - INTERFACE
    0x00,                                                   // bInterfaceNumber
    0x00,                                                   // bAlternateSetting            
    0x00,                                                   // bNumEndpoints - 0 endpoint (interrupt endpoint)
    0x0e,                                                   // bFunctionClass - CC_VIDEO
    0x01,                                                   // bFunctionSubClass - SC_VIDEOCONTROL
    0x00,                                                   // bFunctionProtocol
    0x08,                                                   // iFunction

    /* Dvp_Class-specific VC Interface Descriptor */
    0x0d,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x01,                                                   // bDescriptorSubType - VC_HEADER
    0x00, 0x01,                                             // bcdUVC 
    0x4d, 0x00,                                             // wTotalLength
    0x00, 0x0e, 0x27, 0x07,                                 // dwClockFrequency
    0x01,                                                   // bInCollection
    0x01,                                                   // baInterfaceNr(1)
    
    /* Input Terminal Descriptor (Camera) */
    0x12,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x02,                                                   // bDescriptorSubType - VC_INPUT_TERMINAL
    0x01,                                                   // bTerminalID - 01
    0x01, 0x02,                                             // wTerminalType - ITT_CAMERA
    0x00,                                                   // bAssocTerminal
    0x00,                                                   // iTerminal
    0x00, 0x00,                                             // wObjectiveFocalLengthMin
    0x00, 0x00,                                             // wObjectiveFocalLengthMax 
    0x00, 0x00,                                             // wOcularFocalLength
    0x03,                                                   // bControlSize
    0x00, 0x00, 0x00,                                       // bmControls - No controls are supported.

    /* Processing Unit Descriptor */
    0x0b,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x05,                                                   // bDescriptorSubType - VC_PROCESSING_UNIT
    0x03,                                                   // bUnitID - 05
    0x01,                                                   // bSourceID - 01
    0x00, 0x00,                                             // wMaxMultiplier
    0x02,                                                   // bControlSize
    0x00, 0x00,                                             // bmControls
    0x00,

    /* Output Terminal Descriptor */
    0x09,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x03,                                                   // bDescriptorSubType - VC_OUTPUT_TERMINAL
    0x02,                                                   // bTerminalID
    0x01, 0x01,                                             // wTerminalType - TT_STREAMING
    0x00,                                                   // bAssocTerminal
    0x04,                                                   // bSourceID
    0x00,                                                   // iTerminal

    /* Extension Unit Descriptor */
    0x1a,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x06,                                                   // bDescriptorSubType - VC_EXTENSION_UNIT
    0x04,                                                   // bUnitID
    0x5b, 0x12, 0x6d, 0xc6,                                 // guidExtensionCode
    0x80, 0x04, 0x44, 0x08,
    0x8c, 0x26, 0xb8, 0x36,
    0x3a, 0x84, 0xde, 0x63,
    0x03,                                                   // bNumControls
    0x01,                                                   // bNrInPins
    0x03,                                                   // baSourceID(1)
    0x01,                                                   // bNumControls
    0x07,                                                   // bNrInPins
    0x00,                                                   // baSourceID(2)

    /* Standard VS Interface Descriptor */
    0x09,                                                   // bLength
    0x04,                                                   // bDescriptorType - INTERFACE
    0x01,                                                   // bInterfaceNumber
    0x00,                                                   // bAlternateSetting            
    0x00,                                                   // bNumEndpoints - 0 endpoint (interrupt endpoint)
    0x0e,                                                   // bFunctionClass - CC_VIDEO
    0x02,                                                   // bFunctionSubClass - SC_VIDEOCONTROL
    0x00,                                                   // bFunctionProtocol
    0x00,                                                   // iFunction
    /* VS Interface Input Header Descriptor */
    0x0e,                                                   // bLength
    0x24,                                                   // bDescriptorType - INTERFACE
    0x01,                                                   // bDescriptorSubtype - VS_INPUT_HEADER
    0x01,                                                   // bNumFormats
    0x6c, 0x01,                                             // wTotalLength
    0x81,                                                   // bEndpointAddress - endpoint 1
    0x00,                                                   // bmInfo
    0x02,                                                   // bTerminalLink
    0x00,                                                   // bStillCaptureMethod
    0x00,                                                   // bTriggerSupport
    0x00,                                                   // bTriggerUsage
    0x01,                                                   // bControlSize
    0x00,                                                   // bmaControls

    /* Dvp_Class-specific VS Format Descriptor--- VS_FORMAT_MJPEG */
    0x0b,                                                   // bLength
    0x24,                                                   // bDescriptorType£ºCS_INTERFACE
    0x06,                                                   // bDescriptorSubtype£º VS_FORMAT_MJPEG
    0x01,                                                   // bFormatIndex£ºFirst (and only) format descriptor
    0x05,                                                   // bNumFrameDescriptors£º12 frame descriptor for this format follows
    0x01,                                                   // bmFlags£ºUses fixed size samples
    0x01,                                                   // bDefaultFrameIndex£ºDefault frame index is 1.
    0x00,                                                   // bAspectRatioX 
    0x00,                                                   // bAspectRatioY 
    0x00,                                                   // bmInterlaceFlags 
    0x00,                                                   // bCopyProtect 

    /* 800*600@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type
    0x07,                                                   // bDescriptorSubtype: VS_FRAME_MJPEG 
    0x01,                                                   // bFrameIndex: First (and only) frame descriptor 
    0x00,                                                   // bmCapabilities
    0x20, 0x03,                                             // Width of frame£º800 
    0x58, 0x02,                                             // Height of frame£º 600 
    0x00, 0xA0, 0xBB, 0x0D,                                 // Min bit rate in bits/s 
    0x00, 0xA0, 0xBB, 0x0D,                                 // Max bit rate in bits/s 800*600*2*30fps * 8
    0x00, 0xA6, 0x0E, 0x00,                                 // Maximum video or still frame size, in bytes£º 800*600*2 
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval 
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps 

    /* 352*288@15fps */ 
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                                                 
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type
    0x07,                                                   // bDescriptorSubtype: VS_FRAME_MJPEG
    0x02,                                                   // bFrameIndex: First (and only) frame descriptor 
    0x00,                                                   // bmCapabilities
    0x60, 0x01,                                             // Width of 352 
    0x20, 0x01,                                             // Height of frame£º 288 
    0x00, 0x80, 0xE6, 0x02,                                 // Min bit rate in bits/s 
    0x00, 0x80, 0xE6, 0x02,                                 // Max bit rate in bits/s 
    0x00, 0x18, 0x03, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval 
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps                                         

    /* 640*480@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m             
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type
    0x07,                                                   // bDescriptorSubtype: VS_FRAME_MJPEG
    0x03,                                                   // bFrameIndex: First (and only) frame descriptor 
    0x00,                                                   // bmCapabilities
    0x80, 0x02,                                             // Width of 640 
    0xe0, 0x01,                                             // Height of frame£º 480 
    0x00, 0x00, 0xca, 0x08,                                 // Min bit rate in bits/s 
    0x00, 0x00, 0xca, 0x08,                                 // Max bit rate in bits/s 
    0x00, 0x60, 0x09, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval 
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps  

    /* 1024*768@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m    
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type
    0x07,                                                   // bDescriptorSubtype: VS_FRAME_MJPEG
    0x04,                                                   // bFrameIndex: First (and only) frame descriptor 
    0x00,                                                   // bmCapabilities
    0x00, 0x04,                                             // Width of 1024 
    0x00, 0x03,                                             // Height of frame£º 768 
    0x00, 0x00, 0x80, 0x16,                                 // Min bit rate in bits/s 
    0x00, 0x00, 0x80, 0x16,                                 // Max bit rate in bits/s 
    0x00, 0x00, 0x18, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval             
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps          

    /* 1600*1200@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m        
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type    
    0x07,                                                   // bDescriptorSubtype: VS_FRAME_MJPEG        
    0x05,                                                   // bFrameIndex: First (and only) frame descriptor                 
    0x00,                                                   // bmCapabilities            
    0x40, 0x06,                                             // Width of 1600                         
    0xb0, 0x04,                                             // Height of frame£º 1200             
    0x00, 0x80, 0xee, 0x36,                                 // Min bit rate in bits/s 
    0x00, 0x80, 0xee, 0x36,                                 // Max bit rate in bits/s 
    0x00, 0x98, 0x3a, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval                         
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps                  


    /* Color Matching Descriptor */
    0x06,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x0d,                                                   // bDescriptorSubtype - VS_COLORFORMAT
    0x01,                                                   // bColorPrimaries
    0x01,                                                   // bTransferCharacteristics 
    0x04,                                                   // bMatrixCoefficients

    /* Dvp_Class-specific VS Format Descriptor----VS_FORMAT_UNCOMPRESSED */
    0x1b,
    0x24,
    0x04,
    0x02,
    0x05,
    0x59,
    0x55,
    0x59,
    0x32,
    0x00,
    0x00,
    0x10,
    0x00,
    0x80,
    0x00,
    0x00,
    0xaa,
    0x00,
    0x38,
    0x9b,
    0x71,
    0x10,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,

    /* Dvp_Class-specific VS Frame Descriptor*/
    /* 640*480@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type                    
    0x05,                                                   // bDescriptorSubtype: VS_FRAME_UNCOMPRESSED                              
    0x01,                                                   // bFrameIndex: First (and only) frame descriptor                
    0x00,                                                   // bmCapabilities            
    0x80, 0x02,                                             // Width of 640                             
    0xe0, 0x01,                                             // Height of frame£º 480                 
    0x00, 0x00, 0xca, 0x08,                                 // Min bit rate in bits/s 
    0x00, 0x00, 0xca, 0x08,                                 // Max bit rate in bits/s 
    0x00, 0x60, 0x09, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval             
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps                      

    /* 800*600@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                 
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type     
    0x05,                                                   // bDescriptorSubtype: VS_FRAME_UNCOMPRESSED                        
    0x02,                                                   // bFrameIndex: First (and only) frame descriptor        
    0x00,                                                   // bmCapabilities            
    0x20, 0x03,                                             // Width of 800                              
    0x58, 0x02,                                             // Height of frame£º 600                  
    0x00, 0xA0, 0xBB, 0x0D,                                 // Min bit rate in bits/s        
    0x00, 0xA0, 0xBB, 0x0D,                                 // Max bit rate in bits/s        
    0x00, 0xA6, 0x0E, 0x00,                                 // Maximum video or still frame size, in bytes        
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps           
    0x01,                                                   // bFrameIntervalType: Continuous frame interval         
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps                              

    /* 176*144@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                    
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type                     
    0x05,                                                   // bDescriptorSubtype: VS_FRAME_UNCOMPRESSED                                                   
    0x03,                                                   // bFrameIndex: First (and only) frame descriptor           
    0x00,                                                   // bmCapabilities                 
    0xb0, 0x00,                                             // Width of 176                                   
    0x90, 0x00,                                             // Height of frame£º 144                        
    0x00, 0xa0, 0xb9, 0x00,                                 // Min bit rate in bits/s        
    0x00, 0xa0, 0xb9, 0x00,                                 // Max bit rate in bits/s        
    0x00, 0xc6, 0x00, 0x00,                                 // Maximum video or still frame size, in bytes        
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps           
    0x01,                                                   // bFrameIntervalType: Continuous frame interval                 
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps                      

    /* 320*240@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                     
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type                             
    0x05,                                                   // bDescriptorSubtype: VS_FRAME_UNCOMPRESSED                            
    0x04,                                                   // bFrameIndex: First (and only) frame descriptor                        
    0x00,                                                   // bmCapabilities                                  
    0x40, 0x01,                                             // Width of 320                                                            
    0xf0, 0x00,                                             // Height of frame£º 240                                                 
    0x00, 0x80, 0x32, 0x02,                                 // Min bit rate in bits/s        
    0x00, 0x80, 0x32, 0x02,                                 // Max bit rate in bits/s        
    0x00, 0x58, 0x02, 0x00,                                 // Maximum video or still frame size, in bytes        
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps           
    0x01,                                                   // bFrameIntervalType: Continuous frame interval                 
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps                      

    /* 352*288@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                       
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type               
    0x05,                                                   // bDescriptorSubtype: VS_FRAME_UNCOMPRESSED                   
    0x05,                                                   // bFrameIndex: First (and only) frame descriptor              
    0x00,                                                   // bmCapabilities                                       
    0x60, 0x01,                                             // Width of 352                                                
    0x20, 0x01,                                             // Height of frame£º 288                                       
    0x00, 0x80, 0xE6, 0x02,                                 // Min bit rate in bits/s            
    0x00, 0x80, 0xE6, 0x02,                                 // Max bit rate in bits/s            
    0x00, 0x18, 0x03, 0x00,                                 // Maximum video or still frame size, in bytes            
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps               
    0x01,                                                   // bFrameIntervalType: Continuous frame interval               
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps                                  

    /* Color Matching Descriptor */
    0x06,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x0d,                                                   // bDescriptorSubtype - VS_COLORFORMAT
    0x01,                                                   // bColorPrimaries
    0x01,                                                   // bTransferCharacteristics 
    0x04,                                                   // bMatrixCoefficients

    0x09,                                                   // bLength
    0x04,                                                   // bDescriptorType (Interface)
    0x01,                                                   // bInterfaceNumber 0
    0x01,                                                   // bAlternateSetting
    0x01,                                                   // bNumEndpoints 6
    0x0e,                                                   // bInterfaceClass
    0x02,                                                   // bInterfaceSubClass
    0x00,                                                   // bInterfaceProtocol
    0x00,                                                   // iInterface (String Index)

    0x07,                                                   // bLength
    0x05,                                                   // bDescriptorType (Endpoint)
    0x81,                                                   // bEndpointAddress (IN/H1D)
    0x05,                                                   // bmAttributes 
    0x00, 0x14,                                             // wMaxPacketSize
    0x01,                                                   // bInterval 
};

/* Configuration Descriptor(full speed) */
const uint8_t  MyCfgDescr_FS[ ] =
{
    0x09,                                                   // bLength
    0x02,                                                   // bDescriptorType (Configuration)
    0x3C, 0x00,                                             // wTotalLength 60
    0x01,                                                   // bNumInterfaces 1
    0x01,                                                   // bConfigurationValue
    0x00,                                                   // iConfiguration (String Index)
    0x80,                                                   // bmAttributes
    0x32,                                                   // bMaxPower 100mA

    0x09,                                                   // bLength
    0x04,                                                   // bDescriptorType (Interface)
    0x00,                                                   // bInterfaceNumber 0
    0x00,                                                   // bAlternateSetting
    0x06,                                                   // bNumEndpoints 6
    0xFF,                                                   // bInterfaceClass
    0xFF,                                                   // bInterfaceSubClass
    0xFF,                                                   // bInterfaceProtocol
    0x00,                                                   // iInterface (String Index)

    0x07,                                                   // bLength
    0x05,                                                   // bDescriptorType (Endpoint)
    0x01,                                                   // bEndpointAddress (OUT/H2D)
    0x02,                                                   // bmAttributes (Bulk)
    (uint8_t)DEF_USB_EP1_FS_SIZE, (uint8_t)( DEF_USB_EP1_FS_SIZE >> 8 ), // wMaxPacketSize 64
    0x00,                                                   // bInterval 0 (unit depends on device speed)

    0x07,                                                   // bLength
    0x05,                                                   // bDescriptorType (Endpoint)
    0x81,                                                   // bEndpointAddress (IN/D2H)
    0x02,                                                   // bmAttributes (Bulk)
    (uint8_t)DEF_USB_EP2_FS_SIZE, (uint8_t)( DEF_USB_EP2_FS_SIZE >> 8 ),  // wMaxPacketSize 64
    0x00,                                                   // bInterval 0 (unit depends on device speed)

    0x07,                                                   // bLength
    0x05,                                                   // bDescriptorType (Endpoint)
    0x03,                                                   // bEndpointAddress (OUT/H2D)
    0x02,                                                   // bmAttributes (Bulk)
    (uint8_t)DEF_USB_EP3_FS_SIZE, (uint8_t)( DEF_USB_EP3_FS_SIZE >> 8 ),  // wMaxPacketSize 64
    0x00,                                                   // bInterval 0 (unit depends on device speed)

    0x07,                                                   // bLength
    0x05,                                                   // bDescriptorType (Endpoint)
    0x84,                                                   // bEndpointAddress (IN/D2H)
    0x02,                                                   // bmAttributes (Bulk)
    (uint8_t)DEF_USB_EP4_FS_SIZE, (uint8_t)( DEF_USB_EP4_FS_SIZE >> 8 ),  // wMaxPacketSize 64
    0x00,                                                   // bInterval 0 (unit depends on device speed)

    0x07,                                                   // bLength
    0x05,                                                   // bDescriptorType (Endpoint)
    0x05,                                                   // bEndpointAddress (OUT/H2D)
    0x02,                                                   // bmAttributes (Bulk)
    (uint8_t)DEF_USB_EP5_FS_SIZE, (uint8_t)( DEF_USB_EP5_FS_SIZE >> 8 ),  // wMaxPacketSize 64
    0x00,                                                   // bInterval 0 (unit depends on device speed)

    0x07,                                                   // bLength
    0x05,                                                   // bDescriptorType (Endpoint)
    0x86,                                                   // bEndpointAddress (IN/D2H)
    0x02,                                                   // bmAttributes (Bulk)
    (uint8_t)DEF_USB_EP6_FS_SIZE, (uint8_t)( DEF_USB_EP6_FS_SIZE >> 8 ), // wMaxPacketSize 64
    0x00,                                                   // bInterval 0 (unit depends on device speed)
};

/* Device Qualified Descriptor */
const uint8_t MyQuaDesc[ ] =
{
    0x0A, 0x06, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0x40, 0x01, 0x00,
};

/* Device BOS Descriptor */
const uint8_t MyBOSDesc[ ] =
{
    /* Binary Object Store (BOS) Descriptor */
    0x05, 0x0F, 0x2A, 0x00, 0x01,
    /* USB 2.0 Extension Descriptor */
    0x07, 0x10, 0x02, 0x02, 0x00, 0x00, 0x00,
    /* SuperSpeed USB Device Capability Descriptor */
    0x0A, 0x10, 0x03, 0x00, 0x0E, 0x00, 0x10, 0x0A, 0x20, 0x00,
    /* SuperSpeedPlus USB Device Capability */
    0x14, 0x10, 0x0A, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x11,
    0x00, 0x00, 0x30, 0x40, 0x0A, 0x00, 0xB0, 0x40, 0x0A, 0x00,
    
};

/* USB Full-Speed Mode, Other speed configuration Descriptor */
uint8_t TAB_USB_FS_OSC_DESC[ sizeof(MyCfgDescr_HS) ] =
{
    /* Other parts are copied through the program */
    0x09, 0x07,
};

/* USB High-Speed Mode, Other speed configuration Descriptor */
uint8_t TAB_USB_HS_OSC_DESC[ sizeof(MyCfgDescr_FS) ] =
{
    /* Other parts are copied through the program */
    0x09, 0x07,
};

/*Superspeed device descriptor*/
const uint8_t SS_DeviceDescriptor[ ] =
{
    0x12,                                                   // bLength
    0x01,                                                   // bDescriptorType
    0x00, 0x03,                                             // bcdUSB
    0xEF,                                                   // bDeviceClass
    0x02,                                                   // bDeviceSubClass
    0x01,                                                   // bDeviceProtocol
    0x09,                                                   // bMaxPacketSize0
    (uint8_t)DEF_USB_VID, (uint8_t)(DEF_USB_VID >> 8),      // idVendor
    (uint8_t)DEF_USB30_PID, (uint8_t)(DEF_USB30_PID >> 8),  // idProduct
    DEF_IC_PRG_VER, 0x00,                                   // bcdDevice
    0x01,                                                   // iManufacturer
    0x02,                                                   // iProduct
    0x03,                                                   // iSerialNumber
    0x01,                                                   // bNumConfigurations
};

const uint8_t SS_ConfigDescriptor[ ] =
{
    /* Configuration Descriptor */
    0x09,                                                   // bLength
    0x02,                                                   // bDescriptorType
    0xF2, 0x01,                                             // wTotalLength
    0x02,                                                   // bNumInterfaces
    0x01,                                                   // bConfigurationValue
    0x00,                                                   // iConfiguration
    0xA0,                                                   // bmAttributes: Bus Powered; Remote Wakeup
    0x4B,                                                   // MaxPower: 100mA

    /* Interface Association Descriptor */
    0x08,                                                   // bLength
    0x0b,                                                   // bDescriptorType - INTERFACE ASSOCIATION Descriptor
    0x00,                                                   // bFirstInterface
    0x02,                                                   // bInterfaceCount
    0x0e,                                                   // bFunctionClass - CC_VIDEO
    0x03,                                                   // bFunctionSubClass - SC_VIDEO_INTERFACE_COLLECTION
    0x00,                                                   // bFunctionProtocol - PC_PROTOCOL_UNDEFINED
    0x08,                                                   // iFunction

    /* Standard VC Interface Descriptor */
    0x09,                                                   // bLength
    0x04,                                                   // bDescriptorType - INTERFACE
    0x00,                                                   // bInterfaceNumber
    0x00,                                                   // bAlternateSetting            
    0x00,                                                   // bNumEndpoints - 0 endpoint (interrupt endpoint)
    0x0e,                                                   // bFunctionClass - CC_VIDEO
    0x01,                                                   // bFunctionSubClass - SC_VIDEOCONTROL
    0x00,                                                   // bFunctionProtocol
    0x08,                                                   // iFunction

    /* Dvp_Class-specific VC Interface Descriptor */
    0x0d,                                                   // bLength                
    0x24,                                                   // bDescriptorType - CS_INTERFACE                
    0x01,                                                   // bDescriptorSubType - VC_HEADER                    
    0x00, 0x01,                                             // bcdUVC             
    0x4d, 0x00,                                             // wTotalLength                
    0x00, 0x0e, 0x27, 0x07,                                 // dwClockFrequency
    0x01,                                                   // bInCollection    
    0x01,                                                   // baInterfaceNr(1)     

    /* Input Terminal Descriptor (Camera) */
    0x12,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x02,                                                   // bDescriptorSubType - VC_INPUT_TERMINAL
    0x01,                                                   // bTerminalID - 01
    0x01, 0x02,                                             // wTerminalType - ITT_CAMERA
    0x00,                                                   // bAssocTerminal
    0x00,                                                   // iTerminal
    0x00, 0x00,                                             // wObjectiveFocalLengthMin
    0x00, 0x00,                                             // wObjectiveFocalLengthMax 
    0x00, 0x00,                                             // wOcularFocalLength
    0x03,                                                   // bControlSize
    0x00, 0x00, 0x00,                                       // bmControls - No controls are supported.

    /* Processing Unit Descriptor */
    0x0b,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x05,                                                   // bDescriptorSubType - VC_PROCESSING_UNIT
    0x03,                                                   // bUnitID - 05
    0x01,                                                   // bSourceID - 01
    0x00, 0x00,                                             // wMaxMultiplier
    0x02,                                                   // bControlSize
    0x00, 0x00,                                             // bmControls
    0x00,

    /* Output Terminal Descriptor */
    0x09,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x03,                                                   // bDescriptorSubType - VC_OUTPUT_TERMINAL
    0x02,                                                   // bTerminalID
    0x01, 0x01,                                             // wTerminalType - TT_STREAMING
    0x00,                                                   // bAssocTerminal
    0x04,                                                   // bSourceID
    0x00,                                                   // iTerminal
    /* Extension Unit Descriptor */
    0x1a,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x06,                                                   // bDescriptorSubType - VC_EXTENSION_UNIT
    0x04,                                                   // bUnitID
    0x5b, 0x12, 0x6d, 0xc6,                                 // guidExtensionCode
    0x80, 0x04, 0x44, 0x08,
    0x8c, 0x26, 0xb8, 0x36,
    0x3a, 0x84, 0xde, 0x63,
    0x03,                                                   // bNumControls
    0x01,                                                   // bNrInPins
    0x03,                                                   // baSourceID(1)
    0x01,                                                   // bNumControls
    0x07,                                                   // bNrInPins
    0x00,                                                   // baSourceID(2)

    /* Standard VS Interface Descriptor */
    0x09,                                                   // bLength
    0x04,                                                   // bDescriptorType - INTERFACE
    0x01,                                                   // bInterfaceNumber
    0x00,                                                   // bAlternateSetting            
    0x00,                                                   // bNumEndpoints - 0 endpoint (interrupt endpoint)
    0x0e,                                                   // bFunctionClass - CC_VIDEO
    0x02,                                                   // bFunctionSubClass - SC_VIDEOCONTROL
    0x00,                                                   // bFunctionProtocol
    0x00,                                                   // iFunction
    
    /* VS Interface Input Header Descriptor */
    0x0e,                                                   // bLength
    0x24,                                                   // bDescriptorType - INTERFACE
    0x01,                                                   // bDescriptorSubtype - VS_INPUT_HEADER
    0x01,                                                   // bNumFormats
    0x6c, 0x01,                                             // wTotalLength
    0x81,                                                   // bEndpointAddress - endpoint 1
    0x00,                                                   // bmInfo
    0x02,                                                   // bTerminalLink
    0x00,                                                   // bStillCaptureMethod
    0x00,                                                   // bTriggerSupport
    0x00,                                                   // bTriggerUsage
    0x01,                                                   // bControlSize
    0x00,                                                   // bmaControls

    /* Dvp_Class-specific VS Format Descriptor--- VS_FORMAT_MJPEG */
    0x0b,                                                   // bLength
    0x24,                                                   // bDescriptorType£ºCS_INTERFACE
    0x06,                                                   // bDescriptorSubtype£º VS_FORMAT_MJPEG
    0x01,                                                   // bFormatIndex£ºFirst (and only) format descriptor
    0x05,                                                   // bNumFrameDescriptors£º12 frame descriptor for this format follows
    0x01,                                                   // bmFlags£ºUses fixed size samples
    0x01,                                                   // bDefaultFrameIndex£ºDefault frame index is 1.
    0x00,                                                   // bAspectRatioX 
    0x00,                                                   // bAspectRatioY 
    0x00,                                                   // bmInterlaceFlags 
    0x00,                                                   // bCopyProtect 

    /* 800*600@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type
    0x07,                                                   // bDescriptorSubtype: VS_FRAME_MJPEG 
    0x01,                                                   // bFrameIndex: First (and only) frame descriptor 
    0x00,                                                   // bmCapabilities
    0x20, 0x03,                                             // Width of frame£º800 
    0x58, 0x02,                                             // Height of frame£º 600 
    0x00, 0xA0, 0xBB, 0x0D,                                 // Min bit rate in bits/s 
    0x00, 0xA0, 0xBB, 0x0D,                                 // Max bit rate in bits/s 800*600*2*30fps * 8
    0x00, 0xA6, 0x0E, 0x00,                                 // Maximum video or still frame size, in bytes£º 800*600*2 
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval 
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps 


    /* 352*288@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                                                 
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type
    0x07,                                                   // bDescriptorSubtype: VS_FRAME_MJPEG
    0x02,                                                   // bFrameIndex: First (and only) frame descriptor 
    0x00,                                                   // bmCapabilities
    0x60, 0x01,                                             // Width of 352 
    0x20, 0x01,                                             // Height of frame£º 288 
    0x00, 0x80, 0xE6, 0x02,                                 // Min bit rate in bits/s 
    0x00, 0x80, 0xE6, 0x02,                                 // Max bit rate in bits/s 
    0x00, 0x18, 0x03, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval 
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps  

    /* 640*480@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m             
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type
    0x07,                                                   // bDescriptorSubtype: VS_FRAME_MJPEG
    0x03,                                                   // bFrameIndex: First (and only) frame descriptor 
    0x00,                                                   // bmCapabilities
    0x80, 0x02,                                             // Width of 640 
    0xe0, 0x01,                                             // Height of frame£º 480 
    0x00, 0x00, 0xca, 0x08,                                 // Min bit rate in bits/s 
    0x00, 0x00, 0xca, 0x08,                                 // Max bit rate in bits/s 
    0x00, 0x60, 0x09, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval 
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps  

    /* 1024*768@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m    
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type
    0x07,                                                   // bDescriptorSubtype: VS_FRAME_MJPEG
    0x04,                                                   // bFrameIndex: First (and only) frame descriptor 
    0x00,                                                   // bmCapabilities
    0x00, 0x04,                                             // Width of 1024 
    0x00, 0x03,                                             // Height of frame£º 768 
    0x00, 0x00, 0x80, 0x16,                                 // Min bit rate in bits/s 
    0x00, 0x00, 0x80, 0x16,                                 // Max bit rate in bits/s 
    0x00, 0x00, 0x18, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval             
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps       

    /* 1600*1200@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m        
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type    
    0x07,                                                   // bDescriptorSubtype: VS_FRAME_MJPEG        
    0x05,                                                   // bFrameIndex: First (and only) frame descriptor                 
    0x00,                                                   // bmCapabilities            
    0x40, 0x06,                                             // Width of 1600                         
    0xb0, 0x04,                                             // Height of frame£º 1200             
    0x00, 0x80, 0xee, 0x36,                                 // Min bit rate in bits/s 
    0x00, 0x80, 0xee, 0x36,                                 // Max bit rate in bits/s 
    0x00, 0x98, 0x3a, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval                         
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps                  


    /* Color Matching Descriptor */
    0x06,
    0x24,
    0x0d,
    0x01,
    0x01,
    0x04,

    /* Dvp_Class-specific VS Format Descriptor----VS_FORMAT_UNCOMPRESSED */
    0x1b,
    0x24,
    0x04,
    0x02,
    0x05,
    0x59,
    0x55,
    0x59,
    0x32,
    0x00,
    0x00,
    0x10,
    0x00,
    0x80,
    0x00,
    0x00,
    0xaa,
    0x00,
    0x38,
    0x9b,
    0x71,
    0x10,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,

    /* Dvp_Class-specific VS Frame Descriptor*/
    /* 640*480@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type                    
    0x05,                                                   // bDescriptorSubtype: VS_FRAME_UNCOMPRESSED                              
    0x01,                                                   // bFrameIndex: First (and only) frame descriptor                
    0x00,                                                   // bmCapabilities            
    0x80, 0x02,                                             // Width of 640                             
    0xe0, 0x01,                                             // Height of frame£º 480                 
    0x00, 0x00, 0xca, 0x08,                                 // Min bit rate in bits/s 
    0x00, 0x00, 0xca, 0x08,                                 // Max bit rate in bits/s 
    0x00, 0x60, 0x09, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval             
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps          

    /* 1600*1200@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                        
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type                  
    0x05,                                                   // bDescriptorSubtype: VS_FRAME_UNCOMPRESSED                                  
    0x02,                                                   // bFrameIndex: First (and only) frame descriptor                     
    0x00,                                                   // bmCapabilities                            
    0x40, 0x06,                                             // Width of 1600                                         
    0xb0, 0x04,                                             // Height of frame£º 1200                     
    0x00, 0x80, 0xee, 0x36,                                 // Min bit rate in bits/s 
    0x00, 0x80, 0xee, 0x36,                                 // Max bit rate in bits/s 
    0x00, 0x98, 0x3a, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval              
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps              

    /* 1280*960@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                                 
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type                     
    0x05,                                                   // bDescriptorSubtype: VS_FRAME_UNCOMPRESSED                             
    0x03,                                                   // bFrameIndex: First (and only) frame descriptor                        
    0x00,                                                   // bmCapabilities                                                
    0x00, 0x05,                                             // Width of 1280                                                             
    0xC0, 0x03,                                             // Height of frame£º 960                                 
    0x00, 0xa0, 0x28, 0x23,                                 // Min bit rate in bits/s 
    0x00, 0xa0, 0x28, 0x23,                                 // Max bit rate in bits/s 
    0x00, 0x80, 0x25, 0x00,                                 // Maximum video or still frame size, in bytes
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval             
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps              

    /* 1024*768@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                                  
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type                     
    0x05,                                                   // bDescriptorSubtype: VS_FRAME_UNCOMPRESSED                         
    0x04,                                                   // bFrameIndex: First (and only) frame descriptor                            
    0x00,                                                   // bmCapabilities                                                            
    0x00,0x04,                                              // Width of 1024                                         
    0x00,0x03,                                              // Height of frame£º 768                                 
    0x00,0x00,0x80,0x16,                                    // Min bit rate in bits/s 
    0x00,0x00,0x80,0x16,                                    // Max bit rate in bits/s 
    0x00,0x00,0x18,0x00,                                    // Maximum video or still frame size, in bytes
    0x2a,0x2c,0x0a,0x00,                                    // Default frame interval is 15fps
    0x01,                                                   // bFrameIntervalType: Continuous frame interval                         
    0x2a,0x2c,0x0a,0x00,                                    // Default frame interval is 15fps              

    /* 352*288@15fps */
    0x1e,                                                   // bLength: 10+(4*n)-4+m                                       
    0x24,                                                   // bDescriptorType: CS_INTERFACE descriptor type               
    0x05,                                                   // bDescriptorSubtype: VS_FRAME_UNCOMPRESSED                   
    0x05,                                                   // bFrameIndex: First (and only) frame descriptor              
    0x00,                                                   // bmCapabilities                                       
    0x60, 0x01,                                             // Width of 352                                                
    0x20, 0x01,                                             // Height of frame£º 288                                       
    0x00, 0x80, 0xE6, 0x02,                                 // Min bit rate in bits/s            
    0x00, 0x80, 0xE6, 0x02,                                 // Max bit rate in bits/s            
    0x00, 0x18, 0x03, 0x00,                                 // Maximum video or still frame size, in bytes            
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps               
    0x01,                                                   // bFrameIntervalType: Continuous frame interval               
    0x2a, 0x2c, 0x0a, 0x00,                                 // Default frame interval is 15fps      

    /* Color Matching Descriptor */
    0x06,                                                   // bLength
    0x24,                                                   // bDescriptorType - CS_INTERFACE
    0x0d,                                                   // bDescriptorSubtype - VS_COLORFORMAT
    0x01,                                                   // bColorPrimaries
    0x01,                                                   // bTransferCharacteristics 
    0x04,                                                   // bMatrixCoefficients

    0x09,                                                   // bLength
    0x04,                                                   // bDescriptorType (Interface)
    0x01,                                                   // bInterfaceNumber 0
    0x01,                                                   // bAlternateSetting
    0x01,                                                   // bNumEndpoints 6
    0x0e,                                                   // bInterfaceClass
    0x02,                                                   // bInterfaceSubClass
    0x00,                                                   // bInterfaceProtocol
    0x00,                                                   // iInterface (String Index)

    0x07,                                                   // bLength
    0x05,                                                   // bDescriptorType (Endpoint)
    0x81,                                                   // bEndpointAddress (IN/H1D)
    0x05,                                                   // bmAttributes 
    0x00, 0x04,                                             // wMaxPacketSize
    0x01,                                                   // bInterval 

    /* Endpoint Compansion Descriptor */
    0x06,                                                   // bLength
    0x30,                                                   // bDescriptorType
    0x0b,                                                   // bMaxBurst
    0x00,                                                   // bmAttributes
    0x00, 0x80,                                             // wBytesPerInterval 
};

/*String Descriptor Lang ID*/
const uint8_t MyLangDescr[ ] =
{
    0x04,
    0x03,
    0x09,
    0x04
};

/*String Descriptor Vendor*/
const uint8_t MyManuInfo[ ] =
{
    38,
    0x03,
	'W', 0, 'C', 0, 'H', 0, 'I', 0, 'C', 0, 'r', 0, 'o', 0, 'e', 0,
	'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
	'c', 0, 's', 0
};

/*String Descriptor Product*/
const uint8_t MyProdInfo[ ] =
{
    42,         
    0x03,      
	'C', 0, 'H', 0, '3', 0, '2', 0, 'H', 0, '4', 0, '1', 0, '7', 0, '_', 0,'C', 0, 'a', 0,
	'm', 0, 'e', 0, 'r', 0, 'a', 0
};

/*String Descriptor Serial*/
const uint8_t MySerNumInfo[ ] =
{
    0x16,
    0x03,
    '0',
    0,
    '1',
    0,
    '2',
    0,
    '3',
    0,
    '4',
    0,
    '5',
    0,
    '6',
    0,
    '7',
    0,
    '8',
    0,
    '9',
    0
};

const uint8_t OSStringDescriptor[ ] =
{
    0x12, 
    0x03,
    'M',
    0x00,
    'S',
    0x00,
    'F',
    0x00,
    'T',
    0x00,
    '1',
    0x00,
    '0',
    0x00,
    '0',
    0x00,
    0x01,
    0x00
};

const uint8_t MyBOSDesc_SS[ ] =
{
    0x05,
    0x0f, 
    0x16,
    0x00,
    0x02,
   
    0x07,
    0x10,
    0x02, 
    0x06,
    0x00,
    0x00,
    0x00,
   
    0x0a,
    0x10,
    0x03,
    0x00,
    0x0e, 
    0x00,
    0x01, 
    0x0a, 
    0xff, 
    0x07
};

const uint8_t MSOS20DescriptorSet[ ] =
{
    // Microsoft OS 2.0 Descriptor Set Header
    0x0A, 0x00,            
    0x00, 0x00,          
    0x00, 0x00, 0x03, 0x06, 
    0x48, 0x00,           
                             
    0x3E, 0x00,          
    0x04, 0x00,          
    0x04, 0x00,          
    0x30, 0x00,            
    0x53, 0x00, 0x65, 0x00, 
    0x6C, 0x00, 0x65, 0x00,
    0x63, 0x00, 0x74, 0x00,
    0x69, 0x00, 0x76, 0x00,
    0x65, 0x00, 0x53, 0x00,
    0x75, 0x00, 0x73, 0x00,
    0x70, 0x00, 0x65, 0x00,
    0x6E, 0x00, 0x64, 0x00,
    0x45, 0x00, 0x6E, 0x00,
    0x61, 0x00, 0x62, 0x00,
    0x6C, 0x00, 0x65, 0x00,
    0x64, 0x00, 0x00, 0x00,
    0x04, 0x00,            
    0x01, 0x00, 0x00, 0x00
};

const uint8_t PropertyHeader[ ] =
{
    0x8e, 0x00, 0x00, 0x00, 0x00, 01, 05, 00, 01, 00,
    0x84, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,
    0x28, 0x00,
    0x44, 0x00, 0x65, 0x00, 0x76, 0x00, 0x69, 0x00, 0x63, 0x00, 0x65, 0x00, 0x49, 0x00, 0x6e,
    0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00, 0x66, 0x00, 0x61, 0x00, 0x63, 0x00, 0x65, 0x00, 0x47, 0x00, 0x55, 0x00, 0x49, 0x00, 0x44, 0x00, 0x00, 0x00,

    0x4e, 0x00, 0x00, 0x00,
    0x7b, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35, 0x00, 0x36, 0x00, 0x37, 0x00, 0x38, 0x00,
    0x2d, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00,
    0x2d, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00,
    0x2d, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x2d, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00,
    0x34, 0x00, 0x35, 0x00, 0x36, 0x00, 0x37, 0x00, 0x38, 0x00, 0x39, 0x00, 0x41, 0x00, 0x42, 0x00, 0x43, 0x00,
    0x7d, 0x00, 0x00, 0x00
};

const uint8_t CompactId[ ] =
{
    0x28, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x57, 0x49, 0x4e, 0x55, 0x53, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t GetStatus[ ] =
{
    0x01, 0x00
};


