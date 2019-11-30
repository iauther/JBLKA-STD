#include "menu.h"


u8 micMenu=0;
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

