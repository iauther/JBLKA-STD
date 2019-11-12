#include "task.h"


#define MSG_MAX         6

#ifdef RTX
osTimerId_t dev_tmr;
msg_t *dev_msg=NULL;
status_data_t gStatus;

extern paras_data_t gParams;

static void e2p_proc(void)
{
    int r;
    node_t n;

    r = e2p_get(&n);
    if(r==0) {
        paras_write(n.ptr, n.len);
    }
}
static void tmr1_fun (void *arg)
{
    evt_dev_t e={0};
    e.evt = EVT_TIMER;
    msg_post(dev_msg, &e, sizeof(e));
}
//////////////////////////////////////////////////////////


void dev_task(void *arg)
{
    int r;
    u8  i;
    evt_dev_t e;
    osStatus_t st;
    
    dev_msg = msg_init(MSG_MAX, sizeof(e));
    dev_tmr = osTimerNew(tmr1_fun, osTimerPeriodic, NULL, NULL);
    osTimerStart(dev_tmr, 1000);

    while(1) {
        r = msg_recv(dev_msg, &e, NULL);
        if(r==0) {
            switch(e.evt) {
                
                case EVT_TIMER:
                {
                    e2p_proc();
                }
                break;
                
                case EVT_DSP:
                {
                    //
                    i++;
                }
                break;
            }
        }
    }

    osTimerDelete(dev_tmr);
    msg_free(&dev_msg);
}


void dev_post_evt(evt_dev_t *e)
{
    msg_post(dev_msg, e, sizeof(*e));
}

#endif




