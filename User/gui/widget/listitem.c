#include "gbl.h"
#include "menu.h"
#include "listitem.h"


static void li_reset(listitem_t *l)
{
    l->focusId = 0;
    l->firstId = 0;
    l->prev_firstId = 0;
    l->prev_focusId = 0;
    l->refreshFlag  = 0xffff;
}


listitem_t *listitem_init(cchr *title, rect_t *rect, u8 max, int node_size)
{
    listitem_t *l=(listitem_t *)malloc(sizeof(listitem_t));
    if(!l) {
        return NULL;
    }
    
    l->rect = *rect;
    l->list = list_init(max, sizeof(node_size));

    l->imax = l->rect.h/ITEM_HEIGHT;
    l->title = title;
    l->parent = NULL;
    li_reset(l);
    
    return 0;
}


int listitem_free(listitem_t **l)
{
    if(!l) {
        return -1;
    }
    list_free(&(*l)->list);
    free(*l);

    return 0;
}


int listitem_reset(listitem_t *l)
{
    if(!l) {
        return -1;
    }
    li_reset(l);

    return 0;
}


int listitem_set_parent(listitem_t *l, listitem_t *parent)
{
    if(!l) {
        return -1;
    }
    l->parent = parent;

    return 0;
}


listitem_t* listitem_get_parent(listitem_t *l)
{
    if(!l) {
        return NULL;
    }

    return l->parent;
}


int listitem_add(listitem_t *l, node_t *n)
{
    return list_add(l->list, -1, n);
}


int listitem_get(listitem_t *l, u8 index, node_t *n)
{
    return list_get(l->list, index, n);
}


int listitem_get_focus(listitem_t *l, node_t *n)
{
    return list_get(l->list, l->focusId, n);
}


int listitem_set_refresh(listitem_t *l, u32 flag)
{
    if(!l) {
        return -1;
    }

    l->refreshFlag |= flag;

    return 0;
}


static void lcd_draw_item(listitem_t *l, u8 index, u16 font_color, u16 bgcolor)
{
    node_t n;
    item_data_t *i;

    list_get(l->list, index, &n);
    i = (item_data_t*)n.ptr;
    lcd_draw_round_rect(l->rect.x, l->rect.y+index*ITEM_HEIGHT, l->rect.w, ITEM_HEIGHT, 6, bgcolor);
    lcd_draw_string_align(l->rect.x, l->rect.h, l->rect.w, l->rect.h, (u8*)i->txt, FONT_32, font_color, bgcolor, ALIGN_MIDDLE);
}
int listitem_refresh(listitem_t *l)
{
    u8  i;
    u16 color;

    if(!l) {
        return -1;
    }

    if(l->refreshFlag & REFRESH_TITLE) {
        rect_t r=TITLE_RECT;
        lcd_fill_rect(r.x, r.h, r.w, r.h, LCD_BC);
        lcd_draw_string_align(r.x, r.h, r.w, r.h, (u8*)l->title, FONT_32, LCD_FC, LCD_BC, ALIGN_MIDDLE);
        lcd_draw_line(r.x, r.y, r.w, r.h+4, LCD_FC);
    }

    if(l->refreshFlag & REFRESH_FOCUS) {
        lcd_draw_item(l, l->prev_focusId, LCD_FC, ITEM_FOCUS_BGCOLOR);
        lcd_draw_item(l, l->focusId, LCD_FC, ITEM_FOCUS_COLOR);
    }

    if(l->refreshFlag & REFRESH_LIST) {
        lcd_fill_rect(l->rect.x, l->rect.h, l->rect.w, l->rect.h, LCD_BC);
        for(i=l->firstId; i<=l->focusId; i++) {
            color = (i==l->focusId)?ITEM_FOCUS_COLOR:ITEM_FOCUS_BGCOLOR;
            lcd_draw_item(l, i, LCD_FC, color);
        }
    }

    if(l->refreshFlag & REFRESH_TXT_SCROLL) {

    }
    l->refreshFlag = 0;
    l->prev_firstId = l->firstId;
    l->prev_focusId = l->focusId;
    
    return 0;
}


int listitem_clear(listitem_t *l)
{
    if(!l) {
        return -1;
    }
    
    rect_t r=TITLE_RECT;
    lcd_fill_rect(r.x, r.h, r.w, r.h, LCD_BC);
    lcd_fill_rect(l->rect.x, l->rect.h, l->rect.w, l->rect.h, LCD_BC);

    return 0;
}


int listitem_move(listitem_t *l, int dir)
{
    if(!l) {
        return -1;
    }

    if(dir==UP) {
        if(l->firstId>0) {
            l->focusId--;
            if(l->focusId<=l->firstId) {
                l->firstId = l->focusId;
            }
        }
        else {
            if(l->focusId>0) {
                l->focusId--;
            }
        }
    }
    else if(dir==DOWN) {
        if(l->firstId>0) {
            if(l->focusId<list_size(l->list)-1) {
                l->focusId++;
                l->firstId++;
            }
        }
        else {
            if(l->focusId<list_size(l->list)-1) {
                l->focusId++;
                if(l->focusId>=l->imax) {
                    l->firstId++;
                }
            }
        }
    }
    else {
        return -1;
    }

    if(l->firstId != l->prev_firstId) {
        listitem_set_refresh(l, REFRESH_LIST);
    }
    else if(l->focusId != l->prev_focusId){
        listitem_set_refresh(l, REFRESH_FOCUS);
    }

    return 0;
}


int listitem_handle(listitem_t *l, u8 key)
{
    int r;

    if(!l) {
        return -1;
    }

    switch(key) {
        case KEY_UP:
        listitem_move(l, UP);
        break;

        case KEY_DOWN:
        listitem_move(l, DOWN);
        break;

        case KEY_ENTER:
        {
            node_t n;
            r = listitem_get_focus(l, &n);
            if(r==0) {
                item_data_t *pi=(item_data_t*)n.ptr;
                if(pi->sub) {
                    gb.pl = pi->sub;
                    listitem_set_refresh(l, REFRESH_ALL);
                }
            }
        }
        break;
        
        case KEY_EXIT:
        if(listitem_get_parent(l)){
            gb.pl = listitem_get_parent(gb.pl);
            listitem_set_refresh(l, REFRESH_ALL);
        }
        else {
            listitem_clear(l);
        }
        break;
    }

    return 0;
}





