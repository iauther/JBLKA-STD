#ifndef __LISTITEM_H__
#define __LISTITEM_H__

#include "base.h"
#include "list.h"
#include "default.h"

enum {
    REFRESH_CLEAR       = 1<<0,
    REFRESH_TITLE       = 1<<1,
    REFRESH_MOVE        = 1<<2,
    REFRESH_FOCUS       = 1<<3,
    REFRESH_VALUE       = 1<<4,
    REFRESH_LIST        = 1<<5,
    REFRESH_TXT_SCROLL  = 1<<6,

    REFRESH_ALL         = 0xffff,
};


typedef struct _listitem {
    cchr    *title;
    rect_t  rect;
    u8      pageItems;
    u8      dispItems;
    
    list_t  *list;
    u8      firstId;
    u8      focusId;
    u8      prev_firstId;
    u8      prev_focusId;
    u32     refreshFlag;
    u8      inEdit;

    void    *data;
    trigger_fn trigger;
    struct _listitem *parent;
    struct _listitem *child;
}listitem_t;




listitem_t *listitem_init(cchr *title, u8 max, int node_size);

int listitem_free(listitem_t **l);

int listitem_reset(listitem_t *l);

listitem_t * listitem_get_parent(listitem_t *l);

int listitem_set_parent(listitem_t *l, listitem_t *parent);

listitem_t * listitem_get_child(listitem_t *l);

int listitem_set_child(listitem_t *l, listitem_t *child);

int listitem_append(listitem_t *l, item_info_t *inf);

item_info_t* listitem_get(listitem_t *l, u8 index);

item_info_t* listitem_get_focus(listitem_t *l);

int listitem_set_refresh(listitem_t *l, u32 flag);

int listitem_refresh(listitem_t *l);

int listitem_clear(void);

int listitem_move(listitem_t *l, u8 dir, u8 size);

int listitem_size(listitem_t *l);

int listitem_set_trigger(listitem_t *l, trigger_fn trigger);

listitem_t* listitem_create(cchr *title, item_info_t *info, void *data, trigger_fn trigger);

int listitem_handle(listitem_t **l, key_t *key);

int listitem_quit(listitem_t *l);

#endif
