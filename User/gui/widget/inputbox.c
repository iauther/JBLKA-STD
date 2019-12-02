#include <stdio.h>
#include "lcd.h"
#include "inputbox.h"



#define LABEL_RECT  {1,1,1,1}
#define BOX_RECT    {2,1,1,1}


static void show_label(inputbox_t *i, u16 color, u16 bgcolor)
{
    rect_t r=i->rect;
    r.w = i->rect.w/2;
    lcd_draw_string_align(r.x, r.h, r.w, r.h, (u8*)i->dat->txt, FONT_32, color, bgcolor, ALIGN_RIGHT, 0);
}
static void show_value(inputbox_t *i, u16 color, u16 bgcolor)
{
    rect_t r=i->rect;
    para_info_t *inf=i->dat->info;

    r.x = i->rect.w/2;
    r.w = i->rect.w/4;

    lcd_draw_rect(r.x, r.y, r.w, r.h, color);
    lcd_draw_string_align(r.x, r.h, r.w, r.h, (u8*)inf->unit, FONT_16, color, bgcolor, ALIGN_MIDDLE, 0);
}
static void show_unit(inputbox_t *i, u16 color, u16 bgcolor)
{
    char *ptxt;
    char tmp[20];
    rect_t r=i->rect;
    para_info_t *inf=i->dat->info;
    s16 v=*(s16*)i->dat->data;

    r.x = i->rect.w*3/4;
    r.w = i->rect.w/4;
    if(inf->ptxt) {
        if(i->dat->cmd==CMD_ID_PitchShift) {
            ptxt = (char*)inf->ptxt->txt[v+5];
        }
        else {
            ptxt = (char*)inf->ptxt->txt[v];
        }
    }
    else{
        if(inf->flt) {
            sprintf(tmp, "%.1f", (f32)v/inf->step);
        }
        else {
            sprintf(tmp, "%d", v/(int)inf->step);
        }
        ptxt = tmp;
    }

    lcd_draw_string_align(r.x, r.h, r.w, r.h, (u8*)ptxt, FONT_16, color, bgcolor, ALIGN_MIDDLE, 0);
}
static void input_show_all(inputbox_t *i, u16 color, u16 bgcolor)
{
    show_label(i, color, bgcolor);
    show_value(i, color, bgcolor);
    show_unit(i, color, bgcolor);
}
/////////////////////////////////////////////

inputbox_t *inputbox_init(rect_t *rect, item_data_t *dat)
{
    inputbox_t *i=(inputbox_t*)malloc(sizeof(inputbox_t));
    if(!i) {
        return NULL;
    }

    
    return i;
}



int inputbox_free(inputbox_t **i)
{
    if(!*i) {
        return -1;
    }
    
    free(*i);
    
    return 0;
}


int inputbox_refresh(inputbox_t *i)
{
    
    
    return 0;
}


int inputbox_handle(inputbox_t *i, u8 key)
{
    switch(key) {
        
        case KEY_UP:
        break;
        
        case KEY_DOWN:
        break;

        case KEY_ENTER:
        break;
        
        case KEY_EXIT:
        break;
    }
    
    return 0;
}


int inputbox_clear(inputbox_t *i)
{
    //lcd_fill_rect();
    return 0;
}


int inputbox_show(inputbox_t *i, u16 color, u16 bgcolor)
{
    return 0;
}
