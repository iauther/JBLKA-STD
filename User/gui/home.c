#include "menu.h"
#include "default.h"

static u8 prevKey=KEY_NONE;
static void draw_title(key_info_t *info)
{
    rect_t r=TITLE_RECT;
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)info->title, FONT_24, LCD_FC, LCD_BC, ALIGN_MIDDLE, 0);
}


static void draw_name(rect_t rect, key_info_t *info)
{
    u8 font=FONT_24;
    rect_t r=rect;

    r.w = r.w*2/5;
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)info->name, font, LCD_FC, LCD_BC, ALIGN_RIGHT, 0);
}

static void draw_value(rect_t rect, s16 v, key_info_t *info)
{
    u8 tmp[20];
    u8 font=FONT_48;
    rect_t r=rect;

    r.x += r.w*2/5;
    r.w = r.w*2/5;
    sprintf((char*)tmp, "%d", v);
    lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_BC);
    lcd_draw_string_align(r.x, r.y, r.w, r.h, tmp, font, LCD_FC, LCD_BC, ALIGN_MIDDLE, 0);
}


static void draw_unit(rect_t rect, key_info_t *info)
{
    int len;
    u8 tmp[20];
    u8 font=FONT_16;
    rect_t r=rect;
    para_info_t *pinfo=(para_info_t*)&PARA_INFO[info->cmd].info[info->index];

    r.x += r.w*4/5;
    r.w = r.w/5;
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)pinfo->unit, font, LCD_FC, LCD_BC, ALIGN_LEFT, 0);
}

static int draw_clear(void)
{
    rect_t r=BODY_RECT;

    lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_BC);
}

static int need_refresh_all(u8 key)
{
    int flag=1;

    if(key != prevKey) {
        switch(key) {
            case KEY_MUSIC_UP:
            case KEY_MUSIC_DN:
            if((key==KEY_MUSIC_UP && prevKey==KEY_MUSIC_DN) || (key==KEY_MUSIC_DN && prevKey==KEY_MUSIC_UP)) {
                flag = 0;
            }
            break;

            case KEY_EFFECT_UP:
            case KEY_EFFECT_DN:
            if((key==KEY_EFFECT_UP && prevKey==KEY_EFFECT_DN) || (key==KEY_EFFECT_DN && prevKey==KEY_EFFECT_UP)) {
                flag = 0;
            }
            break;

            case KEY_MIC_UP:
            case KEY_MIC_DN:
            if((key==KEY_MIC_UP && prevKey==KEY_MIC_DN) || (key==KEY_MIC_DN && prevKey==KEY_MIC_UP)) {
                flag = 0;
            }
            break;
            
            case KEY_MUSIC_TREBLE_UP:
            case KEY_MUSIC_TREBLE_DN:
            if((key==KEY_MUSIC_TREBLE_UP && prevKey==KEY_MUSIC_TREBLE_DN) || (key==KEY_MUSIC_TREBLE_DN && prevKey==KEY_MUSIC_TREBLE_UP)) {
                flag = 0;
            }
            break;
            
            case KEY_MUSIC_BASS_UP:
            case KEY_MUSIC_BASS_DN:
            if((key==KEY_MUSIC_BASS_UP && prevKey==KEY_MUSIC_BASS_DN) || (key==KEY_MUSIC_BASS_DN && prevKey==KEY_MUSIC_BASS_UP)) {
                flag = 0;
            }
            break;
            
            case KEY_MIC_TREBLE_UP:
            case KEY_MIC_TREBLE_DN:
            if((key==KEY_MIC_TREBLE_UP && prevKey==KEY_MIC_TREBLE_DN) || (key==KEY_MIC_TREBLE_DN && prevKey==KEY_MIC_TREBLE_UP)) {
                flag = 0;
            }
            break;

            case KEY_MIC_MIDDLE_UP:
            case KEY_MIC_MIDDLE_DN:
            if((key==KEY_MIC_MIDDLE_UP && prevKey==KEY_MIC_MIDDLE_DN) || (key==KEY_MIC_MIDDLE_DN && prevKey==KEY_MIC_MIDDLE_UP)) {
                flag = 0;
            }
            break;

            case KEY_MIC_BASS_UP:               //-24~+12dB
            case KEY_MIC_BASS_DN:
            if((key==KEY_MIC_BASS_UP && prevKey==KEY_MIC_BASS_DN) || (key==KEY_MIC_BASS_DN && prevKey==KEY_MIC_BASS_UP)) {
                flag = 0;
            }
            break;
            
            case KEY_ECHO_LEVEL_UP:
            case KEY_ECHO_LEVEL_DN:
            if((key==KEY_ECHO_LEVEL_UP && prevKey==KEY_ECHO_LEVEL_DN) || (key==KEY_ECHO_LEVEL_DN && prevKey==KEY_ECHO_LEVEL_UP)) {
                flag = 0;
            }
            break;

            case KEY_ECHO_DELAY_UP:
            case KEY_ECHO_DELAY_DN:
            if((key==KEY_ECHO_DELAY_UP && prevKey==KEY_ECHO_DELAY_DN) || (key==KEY_ECHO_DELAY_DN && prevKey==KEY_ECHO_DELAY_UP)) {
                flag = 0;
            }
            break;

            case KEY_ECHO_REPEAT_UP:
            case KEY_ECHO_REPEAT_DN:
            if((key==KEY_ECHO_REPEAT_UP && prevKey==KEY_ECHO_REPEAT_DN) || (key==KEY_ECHO_REPEAT_DN && prevKey==KEY_ECHO_REPEAT_UP)) {
                flag = 0;
            }
            break;
            
            case KEY_REVERB_LEVEL_UP:
            case KEY_REVERB_LEVEL_DN:
            if((key==KEY_REVERB_LEVEL_UP && prevKey==KEY_REVERB_LEVEL_DN) || (key==KEY_REVERB_LEVEL_DN && prevKey==KEY_REVERB_LEVEL_UP)) {
                flag = 0;
            }
            break;

            case KEY_REVERB_TIME_UP:
            case KEY_REVERB_TIME_DN:
            if((key==KEY_REVERB_TIME_UP && prevKey==KEY_REVERB_TIME_DN) || (key==KEY_REVERB_TIME_DN && prevKey==KEY_REVERB_TIME_UP)) {
                flag = 0;
            }
            break;
    
            default:
            break;
        }
        prevKey = key;
    }
    else {
        flag = 0;
    }

    return flag;
}


int home_refresh(u8 key, s16 v, key_info_t *info)
{
    rect_t r=INPUTBOX_RECT;

    if(need_refresh_all(key)) {
        draw_clear();
        draw_title(info);
        draw_name(r, info);
        draw_value(r, v, info);
        draw_unit(r, info);
    }
    else {
        draw_value(r, v, info);
    }
    
    return 0;
}





