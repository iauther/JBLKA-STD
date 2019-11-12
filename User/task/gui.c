#include "task.h"

#define MSG_MAX         6

#ifdef RTX
msg_t *gui_msg=NULL;
static void key_proc(u8 key)
{
    switch(key) {
        case KEY_UP:
        break;
        
        case KEY_DOWN:
        break;

        case KEY_MUSIC:
        break;

        case KEY_MIC:
        break;

        case KEY_EFFECT:
        break;

        case KEY_ENTER:
        break;

        case KEY_EXIT:
        break;

        case KEY_SAVE:
        break;

        case KEY_PRESET:
        break;
    }
}
static void ir_proc(u8 key)
{
    int r;
    s16 g;
    node_t n;

    switch(key) {
        case KEY_LOCK:
        break;
        case KEY_INPUT:
        break;

        case KEY_MUTE:  
        break;
        
        //player key
        case KEY_PREV:
        case KEY_NEXT:
        case KEY_PLAYPAUSE:
        case KEY_ONELOOP:
        //do nothing
        break;
        
        case KEY_MODE:
        case KEY_M1:
        case KEY_M2:
        case KEY_M3:
        break;
        
        case KEY_SHARP:      //#
        case KEY_0:          //
        case KEY_b:
        break;
        
        case KEY_MUSIC_UP:
        case KEY_MUSIC_DN:
        case KEY_EFFECT_UP:
        case KEY_EFFECT_DN:
        case KEY_MIC_UP:
        case KEY_MIC_DN:
        r = dsp_gain_step(key, 1, &g, &n);
        break;

        default:
        return;  
    }

    if(r==0) {
        e2p_put(&n);
    }
}
static void knob_proc(u8 key)
{
    int r;
    s16 g;
    node_t n;

    r = dsp_gain_step(key, 1, &g, &n);
    if(r==0) {
        e2p_put(&n);
    }
}



void gui_task(void *arg)
{
    int r;
    evt_gui_t e;
    osStatus_t st;

    //menu_init();
    gui_msg = msg_init(MSG_MAX, sizeof(e));
    while(1) {
        r = msg_recv(gui_msg, &e, NULL);
        if(r==0) {
            switch(e.evt) {
                
                case EVT_KEY:
                {
                    key_t *k = &e.key;
                    switch(k->src) {
                        
                        case SRC_IR:
                        ir_proc(k->value);
                        break;

                        case SRC_KEY:
                        key_proc(k->value);
                        break;
                    
                        case SRC_KNOB:
                        knob_proc(k->value);
                        break;
                    }
                    //menu_handle(k);
                }
                break;

                case EVT_REFRESH:
                {
                    //menu_refresh();
                }
                break;
            }
        }
    }

    msg_free(&gui_msg);
}


void gui_post_evt(evt_gui_t *e)
{
    msg_post(gui_msg, e, sizeof(*e));
}

#endif




