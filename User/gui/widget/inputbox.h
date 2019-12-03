#ifndef __INPUTBOX_H__
#define __INPUTBOX_H__

#include "base.h"
#include "default.h"

typedef struct {
    rect_t          rect;
    u8              index;
    u8              cmd;
    const para_info_t *info;
    s16             *data;
}inputbox_t;

inputbox_t *inputbox_init(const item_info_t *info, u8 index);
int inputbox_free(inputbox_t **i);
int inputbox_handle(inputbox_t *i, key_t key);
int inputbox_show(inputbox_t *i, rect_t *rect, u16 color, u16 bgcolor);

#endif
