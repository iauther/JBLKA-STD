#ifndef __LISTITEM_H__
#define __LISTITEM_H__

#include "base.h"
#include "list.h"

enum {
    REFRESH_TITLE       = 1<<0,
    REFRESH_FOCUS       = 1<<1,
    REFRESH_LIST        = 1<<2,
    REFRESH_TXT_SCROLL  = 1<<3,

    REFRESH_ALL         = 0xff,
};

typedef struct _listitem {
    u8      control;
    rect_t  rect;
    u8      imax;
    cchr    *title;
    
    list_t  *list;
    u8      firstId;
    u8      focusId;
    u8      prev_firstId;
    u8      prev_focusId;
    u32     refreshFlag;

    struct _listitem *parent;
    struct _listitem *child;
}listitem_t;

listitem_t *listitem_init(cchr *title, rect_t *rect, u8 max, int node_size);

int listitem_free(listitem_t **l);

int listitem_reset(listitem_t *l);

listitem_t * listitem_get_parent(listitem_t *l);

int listitem_set_parent(listitem_t *l, listitem_t *parent);

listitem_t * listitem_get_child(listitem_t *l);

int listitem_set_child(listitem_t *l, listitem_t *child);

int listitem_add(listitem_t *l, node_t *n);

int listitem_get(listitem_t *l, u8 index, node_t *n);

int listitem_get_focus(listitem_t *l, node_t *n);

int listitem_set_refresh(listitem_t *l, u32 flag);

int listitem_refresh(listitem_t *l);

int listitem_clear(listitem_t *l);

int listitem_move(listitem_t *l, int dir);

int listitem_handle(listitem_t *l, u8 key);

#endif
