#include <stdlib.h>
#include <string.h>
#include "msg.h"

#define FLAGS_MASK   0x00000001

static int msg_put(msg_t *m, void *ptr, int len)
{
    void *p;

#ifdef RTX
    osStatus_t st;
    if(!m || !ptr || !len) {
        return -1;
    }

    st = osMessageQueuePut(m->mq, ptr, 0, 0);
    if(st!=osOK) {
        return -1;
    }
#endif
   
    return 0;
}

msg_t* msg_init(int max, int msg_size)
{
    msg_t *m=NULL;
    
    m = (msg_t*)calloc(1, sizeof(msg_t));
    if(!m) {
        return NULL;
    }

#ifdef RTX
    m->mq = osMessageQueueNew(max, msg_size, NULL);
    m->ef = osEventFlagsNew(NULL);
    m->ack = 0;
#endif

    return m;
}


int msg_send(msg_t *m, void *ptr, int len)
{
    int r;
    
    if(!m || !ptr || !len) {
        return -1;
    }

#ifdef RTX
    osStatus_t st;
    r = msg_put(m, ptr, len);
    if(r==0) {
        st = osEventFlagsWait(m->ef, FLAGS_MASK, osFlagsWaitAny, osWaitForever);
    }
#endif

    return r;
}


int msg_post(msg_t *m, void *ptr, int len)
{
    int r=0;

    if(!m || !ptr || !len) {
        return -1;
    }

#ifdef RTX
    r = msg_put(m, ptr, len);
#endif

    return r;
}


int msg_recv(msg_t *m, void *ptr, int len)
{
    int r=0;
    void *p=NULL;

    if(!m || !ptr || !len) {
        return -1;
    }

#ifdef RTX
    osStatus_t st;
    st = osMessageQueueGet(m->mq, ptr, NULL, osWaitForever);
    if(st!=osOK) {
        return -1;
    }
#endif

    return r;
}


int msg_ack(msg_t *m)
{
    int r=0;

    if(!m) {
        return -1;
    }

#ifdef RTX
    osStatus_t st;
    if(m->ack) {
        osEventFlagsSet(m->ef, FLAGS_MASK);
        r = (st==osOK)?0:-1;
    }
#endif

    return r;
}


int msg_reset(msg_t *m)
{
    int r=0;

    if(!m) {
        return -1;
    }

#ifdef RTX
    osStatus_t st;
    st = osMessageQueueReset(m->mq);
    r = (st==osOK)?0:-1;
#endif

    return 0;
}


int msg_free(msg_t **m)
{
    int r=0;

    if(!m) {
        return -1;
    }

#ifdef RTX
    osStatus_t st;
    osMessageQueueDelete((*m)->mq);
    osEventFlagsDelete((*m)->ef);
    free(*m);
#endif

    return 0;
}