/********************************** (C) COPYRIGHT  *******************************
* File Name          : camera.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/10/26
* Description        : DVP sensor abstraction - probe and dispatch.
*********************************************************************************/
#ifndef __CAMERA_H
#define __CAMERA_H

#include "ch32h417.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum {
    CAM_UNKNOWN = 0,
    CAM_OV2640,
    CAM_OV5640,
} camera_model_t;

extern volatile camera_model_t g_camera_model;

/* 0 = ok, non-zero = no sensor found */
uint8_t Camera_Init(void);

void Camera_Format_Mode(uint8_t choice);
void Camera_Change_Resolution(uint8_t formatindex, uint8_t frameindex);

uint8_t  Camera_Get_Resolution(uint8_t formatindex, uint8_t frameindex,
                               uint16_t *w, uint16_t *h);
uint32_t Camera_Get_Interval(uint8_t formatindex, uint8_t frameindex);
uint8_t  Camera_Get_FrameCount(void);

#ifdef __cplusplus
}
#endif

#endif /* __CAMERA_H */
