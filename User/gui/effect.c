#include "menu.h"


u8 effMenu=0;
int effect_init(void *p)
{
    effMenu = 0;
    return 0;
}


int effect_free(void *p)
{
    effMenu = 0;
    return 0;
}


int effect_handle(void *p, u8 key)
{
    return 0;
}


int effect_refresh(void *p)
{
    return 0;
}
