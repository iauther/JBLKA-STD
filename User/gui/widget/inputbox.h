#ifndef __INPUT_H__
#define __INPUT_H__

#include "base.h"

typedef struct {
    u8      type;
    rect_t  rect;
    data_t  data;

    u32     flag;
}inputbox_t;

#endif
