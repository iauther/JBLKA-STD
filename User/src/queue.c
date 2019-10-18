#include <stdio.h>
#include <stdlib.h>
#include "queue.h"


queue_t* queue_init(int max)
{
	queue_t *q = (queue_t*)malloc(sizeof(queue_t));
	
	q->nodes = (node_t*)malloc(max * sizeof(node_t));
	q->max  = max;
	q->cnt = 0;
	q->head = 0;
	q->tail = -1;

	return q;
}


int queue_free(queue_t **q)
{
    if(!q) {
        return -1;
    }

    free(*q);
    free((*q)->nodes);
    
    return 0;
}


int queue_cnt(queue_t *q)
{
	return q?q->cnt:-1;
}



int queue_head(queue_t *q, node_t *n)
{
	if (!q || !n || q->cnt==0) {
		return -1;
	}

	*n = q->nodes[q->head];
    return 0;
}


int queue_put(queue_t *q, node_t *n)
{
	if (!q || !n || q->cnt==q->max) {
		return -1;      //queue full
	}
    
	q->tail = (q->tail + 1) % q->max;	// circular queue
	q->nodes[q->tail] = *n;
	q->cnt++;
	
	return 0;
}


int queue_get(queue_t *q, node_t *n)
{
	if (!q || !n || q->cnt==0) {
		return -1;
	}
    
	q->head = (q->head + 1) % q->max;	// circular queue
    *n = q->nodes[q->head];
	q->cnt--;
	
	return 0;
}


int queue_clear(queue_t *q)
{
    if (!q) {
		return -1;
	}
    
    q->cnt = 0;
	q->head = 0;
	q->tail = -1;

    return 0;
}

