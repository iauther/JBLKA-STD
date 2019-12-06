#include "gbl.h"
#include "menu.h"
#include "listitem.h"

static u8 get_num(item_info_t *info)
{
    u8 i=0;
    while(info[i].control!=CONTROL_NONE) i++;
    return i;
}

static void li_reset(listitem_t *l)
{
    l->focusId = 0;
    l->firstId = 0;
    l->prev_firstId = 0;
    l->prev_focusId = 0;
    l->refreshFlag  = 0;//REFRESH_ALL;
    l->parent = NULL;
    l->child  = NULL;

    l->inEdit = 0;
}


listitem_t *listitem_init(cchr *title, u8 max, int node_size)
{
    rect_t r=MENU_RECT;
    listitem_t *l=(listitem_t *)malloc(sizeof(listitem_t));
    if(!l) {
        return NULL;
    }
    
    l->rect = r;
    l->list = slist_init(max, node_size);

    l->pageItems = l->rect.h/ITEM_HEIGHT;
    l->dispItems = 0;
    l->title = title;
    
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


listitem_t* listitem_get_child(listitem_t *l)
{
    if(!l) {
        return NULL;
    }

    return l->child;
}


int listitem_set_child(listitem_t *l, listitem_t *child)
{
    if(!l|| !child) {
        return -1;
    }
    l->child = child;
    child->parent = l;

    return 0;
}


int listitem_append(listitem_t *l, item_info_t *inf)
{
    int r,size;
    node_t n={&inf, sizeof(inf)};

    r = slist_append(l->list, &n);
    size = slist_size(l->list);
    l->dispItems = MIN(l->pageItems, size);

    return r;
}


item_info_t* listitem_get(listitem_t *l, u8 index)
{
    item_info_t *inf=NULL;
    node_t n={&inf, sizeof(inf)};

    slist_get(l->list, index, &n);
    return inf;
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


#define LABEL_RECT  {1,1,1,1}
#define BOX_RECT    {2,1,1,1}


static void draw_box_label(listitem_t *l, u8 index, item_info_t *info, rect_t *rect, u16 color, u16 bgcolor)
{
    rect_t r=*rect;
    const para_info_t *pi=&PARA_INFO[info->cmd].info[index];

    r.w = rect->w/2;
    lcd_draw_string_align(r.x, r.h, r.w, r.h, (u8*)pi->name, FONT_24, color, bgcolor, ALIGN_RIGHT, 0);
}
static void draw_box_value(listitem_t *l, u8 index, item_info_t *info, rect_t *rect, u16 color, u16 bgcolor)
{
    char *ptxt;
    char tmp[20];
    rect_t r=*rect;
    const para_info_t *pi=&PARA_INFO[info->cmd].info[index];
    s16 v=*((s16*)info->data+index);

    r.x = rect->w*3/4;
    r.w = rect->w/4;
    if(pi->ptxt) {
        if(info->cmd==CMD_ID_PitchShift) {
            ptxt = (char*)pi->ptxt->txt[v+5];
        }
        else {
            ptxt = (char*)pi->ptxt->txt[v];
        }
    }
    else{
        if(pi->flt) {
            sprintf(tmp, "%.1f", (f32)v/pi->step);
        }
        else {
            sprintf(tmp, "%d", v/(int)pi->step);
        }
        ptxt = tmp;
    }

    lcd_draw_string_align(r.x, r.h, r.w, r.h, (u8*)ptxt, FONT_16, color, bgcolor, ALIGN_MIDDLE, 0);
}
static void draw_box_unit(listitem_t *l, u8 index, item_info_t *info, rect_t *rect, u16 color, u16 bgcolor)
{
    rect_t r=*rect;
    const para_info_t *pi=&PARA_INFO[info->cmd].info[index];

    r.x = rect->w/2;
    r.w = rect->w/4;

    lcd_draw_rect(r.x, r.y, r.w, r.h, color);
    lcd_draw_string_align(r.x, r.h, r.w, r.h, (u8*)pi->unit, FONT_16, color, bgcolor, ALIGN_MIDDLE, 0);
}
static void draw_paras(listitem_t *l, u8 index, item_info_t *info, u16 color, u16 bgcolor)
{
    rect_t r=MENU_RECT;
    draw_box_label(l, index, info, &r, color, bgcolor);
    draw_box_value(l, index, info, &r, color, bgcolor);
    draw_box_unit (l, index, info, &r, color, bgcolor);
}


static void draw_item(listitem_t *l, int id, u16 color, u16 bgcolor, u8 clear)
{
    u8 r=6,index=id-l->firstId;
    item_info_t *inf=listitem_get(l, id);

    if(inf->control==CONTROL_LIST) {
        if(clear) {
            lcd_fill_rect(l->rect.x+1, l->rect.y+index*(ITEM_HEIGHT+1), l->rect.w-2, ITEM_HEIGHT-2, LCD_BC);
        }
        lcd_draw_round_rect(l->rect.x, l->rect.y+index*ITEM_HEIGHT, l->rect.w, ITEM_HEIGHT, r, color);
        lcd_draw_string_align(l->rect.x, l->rect.y+index*ITEM_HEIGHT, l->rect.w, ITEM_HEIGHT, (u8*)inf->txt, FONT_24, color, bgcolor, ALIGN_MIDDLE, 0);
    }
    else if(inf->control==CONTROL_INPUTBOX){
        draw_paras(l, index, inf, color, bgcolor);
    }
}
static void draw_title(listitem_t *l, u16 font_color, u16 bgcolor)
{
    rect_t r=TITLE_RECT;
    lcd_fill_rect(r.x, r.h, r.w, r.h, bgcolor);
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)l->title, FONT_24, font_color, bgcolor, ALIGN_MIDDLE, 0);
    //lcd_draw_line(r.x, r.y+r.h+4, r.w, r.h, font_color);
}
static void draw_arrow(listitem_t *l, u16 color, u16 bgcolor)
{
    if(l->firstId>0) {
        //lcd_fill_rect(r.x, r.h, r.w, r.h, bgcolor);
    }
    else {
        //
    }

    if(l->firstId+l->dispItems>listitem_size(l)) {
        //lcd_fill_rect(r.x, r.h, r.w, r.h, bgcolor);
    }
    else {

    }
}



int listitem_refresh(listitem_t *l)
{
    u8  i;
    u16 color,bgcolor;

    if(!l) {
        return -1;
    }

    if(l->refreshFlag & REFRESH_TITLE) {
        draw_title(l, LCD_FC, LCD_BC);
    }

    if(l->refreshFlag & REFRESH_LIST) {
        u8 maxId = MIN((slist_size(l->list)-1),(l->firstId+l->dispItems-1));
        lcd_fill_rect(l->rect.x, l->rect.y, l->rect.w, l->rect.h, LCD_BC);
        
        for(i=l->firstId; i<=maxId; i++) {
            color = (i==l->focusId)?ITEM_FOCUS_COLOR:LCD_FC;
            //bgcolor = (i==l->focusId)?ITEM_FOCUS_BGCOLOR:LCD_BC;
            draw_item(l, i, color, LCD_BC, 0);
        }

        draw_arrow(l, LCD_FC, LCD_BC);
    }
    else if(l->refreshFlag & REFRESH_FOCUS) {
        draw_item(l, l->prev_focusId, LCD_FC, LCD_BC, 1);
        draw_item(l, l->focusId, ITEM_FOCUS_COLOR, LCD_BC, 0);

        draw_arrow(l, LCD_FC, LCD_BC);
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


int listitem_size(listitem_t *l)
{
    return l?-1:slist_size(l->list);
}

listitem_t* listitem_create(cchr *title, item_info_t *info)
{
    u8 i,n;
    listitem_t *l;
    item_info_t *inf;

    if(!info) {
        return NULL;
    }

    n = get_num(info);
    if(n==0) {
        return NULL;
    }

    l = listitem_init(title, n, sizeof(inf));
    if(!l) {
        return NULL;
    }

    for(i=0;;i++) {
        inf = info+i;
        if(inf->control==CONTROL_NONE) {
            break;
        }
        listitem_append(l, inf);
    }
    
    return l;
}


int listitem_handle(listitem_t **l, key_t key)
{
    if(!l || !*l) {
        return -1;
    }

    switch(key.value) {
        case KEY_UP:
        listitem_move(*l, UP);
        break;

        case KEY_DOWN:
        listitem_move(*l, DOWN);
        break;

        case KEY_ENTER:
        {
            listitem_t *child;
            item_info_t *info=listitem_get((*l), (*l)->focusId);
            if(info->control==CONTROL_LIST) {
                child = listitem_create(info->txt, info);
                if(child) {
                    listitem_set_child(*l, child);
                    *l = child;     //进到子列表
                    listitem_set_refresh(*l, REFRESH_ALL);
                }
            }
            else {
                (*l)->inEdit = 1;
            }
        }
        break;

        case KEY_EXIT:
        {
            listitem_t *parent=(*l)->parent;
            if(parent) {
                listitem_free(l);
                *l = parent;        //退到父列表
                listitem_set_refresh(*l, REFRESH_ALL);
            }
            else {
                //listitem_free(l);     //退出菜单，是否释放？
                listitem_clear((*l));
                listitem_reset((*l));
                //listitem_set_refresh(*l, REFRESH_ALL);
            }
        }
        break;

        case KEY_SAVE:
        if(gM==MENU_PRESET) {
            //do something ...
        }
        break;
    }

    return 0;
}





