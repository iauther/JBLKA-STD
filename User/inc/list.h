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
    u8              quit;
}list_t;

list_t* list_init(int max, int node_size);

int list_add(list_t *l, int index, node_t *n);

int list_remove(list_t *l, int index);

int list_get(list_t *l, int index, node_t *n);

int list_set(list_t *l, int index, node_t *n);

int list_size(list_t *l);

int list_quit(list_t *l);

int list_clear(list_t *l);

int list_free(list_t **l);

#endif
