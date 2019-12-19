#include "task.h"


#define MSG_MAX         6

#ifdef RTX
msg_t *e2p_msg=NULL;

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
    u8  i;
    evt_eep_t e;
    osStatus_t st;
    
    e2p_msg = msg_init(MSG_MAX, sizeof(e));
    if(!e2p_msg) {
        return;
    }

    while(1) {
        r = msg_recv(e2p_msg, &e, sizeof(e));
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

    msg_free(&e2p_msg);
}


void eep_post_evt(evt_eep_t *e)
{
    msg_post(e2p_msg, e, sizeof(*e));
}

#endif




