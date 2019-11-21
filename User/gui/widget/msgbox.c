#include "msgbox.h"
#include "cmsis_os2.h"


msgbox_t mbox;
u8 msgbox_flag=0;
osTimerId_t msg_tmr=NULL;
static void msg_tmr_fun(void *arg)
{
    //msg_clear();
}



int msgbox_init(rect_t *rect, s8 *title, s8 *txt, s8 sec)
{
    if(msgbox_flag==1) {
        return -1;
    }
    
    mbox.title = title;
    mbox.rect  = *rect;
    mbox.txt   = txt;
    mbox.sec   = sec;
    
    if(msg_tmr) {
        msg_tmr = osTimerNew(msg_tmr_fun, osTimerOnce, NULL, NULL);
    }
    
    if(mbox.sec>0 && msg_tmr) {
        osTimerStart(msg_tmr, mbox.sec*1000);
    }
    msgbox_flag = 1;
    
    return 0;
}


int msgbox_clear()
{
    
    msgbox_flag = 0;
    
    return 0;
}


int msgbox_show()
{
    //draw box framework
//    lcd_draw_rect(mbox.rect.x, mbox.rect.y, mbox.rect.w, mbox.rect.h, );
    
    //draw title
    //lcd_draw_str();
    
    //draw button
    //lcd_fill_rect();
    
    //draw button text
    //lcd_draw_str();
    
    msgbox_flag = 1;
    
    return 0;
}


int msgbox_update()
{
    
    
    
    return 0;
}


int msgbox_handle(key_t *k)
{
    if(mbox.sec>0 && msgbox_flag>0) {
        switch(k->value) {
            case KEY_UP:
            break;
            
            case KEY_DOWN:
            break;
        }
    }
    
    return 0;
}




