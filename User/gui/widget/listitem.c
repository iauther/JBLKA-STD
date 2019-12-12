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
    l->trigger = NULL;
    l->parent = NULL;
    l->child  = NULL;

    l->inEdit = 0;
}
static void set_refresh(listitem_t *l, u32 flag)
{

    l->refreshFlag |= flag;
    if(l->trigger) l->trigger();
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

    set_refresh(l, flag);

    return 0;
}


static void draw_paras_label(listitem_t *l, u8 index, item_info_t *info, para_info_t *pinfo, rect_t *rect, u16 color, u16 bgcolor)
{
    rect_t r=*rect;

    r.w = rect->w/3;
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)pinfo->name, FONT_16, color, bgcolor, ALIGN_RIGHT, 0);
}
static void draw_paras_value(listitem_t *l, u8 index, item_info_t *info, para_info_t *pinfo, rect_t *rect, u16 color, u16 bgcolor, u8 drawbox)
{
    char *ptxt;
    char tmp[20];
    rect_t r=*rect;
    s16 v=*(*(s16**)l->data+index);

    r.x = rect->w/3+10;
    r.y += 2;
    r.w = rect->w/3-10;
    r.h -= 4;
    if(pinfo->ptxt) {
        if(info->cmd==CMD_ID_PitchShift) {
            ptxt = (char*)pinfo->ptxt->txt[v+5];
        }
        else {
            ptxt = (char*)pinfo->ptxt->txt[v];
        }
    }
    else{
        if(pinfo->flt) {
            sprintf(tmp, "%.1f", (f32)v/pinfo->step);
        }
        else {
            sprintf(tmp, "%d", v/(int)pinfo->step);
        }
        ptxt = tmp;
    }

    if(drawbox) {
        lcd_draw_rect(r.x, r.y, r.w, r.h, color);
    }
    lcd_fill_rect(r.x+1, r.y+1, r.w-2, r.h-2, bgcolor);
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)ptxt, FONT_16, color, bgcolor, ALIGN_MIDDLE, 0);
}
static void draw_paras_unit(listitem_t *l, u8 index, item_info_t *info, para_info_t *pinfo, rect_t *rect, u16 color, u16 bgcolor)
{
    rect_t r=*rect;

    r.x = rect->w*2/3+6;
    r.w = rect->w/3-4;
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)pinfo->unit, FONT_16, color, bgcolor, ALIGN_LEFT, 0);
}

static void draw_value(listitem_t *l, int id, u16 color, u16 bgcolor)
{
    u8 index=l->focusId-l->firstId;
    rect_t r=INPUTBOX_RECT;
    item_info_t *info=listitem_get(l, id);
    para_info_t *pinfo=(para_info_t*)&PARA_INFO[info->cmd].info[index];
    
    r.h = ITEM_HEIGHT;
    r.y = r.y+index*ITEM_HEIGHT;
    draw_paras_value(l, index, info, pinfo, &r, color, bgcolor, 0);
}
static void draw_paras(listitem_t *l, u8 index, int id, item_info_t *info, u16 color, u16 bgcolor)
{
    rect_t r=INPUTBOX_RECT;
    para_info_t *pinfo=(para_info_t*)&PARA_INFO[info->cmd].info[id];
    
    r.h = ITEM_HEIGHT;
    r.y = r.y+index*ITEM_HEIGHT;
    draw_paras_label(l, index, info, pinfo, &r, color, bgcolor);
    draw_paras_value(l, index, info, pinfo, &r, color, bgcolor, 1);
    draw_paras_unit (l, index, info, pinfo, &r, color, bgcolor);
}


static void draw_item(listitem_t *l, int id, u16 color, u16 bgcolor, u8 clear)
{
    u8 r=6,index=id-l->firstId;
    item_info_t *inf=listitem_get(l, id);

    if(inf->control==CONTROL_LIST) {
        if(clear) {
            lcd_fill_round_rect(l->rect.x+1, l->rect.y+index*ITEM_HEIGHT+1, l->rect.w-2, ITEM_HEIGHT-2, r, bgcolor);
        }
        lcd_draw_round_rect(l->rect.x, l->rect.y+index*ITEM_HEIGHT, l->rect.w, ITEM_HEIGHT, r, color);
        lcd_draw_string_align(l->rect.x, l->rect.y+index*ITEM_HEIGHT, l->rect.w, ITEM_HEIGHT, (u8*)inf->txt, FONT_24, color, bgcolor, ALIGN_MIDDLE, 0);
    }
    else if(inf->control==CONTROL_INPUTBOX){
        draw_paras(l, index, id, inf, color, bgcolor);
    }
}
static void draw_title(listitem_t *l, u16 font_color, u16 bgcolor)
{
    rect_t r=TITLE_RECT;
    lcd_fill_rect(r.x, r.h, r.w, r.h, bgcolor);
    lcd_draw_string_align(r.x, r.y, r.w, r.h, (u8*)l->title, FONT_24, font_color, bgcolor, ALIGN_MIDDLE, 0);
    //lcd_draw_line(r.x, r.y+r.h+4, r.w, r.h, font_color);
}

static void draw_arraw(listitem_t *l, u8 dir, u16 color)
{
    u8 wh=6,offset=2;
    u16 x,y,x1,y1,x2,y2;
    rect_t r=l->rect;
    
    if(dir==UP) {
        x = r.x+r.w/2;
        y = r.y-wh-offset;
        x1 = r.x+r.w/2-wh/2;
        y1 = r.y-offset;
        x2 = r.x+r.w/2+wh/2;
        y2 = r.y-offset;
    }
    else {
        x = r.x+r.w/2;
        y = r.y+l->dispItems*ITEM_HEIGHT+wh+offset;
        x1 = r.x+r.w/2-wh/2;
        y1 = r.y+l->dispItems*ITEM_HEIGHT+offset;
        x2 = r.x+r.w/2+wh/2;
        y2 = r.y+l->dispItems*ITEM_HEIGHT+offset;
    }

    lcd_draw_line(x, y, x1, y1, color);
    lcd_draw_line(x, y, x2, y2, color);
}

static void draw_arrows(listitem_t *l, u16 color, u16 bgcolor)
{
    int size=listitem_size(l);
    //return;
    if(l->firstId>0) {
        draw_arraw(l, UP, color);
    }
    else {
        draw_arraw(l, UP, bgcolor);
    }

    if(l->firstId+l->dispItems<size) {
        draw_arraw(l, DOWN, color);
    }
    else {
        draw_arraw(l, DOWN, bgcolor);
    }
}


static int list_refresh(listitem_t *l)
{
    u8  i,maxId;

    if(!l) {
        return -1;
    }
    
    if(l->refreshFlag & REFRESH_TITLE) {
        draw_title(l, LCD_FC, LCD_BC);
    }

    if(l->refreshFlag & REFRESH_LIST) {
        rect_t r=INPUTBOX_RECT;
        maxId = MIN((slist_size(l->list)-1),(l->firstId+l->dispItems-1));
        lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_BC);
        for(i=l->firstId; i<=maxId; i++) {
            if(i!=l->focusId) {
                draw_item(l, i, LCD_FC, LCD_BC, 0);
            }
        }
        draw_item(l, l->focusId, ITEM_FOCUS_COLOR, ITEM_FOCUS_BGCOLOR, 1);

        draw_arrows(l, LCD_FC, LCD_BC);
    }
    else {
        if(l->refreshFlag & REFRESH_MOVE) {
            draw_item(l, l->prev_focusId, LCD_FC, LCD_BC, 1);
            draw_item(l, l->focusId, ITEM_FOCUS_COLOR, ITEM_FOCUS_BGCOLOR, 1);

            draw_arrows(l, LCD_FC, LCD_BC);
        }
    
        if(l->refreshFlag & REFRESH_FOCUS) {
            draw_item(l, l->focusId, ITEM_FOCUS_COLOR, ITEM_FOCUS_BGCOLOR, 1);
        }

        if(l->refreshFlag & REFRESH_VALUE) {
            draw_value(l, l->focusId, ITEM_EDIT_COLOR, LCD_BC);
        }

        if(l->refreshFlag & REFRESH_TXT_SCROLL) {

        }
    }
    l->prev_firstId = l->firstId;
    l->prev_focusId = l->focusId;
    l->refreshFlag = 0;
    
    return 0;
}
int listitem_refresh(listitem_t *l)
{
    int r=0;

    if(msgbox_is_show()) {
        msgbox_refresh();
    }
    else {
        r = list_refresh(l);
    }

    return r;
}


int listitem_clear(void)
{
    rect_t r1=TITLE_RECT;
    rect_t r2=INPUTBOX_RECT;

    lcd_fill_rect(r1.x, r1.y, r1.w, r1.h, LCD_BC);
    lcd_fill_rect(r2.x, r2.y, r2.w, r2.h, LCD_BC);

    return 0;
}


static int adjust_value(listitem_t *l, u8 dir, u8 size)
{
    u8 index=l->focusId;
    s16 *pv=*(s16**)l->data+index;
    item_info_t *info=listitem_get(l, l->focusId);
    para_info_t *pinfo=(para_info_t*)&PARA_INFO[info->cmd].info[index];
    
    if(dir==UP) {
        if(*pv==pinfo->min) {
            return -1;
        }

        if(*pv-pinfo->step*size <= pinfo->min) {
            *pv = pinfo->min;
        }
        else {
            *pv -= pinfo->step*size;
        }
    }
    else {
        if(*pv==pinfo->max) {
            return -1;
        }

        if(*pv+pinfo->step*size >= pinfo->max) {
            *pv = pinfo->max;
        }
        else {
            *pv += pinfo->step*size;
        }
    }
    return 0;
}
int listitem_move(listitem_t *l, u8 dir, u8 size)
{
    int r;

    if(!l) {
        return -1;
    }

    if(l->inEdit) {
        r = adjust_value(l, dir, size);
        if(r==0) {
            set_refresh(l, REFRESH_VALUE);
        }
    }
    else {
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
            set_refresh(l, REFRESH_LIST);
        }
        else if(l->focusId != l->prev_focusId){
            set_refresh(l, REFRESH_MOVE);
        }
    }

    return 0;
}


int listitem_size(listitem_t *l)
{
    return l?slist_size(l->list):-1;
}


int listitem_set_trigger(listitem_t *l, trigger_fn trigger)
{
    if(!l) {
        return -1;
    }
    l->trigger = trigger;

    return 0;
}


listitem_t* listitem_create(cchr *title, item_info_t *info, void *data)
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
    
    l->data = data;
    for(i=0;;i++) {
        inf = info+i;
        if(inf->control==CONTROL_NONE) {
            break;
        }
        listitem_append(l, inf);
    }
    
    return l;
}

const cchr *MSGBOX_TXT[2]={
    "LOAD THIS PRESET?",
    "SAVE THIS PRESET?",
};
static int list_handle(listitem_t **l, key_t *key)
{
    switch(key->value) {
        case KEY_UP:
        listitem_move(*l, UP, key->times);
        break;

        case KEY_DOWN:
        listitem_move(*l, DOWN, key->times);
        break;

        case KEY_ENTER:
        {
            listitem_t *child;
            item_info_t *info=listitem_get((*l), (*l)->focusId);
            if(gM==MENU_PRESET) {
                msgbox_show("WARNNING", (char *)MSGBOX_TXT[0], 0, (*l)->trigger);
                
            }
            else {
                if(info->control==CONTROL_LIST) {
                    child = listitem_create(info->txt, (item_info_t*)info->info, info->data);
                    if(child) {
                        listitem_set_child(*l, child);
                        *l = child;     //进到子列表
                        set_refresh(*l, REFRESH_ALL);
                    }
                }
                else {
                    if(!(*l)->inEdit) {
                        (*l)->inEdit = 1;
                        (*l)->refreshFlag |= REFRESH_VALUE;
                        set_refresh(*l, REFRESH_VALUE);
                    }
                }
            }
        }
        break;

        case KEY_EXIT:
        {
            listitem_t *pl=*l;
            listitem_t *parent=(*l)->parent;

            if(pl->inEdit) {
                pl->inEdit = 0;
                set_refresh(pl, REFRESH_FOCUS);
            }
            else {
                if(parent) {
                    listitem_free(&pl);
                    *l = parent;        //退到父列表
                    set_refresh(*l, REFRESH_ALL);
                }
                else {
                    //listitem_free(l);     //退出菜单，是否释放？
                    listitem_reset((*l));
                    listitem_clear();
                    //(*l)->refreshFlag |= REFRESH_ALL;
                    gM = MENU_HOME;
                }
            }
        }
        break;

        case KEY_SAVE:
        if(gM==MENU_PRESET) {
            //do something ...
            msgbox_show("WARNNING", (char *)MSGBOX_TXT[1], 0, (*l)->trigger);
        }
        break;

        default:
        return -1;
    }

    return 0;
}
int listitem_handle(listitem_t **l, key_t *key)
{
    int r=0;

    if(!l || !(*l) || !key) {
        return -1;
    }

    if(msgbox_is_show()) {
        r = msgbox_handle(key);
        if(gM==MENU_PRESET) {
            if(key->value==KEY_EXIT) {
                msgbox_clear();
                set_refresh(*l, REFRESH_LIST);
            }
            else if(key->value==KEY_ENTER){
                //LOAD PRESET
                set_refresh(*l, REFRESH_LIST);
            }
            else if(key->value==KEY_SAVE){
                //SAVE PRESET
                set_refresh(*l, REFRESH_LIST);
            }
        }
    }
    else {
        r = list_handle(l, key);
    }

    return r;
}





