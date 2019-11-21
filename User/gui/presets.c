#include "menu.h"


u8 preMenu=0;
int presets_init(void *p)
{
    preMenu = 0;
    return 0;
}


int presets_free(void *p)
{
    preMenu = 0;
    return 0;
}


int presets_handle(void *p, key_t *k)
{
    return 0;
}


int presets_refresh(void *p)
{
    return 0;
}

