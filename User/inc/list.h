#ifndef __LIST_H__
#define __LIST_H__

#include "types.h"


typedef struct _lnode {
    node_t          data;
    struct _lnode   *prev;
    struct _lnode   *next;
}lnode_t;

typedef struct _list {
    int             max;
    int             size;
    lnode_t         *head;
    lnode_t         *tail;
    
    void            *pool;
    lnode_t         *lfree;
}list_t;

typedef int (*literater)(list_t *l, int index, void *n1, void *n2);

list_t* list_init(int max, int node_size);

int list_add(list_t *l, node_t *n, int index);

int list_append(list_t *l, node_t *n);

int list_rmv(list_t *l, node_t *n, int index);

int list_get(list_t *l, node_t *n, int index);

int list_set(list_t *l, node_t *n, int index);

int list_iterate(list_t *l, node_t *n, literater iter);

int list_clear(list_t *l);

int list_free(list_t *l);

#endif
