#include "menu.h"
#include "listitem.h"

#define ITEM_MAX    6
menu_t *mscMenu=NULL;
listitem_t *mscList=NULL;
int music_init(void)
{
    u8 i;
    node_t n;
    rect_t rect=MENU_RECT;
    item_data_t e;
    listitem_t *l;
    queue_t *q;

    if(mscMenu) {
        return 0;
    }
    
    mscMenu = malloc(sizeof(menu_t));
    if(!mscMenu) {
        return -1;
    }

    q = queue_init(ITEM_MAX, sizeof(item_t));
    if(!q) {
        free(mscMenu);
        return -1;
    }

    l = listitem_init("MUSIC", &rect, ITEM_MAX, sizeof(e));
    if(!l) {
        free(mscMenu);
        return -1;
    }
    mscMenu->l = l;
    mscMenu->q = q;

    menu_add_item(mscList, MENU_MUSIC);
    gb.pl = mscList;

    return 0;
}


int music_free(void)
{
    return 0;
}


int music_handle(u8 key)
{
    int r;
    node_t n;
    item_t it,it1;

    n.ptr = &it;
    n.len = sizeof(it);
    r = queue_peer(mscMenu->q, &n);
    if(r) {
        return -1;
    }

    switch(it.control) {
        case CONTROL_LIST:
        {
            listitem_t *l=(listitem_t*)it.handle;
            if(key==KEY_ENTER) {
                n.ptr = &it1;
                n.len = sizeof(it1);
                listitem_get_focus(l, &n);
                queue_put(mscMenu->q, &n, NULL);
            }
            else if(key==KEY_EXIT) {
                queue_pop(mscMenu->q);
            }
            else {
                listitem_handle(l, key);
            }
        }
        break;
        
        case CONTROL_INPUTBOX:
        {
            inputbox_t *box=(inputbox_t*)it.handle;
            if(key==KEY_ENTER || key==KEY_EXIT) {
                queue_pop(mscMenu->q);
            }
            else {
                inputbox_handle(box, key);
            }
        }
        break;
    }


    return 0;
}


int music_refresh(void)
{
    listitem_refresh(mscList);
    return 0;
}
