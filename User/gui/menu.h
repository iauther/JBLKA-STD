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
    CONTROL_TEXT,
    CONTROL_SLIDER,
    CONTROL_INPUTBOX,

    CONTROL_MAX
};

typedef struct{
    void *(*init)(rect_t *r, void *data);
    int (*free)(void **p);
    int (*handle)(void *p, key_t *key);
    int (*refresh)(void *p);
    int (*clear)(void *p);
}control_func_t;


int menu_init(void);

int menu_free(void);

int menu_clear(void);

int menu_switch(u8 menu);

int menu_refresh(void);

int menu_handle(key_t *key);

/////////////////////////////////
enum {
    REFRESH_HOME_CLEAR = 1<<0,
    REFRESH_HOME_TITLE = 1<<1,
    REFRESH_HOME_NAME = 1<<2,
    REFRESH_HOME_VALUE = 1<<3,

    REFRESH_HOME_ALL = 0xff,
};


int home_clear(void);
int home_refresh(void);
int home_set_key(u8 key);
int home_set_refresh(u16 flag);

extern u8 gM;

#endif
