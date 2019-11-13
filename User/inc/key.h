#ifndef __KEY_H__
#define __KEY_H__

#include "types.h"

enum {
    KEY_NONE,

    //panel key
    KEY_UP,
    KEY_DOWN,
    KEY_MIC,
    KEY_MUSIC,
    KEY_EFFECT,
    KEY_ENTER,
    KEY_EXIT,
    KEY_SAVE,
    KEY_PRESET,
    //panel key end
    
    //remote key
    KEY_LOCK,
    KEY_INPUT,
    KEY_MUTE,
    
    KEY_PREV,
    KEY_NEXT,
    KEY_PLAYPAUSE,
    KEY_ONELOOP,
    
    KEY_MODE,
    KEY_M1,
    KEY_M2,
    KEY_M3,
    
    KEY_SHARP,      //#
    KEY_0,          //
    KEY_b,
    //remote key end
    
    //knob key
    KEY_MUSIC_UP,
    KEY_MUSIC_DN,
    KEY_EFFECT_UP,
    KEY_EFFECT_DN,
    KEY_MIC_UP,
    KEY_MIC_DN,

    KEY_MUSIC_TREBLE_UP,
    KEY_MUSIC_TREBLE_DN,
    KEY_MUSIC_BASS_UP,
    KEY_MUSIC_BASS_DN,

    KEY_ECHO_LEVEL_UP,
    KEY_ECHO_LEVEL_DN,
    KEY_ECHO_DELAY_UP,
    KEY_ECHO_DELAY_DN,
    KEY_ECHO_REPEAT_UP,
    KEY_ECHO_REPEAT_DN,

    KEY_REVERB_LEVEL_UP,
    KEY_REVERB_LEVEL_DN,
    KEY_REVERB_TIME_UP,
    KEY_REVERB_TIME_DN,
    
    KEY_MIC_TREBLE_UP,
    KEY_MIC_TREBLE_DN,
    KEY_MIC_MIDDLE_UP,
    KEY_MIC_MIDDLE_DN,
    KEY_MIC_BASS_UP,
    KEY_MIC_BASS_DN,
    //knob key end
};

enum {  //up or down
    UP,
    DOWN,
};

enum {  //key src
    SRC_IR,
    SRC_KEY,
    SRC_KNOB,
};


typedef struct {
    u8      src;
    u8      value;
    u8      longPress;
    u8      updown;       //UP or DOWN
    u16     times;
}key_t;

#endif
