#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "types.h"

typedef struct {
	void       **nodes;		// array to store queue elements
	int         max;	    // maximum capacity of the queue
	int         size;		// element counter in the queue
	int         head;		// head points to head element in the queue (if any)
	int         tail;		// tail points to last element in the queue

    int         quit;
    int         locked;
    int         node_size;
    
}queue_t;


typedef int (*iterater)(queue_t *q, int index, void *n, void *n2);


queue_t* queue_init(int max, int node_size);

int queue_free(queue_t **q);

int queue_size(queue_t *q);

int queue_capacity(queue_t *q);

int queue_get(queue_t *q, void *n, iterater iter);

int queue_put(queue_t *q, void *n, iterater iter);

int queue_iterate(queue_t *q, void *n, iterater iter);

int queue_clear(queue_t *q);

#endif
