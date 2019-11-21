#include "knob.h"
#include "key.h"
#include "tmr.h"
#include "queue.h"
#include "usart.h"
#include "task.h"
#include "config.h"

#define KQ_MAX      10


u8 knobValue=0;
u8 keyPool[40];
u16 keyTimes[40];
queue_t *kq=NULL;

typedef struct {
    u8      code;
    u8      key;
}knob_info_t;

const knob_info_t knobTab[] = {

    {0x03,  KEY_MUSIC_DN},
    {0x13,  KEY_MUSIC_UP},
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


static int kfind(queue_t *q, int index, node_t *n, node_t *n2)
{
    key_t *k1=(key_t*)n->ptr, *k2=(key_t*)n2->ptr;

    if(k1->value==k2->value) {
        k1->times += k2->times;
        return index;
    }

    return -1;
}
static void knob_rx_cb(u8 *data, u16 data_len)
{
    keyTimes[knobValue]++;

#ifdef RTX
    {
        key_t k;
        node_t n;

        k.src = SRC_KNOB;
        k.value = keyPool[knobValue];
        k.times = keyTimes[knobValue];
        k.updown = 0;
        k.longPress = 0;

        n.ptr = &k;
        n.len = sizeof(k);
        queue_put(kq, &n, kfind);
        keyTimes[knobValue] = 0;
    }
#endif
}
static void knob_tmr_cb(void)
{
#ifdef RTX
    {
        int r;
        node_t n;
        evt_gui_t e={0};

        e.evt = EVT_KEY;
        n.ptr = &e.key;
        n.len = sizeof(e.key);
        r = queue_get(kq, &n, NULL);
        if(r==0) {
            gui_post_evt(&e);
        }
    }
#endif
}

int knob_init(void)
{
    uart_paras_t para={knob_rx_cb, &knobValue, sizeof(knobValue)};
    
    keyPool_init();

#ifdef RTX
    kq = queue_init(KQ_MAX, sizeof(key_t));
    tim_init(TIMER4, 50, knob_tmr_cb);
#endif

    return usart_init(KNOB_UART, &para);
}


u8 knob_get_key(u16 *times)
{
    u8 kv=knobValue;
    
    if(keyPool[kv]==KEY_NONE || keyTimes[kv]==0) {
        return KEY_NONE;
    }

    if(times) *times=keyTimes[kv];

    keyTimes[kv] = 0;
    knobValue = 0;

    return keyPool[kv];
}

