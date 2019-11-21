#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "types.h"
#include "base.h"

typedef struct {
    s8          *title;
    s8          *txt;
    rect_t      rect;
    s8          sec;   //-1: not diappear
}msgbox_t;

#endif
