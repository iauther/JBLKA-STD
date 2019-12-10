#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "types.h"
#include "base.h"
#ifdef RTX
#include "cmsis_os2.h"
#endif

enum {
    REFRESH_MSGBOX_CLEAR        = 1<<0,
    REFRESH_MSGBOX_TITLE        = 1<<1,
    REFRESH_MSGBOX_MESSAGE      = 1<<2,
    REFRESH_MSGBOX_BUTTONS      = 1<<3,

    REFRESH_MSGBOX_ALL          =0xffff,
};

enum {
    MSGBOX_NO,
    MSGBOX_YES,

    MSGBOX_MAX
};


typedef struct {
    char        *title;
    char        *message;
    rect_t      rect;
    s8          sec;   //-1: not diappear
    u8          show;
    u8          focusId;
    u16         refreshFlag;
    trigger_fn  trigger;
#ifdef RTX
    osTimerId_t  tmr;
#endif
}msgbox_t;

int msgbox_is_show(void);

int msgbox_get_focus(void);

int msgbox_clear(void);

int msgbox_show(char *title, char *message, s8 sec, trigger_fn trigger);

int msgbox_refresh(void);

int msgbox_set_refresh(u16 flag);

int msgbox_handle(key_t *key);

#endif
