#include "menu.h"


menu_t *micMenu=NULL;
int mic_init(void)
{
    micMenu = 0;
    return 0;
}


int mic_free(void)
{
    micMenu = 0;
    return 0;
}



int mic_handle(u8 key)
{
    return 0;
}


int mic_refresh(void)
{
    return 0;
}


int mic_set_refresh(u32 flag)
{
    return listitem_set_refresh(micMenu->l, flag);
}

