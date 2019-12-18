#include "msgbox.h"
#include "lcd.h"

msgbox_t mbox={
    NULL,
    NULL,
    MSGBOX_RECT,
    0,
    0,
    0,
    0,
    NULL,
#ifdef RTX
    0,
#endif
};

static void msg_tmr_fun(void *arg)
{
    msgbox_clear();
    if(mbox.trigger) {
        mbox.trigger();
    }
    mbox.show = 0;
    osTimerDelete(mbox.tmr);
}
static void msg_reset(void)
{
    mbox.title = NULL;
    mbox.message = NULL;
    mbox.sec = 0;
    mbox.show = 0;
    mbox.focusId = MSGBOX_NO;
    mbox.refreshFlag = 0;
    mbox.trigger = NULL;
}

static void set_fresh(u16 flag)
{
    mbox.refreshFlag |= flag;
    if(mbox.trigger) {
        mbox.trigger();
    }
}

int msgbox_is_show(void)
{
    return mbox.show;
}

int msgbox_get_focus(void)
{
    return mbox.focusId;
}

int msgbox_show(char *title, char *message, s8 sec, trigger_fn trigger)
{
    if(mbox.show) {
        return -1;
    }

    mbox.title = title;
    mbox.message = message;
    mbox.sec = sec;
    mbox.focusId = 0;
    mbox.refreshFlag = REFRESH_MSGBOX_ALL;
    mbox.trigger = trigger;

#ifdef RTX
    if(mbox.tmr) {
        osTimerStop(mbox.tmr);
    }
        
    mbox.tmr = osTimerNew(msg_tmr_fun, osTimerOnce, NULL, NULL);
    if(mbox.sec>0 && mbox.tmr) {
        osTimerStart(mbox.tmr, mbox.sec*1000);
    }
#endif
    mbox.show = 1;
    if(trigger) {
        trigger();
    }
    
    return 0;
}


int msgbox_clear(void)
{
    rect_t r=mbox.rect;
    
    lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_BC);
    
    return 0;
}

static void draw_rect(rect_t rect, u16 color)
{
    rect_t r=rect;

    lcd_draw_rect(r.x, r.y, r.w, r.h, color);

}
static void draw_title(rect_t rect, char *title, u16 color, u16 bgcolor)
{
    rect_t r=rect;

    r.h = 32;
    lcd_fill_rect(r.x+1, r.y+1, r.w-2, r.h-2, bgcolor);
    lcd_draw_string_align(r.x+4, r.y, r.w, r.h, (u8*)title, FONT_16, color, bgcolor, ALIGN_LEFT);

}
static void draw_message(rect_t rect, char *message, u16 color, u16 bgcolor)
{
    rect_t r=rect;

    r.x += 4;
    r.y += 40;
    r.h = 32;
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)message, FONT_16, color, bgcolor, ALIGN_LEFT);
}
static void draw_button(rect_t rect, u16 color, u16 bgcolor)
{
    u8 i;
    char *btnTxt[2]={"NO", "YES"};
    rect_t r=rect;
    #define BTN_GAP             40
    #define BTN_FOCUS_BGCOLOR   BROWN

    r.y += r.h-34;
    r.w = 30;
    r.h = 30;
    for(i=0; i<MSGBOX_MAX; i++) {
        r.x += i*BTN_GAP+30;
        
        lcd_fill_rect(r.x, r.y, r.w, r.h, (i==mbox.focusId)?BTN_FOCUS_BGCOLOR:bgcolor);
        lcd_draw_rect(r.x, r.y, r.w, r.h, color);
        lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)btnTxt[i], FONT_16, color, bgcolor, ALIGN_MIDDLE);
    }
}


int msgbox_refresh(void)
{
    if(mbox.refreshFlag&REFRESH_MSGBOX_CLEAR) {
        msgbox_clear();
        draw_rect(mbox.rect, BROWN);
    }

    if(mbox.refreshFlag&REFRESH_MSGBOX_TITLE) {
        draw_title(mbox.rect, mbox.title, LCD_FC, BLUE);
    }

    if(mbox.refreshFlag&REFRESH_MSGBOX_MESSAGE) {
        draw_message(mbox.rect, mbox.message, LCD_FC, LCD_BC);
    }

    if(mbox.refreshFlag&REFRESH_MSGBOX_BUTTONS) {
        draw_button(mbox.rect, LCD_FC, LCD_BC);
    }
    mbox.refreshFlag = 0;
    
    return 0;
}


int msgbox_set_refresh(u16 flag)
{
    set_fresh(flag);
    return 0;
}


int msgbox_handle(key_t *key)
{
    if(!mbox.show) {
        return -1;
    }
    
    switch(key->value) {
        case KEY_UP:
        {
            if(mbox.focusId>MSGBOX_NO) {
                mbox.focusId--;
                set_fresh(REFRESH_MSGBOX_BUTTONS);
            }
        }
        break;
        
        case KEY_DOWN:
        {
            if(mbox.focusId<MSGBOX_YES) {
                mbox.focusId++;
                set_fresh(REFRESH_MSGBOX_BUTTONS);
            }
        }
        break;

        case KEY_ENTER:
        case KEY_EXIT:
        mbox.show = 0;
        break;
    }

    if(key->src==SRC_KNOB) {
        msg_reset();
    }
    
    return 0;
}




