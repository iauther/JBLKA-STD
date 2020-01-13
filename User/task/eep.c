#include "task.h"


#define MSG_MAX         6


#ifdef RTX
msg_t *eep_msg=NULL;

static void e2p_check(void)
{
    int r;
    node_t n;

    r = e2p_get(&n);
    if(r==0) {
        paras_write(n.ptr, n.len);
    }
}


//////////////////////////////////////////////////////////

void eep_task(void *arg)
{
    int r;
    evt_eep_t e;
    osStatus_t st;
    
    eep_msg = msg_init(MSG_MAX, sizeof(e));
    if(!eep_msg) {
        return;
    }

    while(1) {
        r = msg_recv(eep_msg, &e, sizeof(e));
        if(r==0) {
            switch(e.evt) {
                
                case EVT_E2P:
                {
                    e2p_check();
                }
                break;                
            }
        }
    }

    msg_free(&eep_msg);
}


int eep_post_evt(evt_eep_t *e)
{
    return msg_post(eep_msg, e, sizeof(*e));
}

#endif




