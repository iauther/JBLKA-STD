#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "base.h"

typedef struct {
    u8      font;
    COLOR   color;
    s8      *str;

    u8      inedit;
}texture_t;

#endif
