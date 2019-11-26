#include "menu.h"


typedef struct {
    rect_t      rect;
    listitem_t  *pl;
    //slider_t    *sl[];
}home_t;

home_t mHome={
    .rect = MENU_RECT,
    .pl = NULL
};










int home_init(void *p)
{
    element_t e;
    mHome.pl = listitem_init(&mHome.rect, 6);

    listitem_add(mHome.pl, &e);
    listitem_add(mHome.pl, &e);
    listitem_add(mHome.pl, &e);
    listitem_add(mHome.pl, &e);
    listitem_add(mHome.pl, &e);
    listitem_add(mHome.pl, &e);
    
    return 0;
}


int home_free(void *p)
{
    listitem_free(mHome.pl);
    free(mHome.pl);
    return 0;
}


int home_handle(void *p, u8 key)
{
    //listitem_handle(mHome.pl, k);
    return 0;
}


int home_refresh(void *p)
{
    
    return 0;
}

