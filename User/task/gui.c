#include "task.h"
#include "menu.h"

#define MSG_MAX         6

#ifdef RTX
extern u8 adc_key_using;
extern paras_data_t gParams;
msg_t *gui_msg=NULL;
static void key_proc(key_t *key)
{
    switch(key->value) {
        case KEY_MUSIC:
        menu_switch(MENU_MUSIC);
        break;

        case KEY_MIC:
        menu_switch(MENU_MIC);
        break;

        case KEY_EFFECT:
        menu_switch(MENU_EFFECT);
        break;

        case KEY_PRESET:
        menu_switch(MENU_PRESET);
        break;
    
        case KEY_SHARP:      //#
        case KEY_0:          //
        case KEY_b:
        {
            int r;node_t n;
            r = dsp_set_pitch(key->value, NULL, &n);
            if(r==0) e2p_put(&n);
        }
        break;

        default:
        menu_handle(key);
        break;
    }

    adc_key_using = 0;
}
static void ir_proc(key_t *key)
{
    int r;
    s16 g;
    node_t n;

    switch(key->value) {
        case KEY_LOCK:
        {
            //
        }
        break;

        case KEY_INPUT:
        r = dsp_set_input(NULL, &n);
        break;

        case KEY_MUTE:
        r = dsp_set_mute(NULL, &n);
        break;

#if 0        
        //player key
        case KEY_PREV:
        case KEY_NEXT:
        case KEY_PLAYPAUSE:
        case KEY_ONELOOP:
        {
            //
        }
        break;
#endif
        
        case KEY_MODE:
        case KEY_M1:
        case KEY_M2:
        case KEY_M3:
        r = dsp_set_preset(key->value, NULL, &n);
        break;
        
        case KEY_SHARP:      //#
        case KEY_0:          //
        case KEY_b:
        r = dsp_set_pitch(key->value, NULL, &n);
        break;
        
        case KEY_MUSIC_UP:
        case KEY_MUSIC_DN:
        case KEY_EFFECT_UP:
        case KEY_EFFECT_DN:
        case KEY_MIC_UP:
        case KEY_MIC_DN:
        r = dsp_gain_step(key->value, 1, &g, &n);
        break;

        default:
        return;  
    }

    if(r==0) e2p_put(&n);
}
static void knob_proc(key_t *key)
{
    int r;
    s16 g;
    u8 tmp[20];
    node_t n;

    r = dsp_gain_step(key->value, key->times, &g, &n);
    if(r) {
        return;
    }

    if(gM!=MENU_HOME) {
        gM = MENU_HOME;
        menu_refresh();
    }
    
    sprintf((char*)tmp, "%d", g);
    lcd_draw_string_align(0, 100, LCD_WIDTH, 60, tmp, FONT_32, LCD_FC, LCD_BC, ALIGN_MIDDLE, 0);
    e2p_put(&n);
}


void gui_task(void *arg)
{
    int r;
    evt_gui_t e;
    osStatus_t st;

    topbar_init();
    menu_init();

    gui_msg = msg_init(MSG_MAX, sizeof(e));
    if(!gui_msg) {
        return;
    }

    while(1) {
        r = msg_recv(gui_msg, &e, sizeof(e));
        if(r==0) {
            switch(e.evt) {
                
                case EVT_KEY:
                {
                    key_t *k=&e.key;
                    switch(k->src) {
                        
                        case SRC_IR:
                        ir_proc(&e.key);
                        break;

                        case SRC_KEY:
                        key_proc(&e.key);
                        break;
                    
                        case SRC_KNOB:
                        knob_proc(&e.key);
                        break;
                    }
                }
                break;

                case EVT_REFRESH:
                {
                    menu_refresh();
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

void gui_post_refresh(void)
{
    evt_gui_t e;
    e.evt = EVT_REFRESH;
    msg_post(gui_msg, &e, sizeof(e));
}

#endif




