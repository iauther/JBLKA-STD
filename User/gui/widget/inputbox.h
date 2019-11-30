#ifndef __INPUT_H__
#define __INPUT_H__

#include "base.h"
#include "default.h"

typedef struct {
    u8          type;
    rect_t      rect;
    data_t      data;

    u32         flag;
}inputbox_t;

inputbox_t *inputbox_init(rect_t *rect, para_item_t *para);
int inputbox_free(inputbox_t **i);
int inputbox_handle(inputbox_t *i, u8 key);
int inputbox_refresh(inputbox_t *i);

#endif
