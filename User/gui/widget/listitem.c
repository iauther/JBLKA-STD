#include "menu.h"
#include "listitem.h"


enum {

    REFRESH_PAGE        = 1<<0,
    REFRESH_FOCUS       = 1<<1,
    REFRESH_TXT_SCROLL  = 1<<2,
};


listitem_t *listitem_init(rect_t *rect, u8 max)
{
    listitem_t *l=(listitem_t *)malloc(sizeof(listitem_t));
    if(!l) {
        return NULL;
    }
    
    l->rect = *rect;
    l->list = list_init(max, sizeof(element_t));
    l->focusId = 0;
    
    return 0;
}


int listitem_free(listitem_t *l)
{
    list_free(&l->list);
    return 0;
}


int listitem_add(listitem_t *l, element_t *e)
{
    node_t n;
    n.ptr = e;
    n.len = sizeof(element_t);
    return list_add(l->list, -1, &n);
}


int listitem_set_refresh(listitem_t *l, u32 flag)
{
    l->refreshFlag |= flag;
    return 0;
}

int listitem_refresh(listitem_t *l)
{
    if(l->refreshFlag & REFRESH_PAGE) {
        lcd_fill_rect(l->rect.x, l->rect.y, l->rect.w, l->rect.h, LCD_BC);
        //
    }

    if(l->refreshFlag & REFRESH_FOCUS) {
        
    }

    if(l->refreshFlag & REFRESH_TXT_SCROLL) {

    }
    l->refreshFlag = 0;
    //lcd_clear_rect();
    
    return 0;
}

int listitem_set_focus(listitem_t *l, int id)
{
    
    return 0;
}


















