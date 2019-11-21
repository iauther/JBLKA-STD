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
    lcd_fill_rect(r->x, r->y, r->w, r->h, BLACK);
}

static void show_temp(void)
{
    u8 tmp[10];
    rect_t r = mTopbar.rect;

    sprintf((char*)tmp, "%02d", mTopbar.stat->pwr_temp);
    
    lcd_draw_string_center(r.x, r.y, r.w, r.h, tmp, FONT_16, LCD_FC, LCD_BC);
}

static void show_bt(void)
{
    u8 tmp[10];
    rect_t r = mTopbar.rect;

    r.x += mTopbar.rect.w/4;
    sprintf((char*)tmp, "BT");
    
    lcd_draw_string_center(r.x, r.y, r.w, r.h, tmp, FONT_16, LCD_FC, LCD_BC);
}

static void show_input(void)
{
    //rect_t rect={0};
}

static void show_preset(void)
{
    //rect_t rect={0};
}


static int show_title(void)
{
    //rect_t r=mTopbar.rect;

    //topbar_clear();
    //lcd_draw_string();

    return 0;
}

//////////////////////////////////////////////
int topbar_init(u16 h)
{
    mTopbar.rect.x = 0;
    mTopbar.rect.y = 0;
    mTopbar.rect.w = LCD_WIDTH;
    mTopbar.rect.h = h;

    mTopbar.pio  = uiParams.pio;
    mTopbar.stat = &gStatus;
    mTopbar.title = NULL;
    mTopbar.refreshFlag = 0;
    
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

    if(mTopbar.refreshFlag & TOPBAR_REFRESH_TITLE) {
        show_title();
    }
    mTopbar.refreshFlag = 0;

    return 0;
}









