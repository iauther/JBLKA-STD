#ifndef __TOPBAR_H__
#define __TOPBAR_H__

#include "base.h"
#include "packet.h"

enum {
    TOPBAR_REFRESH_BT     = 1<<0,
    TOPBAR_REFRESH_TEMP   = 1<<1,
    TOPBAR_REFRESH_INPUT  = 1<<2,
    TOPBAR_REFRESH_PRESET = 1<<3,

    TOPBAR_REFRESH_ALL    = 0xff,
};


int topbar_init(u16 h);

int topbar_free(void);

int topbar_set_title(s8 *title);

int topbar_set_refresh(u32 flag);

int topbar_refresh(void);

#endif
