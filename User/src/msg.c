#include <stdlib.h>
#include "msg.h"

#define FLAGS_MASK   0x00000001

static int msg_put(msg_t *m, void *ptr, int len)
{
    void *p;
    osStatus_t st;
    
    st = osMessageQueuePut(m->mq, ptr, 0, 0);
    if(st!=osOK) {
        return -1;
    }
    
    return 0;
}

msg_t* msg_init(int max, int msg_size)
{
    msg_t *m = (msg_t*)malloc(sizeof(msg_t));
    if(!m) {
        return NULL;
    }
    m->mq = osMessageQueueNew(max, msg_size, NULL);
    m->ef = osEventFlagsNew(NULL);
    m->ack = 0;
    
    return m;
}


int msg_send(msg_t *m, void *ptr, int len)
{
    int r;
    osStatus_t st;
    
    r = msg_put(m, ptr, len);
    if(r==0) {
        st = osEventFlagsWait(m->ef, FLAGS_MASK, osFlagsWaitAny, osWaitForever);
    }
    
    return r;
}


int msg_post(msg_t *m, void *ptr, int len)
{
    int r;
    
    r = msg_put(m, ptr, len);
    
    return r;
}


int msg_recv(msg_t *m, void *ptr, int *len)
{
    void *p;
    osStatus_t st;
    
    st = osMessageQueueGet(m->mq, ptr, NULL, osWaitForever);
    
    return 0;
}


int msg_ack(msg_t *m)
{
    osStatus_t st;
    
    if(m->ack) {
        osEventFlagsSet(m->ef, FLAGS_MASK);
    }
    return 0;
}


int msg_reset(msg_t *m)
{
    osStatus_t st;
    
    st = osMessageQueueReset(m->mq);
    return 0;
}


int msg_free(msg_t **m)
{
    osStatus_t st;
    
    osMessageQueueDelete((*m)->mq);
    osEventFlagsDelete((*m)->ef);
    free(*m);
    
    return 0;
}