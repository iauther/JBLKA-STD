#ifndef __EVT_H__
#define __EVT_H__

#include "key.h"

enum {
    EVT_KEY,
    EVT_DSP,
    //EVT_E2P,

    EVT_HID,
    EVT_RS485,

    EVT_TIMER,
    EVT_REFRESH,
};

typedef struct {
    u8      evt;
    key_t   k;
}evt_dev_t;

typedef struct {
    u8      evt;
    u8      data[64];
}evt_com_t;

typedef struct {
    u32     evt;
    void    *ptr;
    u8      data[32];
}evt_ui_t;

#endif
