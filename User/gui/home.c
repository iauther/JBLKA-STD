#include "menu.h"


typedef struct {
    rect_t      rect;
    listitem_t  *pl;
}home_t;

home_t mHome={
    .rect = MENU_RECT,
    .pl = NULL
};



int home_init(void)
{
    
    return 0;
}


int home_free(void)
{
    return 0;
}


int home_handle(u8 key)
{
    //listitem_handle(mHome.pl, key);
    return 0;
}


int home_refresh(void)
{
    
    return 0;
}

