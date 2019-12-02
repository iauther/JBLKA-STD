#ifndef __INPUTBOX_H__
#define __INPUTBOX_H__

#include "base.h"
#include "default.h"

typedef struct {
    rect_t          rect;
    item_data_t     *dat;
}inputbox_t;

inputbox_t *inputbox_init(rect_t *rect, item_data_t *dat);
int inputbox_free(inputbox_t **i);
int inputbox_handle(inputbox_t *i, u8 key);
int inputbox_show(inputbox_t *i, u16 color, u16 bgcolor);

#endif
