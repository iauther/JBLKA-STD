#include "menu.h"
#include "listitem.h"


static void item_add(listitem_t *l, dsp_item_t *dsp)
{
    node_t n;

    if(l && dsp) {
        listitem_add(l, &n);
    }

}



listitem_t *mscList=NULL;
int music_init(void *p)
{
    node_t n;
    rect_t rect=MENU_RECT;
    item_data_t e;
    listitem_t *pl;
    mscList = listitem_init("MUSIC", &rect, 6, sizeof(e));
    if(!mscList) {
        return -1;
    }
    
    gb.pl = mscList;

    return 0;
}


int music_free(void *p)
{
    return 0;
}


int music_handle(void *p, u8 key)
{
    listitem_handle(gb.pl, key);

    return 0;
}


int music_refresh(void *p)
{
    listitem_refresh(gb.pl);
    return 0;
}
