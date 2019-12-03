#include <stdio.h>
#include "lcd.h"
#include "inputbox.h"



#define LABEL_RECT  {1,1,1,1}
#define BOX_RECT    {2,1,1,1}


static void show_label(inputbox_t *i, rect_t *rect, u16 color, u16 bgcolor)
{
    rect_t r=*rect;
    r.w = rect->w/2;
    lcd_draw_string_align(r.x, r.h, r.w, r.h, (u8*)i->info->name, FONT_32, color, bgcolor, ALIGN_RIGHT, 0);
}
static void show_value(inputbox_t *i, rect_t *rect, u16 color, u16 bgcolor)
{
    rect_t r=*rect;

    r.x = rect->w/2;
    r.w = rect->w/4;

    lcd_draw_rect(r.x, r.y, r.w, r.h, color);
    lcd_draw_string_align(r.x, r.h, r.w, r.h, (u8*)i->info->unit, FONT_16, color, bgcolor, ALIGN_MIDDLE, 0);
}
static void show_unit(inputbox_t *i, rect_t *rect, u16 color, u16 bgcolor)
{
    char *ptxt;
    char tmp[20];
    rect_t r=*rect;
    s16 v=*i->data;

    r.x = rect->w*3/4;
    r.w = rect->w/4;
    if(i->info->ptxt) {
        if(i->cmd==CMD_ID_PitchShift) {
            ptxt = (char*)i->info->ptxt->txt[v+5];
        }
        else {
            ptxt = (char*)i->info->ptxt->txt[v];
        }
    }
    else{
        if(i->info->flt) {
            sprintf(tmp, "%.1f", (f32)v/i->info->step);
        }
        else {
            sprintf(tmp, "%d", v/(int)i->info->step);
        }
        ptxt = tmp;
    }

    lcd_draw_string_align(r.x, r.h, r.w, r.h, (u8*)ptxt, FONT_16, color, bgcolor, ALIGN_MIDDLE, 0);
}
static void show_all(inputbox_t *i, rect_t *rect, u16 color, u16 bgcolor)
{
    show_label(i, rect, color, bgcolor);
    show_value(i, rect, color, bgcolor);
    show_unit(i,  rect, color, bgcolor);
}
/////////////////////////////////////////////

inputbox_t *inputbox_init(const item_info_t *info, u8 index)
{
    inputbox_t *i=(inputbox_t*)malloc(sizeof(inputbox_t));
    if(!i) {
        return NULL;
    }
    i->cmd = info->cmd;
    i->info = &PARA_INFO[info->cmd].info[index];
    i->data = (s16*)info->data;
    
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


int inputbox_handle(inputbox_t *i, key_t key)
{
    switch(key.value) {
        
        case KEY_UP:
        {
            //*i->data += 
        }
        break;
        
        case KEY_DOWN:
        {
            
        }
        break;

        default:
        return -1;
        break;
    }
    
    return 0;
}


int inputbox_clear(inputbox_t *i)
{
    //lcd_fill_rect();
    return 0;
}


int inputbox_show(inputbox_t *i, rect_t *rect, u16 color, u16 bgcolor)
{
    show_all(i, rect, color, bgcolor);
    i->rect = *rect;

    return 0;
}
