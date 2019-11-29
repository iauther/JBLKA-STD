#ifndef __MENU_H__
#define __MENU_H__

#include "evt.h"
#include "key.h"
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
    TYPE_NONE=0,
    TYPE_LIST,
    TYPE_DATA,

    TYPE_MAX
};

typedef struct {
    int (*init)(void *p);
    int (*free)(void *p);
    int (*handle)(void *p, u8 key);
    int (*refresh)(void *p);
}func_t;

typedef struct _item_{
    u8              type;
    cchr            *txt;
    u8              cmd;       //dsp cmd id
    void            *data;
    listitem_t      *sub;
}item_data_t;

typedef struct {
    item_data_t items[VAR_MAX];
}dsp_item_t;


#define DEF_FUNC(XX) \
        int XX##_init(void *p); \
        int XX##_free(void *p); \
        int XX##_handle(void *p, u8 key); \
        int XX##_refresh(void *p)
DEF_FUNC(home);
DEF_FUNC(mic);
DEF_FUNC(music);
DEF_FUNC(effect);
DEF_FUNC(presets);


int menu_init(void);

int menu_clear(void);

int menu_refresh(void);

int menu_handle(u8 key);



extern u8 gMenu;

#endif
