#ifndef __SLIST_H__
#define __SLIST_H__

#include "types.h"

typedef struct _list {
    int             max;
    int             size;
    lnode_t         *pool;
    u8              quit;
}slist_t;

slist_t* slist_init(int max, int node_size);

int slist_append(slist_t *l, node_t *n);

int slist_get(slist_t *l, int index, node_t *n);

int slist_set(slist_t *l, int index, node_t *n);

int slist_size(slist_t *l);

int slist_quit(slist_t *l);

int slist_clear(slist_t *l);

int slist_free(slist_t **l);

#endif
