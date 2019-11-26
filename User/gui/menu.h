#ifndef __MENU_H__
#define __MENU_H__

#include "evt.h"
#include "key.h"
#include "inputbox.h"
#include "listitem.h"
#include "msgbox.h"
#include "topbar.h"
#include "texture.h"
#include "packet.h"
#include "task.h"

enum {
    MENU_HOME=0,
    MENU_GAIN,
    MENU_MIC,
    MENU_MUSIC,
    MENU_EFFECT,
    MENU_PRESET,
    
    MENU_MAX
};

enum {
    HOME_MUSIC,
    HOME_EFFECT,
    HOME_MIC,
    
    HOME_MAX
};

enum {
    MIC_GAIN,
    MIC_GEQ,
    MIC_PEQ,
    MIC_HPF,
    MIC_LPF,
    MIC_LIMITER,
    MIC_FEEDBACK,
    MIC_NOISEGATE,
    
    MIC_MAX
};

enum {
    EFFECT_GAIN,
    EFFECT_ECHO,
    EFFECT_REVERB,
    
    EFFECT_MAX
};

enum {
    ECHO_HPF,
    ECHO_LPF,
    ECHO_PEQ,
    ECHO_DELAY,
    ECHO_REPEAT,
    ECHO_EFFECT_VOL,
    ECHO_DIRECT_VOL, 
    
    ECHO_MAX
};

enum {
    REVERB_HPF,
    REVERB_LPF,
    REVERB_PEQ,
    REVERB_DELAY,
    REVERB_EFFECT_VOL,
    REVERB_DIRECT_VOL,
    
    REVERB_MAX
};

enum {
    MUSIC_HPF,
    MUSIC_LPF,
    MUSIC_GEQ,
    MUSIC_PEQ,
    MUSIC_INPUT,
    MUSIC_PITCH,
    MUSIC_NOISEGATE,
    
    MUSIC_MAX
};

enum {
    TYPE_SLIDER,
    TYPE_INPUTBOX,
    TYPE_LISTITEM,

    TYPE_MAX
};


typedef struct {
    int (*init)(void *p);
    int (*free)(void *p);
    int (*handle)(void *p, key_t *k);
    int (*refresh)(void *p);
}func_t;


typedef struct _item_{
    u8              type;       //menu type
    const char      *txt;
    struct  _item_  *parent;
}item_t;


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
