#include "task.h"
#include "menu.h"

#define MSG_MAX         20


#ifdef RTX
extern u8 adc_key_using;
extern paras_data_t gParams;
msg_t *gui_msg=NULL;




static void key_proc(key_t *key, dsp_info_t *info)
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
            int r;
            r = dsp_set_pitch(key->value, info);
            if(r==0) e2p_put(&info->node);
        }
        break;

        default:
        menu_handle(key);
        break;
    }

    adc_key_using = 0;
}
static void ir_proc(key_t *key, dsp_info_t *info)
{
    int r;
    u8  m;

    switch(key->value) {
        case KEY_LOCK:
        {
            //
        }
        break;

        case KEY_INPUT:
        r = dsp_set_input(info);
        break;

        case KEY_MUTE:
        r = dsp_set_mute(info, &m);
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
        r = dsp_set_preset(key->value, info);
        break;
        
        case KEY_SHARP:      //#
        case KEY_0:          //
        case KEY_b:
        r = dsp_set_pitch(key->value, info);
        break;
        
        case KEY_MUSIC_UP:
        case KEY_MUSIC_DN:
        case KEY_EFFECT_UP:
        case KEY_EFFECT_DN:
        case KEY_MIC_UP:
        case KEY_MIC_DN:
        r = dsp_set_gain(key->value, key->times, info);
        break;

        default:
        return;  
    }

    if(r==0) e2p_put(&info->node);
}



static void knob_proc(key_t *key, dsp_info_t *info)
{
    int r;
    node_t n;

    r = dsp_set_gain(key->value, key->times, info);
    if(r) {
        return;
    }

    menu_switch(MENU_HOME);
    home_set_key(key->value);
    e2p_put(&info->node);
}

static void draw_tune(void)
{
    rect_t r=BODY_RECT;
    char *txt="PC IS TUNNING!";

    lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_BC);
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)txt, FONT_24, LCD_FC, LCD_BC, ALIGN_MIDDLE);
}

void gui_task(void *arg)
{
    int r;
    evt_gui_t e;
    osStatus_t st;
    dsp_info_t info;

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
                    dsp_get_info(k->value, &info);
                    switch(k->src) {
                        
                        case SRC_IR:
                        ir_proc(&e.key, &info);
                        break;

                        case SRC_KEY:
                        key_proc(&e.key, &info);
                        break;
                    
                        case SRC_KNOB:
                        knob_proc(&e.key, &info);
                        break;
                    }
                }
                break;

                case EVT_TUNE:
                {
                    extern int pc_is_tuning(void);
                    if(pc_is_tuning()) {
                        gM = MENU_HOME;
                        draw_tune();
                    }
                    else {
                        menu_refresh();
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




