#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "types.h"

typedef struct {
	node_t      *nodes;		// array to store queue elements
	int         max;	    // maximum capacity of the queue
	int         cnt;		// element counter in the queue
	int         head;		// head points to head element in the queue (if any)
	int         tail;		// tail points to last element in the queue
}queue_t;


queue_t* queue_init(int max);

int queue_free(queue_t **q);

int queue_cnt(queue_t *q);

int queue_head(queue_t *q, node_t *n);

int queue_get(queue_t *q, node_t *n);

int queue_put(queue_t *q, node_t *n, int unique);

int queue_clear(queue_t *q);

#endif
