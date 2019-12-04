#ifndef __DLIST_H__
#define __DLIST_H__

#include "types.h"

typedef struct _list {
    int             max;
    int             size;
    lnode_t         *head;
    lnode_t         *tail;
    
    void            *pool;
    lnode_t         *lfree;
    u8              quit;
}dlist_t;

dlist_t* list_init(int max, int node_size);

int list_add(dlist_t *l, int index, node_t *n);

int list_remove(dlist_t *l, int index);

int list_get(dlist_t *l, int index, node_t *n);

int list_set(dlist_t *l, int index, node_t *n);

int list_size(dlist_t *l);

int list_quit(dlist_t *l);

int list_clear(dlist_t *l);

int list_free(dlist_t **l);

#endif
