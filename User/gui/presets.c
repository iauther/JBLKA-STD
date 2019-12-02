#include "menu.h"


menu_t *preMenu=NULL;
int presets_init(void)
{
    preMenu = 0;
    return 0;
}


int presets_free(void)
{
    preMenu = 0;
    return 0;
}


int presets_handle(u8 key)
{
    return 0;
}


int presets_refresh(void)
{
    return 0;
}


int presets_set_refresh(u32 flag)
{
    return listitem_set_refresh(preMenu->l, flag);
}


