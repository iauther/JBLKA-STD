#ifndef __MSG_H__
#define __MSG_H__

#include "types.h"
#include "queue.h"
#include "cmsis_os2.h"

typedef struct {
    osMessageQueueId_t  mq;
    osEventFlagsId_t    ef;
    osMutexId_t         mtx;
    u8                  ack;
}msg_t;


msg_t* msg_init(int max, int msg_size);

int msg_send(msg_t *m, void *ptr, int len);

int msg_post(msg_t *m, void *ptr, int len);

int msg_recv(msg_t *m, void *ptr, int len);

int msg_reset(msg_t *m);

int msg_free(msg_t **m);

#endif
