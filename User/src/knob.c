#include "knob.h"
#include "key.h"
#include "usart.h"
#include "config.h"

u8 knobKey=0;
u8 keyPool[40];
u16 keyTimes[40];

typedef struct {
    u8      code;
    u8      key;
}knob_info_t;

const knob_info_t knobTab[] = {

    {0x03,  KEY_MUSIC_UP},
    {0x13,  KEY_MUSIC_DN},
    {0x12,  KEY_EFFECT_UP},
    {0x02,  KEY_EFFECT_DN},
    {0x11,  KEY_MIC_UP},
    {0x01,  KEY_MIC_DN},

    {0x14,  KEY_MUSIC_TREBLE_UP},
    {0x04,  KEY_MUSIC_TREBLE_DN},
    {0x15,  KEY_MUSIC_BASS_UP},
    {0x05,  KEY_MUSIC_BASS_DN},

    {0x16,  KEY_ECHO_LEVEL_UP},
    {0x06,  KEY_ECHO_LEVEL_DN},
    {0x17,  KEY_ECHO_DELAY_UP},
    {0x07,  KEY_ECHO_DELAY_DN},
    {0x18,  KEY_ECHO_REPEAT_UP},
    {0x08,  KEY_ECHO_REPEAT_DN},

    {0x19,  KEY_REVERB_LEVEL_UP},
    {0x09,  KEY_REVERB_LEVEL_DN},
    {0x1a,  KEY_REVERB_TIME_UP},
    {0x0a,  KEY_REVERB_TIME_DN},
    
    {0x1b,  KEY_MIC_TREBLE_UP},
    {0x0b,  KEY_MIC_TREBLE_DN},
    {0x1c,  KEY_MIC_MIDDLE_UP},
    {0x0c,  KEY_MIC_MIDDLE_DN},
    {0x1d,  KEY_MIC_BASS_UP},
    {0x0d,  KEY_MIC_BASS_DN},

    {0xff,  0},
};

static void keyPool_init(void)
{
    u8 i;
    for(i=0;; i++) {
        if(knobTab[i].code==0xff) {
            break;
        }
        
        keyTimes[i] = 0;
        keyPool[knobTab[i].code] = knobTab[i].key;
    }
}

static void knob_rx_cb(u8 *data, u16 data_len)
{
    
    keyTimes[knobKey]++;
#if 0
{
    evt_t e;
    e.evt = EVT_KEY;
    ui_send_evt(&e);
}
#endif
}
int knob_init(void)
{
    uart_paras_t para={knob_rx_cb, &knobKey, sizeof(knobKey)};
    
    keyPool_init();
    return usart_init(KNOB_UART, &para);
}


u8 knob_get_key(u16 *times)
{
    u8 key=keyPool[knobKey];
    
    if(times) *times=keyTimes[knobKey];
    keyTimes[knobKey] = 0;
    knobKey = 0;

    return key;
}