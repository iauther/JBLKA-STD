#include "task.h"

#define MSG_MAX         8


osMessageQueueId_t ui_mq;
static void panel_key_proc(u8 key)
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
static void remote_key_proc(u8 key)
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
        if(r==0) {
            //queue_put(e2p_q, &n, 1);
        }
        break;

        default:
        return;  
    }

    if(r==0) {
        //queue_put(e2p_q, &n, 1);
    }
}
static void knob_key_proc(u8 key)
{
    int r;
    s16 g;
    node_t n;

    r = dsp_gain_step(key, 1, &g, &n);
    if(r==0) {
        //queue_put(e2p_q, &n, 1);
    }
}



void ui_task(void *arg)
{
    evt_dev_t e;
    key_t     *k=&e.k;
    osStatus_t st;

    //menu_init();

    ui_mq = osMessageQueueNew(MSG_MAX, sizeof(evt_dev_t), NULL);
    while(1) {
        st = osMessageQueueGet(ui_mq, &e, NULL, osWaitForever);
        if(st==osOK) {
            switch(e.evt) {
                
                case EVT_KEY:
                {
                    switch(k->type) {
                        case PANEL:
                        panel_key_proc(k->key);
                        break;

                        case REMOTE:
                        remote_key_proc(k->key);
                        break;
                    
                        case KNOB:
                        knob_key_proc(k->key);
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

    osMessageQueueDelete(ui_mq);
}


void ui_post_evt(evt_ui_t *e)
{
    osMessageQueuePut(ui_mq, e, NULL, 0);
}






