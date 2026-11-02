/********************************** (C) COPYRIGHT *******************************
* File Name          : uvclib.h
* Author             : WCH
* Version            : V1.2
* Date               : 2024/07/06
* Description        :
*********************************************************************************
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef UVCLIB_H_
#define UVCLIB_H_

#include "ch32h417_dvp.h"
#include "ch32h417_usbss_device.h"
#include "ch32h417_usbhs_device.h"


/* UVC bFormatIndex */
#define FORMAT_YUV2                       1

/* UVC payload header and clock */
#define UVC_HEADERSIZE                    12
#define SOURCECLOCK                       ((~(SysTick0->CNT)) & 0xffffffff)

/* Probe/Commit control size */
#define DEF_UVC10_VEDIO_CTL               26
#define DEF_UVC11_VEDIO_CTL               34
#define DEF_UVC15_VEDIO_CTL               48

/* Unit ID and interfaces */
#define UVC_UNIT_NONE                     0x00
#define UVC_UNIT_ID01                     0x01
#define UVC_UNIT_ID02                     0x02
#define UVC_UNIT_ID03                     0x03
#define UVC_UNIT_ID04                     0x04
#define UVC_UNIT_ID05                     0x05
#define UVC_UNIT_ID06                     0x06
#define UVC_CONTROL_INTERFACE_ID          0x00
#define UVC_STERAMING_INTERFACE           0x01
#define UVC_GET_SEL_CONTROL_UNIT          0x01

/* Camera Terminal control selectors */
#define CT_CONTROL_UNDEFINED              0x00
#define CT_SCANNING_MODE_CONTROL          0x01
#define CT_AE_MODE_CONTROL                0x02
#define CT_AE_PRIORITY_CONTROL            0x03
#define CT_EXPOSURE_TIME_ABSOLUTE_CONTROL 0x04
#define CT_EXPOSURE_TIME_RELATIVE_CONTROL 0x05
#define CT_FOCUS_ABSOLUTE_CONTROL         0x06
#define CT_FOCUS_RELATIVE_CONTROL         0x07
#define CT_FOCUS_AUTO_CONTROL             0x08
#define CT_IRIS_ABSOLUTE_CONTROL          0x09
#define CT_IRIS_RELATIVE_CONTROL          0x0A
#define CT_ZOOM_ABSOLUTE_CONTROL          0x0B
#define CT_ZOOM_RELATIVE_CONTROL          0x0C
#define CT_PANTILT_ABSOLUTE_CONTROL       0x0D
#define CT_PANTILT_RELATIVE_CONTROL       0x0E
#define CT_ROLL_ABSOLUTE_CONTROL          0x0F
#define CT_ROLL_RELATIVE_CONTROL          0x10
#define CT_PRIVACY_CONTROL                0x11
#define CT_FOCUS_SIMPLE_CONTROL           0x12
#define CT_WINDOW_CONTROL                 0x13
#define CT_REGION_OF_INTEREST_CONTROL     0x14

/* Global variables */
extern volatile uint8_t  Get_Curr[26];
extern volatile uint16_t Resolution_width;
extern volatile uint16_t Resolution_height;
extern volatile uint16_t Dvp_DataSize;
extern uint8_t *Dvp_Recaddr;

/* Probe/Commit control (UVC 1.0) */
extern uint8_t UVC_Ctrl_Buf[26];
extern uint8_t UVC_Ctrl_Len[2];
extern uint8_t UVC_Ctrl_Info;
extern volatile uint8_t UVC_ControlSel;

/* OV2640 frame resolution tables */
extern const uint16_t ov2640_YUVframe_resolution_USB20[1][2];
extern const uint16_t ov2640_YUVframe_resolution_USB30[6][2];

/* Function declaration */
extern uint16_t USBHS_UVC_NonStandardReq(const uint8_t **pDescr);
extern uint16_t USBSS_UVC_NonStandardReq(const uint8_t **pDescr);
void UVC_Ctrl_Build(uint8_t fmt, uint8_t frm);
void UVC_Ctrl_Build_Def(void);
void UVC_Ctrl_Build_Min(void);
void UVC_Ctrl_Build_Max(void);
void UVC_Ctrl_Build_Res(void);
void UVC_Ctrl_SetData(uint8_t *buf);
void UVC_Control_SetData(uint8_t entity, uint8_t sel, uint8_t *buf, uint16_t len);
void DVP_Stream_Cfg(uint32_t payload);
extern void SS_CtrlCamera(void);
extern void HS_CtrlCamera(void);
extern void DVP_Hander(void);
extern void SS_Endp1_Hander(void);
extern void HS_Endp1_Hander(void);
extern void HS_Endp1_ISOHander(void);

#endif /* UVCLIB_H_ */

