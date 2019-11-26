#include "menu.h"
#include "listitem.h"

u8 mscMenu=0;
typedef struct {
    rect_t     rect;
    listitem_t *li;
}music2_t;

music2_t *mMusic=NULL;
int music_init(void *p)
{
    return 0;
}


int music_free(void *p)
{
    return 0;
}


int music_handle(void *p, u8 key)
{
    return 0;
}


int music_refresh(void *p)
{
    //listitem_refresh();
    return 0;
}
