#include "menu.h"


u8 gMenu=MENU_HOME;
u8 gMenuPrev=MENU_HOME;
#define INIT_FUNC(XX) \
    {\
        XX##_init, \
        XX##_free, \
        XX##_handle, \
        XX##_refresh, \
    }

func_t fn_funcs[MENU_MAX]={
    INIT_FUNC(home),
    INIT_FUNC(mic),
    INIT_FUNC(music),
    INIT_FUNC(effect),
    INIT_FUNC(presets),
};





int menu_clear(void)
{
    rect_t r=MENU_RECT;
    
    lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_BC);
    return 0;
}




/////////////////////////////////////////


int menu_init(void)
{
    topbar_init(TOPBAR_HEIGHT);
    fn_funcs[gMenu].init(NULL);
    
    return 0;
}


int menu_refresh(void)
{
    topbar_refresh();

    if(gMenu!=gMenuPrev) {
        fn_funcs[gMenuPrev].free(NULL);
        fn_funcs[gMenu].init(NULL);
    }

    fn_funcs[gMenu].refresh(NULL);
    return 0;
}


int menu_handle(key_t *k)
{
    fn_funcs[gMenu].handle(NULL, k);
    
    return 0;
}
//////////////////////////////////////////////

























