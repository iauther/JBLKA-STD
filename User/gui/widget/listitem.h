#ifndef __LISTITEM_H__
#define __LISTITEM_H__

#include "base.h"
#include "list.h"

typedef struct {
    s8          id;
    s8          *txt;
    void        *data;
}element_t; 

typedef struct {
    u8      type;
    rect_t  rect;
    
    list_t  *list;
    u8      focusId;
    u32     refreshFlag;
}listitem_t;

listitem_t *listitem_init(rect_t *rect, u8 max);

int listitem_free(listitem_t *l);

int listitem_add(listitem_t *l, element_t *e);

int listitem_set_refresh(listitem_t *l, u32 flag);

int listitem_refresh(listitem_t *l);

int listitem_handle(listitem_t *l, key_t *k);

#endif
