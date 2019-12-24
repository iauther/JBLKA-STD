#include "menu.h"
#include "default.h"

static u16 prevLength=0;
static u8 curKey=KEY_NONE;
static u16 refreshFlag=REFRESH_HOME_ALL;
static void draw_title(rect_t rect, dsp_info_t *info)
{
    rect_t r=rect;

    r.y +=30;
    r.h = 40;
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)info->title, FONT_24, LCD_FC, LCD_BC, ALIGN_MIDDLE);
    
    r.y = 190;
    r.h = 20;
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)info->name, FONT_24, YELLOW, LCD_BC, ALIGN_MIDDLE);
}


static void draw_value(rect_t rect, dsp_info_t *info)
{
    int len;
    u8 tmp[20];
    u8 font;
    rect_t r=rect;
    font_info_t inf1,inf2;
    para_info_t *pinf;

    if(info->dsp.id==CMD_ID_Delay) {
        pinf = (para_info_t*)&DELAY_INFO[info->dsp.ch];
    }
    else {
        pinf = (para_info_t*)&PARA_INFO[info->dsp.id].info[info->index];
    }

    r.y = 80;
    r.h = 100;
    
    font = FONT_96;
    inf1 = font_info(font);
    if(pinf->flt) {
        sprintf((char*)tmp, "%.1f", (f32)(*info->pdata)/pinf->div);
    }
    else {
        sprintf((char*)tmp, "%d", (*info->pdata)/pinf->div);
    }
    
    len = inf1.width*strlen((char*)tmp);
    if(len<prevLength) {
        lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_BC);
    }
    

    lcd_draw_string_align(r.x, r.y, r.w, r.h, tmp, font, LCD_FC, LCD_BC, ALIGN_MIDDLE);

    font = FONT_24;
    inf2 = font_info(font);
    r.x = r.x+r.w/2+len/2+10;
    r.y = r.y+r.h/2+inf1.height/2-40;
    r.h = inf2.height;

    if(prevLength!=len) {
        lcd_fill_rect(r.x-10, r.y, 10, r.h, LCD_BC);
    }
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)pinf->unit, font, LCD_FC, LCD_BC, ALIGN_LEFT);
    prevLength = len;
}


static int draw_clear(rect_t r)
{
    prevLength = 0;
    lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_BC);
    return 0;
}

static int need_refresh_all(u8 key)
{
    int flag=1;

    if(key != curKey) {
        switch(key) {
            case KEY_MUSIC_UP:
            case KEY_MUSIC_DN:
            if((key==KEY_MUSIC_UP && curKey==KEY_MUSIC_DN) || (key==KEY_MUSIC_DN && curKey==KEY_MUSIC_UP)) {
                flag = 0;
            }
            break;

            case KEY_EFFECT_UP:
            case KEY_EFFECT_DN:
            if((key==KEY_EFFECT_UP && curKey==KEY_EFFECT_DN) || (key==KEY_EFFECT_DN && curKey==KEY_EFFECT_UP)) {
                flag = 0;
            }
            break;

            case KEY_MIC_UP:
            case KEY_MIC_DN:
            if((key==KEY_MIC_UP && curKey==KEY_MIC_DN) || (key==KEY_MIC_DN && curKey==KEY_MIC_UP)) {
                flag = 0;
            }
            break;
            
            case KEY_MUSIC_TREBLE_UP:
            case KEY_MUSIC_TREBLE_DN:
            if((key==KEY_MUSIC_TREBLE_UP && curKey==KEY_MUSIC_TREBLE_DN) || (key==KEY_MUSIC_TREBLE_DN && curKey==KEY_MUSIC_TREBLE_UP)) {
                flag = 0;
            }
            break;
            
            case KEY_MUSIC_BASS_UP:
            case KEY_MUSIC_BASS_DN:
            if((key==KEY_MUSIC_BASS_UP && curKey==KEY_MUSIC_BASS_DN) || (key==KEY_MUSIC_BASS_DN && curKey==KEY_MUSIC_BASS_UP)) {
                flag = 0;
            }
            break;
            
            case KEY_MIC_TREBLE_UP:
            case KEY_MIC_TREBLE_DN:
            if((key==KEY_MIC_TREBLE_UP && curKey==KEY_MIC_TREBLE_DN) || (key==KEY_MIC_TREBLE_DN && curKey==KEY_MIC_TREBLE_UP)) {
                flag = 0;
            }
            break;

            case KEY_MIC_MIDDLE_UP:
            case KEY_MIC_MIDDLE_DN:
            if((key==KEY_MIC_MIDDLE_UP && curKey==KEY_MIC_MIDDLE_DN) || (key==KEY_MIC_MIDDLE_DN && curKey==KEY_MIC_MIDDLE_UP)) {
                flag = 0;
            }
            break;

            case KEY_MIC_BASS_UP:               //-24~+12dB
            case KEY_MIC_BASS_DN:
            if((key==KEY_MIC_BASS_UP && curKey==KEY_MIC_BASS_DN) || (key==KEY_MIC_BASS_DN && curKey==KEY_MIC_BASS_UP)) {
                flag = 0;
            }
            break;
            
            case KEY_ECHO_LEVEL_UP:
            case KEY_ECHO_LEVEL_DN:
            if((key==KEY_ECHO_LEVEL_UP && curKey==KEY_ECHO_LEVEL_DN) || (key==KEY_ECHO_LEVEL_DN && curKey==KEY_ECHO_LEVEL_UP)) {
                flag = 0;
            }
            break;

            case KEY_ECHO_DELAY_UP:
            case KEY_ECHO_DELAY_DN:
            if((key==KEY_ECHO_DELAY_UP && curKey==KEY_ECHO_DELAY_DN) || (key==KEY_ECHO_DELAY_DN && curKey==KEY_ECHO_DELAY_UP)) {
                flag = 0;
            }
            break;

            case KEY_ECHO_REPEAT_UP:
            case KEY_ECHO_REPEAT_DN:
            if((key==KEY_ECHO_REPEAT_UP && curKey==KEY_ECHO_REPEAT_DN) || (key==KEY_ECHO_REPEAT_DN && curKey==KEY_ECHO_REPEAT_UP)) {
                flag = 0;
            }
            break;
            
            case KEY_REVERB_LEVEL_UP:
            case KEY_REVERB_LEVEL_DN:
            if((key==KEY_REVERB_LEVEL_UP && curKey==KEY_REVERB_LEVEL_DN) || (key==KEY_REVERB_LEVEL_DN && curKey==KEY_REVERB_LEVEL_UP)) {
                flag = 0;
            }
            break;

            case KEY_REVERB_TIME_UP:
            case KEY_REVERB_TIME_DN:
            if((key==KEY_REVERB_TIME_UP && curKey==KEY_REVERB_TIME_DN) || (key==KEY_REVERB_TIME_DN && curKey==KEY_REVERB_TIME_UP)) {
                flag = 0;
            }
            break;
    
            default:
            break;
        }
        curKey = key;
    }
    else {
        flag = 0;
    }

    return flag;
}

int home_clear(void)
{
    rect_t r=BODY_RECT;

    draw_clear(r);
    return 0;
}


int home_set_refresh(u16 flag)
{
    refreshFlag |= flag;
    gui_post_refresh();
    return 0;
}

int home_refresh(void)
{
    dsp_info_t info;
    rect_t r=BODY_RECT;
    u8 key=(curKey==KEY_NONE)?KEY_MUSIC_UP:curKey;
    
    dsp_get_info(key, &info);

    if(refreshFlag&REFRESH_HOME_CLEAR) {
        draw_clear(r);
    }

    if(refreshFlag&REFRESH_HOME_TITLE) {
        draw_title(r, &info);
    }

    if(refreshFlag&REFRESH_HOME_VALUE) {
        draw_value(r, &info);
    }
    
    refreshFlag = 0;

    return 0;
}


int home_refresh2(u8 key, dsp_info_t *info)
{
    rect_t r=BODY_RECT;
    
    if(need_refresh_all(key)) {
        draw_clear(r);
        draw_title(r, info);
        draw_value(r, info);
    }
    else {
        draw_value(r, info);
    }
    
    return 0;
}







