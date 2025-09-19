/********************************** (C) COPYRIGHT *******************************
* File Name          : uvcclassdes.h
* Author             : WCH
* Version            : V1.2
* Date               : 2025/07/28
* Description        :
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "uvclib.h"
#include "ch32h417_usb.h"
#include "ch32h417_usbss_device.h"

#define NoError         0x00
#define NotReady        0x01
#define WrongState      0x02
#define Power           0x03
#define OutOfRange      0x04
#define InvalidUnit     0x05
#define InvalidControl  0x06
#define InvalidRequest  0x07
#define InvalidValueWithinRange 0x08
#define Unknown         0xFF

typedef struct
{
    UINT8 info[1];
    UINT8 len[2];   //Used by the expansion unit
    UINT8 min[10];
    UINT8 max[10];
    UINT8 res[10];
    UINT8 def[10];
    UINT8 cur[10];
    UINT8 set[10];
}Unit_t;

Unit_t VAICSID[]={//ID=0,send VideoControl Interface Control Selectors
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//VC _CONTROL_ UNDEFINED
        {
                0,{0x01,0x00},
                {0},{0},{0},{0},{0x10},{0}
        },//VC_VIDEO_POWER_MODE_CONTROL
        {
                0,{0x01,0x00},
                {0},{0},{0},{0},{NoError},{0}
        },//VC_REQUEST_ERROR_CODE_CONTROL
        {}//Reserved


};
Unit_t CameraID[]={//ID 01
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//CT_CONTROL_UNDEFINED 0
        {
                0x03,{0x01,0x00},
                {0},{0},{0},{0},{0},{0}

        },//CT_SCANNING_MODE_CONTROL 1
        {
                0x03,{0x01,0x00},
                {0},{0},{0},{0},{0x06},{0}
        },//CT_AE_MODE_CONTROL 2
        {
                0x3,{0x01,0x00},
                {0x00},{0x01},{0x01},{0x00},{0},{0}
        },//CT_AE_PRIORITY_CONTROL 3
        {
                0x0f,{0x04,0x00},
                {0x32,0x00,0x00,0x00},{0x10,0x27,0x00,0x00},{0x01,0x00,0x00,0x00},{0xa6,0x00,0x00,0x00},{0},{0}
        },//CT_EXPOSURE_TIME_ABSOLUTE_CONTROL 4
        {
                0x00,{0x00,0x00},
                {0},{0},{0},{0},{0},{0}
        },//CT_EXPOSURE_TIME_RELATIVE_CONTROL 5
        {
                0x0f,{0x02,0x00},
                {0x00,0x00},{0xff,0x03},{0x01,0x00},{0x44,0x00},{0x44,0x00},{0}
        },//CT_FOCUS_ABSOLUTE_CONTROL 6
        {
                0x00,{0x00,0x00},
                {0},{0},{0},{0},{0},{0}
        },//CT_FOCUS_RELATIVE_CONTROL 7
        {
                0,{0x01},
                {0},{0},{0},{0},{0x01},{0}
        },//CT_FOCUS_AUTO_CONTROL 8
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//CT_IRIS_ABSOLUTE_CONTROL 9
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//CT_IRIS_RELATIVE_CONTROL a
        {
                0x03,{0x02,0x00},
                {0x00,0x00},{0x03,0x00},{0x01,0x00},{0x00,0x00},{0},{0}
        },//CT_ZOOM_ABSOLUTE_CONTROL b
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//CT_ZOOM_RELATIVE_CONTROL c
        {
                0x03,{0x08,0x00},
                {0x00, 0x1f, 0xff, 0xff,  0x40, 0x57, 0xff, 0xff },
                {0x00, 0xe1, 0x00, 0x00,  0xc0, 0xa8, 0x00, 0x00},
                {0x10, 0x0e, 0x00, 0x00,  0x10, 0x0e, 0x00, 0x00},{0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00},{0},{0}
        },//CT_PANTILT_ABSOLUTE_CONTROL d
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//CT_PANTILT_RELATIVE_CONTROL e
        {
                0x03,{0x02,0x00},
                {0},{0x03,0x00},{0x01,0x00},{0},{0},{0}
        },//CT_ROLL_ABSOLUTE_CONTROL f
        {
                0x03,{0x02,0x00},
                {0x00, 0x00},{0x03, 0x00},{0x01, 0x00},{0x00, 0x00},{0},{0}
        },//CT_ROLL_RELATIVE_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//CT_PRIVACY_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//CT_FOCUS_SIMPLE_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//CT_WINDOW_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        }//CT_REGION_OF_INTEREST_CONTROL
};

Unit_t ProcessID[]={//ID 02
        {
                0,{0x00,0x00},
                {0},{0},{0},{0},{0},{0}
        },//PU_CONTROL_UNDEFINED 0
        {
                0x03,{0x02,0x00},
                {0x00, 0x00},{0x02, 0x00},{0x01, 0x00},{0x00, 0x00},{0},{0}
        },//PU_BACKLIGHT_COMPENSATION_CONTROL 1
        {
                0x03,{0x02,0x00},
                {0xc0, 0xff},{0x40, 0x00 },{0x01, 0x00},{0x00, 0x00},{0},{0}
        },//PU_BRIGHTNESS_CONTROL 2
        {
                0x03,{0x02,0x00},
                {0x00, 0x00},{0x64, 0x00},{0x01, 0x00 },{0x32, 0x00},{0},{0}
        },//PU_CONTRAST_CONTROL 3
        {
                0,{0x00,0x00},
                {0},{0},{0},{0},{0},{0}
        },//PU_GAIN_CONTROL 4
        {
                0x03,{0x01,0x00},
                {0x00},{0x02},{0x01},{0x01},{0x01},{0}
        },//PU_POWER_LINE_FREQUENCY_CONTROL 5
        {
                0x03,{0x02,0x00},
                {0x4c, 0xff},{ 0xb4, 0x00},{0x01, 0x00 },{0x00, 0x00},{0},{0}
        },//PU_HUE_CONTROL 6
        {
                0x03,{0x02,0x00},
                {0x00, 0x00},{0x64, 0x00},{0x01, 0x00},{0x40, 0x00},{0},{0}
        },//PU_SATURATION_CONTROL 7
        {
                0x03,{0x02,0x00},
                {0x00, 0x00},{0x64, 0x00},{0x01, 0x00},{0x32, 0x00},{0},{0}
        },//PU_SHARPNESS_CONTROL 8
        {
                0x03,{0},
                {0x64, 0x00},{0xf4, 0x01},{0x01, 0x00},{0x2c, 0x01},{0},{0}
        },//PU_GAMMA_CONTROL 9
        {
                0x0f,{0x02,0x00},
               {0xf0, 0x0a},{0x64, 0x19},{0x0a, 0x00},{0xf8, 0x11},{0},{0}
        },//PU_WHITE_BALANCE_TEMPERATURE_CONTROL a
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//PU_WHITE_BALANCE_COMPONENT_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//PU_DIGITAL_MULTIPLIER_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//PU_HUE_AUTO_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//PU_ANALOG_VIDEO_STANDARD_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        },//PU_ANALOG_LOCK_STATUS_CONTROL
        {
                0,{0},
                {0},{0},{0},{0},{0},{0}
        }//PU_CONTRAST_AUTO_CONTROL

};
Unit_t ExtensionID1[]={//ID 04
        {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
        {
                0x03,{0x08,0x00},
                {0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00},
                {0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff },
                {0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00},{0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00},{0},{0}
        },//a
        {
                0x03,{0x08,0x00},
                {0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00},
                {0xff, 0xff, 0xff, 0xff,  0xff, 0xff, 0xff, 0xff },
                {0x01, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00},{0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00},{0},{0}
        }//b
};
Unit_t ExtensionID2[]={//ID 06
        {0},
        {
                0x03,{0x02,0x00},
                {0x00, 0x00},{0xff, 0xff},{0x00, 0x00},{0x01, 0x00},{0},{0}
        },
        {
                0x03,{0x02,0x00},
                {0xfa, 0xff},{0x06, 0x00},{0x40, 0x00},{0x00, 0x00},{0},{0}
        },
        {
                0x01,{0x02,0x00},
                {0x00, 0x00},{0x00, 0x00},{0x00, 0x00},{0x00, 0x00},{0},{0}
        },
        {
                0x03,{0x01,0x00},
                {0x00, 0x00},{0x00, 0x00},{0x00, 0x00},{0x00, 0x00},{0},{0}
        },
        {
                0x03,{0x01,0x00},
               {0x00, 0x00},{0x00, 0x00},{0x00, 0x00},{0x00, 0x00},{0},{0}
        },
        {
                0x03,{0x01,0x00},
                {0x00, 0x00},{0x00, 0x00},{0x00, 0x00},{0x00, 0x00},{0},{0}
        },{0},{0},{0},{0},{0},
        {0},{0},{0},{0},{0},{0},{0},
        {
                0x03,{0x01,0x00},
               {0x00, 0x00},{0x01, 0x00},{0x01, 0x00},{0x00, 0x00},{0},{0}
        },
        {
                0x03,{0x0a,0x00},
                {0},{0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x09, 0x00 },{0},{0},{0},{0}
        },
        {
                0x01,{0x04,0x00},
                {0},{0},{0},{0},{0},{0}
        },
        {
                0x03,{0x01,0x00},
                {0x00, 0x00},{0x01, 0x00},{0x01, 0x00},{0x00, 0x00},{0},{0}
        },
        {
                0x03,{0x01,0x00},
               {0x01, 0x00},{0x03, 0x00},{0x02, 0x00},{0x01, 0x00},{0},{0}
        },
        {
                0x03,{0x01,0x00},
                {0x01, 0x00},{0x00, 0x00},{0x01, 0x00},{0x01, 0x00},{0},{0}
        }
};
/*******************************************************************************
 * @fn      UVC_NonStandardReq
 *
 * @brief   Nonstandard request processing function
 *
 * @return  Length
 */
UINT16 USBHS_UVC_NonStandardReq(const uint8_t **pDescr)
{
    if(USBHS_SetupReqType == 0xA1)
    {
        switch((uint8_t)( USBHS_SetupReqIndex >> 8 ))
        {
            case 00:
                switch((uint8_t)USBHS_SetupReqIndex )
                {
                    case 00://Only send request to the VideoControl interface (interface ID 00)
                        switch(USBHS_SetupReqCode)
                        {
                            case 0x81://GET CUR
                                *pDescr = (PUINT8)&VAICSID[(uint8_t)( USBHS_SetupReqValue >> 8 )].cur;
                                break;
                        }
                        break;
                    case 01://
                        switch(USBHS_SetupReqCode)
                        {
                            case 0x81://GET CUR
                            case 0x82:
                            case 0x83:
                            case 0x84:
                            case 0x85:
                            case 0x86:
                            case 0x87:
                                *pDescr = (PUINT8)&Get_Curr;
                                break;
                        }
                        break;
                }
                break;
            case 01:
                switch(USBHS_SetupReqCode)
                {
                    //LEN->INFO->MIN->MAX->RES->DEF
                    case 0x81://GET CUR
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBHS_SetupReqValue >> 8 )].cur;
                        break;
                    case 0x82://GET MIN
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBHS_SetupReqValue >> 8 )].min;
                        break;
                    case 0x83://GET MAX
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBHS_SetupReqValue >> 8 )].max;
                        break;
                    case 0x84://GET RES
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBHS_SetupReqValue >> 8 )].res;
                        break;
                    case 0x85://GET LEN
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBHS_SetupReqValue >> 8 )].len;
                        break;
                    case 0x86:// GET INFO
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBHS_SetupReqValue >> 8 )].info;
                        break;
                    case 0x87://GET DEF
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBHS_SetupReqValue >> 8 )].def;
                        break;
                }
                break;
            case 02:
                switch(USBHS_SetupReqCode)
                {
                    //LEN->INFO->MIN->MAX->RES->DEF
                    case 0x81://GET CUR
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBHS_SetupReqValue >> 8 )].cur;
                        break;
                    case 0x82://GET MIN
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBHS_SetupReqValue >> 8 )].min;
                        break;
                    case 0x83://GET MAX
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBHS_SetupReqValue >> 8 )].max;
                        break;
                    case 0x84://GET RES
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBHS_SetupReqValue >> 8 )].res;
                        break;
                    case 0x85://GET LEN
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBHS_SetupReqValue >> 8 )].len;
                        break;
                    case 0x86:// GET INFO
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBHS_SetupReqValue >> 8 )].info;
                        break;
                    case 0x87://GET DEF
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBHS_SetupReqValue >> 8 )].def;
                        break;
                }
                break;
            case 04:
                switch(USBHS_SetupReqCode){//LEN->INFO->MIN->MAX->RES->DEF
                    case 0x81://GET CUR
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBHS_SetupReqValue >> 8 )].cur;
                        break;
                    case 0x82://GET MIN
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBHS_SetupReqValue >> 8 )].min;
                        break;
                    case 0x83://GET MAX
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBHS_SetupReqValue >> 8 )].max;
                        break;
                    case 0x84://GET RES
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBHS_SetupReqValue >> 8 )].res;
                        break;
                    case 0x85://GET LEN
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBHS_SetupReqValue >> 8 )].len;
                        break;
                    case 0x86:// GET INFO
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBHS_SetupReqValue >> 8 )].info;
                        break;
                    case 0x87://GET DEF
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBHS_SetupReqValue >> 8 )].def;
                        break;
                }
                break;
            case 06:
                switch(USBHS_SetupReqCode)
                {
                    //LEN->INFO->MIN->MAX->RES->DEF
                    case 0x81://GET CUR
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBHS_SetupReqValue >> 8 )].cur;
                        break;
                    case 0x82://GET MIN
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBHS_SetupReqValue >> 8 )].min;
                        break;
                    case 0x83://GET MAX
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBHS_SetupReqValue >> 8 )].max;
                        break;
                    case 0x84://GET RES
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBHS_SetupReqValue >> 8 )].res;
                        break;
                    case 0x85://GET LEN
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBHS_SetupReqValue >> 8 )].len;
                        break;
                    case 0x86:// GET INFO
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBHS_SetupReqValue >> 8 )].info;
                        break;
                    case 0x87://GET DEF
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBHS_SetupReqValue >> 8 )].def;
                        break;
                }
                break;
            
            default:
                return 0xFF;
                break;
        }

        if((uint8_t)( USBHS_SetupReqIndex >> 8 ) ==0x01 &&(((uint8_t)( USBHS_SetupReqValue >> 8 )==0x14) ||((uint8_t)( USBHS_SetupReqValue >> 8 )==0x09)))
        {
                VAICSID[2].cur[0]=(UINT8)InvalidControl;
                return 0xFF;
        }
    }
    return 0;
}

/*******************************************************************************
 * @fn      UVC_NonStandardReq
 *
 * @brief   Nonstandard request processing function
 *
 * @return  Length
 */
uint16_t USBSS_UVC_NonStandardReq(const uint8_t **pDescr)
{
    if( ( USBSS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_INTERF )
    {
        switch((uint8_t)( USBSS_SetupReqIndex >> 8 ))
        {
            case UVC_UNIT_NONE:
                switch((uint8_t)USBSS_SetupReqIndex )
                {
                    case UVC_CONTROL_INTERFACE_ID:
                        //Only send request to the VideoControl interface (interface ID 00)
                        switch(USBSS_SetupReqCode)
                        {
                            case UVC_GET_CUR:
                                *pDescr = (PUINT8)&VAICSID[(uint8_t)( USBSS_SetupReqValue >> 8 )].cur;
                                break;
                        }
                        break;
                    case UVC_STERAMING_INTERFACE:
                        switch(USBSS_SetupReqCode)
                        {
                            case UVC_GET_CUR:
                            case UVC_GET_MIN:
                            case UVC_GET_MAX:
                            case UVC_GET_RES:
                            case UVC_GET_LEN:
                            case UVC_GET_INFO:
                            case UVC_GET_DEF:
                                *pDescr = (PUINT8)&Get_Curr;
                                break;
                        }
                        break;
                }
                break;
            case UVC_UNIT_ID01:
                switch(USBSS_SetupReqCode)
                {
                    case UVC_GET_CUR:
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBSS_SetupReqValue >> 8 )].cur;
                        break;
                    case UVC_GET_MIN:
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBSS_SetupReqValue >> 8 )].min;
                        break;
                    case UVC_GET_MAX:
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBSS_SetupReqValue >> 8 )].max;
                        break;
                    case UVC_GET_RES:
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBSS_SetupReqValue >> 8 )].res;
                        break;
                    case UVC_GET_LEN:
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBSS_SetupReqValue >> 8 )].len;
                        break;
                    case UVC_GET_INFO:
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBSS_SetupReqValue >> 8 )].info;
                        break;
                    case UVC_GET_DEF:
                        *pDescr = (PUINT8)&CameraID[(uint8_t)( USBSS_SetupReqValue >> 8 )].def;
                        break;
                }
                break;
            case UVC_UNIT_ID02:
                switch(USBSS_SetupReqCode)
                {
                    case UVC_GET_CUR:
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBSS_SetupReqValue >> 8 )].cur;
                        break;
                    case UVC_GET_MIN:
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBSS_SetupReqValue >> 8 )].min;
                        break;
                    case UVC_GET_MAX:
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBSS_SetupReqValue >> 8 )].max;
                        break;
                    case UVC_GET_RES:
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBSS_SetupReqValue >> 8 )].res;
                        break;
                    case UVC_GET_LEN:
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBSS_SetupReqValue >> 8 )].len;
                        break;
                    case UVC_GET_INFO:
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBSS_SetupReqValue >> 8 )].info;
                        break;
                    case UVC_GET_DEF:
                        *pDescr = (PUINT8)&ProcessID[(uint8_t)( USBSS_SetupReqValue >> 8 )].def;
                        break;
                }
                break;
            case UVC_UNIT_ID04:
                switch(USBSS_SetupReqCode)
                {
                    case UVC_GET_CUR:
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBSS_SetupReqValue >> 8 )].cur;
                        break;
                    case UVC_GET_MIN:
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBSS_SetupReqValue >> 8 )].min;
                        break;
                    case UVC_GET_MAX:
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBSS_SetupReqValue >> 8 )].max;
                        break;
                    case UVC_GET_RES:
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBSS_SetupReqValue >> 8 )].res;
                        break;
                    case UVC_GET_LEN:
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBSS_SetupReqValue >> 8 )].len;
                        break;
                    case UVC_GET_INFO:
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBSS_SetupReqValue >> 8 )].info;
                        break;
                    case UVC_GET_DEF:
                        *pDescr = (PUINT8)&ExtensionID1[(uint8_t)( USBSS_SetupReqValue >> 8 )].def;
                        break;
                }
                break;
            case UVC_UNIT_ID06:
                switch(USBSS_SetupReqCode)
                {
                    case UVC_GET_CUR:
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBSS_SetupReqValue >> 8 )].cur;
                        break;
                    case UVC_GET_MIN:
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBSS_SetupReqValue >> 8 )].min;
                        break;
                    case UVC_GET_MAX:
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBSS_SetupReqValue >> 8 )].max;
                        break;
                    case UVC_GET_RES:
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBSS_SetupReqValue >> 8 )].res;
                        break;
                    case UVC_GET_LEN:
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBSS_SetupReqValue >> 8 )].len;
                        break;
                    case UVC_GET_INFO:
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBSS_SetupReqValue >> 8 )].info;
                        break;
                    case UVC_GET_DEF:
                        *pDescr = (PUINT8)&ExtensionID2[(uint8_t)( USBSS_SetupReqValue >> 8 )].def;
                        break;
                }
                break;
        }

        if(((uint8_t)( USBSS_SetupReqIndex >> 8 ) == UVC_UNIT_ID01 ) && (((uint8_t)( USBSS_SetupReqValue >> 8 )==0x14) ||((uint8_t)( USBSS_SetupReqValue >> 8 )==0x09)))
        {
                VAICSID[ 2 ].cur[ 0 ]=(UINT8)InvalidControl;
                return 0xFF;
        }
    }
    return 0;
}

/*******************************************************************************
 * @fn      clearError
 *
 * @brief   clear error
 *
 * @return  None
 */
void ClearError( void )
{
    VAICSID[(uint8_t)( USBSS_SetupReqValue >> 8 )].cur[ 0 ] = NoError;
}
