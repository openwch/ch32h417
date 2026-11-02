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


/* UVC control values: current / min / max / default / resolution */
typedef struct
{
    uint8_t cur[4];
    uint8_t min[4];
    uint8_t max[4];
    uint8_t def[4];
    uint8_t res[4];
} uvc_ctrl_val_t;

typedef struct
{
    uint8_t entity_id;
    uint8_t control_sel;
    uint16_t data_len;
    uint8_t info;
    uvc_ctrl_val_t *val;
} uvc_control_entry_t;

static uvc_ctrl_val_t s_pu_brightness   = {{0x00,0x00},{0x80,0xFF},{0x7F,0x00},{0x00,0x00},{0x01,0x00}};
static uvc_ctrl_val_t s_pu_contrast     = {{0x64,0x00},{0x00,0x00},{0xC8,0x00},{0x64,0x00},{0x01,0x00}};
static uvc_ctrl_val_t s_pu_hue          = {{0x00,0x00},{0x4C,0xFF},{0xB4,0x00},{0x00,0x00},{0x01,0x00}};
static uvc_ctrl_val_t s_pu_saturation   = {{0x64,0x00},{0x00,0x00},{0xC8,0x00},{0x64,0x00},{0x01,0x00}};
static uvc_ctrl_val_t s_pu_sharpness    = {{0x32,0x00},{0x00,0x00},{0x64,0x00},{0x32,0x00},{0x01,0x00}};
static uvc_ctrl_val_t s_pu_gamma        = {{0x64,0x00},{0x32,0x00},{0xC8,0x00},{0x64,0x00},{0x01,0x00}};
static uvc_ctrl_val_t s_pu_wb_temp      = {{0xA0,0x0F},{0xF0,0x0A},{0x64,0x19},{0xA0,0x0F},{0x01,0x00}};
static uvc_ctrl_val_t s_pu_wb_auto      = {{0x01},{0x00},{0x01},{0x01},{0x01}};
static uvc_ctrl_val_t s_ct_ae_mode      = {{0x02},{0x01},{0x08},{0x02},{0x01}};
static uvc_ctrl_val_t s_ct_ae_priority  = {{0x00},{0x00},{0x01},{0x00},{0x01}};
static uvc_ctrl_val_t s_ct_exposure     = {{0x9C,0x00,0x00,0x00},{0x01,0x00,0x00,0x00},{0x10,0x27,0x00,0x00},{0x9C,0x00,0x00,0x00},{0x01,0x00,0x00,0x00}};
static uvc_ctrl_val_t s_ct_focus_abs    = {{0x00,0x00},{0x00,0x00},{0xFA,0x00},{0x00,0x00},{0x01,0x00}};
static uvc_ctrl_val_t s_ct_focus_auto   = {{0x01},{0x00},{0x01},{0x01},{0x01}};

/* Supported VideoControl controls: Processing Unit (0x03) and Camera Terminal (0x01) */
static uvc_control_entry_t s_ctrl_tbl[] =
{
    {0x03, 0x02, 2, 0x03, &s_pu_brightness},
    {0x03, 0x03, 2, 0x03, &s_pu_contrast},
    {0x03, 0x06, 2, 0x03, &s_pu_hue},
    {0x03, 0x07, 2, 0x03, &s_pu_saturation},
    {0x03, 0x08, 2, 0x03, &s_pu_sharpness},
    {0x03, 0x09, 2, 0x03, &s_pu_gamma},
    {0x03, 0x0A, 2, 0x03, &s_pu_wb_temp},
    {0x03, 0x0B, 1, 0x03, &s_pu_wb_auto},
    {0x01, 0x02, 1, 0x03, &s_ct_ae_mode},
    {0x01, 0x03, 1, 0x03, &s_ct_ae_priority},
    {0x01, 0x04, 4, 0x03, &s_ct_exposure},
    {0x01, 0x06, 2, 0x03, &s_ct_focus_abs},
    {0x01, 0x08, 1, 0x03, &s_ct_focus_auto},
};

/*******************************************************************************
 * @fn      uvc_ctrl_find
 *
 * @brief   Find a control entry
 *
 * @return  Entry pointer, NULL if not supported
 */
static uvc_control_entry_t *uvc_ctrl_find(uint8_t entity, uint8_t sel)
{
    uint16_t i;

    for(i = 0; i < sizeof(s_ctrl_tbl) / sizeof(s_ctrl_tbl[0]); i++)
    {
        if((s_ctrl_tbl[i].entity_id == entity) && (s_ctrl_tbl[i].control_sel == sel))
        {
            return &s_ctrl_tbl[i];
        }
    }
    return 0;
}

/*******************************************************************************
 * @fn      uvc_ctrl_reply
 *
 * @brief   Build the answer of a VideoControl request
 *
 * @return  0 - Handled, 0xFF - Not supported
 */
static uint16_t uvc_ctrl_reply(uint8_t entity, uint8_t sel, uint8_t req, const uint8_t **pDescr)
{
    uvc_control_entry_t *e = uvc_ctrl_find(entity, sel);

    if(!e) return 0xFF;

    switch(req)
    {
        case 0x81: *pDescr = e->val->cur;      break;
        case 0x82: *pDescr = e->val->min;      break;
        case 0x83: *pDescr = e->val->max;      break;
        case 0x84: *pDescr = e->val->res;      break;
        case 0x85: *pDescr = (uint8_t *)&e->data_len; break;
        case 0x86: *pDescr = &e->info;         break;
        case 0x87: *pDescr = e->val->def;      break;
        case 0x01: return 0;                   /* SET_CUR: data arrives in the OUT stage */
        default:   return 0xFF;
    }
    return 0;
}

/*******************************************************************************
 * @fn      UVC_Control_SetData
 *
 * @brief   Store VideoControl SET_CUR data
 *
 * @return  none
 */
void UVC_Control_SetData(uint8_t entity, uint8_t sel, uint8_t *buf, uint16_t len)
{
    uvc_control_entry_t *e = uvc_ctrl_find(entity, sel);

    if(e && buf && (len >= e->data_len))
    {
        uint16_t i;
        for(i = 0; i < e->data_len; i++) e->val->cur[i] = buf[i];
    }
}

/*******************************************************************************
 * @fn      USBHS_UVC_NonStandardReq
 *
 * @brief   UVC class request of the USBHS stack
 *
 * @return  0 - Handled, 0xFF - Stall
 */
uint16_t USBHS_UVC_NonStandardReq(const uint8_t **pDescr)
{
    uint8_t entity = (uint8_t)(USBHS_SetupReqIndex >> 8);
    uint8_t ifc    = (uint8_t)USBHS_SetupReqIndex;
    uint8_t sel    = (uint8_t)(USBHS_SetupReqValue >> 8);

    if((USBHS_SetupReqType != 0xA1) && (USBHS_SetupReqType != 0x21)) return 0xFF;

    if((entity == 0) && (ifc == UVC_STERAMING_INTERFACE))
    {
        UVC_ControlSel = sel;
        switch(USBHS_SetupReqCode)
        {
            case 0x81: UVC_Ctrl_Build(Get_Curr[2], Get_Curr[3]); *pDescr = UVC_Ctrl_Buf; break;
            case 0x82: UVC_Ctrl_Build_Min(); *pDescr = UVC_Ctrl_Buf; break;
            case 0x83: UVC_Ctrl_Build_Max(); *pDescr = UVC_Ctrl_Buf; break;
            case 0x84: UVC_Ctrl_Build_Res(); *pDescr = UVC_Ctrl_Buf; break;
            case 0x85: *pDescr = UVC_Ctrl_Len; break;
            case 0x86: *pDescr = &UVC_Ctrl_Info; break;
            case 0x87: UVC_Ctrl_Build_Def(); *pDescr = UVC_Ctrl_Buf; break;
            default:   return 0xFF;
        }
        return 0;
    }

    return uvc_ctrl_reply(entity, sel, (uint8_t)USBHS_SetupReqCode, pDescr);
}

/*******************************************************************************
 * @fn      USBSS_UVC_NonStandardReq
 *
 * @brief   UVC class request of the USBSS stack
 *
 * @return  0 - Handled, 0xFF - Stall
 */
uint16_t USBSS_UVC_NonStandardReq(const uint8_t **pDescr)
{
    uint8_t entity = (uint8_t)(USBSS_SetupReqIndex >> 8);
    uint8_t ifc    = (uint8_t)USBSS_SetupReqIndex;
    uint8_t sel    = (uint8_t)(USBSS_SetupReqValue >> 8);

    if((USBSS_SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_CLASS) return 0xFF;

    if((entity == 0) && (ifc == UVC_STERAMING_INTERFACE))
    {
        UVC_ControlSel = sel;
        switch((uint8_t)USBSS_SetupReqCode)
        {
            case 0x81: UVC_Ctrl_Build(Get_Curr[2], Get_Curr[3]); *pDescr = UVC_Ctrl_Buf; break;
            case 0x82: UVC_Ctrl_Build_Min(); *pDescr = UVC_Ctrl_Buf; break;
            case 0x83: UVC_Ctrl_Build_Max(); *pDescr = UVC_Ctrl_Buf; break;
            case 0x84: UVC_Ctrl_Build_Res(); *pDescr = UVC_Ctrl_Buf; break;
            case 0x85: *pDescr = UVC_Ctrl_Len; break;
            case 0x86: *pDescr = &UVC_Ctrl_Info; break;
            case 0x87: UVC_Ctrl_Build_Def(); *pDescr = UVC_Ctrl_Buf; break;
            default:   return 0xFF;
        }
        return 0;
    }

    return uvc_ctrl_reply(entity, sel, (uint8_t)USBSS_SetupReqCode, pDescr);
}
