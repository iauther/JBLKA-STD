#ifndef __EVT_H__
#define __EVT_H__

#include "key.h"
#include "packet.h"

enum {
    EVT_KEY,
    EVT_DSP,

    EVT_HID,
    EVT_E2P,
    EVT_RS485,
    EVT_TUNE,

    EVT_TIMER,
    EVT_REFRESH,
};

typedef struct {
    u8              evt;
    key_t           key;
}evt_gui_t;

typedef struct {
    u8              evt;
    packet_t        pkt;
}evt_dev_t;

typedef struct {
    u8              evt;
    union {
        u8          hid[64];
    }param;
}evt_com_t;

typedef struct {
    u8              evt;
}evt_eep_t;

#endif
