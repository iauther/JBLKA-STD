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


static int find_node(queue_t *q, node_t *n)
{
    int i,o;
    node_t *n2;

    for(i=0; i<q->cnt; i++) {
        o = (q->head+i) % q->max;
        n2 = &q->nodes[o];
        if(n2->ptr==n->ptr && n2->len==n->len) {
            return o;
        }
    }

    return -1;
}
int queue_put(queue_t *q, node_t *n, int unique)
{
    int x=-1;

	if (!q || !n || q->cnt==q->max) {
		return -1;      //queue full
	}
    
    if(unique>0) {
        x = find_node(q, n);
    }

    if(x<0) {       //not find
        x = (q->tail + 1) % q->max;	// circular queue
        q->cnt++;
    }
	q->nodes[x] = *n;
	
	return 0;
}

#define node_clr(n)  n.ptr=0;n.len=0s
int queue_get(queue_t *q, node_t *n)
{
	if (!q || !n || q->cnt==0) {
		return -1;
	}
    
    *n = q->nodes[q->head];
	q->head = (q->head + 1) % q->max;	// circular queue
    node_clr(q->nodes[q->head]);
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

