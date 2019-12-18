#include <stdio.h>
#include <string.h>
#include "topbar.h"
#include "font.h"
#include "lcd.h"
#include "packet.h"

typedef struct {
    s8              *title;
    rect_t          rect;
    
    io_data_t       *pio;
    status_data_t   *stat;
    u32             refreshFlag;
}topbar_t;



topbar_t mTopbar;
static void topbar_clear(void)
{
    rect_t *r = &mTopbar.rect;
    lcd_fill_rect(r->x, r->y, r->w, r->h, LCD_BC);
}

static void show_temp(void)
{
    u8 tmp[10];
    rect_t r = mTopbar.rect;

    r.w = mTopbar.rect.w/4;
    sprintf((char*)tmp, "%02d", mTopbar.stat->pwr_temp);
    
    lcd_draw_string_align(r.x, r.y, r.w, r.h, tmp, FONT_16, LCD_FC, LCD_BC, ALIGN_MIDDLE);
}

static void show_bt(void)
{
    u8 tmp[10];
    rect_t r = mTopbar.rect;

    r.x += mTopbar.rect.w/4;
    r.w = mTopbar.rect.w/4;
    sprintf((char*)tmp, "BT/USB");
    
    lcd_draw_string_align(r.x, r.y, r.w, r.h, tmp, FONT_16, LCD_FC, LCD_BC, ALIGN_MIDDLE);
}

static void show_input(void)
{
    u8 tmp[10];
    const char *str[INPUT_MAX]={"VOD", "DVD", "BGM", "BT/USB", "OPT", "HIN", "HARC"};
    rect_t r = mTopbar.rect;
    u16 input=uiParams.dsp.music.input->input;

    r.x += mTopbar.rect.w/2;
    r.w = mTopbar.rect.w/4;
    sprintf((char*)tmp, "%s", str[input]);
    
    lcd_draw_string_align(r.x, r.y, r.w, r.h, tmp, FONT_16, LCD_FC, LCD_BC, ALIGN_MIDDLE);
}

static void show_preset(void)
{
    u8 tmp[10];
    rect_t r = mTopbar.rect;

    r.x += mTopbar.rect.w*3/4;
    r.w = mTopbar.rect.w/4;
    sprintf((char*)tmp, "%d", gParams.pre);
    
    lcd_draw_string_align(r.x, r.y, r.w, r.h, tmp, FONT_16, LCD_FC, LCD_BC, ALIGN_MIDDLE);
}


//////////////////////////////////////////////
int topbar_init(void)
{
    rect_t r=TOPBAR_RECT;

    mTopbar.rect = r;

    mTopbar.pio  = uiParams.pio;
    mTopbar.stat = &gStatus;
    mTopbar.title = NULL;
    mTopbar.refreshFlag = TOPBAR_REFRESH_ALL;

    topbar_refresh();
    
    return 0;
}


int topbar_free(void)
{
    return 0;
}


int topbar_set_refresh(u32 flag)
{
    mTopbar.refreshFlag |= flag;
    return 0;
}


int topbar_refresh(void)
{
    if(mTopbar.refreshFlag & TOPBAR_REFRESH_BT) {
        show_bt();
    }

    if(mTopbar.refreshFlag & TOPBAR_REFRESH_TEMP) {
        show_temp();
    }

    if(mTopbar.refreshFlag & TOPBAR_REFRESH_INPUT) {
        show_input();
    }

    if(mTopbar.refreshFlag & TOPBAR_REFRESH_PRESET) {
        show_preset();
    }
    
    mTopbar.refreshFlag = 0;

    return 0;
}









