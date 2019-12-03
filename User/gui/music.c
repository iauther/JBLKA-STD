#include "menu.h"
#include "listitem.h"

#define ITEM_MAX    6
menu_t *mscMenu=NULL;
int music_init(void)
{
    u8 i;
    node_t n;
    rect_t rect=MENU_RECT;
    item_info_t info;
    listitem_t *l;
    queue_t *q;

    if(mscMenu) {
        listitem_set_refresh(mscMenu->l, REFRESH_ALL);
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

    l = listitem_init("MUSIC", &rect, ITEM_MAX, sizeof(info));
    if(!l) {
        free(mscMenu);
        return -1;
    }
    mscMenu->l = l;
    mscMenu->q = q;

    menu_add_item(l, MENU_MUSIC);

    return 0;
}


int music_free(void)
{
    return 0;
}


int music_handle(key_t key)
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
            if(key.value==KEY_ENTER) {
                n.ptr = &it1;
                n.len = sizeof(it1);
                listitem_get_focus(l, &n);
                queue_put(mscMenu->q, &n, NULL);
            }
            else if(key.value==KEY_EXIT) {
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
            if(key.value==KEY_ENTER || key.value==KEY_EXIT) {
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
    return listitem_refresh(mscMenu->l);
}

int music_set_refresh(u32 flag)
{
    return listitem_set_refresh(mscMenu->l, flag);
}
