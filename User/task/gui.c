#include "task.h"
#include "menu.h"

#define MSG_MAX         6

#ifdef RTX
extern paras_data_t gParams;
msg_t *gui_msg=NULL;
static void key_proc(u8 key)
{
    switch(key) {
        case KEY_MUSIC:
        if(gMenu!=MENU_MUSIC) {
            gMenu = MENU_MUSIC;
            menu_refresh();
        }
        break;

        case KEY_MIC:
        if(gMenu!=MENU_MIC) {
            gMenu = MENU_MIC;
            menu_refresh();
        }
        break;

        case KEY_EFFECT:
        if(gMenu!=MENU_EFFECT) {
            gMenu = MENU_EFFECT;
            menu_refresh();
        }
        break;

        case KEY_PRESET:
        if(gMenu!=MENU_PRESET) {
            gMenu = MENU_PRESET;
            menu_refresh();
        }
        break;

        default:
        menu_handle(key);
        //menu_refresh();
        break;
    }
}
static void ir_proc(u8 key)
{
    int r;
    s16 g;
    node_t n;
    dsp_data_t dd={0};
    Dsp_Paras *dsp=&gParams.dsp;

    switch(key) {
        case KEY_LOCK:
        {
            //
        }
        break;

        case KEY_INPUT:
        {
            dsp->Array_Input.input = (dsp->Array_Input.input+1)%INPUT_MAX;
            sys_set_input(dsp->Array_Input.input);
            dd.id = CMD_ID_Input;
            dd.dlen = sizeof(TypeS_Input);
            r = dsp_send(&dd);
            n.ptr = &dsp->Array_Input;
            n.len = sizeof(TypeS_Input);
        }
        break;

        case KEY_MUTE:
        {
            u8 i;
            dd.id = CMD_ID_Mute;
            for(i=0; i<Mute_CH_NUM; i++) {
                dd.ch = i;
                dd.dlen = sizeof(TypeS_Mute);
                r = dsp_send(&dd);
            }
            n.ptr = dsp->Array_Mute;
            n.len = sizeof(dsp->Array_Mute);
        } 
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
        {
            if(key==KEY_MODE) {
                gParams.pre = (gParams.pre+1)%PRESET_MAX;
            }
            else {
                gParams.pre = key-KEY_M1;
            }
            
            paras_read_preset(gParams.pre, dsp);
            r = dsp_download();
            n.ptr = &gParams.pre;
            n.len = sizeof(gParams.pre);
        }
        break;
        
        case KEY_SHARP:      //#
        case KEY_0:          //
        case KEY_b:
        if(key==KEY_SHARP) {
            dsp->Array_PitchShift.PitchShift = (dsp->Array_PitchShift.PitchShift+1)%5;
        }
        else if(key==KEY_0) {
            dsp->Array_PitchShift.PitchShift = 0;
        }
        else {
            dsp->Array_PitchShift.PitchShift = (dsp->Array_PitchShift.PitchShift-1)%5;
        }
        dd.id = CMD_ID_PitchShift;
        dd.dlen = sizeof(TypeS_PitchShift);
        r = dsp_send(&dd);
        n.ptr = &dsp->Array_PitchShift;
        n.len = sizeof(dsp->Array_PitchShift);
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
static void knob_proc(u8 key, u16 times)
{
    int r;
    s16 g;
    u8 tmp[20];
    node_t n;

    r = dsp_gain_step(key, 1, &g, &n);
    if(r) {
        return;
    }

    if(gMenu!=MENU_HOME) {
        gMenu = MENU_HOME;
        menu_refresh();
    }
    
    sprintf((char*)tmp, "%d", g);
    lcd_draw_string_align(0, 100, LCD_WIDTH, 60, tmp, FONT_32, LCD_FC, LCD_BC, ALIGN_MIDDLE);

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
                    key_t *k = &e.key;
                    switch(k->src) {
                        
                        case SRC_IR:
                        ir_proc(k->value);
                        break;

                        case SRC_KEY:
                        key_proc(k->value);
                        break;
                    
                        case SRC_KNOB:
                        knob_proc(k->value, k->times);
                        break;
                    }
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




