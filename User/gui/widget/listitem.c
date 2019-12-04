#include "gbl.h"
#include "menu.h"
#include "inputbox.h"
#include "listitem.h"


static void li_reset(listitem_t *l)
{
    l->focusId = 0;
    l->firstId = 0;
    l->prev_firstId = 0;
    l->prev_focusId = 0;
    l->refreshFlag  = REFRESH_ALL;
}


listitem_t *listitem_init(cchr *title, rect_t *rect, u8 max, int node_size)
{
    listitem_t *l=(listitem_t *)malloc(sizeof(listitem_t));
    if(!l) {
        return NULL;
    }
    
    l->rect = *rect;
    l->list = slist_init(max, sizeof(node_size));

    l->pageItems = l->rect.h/ITEM_HEIGHT;
    l->dispItems = 0;
    l->title = title;
    l->parent = NULL;
    li_reset(l);
    
    return l;
}


int listitem_free(listitem_t **l)
{
    if(!l) {
        return -1;
    }
    slist_free(&(*l)->list);
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


listitem_t* listitem_get_parent(listitem_t *l)
{
    if(!l) {
        return NULL;
    }

    return l->parent;
}


int listitem_set_parent(listitem_t *l, listitem_t *parent)
{
    if(!l) {
        return -1;
    }
    l->parent = parent;

    return 0;
}


listitem_t* listitem_get_child(listitem_t *l)
{
    if(!l) {
        return NULL;
    }

    return l->child;
}


int listitem_set_child(listitem_t *l, listitem_t *child)
{
    if(!l) {
        return -1;
    }
    l->child = child;

    return 0;
}


int listitem_append(listitem_t *l, node_t *n)
{
    int r;
    int sz;

    r = slist_append(l->list, n);
    sz = slist_size(l->list);
    l->dispItems = MIN(l->dispItems, sz);
    return r;
}


int listitem_get(listitem_t *l, u8 index, node_t *n)
{
    return slist_get(l->list, index, n);
}


int listitem_get_focus(listitem_t *l, node_t *n)
{
    return slist_get(l->list, l->focusId, n);
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
    u8 r=6;
    node_t n;
    item_info_t *info;

    slist_get(l->list, index, &n);
    info = (item_info_t*)n.ptr;
    lcd_draw_round_rect(l->rect.x, l->rect.y+index*ITEM_HEIGHT, l->rect.w, ITEM_HEIGHT, 6, bgcolor);
    lcd_draw_string_align(l->rect.x, l->rect.y, l->rect.w, l->rect.h, (u8*)info->txt, FONT_24, font_color, bgcolor, ALIGN_MIDDLE, 0);
}
static void lcd_draw_title(listitem_t *l, u16 font_color, u16 bgcolor)
{
    rect_t r=TITLE_RECT;
    lcd_fill_rect(r.x, r.h, r.w, r.h, bgcolor);
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)l->title, FONT_24, font_color, bgcolor, ALIGN_MIDDLE, 0);
    //lcd_draw_line(r.x, r.y+r.h+4, r.w, r.h, font_color);
}


int listitem_refresh(listitem_t *l)
{
    u8  i;
    u16 color;

    if(!l) {
        return -1;
    }

    if(l->refreshFlag & REFRESH_TITLE) {
        lcd_draw_title(l, LCD_FC, LCD_BC);
    }

    if(l->refreshFlag & REFRESH_LIST) {
        lcd_fill_rect(l->rect.x, l->rect.h, l->rect.w, l->rect.h, LCD_BC);
        for(i=l->firstId; i<l->dispItems; i++) {
            if(i!=l->focusId) {
                lcd_draw_item(l, i, LCD_FC, ITEM_FOCUS_BGCOLOR);
            }
        }
        lcd_draw_item(l, l->focusId, LCD_FC, ITEM_FOCUS_COLOR);
    }
    else if(l->refreshFlag & REFRESH_FOCUS) {
        node_t n;
        item_info_t it1, it2;
        
        n.ptr = &it1;
        n.len = sizeof(item_info_t);
        listitem_get(l, l->prev_focusId, &n);
        n.ptr = &it2;
        listitem_get(l, l->focusId, &n);
        if(it1.control==CONTROL_LIST) {
            lcd_draw_item(l, l->prev_focusId, LCD_FC, LCD_BC);
            lcd_draw_item(l, l->focusId, ITEM_FOCUS_COLOR, ITEM_FOCUS_BGCOLOR);
        }
        else if(it1.control==CONTROL_INPUTBOX){
            inputbox_t *box1=(inputbox_t*)it1.handle;
            inputbox_t *box2=(inputbox_t*)it2.handle;
            //inputbox_show(box1, LCD_FC, LCD_BC);
            //inputbox_show(box2, ITEM_FOCUS_COLOR, ITEM_FOCUS_BGCOLOR);
        }
    }

    if(l->refreshFlag & REFRESH_TXT_SCROLL) {

    }
    
    l->prev_firstId = l->firstId;
    l->prev_focusId = l->focusId;
    l->refreshFlag = 0;
    
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
            if(l->focusId<slist_size(l->list)-1) {
                l->focusId++;
                l->firstId++;
            }
        }
        else {
            if(l->focusId<slist_size(l->list)-1) {
                l->focusId++;
                if(l->focusId>=l->dispItems) {
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


int listitem_handle(listitem_t *l, key_t key)
{
    if(!l) {
        return -1;
    }

    switch(key.value) {
        case KEY_UP:
        listitem_move(l, UP);
        break;

        case KEY_DOWN:
        listitem_move(l, DOWN);
        break;
    }

    return 0;
}





