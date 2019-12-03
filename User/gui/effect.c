#include "menu.h"


menu_t *effMenu=NULL;
int effect_init(void)
{
    effMenu = 0;
    return 0;
}


int effect_free(void)
{
    effMenu = 0;
    return 0;
}


int effect_handle(key_t key)
{
    return 0;
}


int effect_refresh(void)
{
    return 0;
}


int effect_set_refresh(u32 flag)
{
    return listitem_set_refresh(effMenu->l, flag);
}


