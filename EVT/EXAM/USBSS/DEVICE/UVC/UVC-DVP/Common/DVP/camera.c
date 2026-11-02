/********************************** (C) COPYRIGHT  *******************************
* File Name          : camera.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/10/26
* Description        : DVP sensor abstraction - OV2640/OV5640 probe and dispatch.
*********************************************************************************/
#include "ch32h417.h"
#include "debug.h"
#include "dvp.h"
#include "uvclib.h"
#include "camera.h"
#include "ov2640.h"
#include "ov5640.h"

volatile camera_model_t g_camera_model = CAM_UNKNOWN;

/*******************************************************************************
 * @fn      Camera_Init
 *
 * @brief   Probe the DVP sensor and initialize it
 *
 * @return  0 - Success, 1 - Error
 */
uint8_t Camera_Init(void)
{
    DVP_GPIO_Init();
    SCCB_Init();

    OV_PWDN_CLR;              /* POWER ON */
    Delay_Ms(10);
    OV_RESET_CLR;
    Delay_Ms(10);
    OV_RESET_SET;
    Delay_Ms(10);

    if( OV2640_Detect() )
    {
        g_camera_model = CAM_OV2640;
        printf("Camera: OV2640 detected\r\n");
        return OV2640_Init();
    }

    if( OV5640_Detect() )
    {
        g_camera_model = CAM_OV5640;
        printf("Camera: OV5640 detected\r\n");
        return OV5640_Init();
    }

    g_camera_model = CAM_UNKNOWN;
    printf("Camera: no sensor found\r\n");
    return 1;
}

/*******************************************************************************
 * @fn      Camera_Format_Mode
 *
 * @brief   Select the output format
 *
 * @return  none
 */
void Camera_Format_Mode(uint8_t choice)
{
    if( g_camera_model == CAM_OV5640 )
    {
        OV5640_Format_Mode(choice);
    }
    else
    {
        OV2640_Format_Mode(choice);
    }
}

/*******************************************************************************
 * @fn      Camera_Change_Resolution
 *
 * @brief   Apply the negotiated format/frame
 *
 * @return  none
 */
void Camera_Change_Resolution(uint8_t formatindex, uint8_t frameindex)
{
    if( formatindex != FORMAT_YUV2 )
    {
        return;
    }

    if( g_camera_model == CAM_OV5640 )
    {
        OV5640_Change_Resolution(formatindex, frameindex);
    }
    else
    {
        OV2640_Change_Resolution(formatindex, frameindex);
    }

    DVP_Stream_Reconfig();
}

/* Frame order must match usb_desc.c. */
static const uint16_t s_ov2640_hs[1][2] = 
{
    {640, 480},
};
static const uint16_t s_ov2640_ss[6][2] = 
{
    {640, 480}, {800, 600}, {1024, 768}, {1280, 960}, {1280, 1024}, {1600, 1200},
};
static const uint16_t s_ov5640_hs[1][2] = 
{
    {640, 480},
};
static const uint16_t s_ov5640_ss[4][2] = 
{
    {1024, 768}, {1280, 720}, {1920, 1080}, {2592, 1944},
};
static const uint32_t s_ov2640_interval_hs[1] = 
{
    333333,
};
static const uint32_t s_ov2640_interval_ss[6] = 
{
    333333, 333333, 666667, 666667, 666667, 666667,
};
static const uint32_t s_ov5640_interval_hs[1] =
{
    343800,
};
static const uint32_t s_ov5640_interval_ss[4] = 
{
    247500, 208125, 478667, 1166667,
};

/*******************************************************************************
 * @fn      Camera_Get_Resolution
 *
 * @brief   Get the resolution of a frame index
 *
 * @return  0 - Success, 1 - Error
 */
uint8_t Camera_Get_Resolution(uint8_t formatindex, uint8_t frameindex,
                              uint16_t *w, uint16_t *h)
{
    uint8_t is_hs = (uint8_t)((USB_Enum_Status == U2U3_SUCC) && USBHS_DevEnumStatus);
    const uint16_t (*tbl)[2];
    uint8_t count;

    if(formatindex != FORMAT_YUV2 || frameindex == 0) return 1;

    if(g_camera_model == CAM_OV5640)
    {
        tbl = is_hs ? s_ov5640_hs : s_ov5640_ss;
        count = is_hs ? 1 : 4;
    }
    else
    {
        tbl = is_hs ? s_ov2640_hs : s_ov2640_ss;
        count = is_hs ? 1 : 6;
    }
    if(frameindex > count) return 1;

    *w = tbl[frameindex - 1][0];
    *h = tbl[frameindex - 1][1];
    return 0;
}

/*******************************************************************************
 * @fn      Camera_Get_Interval
 *
 * @brief   Get the frame interval of a frame index
 *
 * @return  Interval in 100ns
 */
uint32_t Camera_Get_Interval(uint8_t formatindex, uint8_t frameindex)
{
    uint8_t is_hs = (uint8_t)((USB_Enum_Status == U2U3_SUCC) && USBHS_DevEnumStatus);

    if(formatindex != FORMAT_YUV2 || frameindex == 0) return 666667;

    if(g_camera_model == CAM_OV5640)
    {
        if(is_hs) return s_ov5640_interval_hs[0];
        if(frameindex > 4) return s_ov5640_interval_ss[0];
        return s_ov5640_interval_ss[frameindex - 1];
    }
    if(is_hs)
    {
        if(frameindex > 1) return s_ov2640_interval_hs[0];
        return s_ov2640_interval_hs[frameindex - 1];
    }
    if(frameindex > 6) return s_ov2640_interval_ss[0];
    return s_ov2640_interval_ss[frameindex - 1];
}

/*******************************************************************************
 * @fn      Camera_Get_FrameCount
 *
 * @brief   Get the frame count of the current speed
 *
 * @return  Frame count
 */
uint8_t Camera_Get_FrameCount(void)
{
    uint8_t is_hs = (uint8_t)((USB_Enum_Status == U2U3_SUCC) && USBHS_DevEnumStatus);

    if(g_camera_model == CAM_OV5640) return is_hs ? 1 : 4;
    return is_hs ? 1 : 6;
}
