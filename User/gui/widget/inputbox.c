#include "inputbox.h"

#define LABEL_RECT  {1,1,1,1}
#define BOX_RECT    {2,1,1,1}


static int show_label(inputbox_t *i)
{
    //lcd_fill_rect();
    //lcd_draw_str();   //label
    //lcd_draw_rect();  //input box
    //lcd_draw_str();   //value
    //lcd_draw_str();   //unit
    
    return 0;
}
static int show_value(inputbox_t *i)
{
    //lcd_draw_str();   //label
    //lcd_draw_rect();  //input box
    //lcd_draw_str();   //value
    //lcd_draw_str();   //unit
    
    return 0;
}
static int show_unit(inputbox_t *i)
{
    //lcd_fill_rect();
    //lcd_draw_rect();  //input box
    //lcd_draw_str();   //value
    //lcd_draw_str();   //unit
    
    return 0;
}
static int input_show_all(inputbox_t *i)
{
    show_label(i);
    show_value(i);
    show_unit(i);
    
    return 0;
}
/////////////////////////////////////////////

inputbox_t *inputbox_init(rect_t *rect, para_item_t *para)
{
    inputbox_t *i=(inputbox_t*)malloc(sizeof(inputbox_t));
    if(!i) {
        return NULL;
    }

    i->type  = 0;//TYPE_INPUT;
    
    return i;
}



int inputbox_free(inputbox_t **i)
{
    if(!*i) {
        return -1;
    }
    
    free(*i);
    
    return 0;
}


int inputbox_refresh(inputbox_t *i)
{
    switch(i->flag) {
        
        case 0:
        input_show_all(i);
        break;
        
        case 1:
        break;
        
        case 2:
        break;
    }
    
    return 0;
}


int inputbox_handle(inputbox_t *i, u8 key)
{
    switch(key) {
        
        case KEY_UP:
        break;
        
        case KEY_DOWN:
        break;

        case KEY_ENTER:
        break;
        
        case KEY_EXIT:
        break;
    }
    
    return 0;
}




