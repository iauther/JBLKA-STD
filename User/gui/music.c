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
int music_init(void)
{
    u8 i;
    node_t n;
    rect_t rect=MENU_RECT;
    item_data_t e;
    listitem_t *pl;

    if(mscList) {
        return 0;
    }
    
    mscList = listitem_init("MUSIC", &rect, 6, sizeof(e));
    if(!mscList) {
        return -1;
    }

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
    listitem_handle(mscList, key);

    return 0;
}


int music_refresh(void)
{
    listitem_refresh(mscList);
    return 0;
}
