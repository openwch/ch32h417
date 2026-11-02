/********************************** (C) COPYRIGHT *******************************
* File Name          : uvc.c
* Author             : WCH
* Version            : V1.3.0
* Date               : 2026/10/26
* Description        : UVC YUY2 streaming pipeline.
*********************************************************************************/
#include "uvclib.h"
#include "debug.h"
#include "string.h"
#include "dvp.h"
#include "camera.h"
#include "ov5640.h"

/* UVC 1.0 payload header (12 bytes) */
#define UVC_HEADER_LEN          12
#define UVC_BFH_FID             0x01
#define UVC_BFH_EOF             0x02
#define UVC_BFH_ERR             0x40
#define UVC_BFH_EOH             0x80

#define UVC_LINE_BYTES          (128 * 1024)
#define UVC_SLOT_SS             (32 * 1024)
#define UVC_SLOT_HS             (3 * 1024)
#define UVC_RING_MAX            64
#define UVC_DMA_ALIGN           32

typedef struct
{
    volatile uint32_t adr;
    volatile uint32_t len;
} uvc_pack_t;

typedef struct
{
    volatile uint8_t load;
    volatile uint8_t deal;
    volatile uint8_t total;
    volatile uint8_t stop;
} uvc_ring_t;

typedef struct
{
    uvc_ring_t ring;
    uvc_pack_t desc[UVC_RING_MAX];
} uvc_line_t;

static uvc_line_t s_line[2];
static uint8_t  s_frame_id;
static uint8_t  s_frame_error;
static uint8_t  s_eof_remaining;
static uint8_t  s_thread_seq;
static uint32_t s_max_payload = UVC_SLOT_SS;
static uint8_t  s_ring_depth = UVC_LINE_BYTES / UVC_SLOT_SS;
static uint32_t s_line_bytes;
static uint16_t s_row_num;
static uint16_t s_yuy2_line;
static uint8_t  s_cur_arr;
static uint32_t s_wr_off[2];
static uint8_t  s_pending;

#define UVC_DUMMY_LEN           6144
static uint8_t UVC_Line0_Buf[UVC_LINE_BYTES] __attribute__((aligned(32), section(".sram_buf")));
static uint8_t UVC_Line1_Buf[UVC_LINE_BYTES] __attribute__((aligned(32), section(".sram_buf")));
static uint8_t UVC_Empty_Buf[UVC_HEADER_LEN] __attribute__((aligned(32), section(".sram_buf")));
static uint8_t UVC_Dummy_Buf[UVC_DUMMY_LEN] __attribute__((aligned(32), section(".sram_buf")));

#define UVC_LINE0   (UVC_Line0_Buf)
#define UVC_LINE1   (UVC_Line1_Buf)

volatile uint8_t Get_Curr[26] =
{
    0x00, 0x00, 0x01, 0x01,
    0x15, 0x16, 0x05, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x20, 0x00, 0x00, 0x48,
    0x3F, 0x00, 0x00, 0x0C,
    0x00, 0x00
};

volatile uint16_t Resolution_width = 0;
volatile uint16_t Resolution_height = 0;
volatile uint16_t Dvp_DataSize = 0;
uint8_t *  Dvp_Recaddr = UVC_Line0_Buf + UVC_DMA_ALIGN;

uint8_t  UVC_Ctrl_Buf[26];
uint8_t  UVC_Ctrl_Len[2] = {26, 0};
uint8_t  UVC_Ctrl_Info = 0x03;
volatile uint8_t UVC_ControlSel;

const uint16_t ov2640_YUVframe_resolution_USB30[6][2] =
{
    0x280,0x1e0, 0x320,0x258, 0x400,0x300, 0x500,0x3C0, 0x500,0x400, 0x640,0x4b0,
};
const uint16_t ov2640_YUVframe_resolution_USB20[1][2] =
{
    0x280,0x1e0,
};

/*******************************************************************************
 * @fn      uvc_slot_addr
 *
 * @brief   Resolve the DMA address of a slot
 *
 * @return  Address
 */
static uint32_t uvc_slot_addr(uint8_t arr, uint32_t off)
{
    uint8_t *base = arr ? UVC_LINE1 : UVC_LINE0;
    return (uint32_t)(base + s_max_payload * s_line[arr].ring.load + UVC_DMA_ALIGN + off);
}

/*******************************************************************************
 * @fn      uvc_ring_init
 *
 * @brief   Initialize the slot descriptors
 *
 * @return  none
 */
static void uvc_ring_init(void)
{
    uint8_t i;

    for(i = 0; i < s_ring_depth; i++)
    {
        s_line[0].desc[i].adr = (uint32_t)(UVC_LINE0 + s_max_payload * i + UVC_DMA_ALIGN);
        s_line[0].desc[i].len = 0;
        s_line[1].desc[i].adr = (uint32_t)(UVC_LINE1 + s_max_payload * i + UVC_DMA_ALIGN);
        s_line[1].desc[i].len = 0;

        memset(&UVC_LINE0[s_max_payload * i + UVC_DMA_ALIGN - UVC_HEADER_LEN], 0, UVC_HEADER_LEN);
        memset(&UVC_LINE1[s_max_payload * i + UVC_DMA_ALIGN - UVC_HEADER_LEN], 0, UVC_HEADER_LEN);
        UVC_LINE0[s_max_payload * i + UVC_DMA_ALIGN - UVC_HEADER_LEN] = UVC_HEADER_LEN;
        UVC_LINE0[s_max_payload * i + UVC_DMA_ALIGN - UVC_HEADER_LEN + 1] = UVC_BFH_EOH;
        UVC_LINE1[s_max_payload * i + UVC_DMA_ALIGN - UVC_HEADER_LEN] = UVC_HEADER_LEN;
        UVC_LINE1[s_max_payload * i + UVC_DMA_ALIGN - UVC_HEADER_LEN + 1] = UVC_BFH_EOH;
    }
}

/*******************************************************************************
 * @fn      uvc_phase_reset
 *
 * @brief   Reset the write phase
 *
 * @return  none
 */
static void uvc_phase_reset(void)
{
    s_cur_arr = 0;
    s_wr_off[0] = 0;
    s_wr_off[1] = 0;
    s_yuy2_line = 0;
}

/*******************************************************************************
 * @fn      uvc_rearm_dma
 *
 * @brief   Point the DVP DMA at the current slot
 *
 * @return  none
 */
static void uvc_rearm_dma(void)
{
    uint32_t addr = uvc_slot_addr(s_cur_arr, s_wr_off[s_cur_arr]);
    DVP->DMA_BUF0 = addr;
    DVP->DMA_BUF1 = addr;
}

/*******************************************************************************
 * @fn      uvc_slot_submit
 *
 * @brief   Push the current slot into the ring
 *
 * @return  none
 */
static void uvc_slot_submit(uint8_t arr, uint32_t data_len)
{
    uint8_t ld = s_line[arr].ring.load;

    NVIC_DisableIRQ(USBSS_IRQn);
    NVIC_DisableIRQ(USBHS_IRQn);

    if(s_line[arr].ring.total < (uint8_t)(s_ring_depth - 1))
    {
        s_line[arr].ring.total++;
        s_line[arr].desc[ld].len = UVC_HEADER_LEN + data_len;

        {
            uint8_t *hdr = (uint8_t *)(s_line[arr].desc[ld].adr - UVC_HEADER_LEN);
            if(s_frame_error) hdr[1] |= UVC_BFH_ERR;
            else              hdr[1] &= (uint8_t)~UVC_BFH_ERR;
        }

        ld++;
        if(ld >= s_ring_depth) ld = 0;
        s_line[arr].ring.load = ld;
    }
    else
    {
        s_frame_error = 1;
    }

    NVIC_EnableIRQ(USBSS_IRQn);
    NVIC_EnableIRQ(USBHS_IRQn);

    s_wr_off[arr] = 0;
}

/*******************************************************************************
 * @fn      uvc_partial_flush
 *
 * @brief   Push the partially filled slot
 *
 * @return  none
 */
static void uvc_partial_flush(void)
{
    if(s_wr_off[0]) uvc_slot_submit(0, s_wr_off[0]);
    if(s_wr_off[1]) uvc_slot_submit(1, s_wr_off[1]);
}

/*******************************************************************************
 * @fn      uvc_stream_reset
 *
 * @brief   Reset the stream rings
 *
 * @return  none
 */
static void uvc_stream_reset(void)
{
    uint8_t i;

    for(i = 0; i < 2; i++)
    {
        s_line[i].ring.load = 0;
        s_line[i].ring.deal = 0;
        s_line[i].ring.total = 0;
        s_line[i].ring.stop = 0;
    }

    s_eof_remaining = 0;
    s_thread_seq = 0;
    s_frame_error = 0;
    s_frame_id = 0;
    uvc_ring_init();
    uvc_phase_reset();
}

/*******************************************************************************
 * @fn      uvc_set_geometry
 *
 * @brief   Set the stream geometry
 *
 * @return  none
 */
static void uvc_set_geometry(uint16_t w, uint16_t h)
{
    s_line_bytes = (uint32_t)w * 2;
    s_row_num = h;

    Dvp_Row_Col_Set(w, h);
    Dvp_DataSize = (uint16_t)s_line_bytes;

    uvc_phase_reset();
}

/*******************************************************************************
 * @fn      uvc_consumer_get
 *
 * @brief   Fetch the next slot to send
 *
 * @return  1 - Got a slot, 0 - Empty
 */
static uint8_t uvc_consumer_get(uint32_t *adr, uint32_t *len, uint8_t *eof)
{
    uint8_t cur = s_thread_seq;
    uint8_t deal;

    if(s_line[cur].ring.total == 0 && s_line[cur ^ 1].ring.total > 0)
    {
        cur ^= 1;
    }
    if(s_line[cur].ring.total == 0)
    {
        return 0;
    }

    NVIC_DisableIRQ(DVP_IRQn);
    s_line[cur].ring.total--;
    *eof = 0;
    if(s_eof_remaining && --s_eof_remaining == 0)
    {
        *eof = 1;
    }

    deal = s_line[cur].ring.deal;
    *adr = s_line[cur].desc[deal].adr;
    *len = s_line[cur].desc[deal].len;
    NVIC_EnableIRQ(DVP_IRQn);

    deal++;
    if(deal >= s_ring_depth) deal = 0;
    s_line[cur].ring.deal = deal;
    s_thread_seq = cur ^ 1;
    return 1;
}

/*******************************************************************************
 * @fn      uvc_header_set
 *
 * @brief   Fill the UVC payload header
 *
 * @return  none
 */
static void uvc_header_set(uint32_t adr, uint8_t eof)
{
    uint8_t *hdr = (uint8_t *)(adr - UVC_HEADER_LEN);

    hdr[1] = (uint8_t)(UVC_BFH_EOH | s_frame_id | (hdr[1] & UVC_BFH_ERR));
    if(eof)
    {
        hdr[1] |= UVC_BFH_EOF;
        s_frame_id ^= UVC_BFH_FID;
    }
}

/*******************************************************************************
 * @fn      uvc_empty_header_set
 *
 * @brief   Prepare an empty payload header
 *
 * @return  none
 */
static void uvc_empty_header_set(void)
{
    memset(UVC_Empty_Buf, 0, UVC_HEADER_LEN);
    UVC_Empty_Buf[0] = UVC_HEADER_LEN;
    UVC_Empty_Buf[1] = (uint8_t)(UVC_BFH_EOH | s_frame_id);
}

/*******************************************************************************
 * @fn      uvc_start_capture
 *
 * @brief   Enable DVP capture
 *
 * @return  none
 */
static void uvc_start_capture(void)
{
    uvc_phase_reset();
    uvc_rearm_dma();
    DVP->IFR = RB_DVP_IF_ROW_DONE | RB_DVP_IF_FRM_DONE | RB_DVP_IF_STR_FRM |
               RB_DVP_IF_STP_FRM | RB_DVP_IF_FIFO_OV;
    DVP->CR0 |= RB_DVP_ENABLE;
    NVIC_EnableIRQ(DVP_IRQn);
}

/*******************************************************************************
 * @fn      uvc_pending_service
 *
 * @brief   Start capture on the first EP1 request
 *
 * @return  none
 */
static void uvc_pending_service(void)
{
    if(s_pending)
    {
        s_pending = 0;
        uvc_start_capture();
    }
}

/*******************************************************************************
 * @fn      uvc_rd32
 *
 * @brief   Read a 32-bit little-endian value
 *
 * @return  Value
 */
static uint32_t uvc_rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/*******************************************************************************
 * @fn      uvc_wr32
 *
 * @brief   Write a 32-bit little-endian value
 *
 * @return  none
 */
static void uvc_wr32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

/*******************************************************************************
 * @fn      DVP_Stream_Cfg
 *
 * @brief   Set the slot size from the negotiated payload
 *
 * @return  none
 */
void DVP_Stream_Cfg(uint32_t payload)
{
    uint8_t depth;

    if(payload < 1024)  payload = 1024;
    if(payload > UVC_SLOT_SS) payload = UVC_SLOT_SS;
    payload &= (uint32_t)~31u;

    depth = (uint8_t)(UVC_LINE_BYTES / payload);
    if(depth > UVC_RING_MAX) depth = UVC_RING_MAX;
    if(depth < 2)            depth = 2;

    if(payload == s_max_payload && depth == s_ring_depth)
    {
        return;
    }

    s_max_payload = payload;
    s_ring_depth = depth;
    uvc_ring_init();
}

/*******************************************************************************
 * @fn      uvc_ctrl_set
 *
 * @brief   Fill the Probe/Commit response
 *
 * @return  none
 */
static void uvc_ctrl_set(uint16_t hint, uint8_t fmt, uint8_t frm, uint32_t interval,
                         uint32_t frame_size, uint32_t payload)
{
    memset(UVC_Ctrl_Buf, 0, sizeof(UVC_Ctrl_Buf));
    UVC_Ctrl_Buf[0] = (uint8_t)(hint & 0xFFu);
    UVC_Ctrl_Buf[1] = (uint8_t)(hint >> 8);
    UVC_Ctrl_Buf[2] = fmt;
    UVC_Ctrl_Buf[3] = frm;
    uvc_wr32(&UVC_Ctrl_Buf[4], interval);
    uvc_wr32(&UVC_Ctrl_Buf[18], frame_size);
    uvc_wr32(&UVC_Ctrl_Buf[22], payload);
}

/*******************************************************************************
 * @fn      uvc_best_frame
 *
 * @brief   Find the smallest or largest frame
 *
 * @return  Frame index
 */
static uint8_t uvc_best_frame(uint8_t want_max, uint16_t *w, uint16_t *h)
{
    uint8_t i, count = Camera_Get_FrameCount(), best = 1;
    uint32_t area = 0, a;
    uint16_t bw = 0, bh = 0;

    for(i = 1; i <= count; i++)
    {
        if(Camera_Get_Resolution(FORMAT_YUV2, i, &bw, &bh) != 0) break;
        a = (uint32_t)bw * bh;
        if((i == 1) || (want_max ? (a > area) : (a < area)))
        {
            area = a;
            best = i;
        }
    }
    Camera_Get_Resolution(FORMAT_YUV2, best, w, h);
    return best;
}

/*******************************************************************************
 * @fn      UVC_Ctrl_Build_Def
 *
 * @brief   Build the default Probe/Commit response
 *
 * @return  none
 */
void UVC_Ctrl_Build_Def(void)
{
    uint16_t w = 640, h = 480;
    uint8_t is_hs = (uint8_t)((USB_Enum_Status == U2U3_SUCC) && USBHS_DevEnumStatus);

    Camera_Get_Resolution(FORMAT_YUV2, 1, &w, &h);
    uvc_ctrl_set(0x0001, FORMAT_YUV2, 1, Camera_Get_Interval(FORMAT_YUV2, 1),
                 (uint32_t)w * h * 2u, is_hs ? UVC_SLOT_HS : UVC_SLOT_SS);
}

/*******************************************************************************
 * @fn      UVC_Ctrl_Build_Min
 *
 * @brief   Build the minimum Probe/Commit response
 *
 * @return  none
 */
void UVC_Ctrl_Build_Min(void)
{
    uint16_t w = 0, h = 0;
    uint8_t i, count, idx;
    uint32_t interval = 0;

    idx = uvc_best_frame(0, &w, &h);
    count = Camera_Get_FrameCount();
    for(i = 1; i <= count; i++)
    {
        uint32_t iv = Camera_Get_Interval(FORMAT_YUV2, i);
        if(iv > interval) interval = iv;
    }
    uvc_ctrl_set(0x0000, FORMAT_YUV2, idx, interval, (uint32_t)w * h * 2u, 1024);
}

/*******************************************************************************
 * @fn      UVC_Ctrl_Build_Max
 *
 * @brief   Build the maximum Probe/Commit response
 *
 * @return  none
 */
void UVC_Ctrl_Build_Max(void)
{
    uint16_t w = 0, h = 0;
    uint8_t idx, is_hs = (uint8_t)((USB_Enum_Status == U2U3_SUCC) && USBHS_DevEnumStatus);

    idx = uvc_best_frame(1, &w, &h);
    uvc_ctrl_set(0xFFFF, FORMAT_YUV2, idx, Camera_Get_Interval(FORMAT_YUV2, idx),
                 (uint32_t)w * h * 2u, is_hs ? UVC_SLOT_HS : UVC_SLOT_SS);
    UVC_Ctrl_Buf[12] = 0x10;    /* wCompQuality = 10000 */
    UVC_Ctrl_Buf[13] = 0x27;
}

/*******************************************************************************
 * @fn      UVC_Ctrl_Build_Res
 *
 * @brief   Build the resolution Probe/Commit response
 *
 * @return  none
 */
void UVC_Ctrl_Build_Res(void)
{
    UVC_Ctrl_Build_Def();
    uvc_wr32(&UVC_Ctrl_Buf[4], 1);   /* frame interval step */
}
/*******************************************************************************
 * @fn      UVC_Ctrl_Build
 *
 * @brief   Fill the Probe/Commit response
 *
 * @return  none
 */
void UVC_Ctrl_Build(uint8_t fmt, uint8_t frm)
{
    uint16_t w = 640, h = 480;
    uint32_t interval;
    uint8_t  is_hs = (uint8_t)((USB_Enum_Status == U2U3_SUCC) && USBHS_DevEnumStatus);

    if(Camera_Get_Resolution(fmt, frm, &w, &h) != 0)
    {
        fmt = FORMAT_YUV2;
        frm = 1;
        Camera_Get_Resolution(fmt, frm, &w, &h);
    }
    interval = Camera_Get_Interval(fmt, frm);

    memset(UVC_Ctrl_Buf, 0, sizeof(UVC_Ctrl_Buf));
    UVC_Ctrl_Buf[2] = fmt;
    UVC_Ctrl_Buf[3] = frm;
    uvc_wr32(&UVC_Ctrl_Buf[4], interval);
    uvc_wr32(&UVC_Ctrl_Buf[18], (uint32_t)w * h * 2u);
    uvc_wr32(&UVC_Ctrl_Buf[22], is_hs ? UVC_SLOT_HS : UVC_SLOT_SS);
}

/*******************************************************************************
 * @fn      UVC_Ctrl_SetData
 *
 * @brief   Handle SET_CUR Probe/Commit data
 *
 * @return  none
 */
void UVC_Ctrl_SetData(uint8_t *buf)
{
    uint8_t  fmt = buf[2];
    uint8_t  frm = buf[3];
    uint32_t interval = uvc_rd32(&buf[4]);
    uint32_t payload  = uvc_rd32(&buf[22]);
    uint8_t  is_hs = (uint8_t)((USB_Enum_Status == U2U3_SUCC) && USBHS_DevEnumStatus);
    uint32_t limit = is_hs ? UVC_SLOT_HS : UVC_SLOT_SS;
    uint16_t w = 640, h = 480;

    if(Camera_Get_Resolution(fmt, frm, &w, &h) != 0)
    {
        fmt = FORMAT_YUV2;
        frm = 1;
        Camera_Get_Resolution(fmt, frm, &w, &h);
    }
    if(payload == 0 || payload > limit) payload = limit;
    if(payload < 1024) payload = 1024;
    if(interval == 0) interval = Camera_Get_Interval(fmt, frm);

    memset(UVC_Ctrl_Buf, 0, sizeof(UVC_Ctrl_Buf));
    UVC_Ctrl_Buf[2] = fmt;
    UVC_Ctrl_Buf[3] = frm;
    uvc_wr32(&UVC_Ctrl_Buf[4], interval);
    uvc_wr32(&UVC_Ctrl_Buf[18], (uint32_t)w * h * 2u);
    uvc_wr32(&UVC_Ctrl_Buf[22], payload);

    Get_Curr[2] = fmt;
    Get_Curr[3] = frm;

    if(UVC_ControlSel == 2)     /* UVC_VS_COMMIT_CONTROL */
    {
        DVP_Stream_Cfg(payload);
        if(g_camera_model == CAM_OV5640)
        {
            OV5640_Set_Pclk_Tier(is_hs);   /* must precede the mode tables */
        }
        Camera_Format_Mode(fmt);
        Camera_Change_Resolution(fmt, frm);
    }
}

/*******************************************************************************
 * @fn      UVC_SourceClock
 *
 * @brief   Enable the UVC timestamp clock
 *
 * @return  none
 */
void UVC_SourceClock(FunctionalState sta)
{
    if(sta)
    {
        SysTick0->CMP = 0xffffffff;
        SysTick0->CTLR = (1 << 3) | (1 << 2) | (1 << 0);
    }
    else
    {
        SysTick0->CTLR = 0;
    }
}

/*******************************************************************************
 * @fn      SS_CtrlCamera
 *
 * @brief   Start or stop the SS stream
 *
 * @return  none
 */
void SS_CtrlCamera(void)
{
    if((uint8_t)USBSS_SetupReqValue > 0)
    {
        DVP->CR0 &= ~RB_DVP_ENABLE;
        uvc_set_geometry(Resolution_width, Resolution_height);
        uvc_stream_reset();
        uvc_empty_header_set();

        USBSSD->EP1_TX.UEP_TX_DMA = (uint32_t)UVC_Empty_Buf;
        USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = UVC_HEADER_LEN;
        USBSSD->EP1_TX.UEP_TX_CHAIN_EXP_NUMP = 1;

        s_pending = 1;
        NVIC_DisableIRQ(DVP_IRQn);
    }
    else
    {
        s_pending = 0;
        DVP->CR0 &= ~RB_DVP_ENABLE;
        NVIC_DisableIRQ(DVP_IRQn);
        DVP->CR1 |= (RB_DVP_ALL_CLR | RB_DVP_RCV_CLR);
        DVP->CR1 &= ~(RB_DVP_ALL_CLR | RB_DVP_RCV_CLR);
    }
}

/*******************************************************************************
 * @fn      HS_CtrlCamera
 *
 * @brief   Start or stop the HS stream
 *
 * @return  none
 */
void HS_CtrlCamera(void)
{
    if((uint8_t)USBHS_SetupReqValue > 0)
    {
        DVP->CR0 &= ~RB_DVP_ENABLE;
        UVC_SourceClock(1);
        uvc_set_geometry(Resolution_width, Resolution_height);
        uvc_stream_reset();
        uvc_empty_header_set();

        USBHSD->UEP1_TX_DMA = (uint32_t)UVC_Empty_Buf;
        USBHSD->UEP1_TX_LEN = UVC_HEADER_LEN;
        USBHSD->UEP1_TX_CTRL = (USBHSD->UEP1_TX_CTRL & ~(USBHS_UEP_T_TOG_MASK | USBHS_UEP_T_RES_MASK)) |
                               (USBHS_UEP_T_RES_ACK | USBHS_UEP_T_TOG_DATA0);

        s_pending = 1;
        NVIC_DisableIRQ(DVP_IRQn);
    }
    else
    {
        s_pending = 0;
        UVC_SourceClock(0);
        DVP->CR0 &= ~RB_DVP_ENABLE;
        NVIC_DisableIRQ(DVP_IRQn);
        DVP->CR1 |= (RB_DVP_ALL_CLR | RB_DVP_RCV_CLR);
        DVP->CR1 &= ~(RB_DVP_ALL_CLR | RB_DVP_RCV_CLR);
    }
}

/*******************************************************************************
 * @fn      SS_Endp1_Hander
 *
 * @brief   EP1 IN handler for SS
 *
 * @return  none
 */
void SS_Endp1_Hander(void)
{
    uint32_t adr, len;
    uint8_t eof;

    uvc_pending_service();

    if(uvc_consumer_get(&adr, &len, &eof))
    {
        uvc_header_set(adr, eof);
        USBSSD->EP1_TX.UEP_TX_DMA = adr - UVC_HEADER_LEN;
        if(len % 1024)
        {
            USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = len % 1024;
            USBSSD->EP1_TX.UEP_TX_CHAIN_EXP_NUMP = len / 1024 + 1;
        }
        else
        {
            USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = 1024;
            USBSSD->EP1_TX.UEP_TX_CHAIN_EXP_NUMP = len / 1024;
        }
    }
    else
    {
        uvc_empty_header_set();
        USBSSD->EP1_TX.UEP_TX_DMA = (uint32_t)UVC_Empty_Buf;
        USBSSD->EP1_TX.UEP_TX_CHAIN_LEN = UVC_HEADER_LEN;
        USBSSD->EP1_TX.UEP_TX_CHAIN_EXP_NUMP = 1;
    }
}

/*******************************************************************************
 * @fn      HS_Endp1_ISOHander
 *
 * @brief   Isochronous error callback (unused)
 *
 * @return  none
 */
void HS_Endp1_ISOHander(void)
{
    /* Work is done in HS_Endp1_Hander. */
}

/*******************************************************************************
 * @fn      HS_Endp1_Hander
 *
 * @brief   EP1 IN handler for HS
 *
 * @return  none
 */
void HS_Endp1_Hander(void)
{
    uint32_t adr, len;
    uint8_t eof;

    uvc_pending_service();

    if(uvc_consumer_get(&adr, &len, &eof))
    {
        uvc_header_set(adr, eof);
        USBHSD->UEP1_TX_DMA = adr - UVC_HEADER_LEN;
        USBHSD->UEP1_TX_LEN = len;

        USBHSD->UEP_TX_TOG_AUTO &= ~USBHS_UEP1_T_EN;
        if(len <= 1024)
        {
            USBHSD->UEP1_TX_CTRL = (USBHSD->UEP1_TX_CTRL & ~(USBHS_UEP_T_TOG_MASK | USBHS_UEP_T_RES_MASK)) |
                                   (USBHS_UEP_T_RES_ACK | USBHS_UEP_T_TOG_DATA0);
        }
        else if(len <= 2048)
        {
            USBHSD->UEP1_TX_CTRL = (USBHSD->UEP1_TX_CTRL & ~(USBHS_UEP_T_TOG_MASK | USBHS_UEP_T_RES_MASK)) |
                                   (USBHS_UEP_T_RES_ACK | USBHS_UEP_T_TOG_DATA1);
        }
        else
        {
            USBHSD->UEP1_TX_CTRL = (USBHSD->UEP1_TX_CTRL & ~(USBHS_UEP_T_TOG_MASK | USBHS_UEP_T_RES_MASK)) |
                                   (USBHS_UEP_T_RES_ACK | USBHS_UEP_T_TOG_DATA2);
        }
        USBHSD->UEP_TX_TOG_AUTO |= USBHS_UEP1_T_EN;
    }
    else
    {
        uvc_empty_header_set();
        USBHSD->UEP1_TX_DMA = (uint32_t)UVC_Empty_Buf;
        USBHSD->UEP1_TX_LEN = UVC_HEADER_LEN;
        USBHSD->UEP_TX_TOG_AUTO &= ~USBHS_UEP1_T_EN;
        USBHSD->UEP1_TX_CTRL = (USBHSD->UEP1_TX_CTRL & ~(USBHS_UEP_T_TOG_MASK | USBHS_UEP_T_RES_MASK)) |
                               (USBHS_UEP_T_RES_ACK | USBHS_UEP_T_TOG_DATA0);
        USBHSD->UEP_TX_TOG_AUTO |= USBHS_UEP1_T_EN;
    }
}

/*******************************************************************************
 * @fn      DVP_Hander
 *
 * @brief   DVP row/frame interrupt handler
 *
 * @return  none
 */
void DVP_Hander(void)
{
    if(DVP->IFR & RB_DVP_IF_ROW_DONE)
    {
        DVP->IFR = RB_DVP_IF_ROW_DONE;
        s_yuy2_line++;

        if(s_frame_error)
        {
            DVP->DMA_BUF0 = (uint32_t)UVC_Dummy_Buf;
            DVP->DMA_BUF1 = (uint32_t)UVC_Dummy_Buf;
        }
        else
        {
            s_wr_off[s_cur_arr] += s_line_bytes;

            if((s_yuy2_line >= s_row_num) || ((s_line_bytes & 31u) != 0) ||
               (s_wr_off[s_cur_arr] + s_line_bytes > (s_max_payload - UVC_DMA_ALIGN)))
            {
                uvc_slot_submit(s_cur_arr, s_wr_off[s_cur_arr]);
                s_cur_arr ^= 1;
            }

            if(s_line[s_cur_arr].ring.total >= (uint8_t)(s_ring_depth - 1))
            {
                s_frame_error = 1;
                DVP->DMA_BUF0 = (uint32_t)UVC_Dummy_Buf;
                DVP->DMA_BUF1 = (uint32_t)UVC_Dummy_Buf;
            }
            else
            {
                uvc_rearm_dma();
            }
        }

        /* Last row: set EOF atomically. */
        if(s_yuy2_line >= s_row_num)
        {
            NVIC_DisableIRQ(USBSS_IRQn);
            NVIC_DisableIRQ(USBHS_IRQn);
            s_eof_remaining = (uint8_t)(s_line[0].ring.total + s_line[1].ring.total);
            NVIC_EnableIRQ(USBSS_IRQn);
            NVIC_EnableIRQ(USBHS_IRQn);
        }
    }

    if(DVP->IFR & RB_DVP_IF_FRM_DONE)
    {
        DVP->IFR = RB_DVP_IF_FRM_DONE;
        uvc_partial_flush();

        if(s_frame_error)
        {
            uint8_t ln;

            s_frame_error = 0;
            NVIC_DisableIRQ(USBSS_IRQn);
            NVIC_DisableIRQ(USBHS_IRQn);
            for(ln = 0; ln < 2; ln++)
            {
                uint8_t d = s_line[ln].ring.deal;
                uint8_t l = s_line[ln].ring.load;
                while(d != l)
                {
                    *(volatile uint8_t *)(s_line[ln].desc[d].adr - (UVC_HEADER_LEN - 1)) |= UVC_BFH_ERR;
                    if(++d >= s_ring_depth) d = 0;
                }
            }
            s_eof_remaining = (uint8_t)(s_line[0].ring.total + s_line[1].ring.total);
            NVIC_EnableIRQ(USBSS_IRQn);
            NVIC_EnableIRQ(USBHS_IRQn);
        }
    }

    if(DVP->IFR & RB_DVP_IF_STR_FRM)
    {
        DVP->IFR = RB_DVP_IF_STR_FRM;
        uvc_partial_flush();
        uvc_phase_reset();
        uvc_rearm_dma();
    }

    if(DVP->IFR & RB_DVP_IF_STP_FRM)
    {
        DVP->IFR = RB_DVP_IF_STP_FRM;
    }

    if(DVP->IFR & RB_DVP_IF_FIFO_OV)
    {
        DVP->IFR = RB_DVP_IF_FIFO_OV;
        s_frame_error = 1;
    }
}

/*******************************************************************************
 * @fn      DVP_Stream_Reconfig
 *
 * @brief   Re-apply the geometry after a sensor change
 *
 * @return  none
 */
void DVP_Stream_Reconfig(void)
{
    uint8_t was_en = (DVP->CR0 & RB_DVP_ENABLE) ? 1 : 0;

    if(was_en)
    {
        DVP->CR0 &= ~RB_DVP_ENABLE;
    }

    uvc_set_geometry(Resolution_width, Resolution_height);
    uvc_rearm_dma();

    DVP->IFR = RB_DVP_IF_ROW_DONE | RB_DVP_IF_FRM_DONE | RB_DVP_IF_STR_FRM |
               RB_DVP_IF_STP_FRM | RB_DVP_IF_FIFO_OV;

    if(was_en)
    {
        DVP->CR0 |= RB_DVP_ENABLE;
    }
}

/*******************************************************************************
 * @fn      USART_Send_Byte
 *
 * @brief   Send one byte over USART
 *
 * @return  none
 */
void USART_Send_Byte(u8 t)
{
    while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
    USART_SendData(USART6, t);
}
