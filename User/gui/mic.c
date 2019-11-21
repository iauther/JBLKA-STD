#include "menu.h"


u8 micMenu=0;
int mic_init(void *p)
{
    micMenu = 0;
    return 0;
}


int mic_free(void *p)
{
    micMenu = 0;
    return 0;
}



int mic_handle(void *p, key_t *k)
{
    return 0;
}


int mic_refresh(void *p)
{
    return 0;
}

