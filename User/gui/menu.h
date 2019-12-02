#ifndef __MENU_H__
#define __MENU_H__

#include "evt.h"
#include "key.h"
#include "gbl.h"
#include "queue.h"
#include "default.h"
#include "msgbox.h"
#include "topbar.h"
#include "texture.h"
#include "inputbox.h"
#include "listitem.h"
#include "packet.h"
#include "task.h"

enum {
    MENU_HOME=0,
    MENU_MIC,
    MENU_MUSIC,
    MENU_EFFECT,
    MENU_PRESET,
    
    MENU_MAX
};

enum {
    CONTROL_NONE=0,
    CONTROL_LIST,
    CONTROL_SLIDER,
    CONTROL_INPUTBOX,

    CONTROL_MAX
};

typedef struct{
    void *(*init)(rect_t *r, void *data);
    int (*free)(void **p);
    int (*handle)(void *p, u8 key);
    int (*refresh)(void *p);
    int (*clear)(void *p);
}control_func_t;

typedef struct{
    int (*init)(void);
    int (*free)(void);
    int (*handle)(u8 key);
    int (*refresh)(void);
    int (*set_refresh)(u32 flag);
}menu_func_t;

#define DEF_MENU_FUNC(XX) \
    int XX##_init(void); \
    int XX##_free(void); \
    int XX##_handle(u8 key); \
    int XX##_refresh(void); \
    int XX##_set_refresh(u32 flag);

#define DEF_CONTROL_FUNC(XX) \
    int XX##_init(rect_t *r, void *data); \
    int XX##_free(void **p); \
    int XX##_handle(void *p, u8 key); \
    int XX##_refresh(void *p);

DEF_MENU_FUNC(home);
DEF_MENU_FUNC(mic);
DEF_MENU_FUNC(music);
DEF_MENU_FUNC(effect);
DEF_MENU_FUNC(presets);

#define MAKE_FUNC(XX) \
    {\
        XX##_init, \
        XX##_free, \
        XX##_handle, \
        XX##_refresh, \
        XX##_set_refresh, \
    }

typedef struct _item_t{
    u8              control;
    void            *handle;
}item_t;

typedef struct {
    listitem_t  *l;
    queue_t     *q;
}menu_t;


int menu_init(void);

int menu_free(void);

int menu_clear(void);

int menu_switch(u8 menu);

int menu_refresh(void);

int menu_handle(u8 key);

int menu_add_item(listitem_t *l, u8 menu);

extern u8 gM;

#endif
