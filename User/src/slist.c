#include <stdlib.h>
#include <string.h>
#include "slist.h"



slist_t* slist_init(int max, int node_size)
{
    int i;
    void *pd,*pool;
    slist_t *l=NULL;
    lnode_t *lf,*ll;

    l = (slist_t*)malloc(sizeof(slist_t));
    if(!l) {
        return NULL;
    }
    pool = malloc(max*node_size+sizeof(lnode_t));
    if(!pool) {
        free(l);
        return NULL;
    }
    l->pool = (lnode_t*)pool;

    lf = ll = l->pool;
    for(i=0; i<l->max; i++) {
        lf->data.ptr = pd+node_size*i;
        lf->data.len = node_size;
        lf->prev = (i==0)?NULL:(ll+i-1);
        lf->next = (i==l->max-1)?NULL:(ll+i+1);

        lf = lf->next;
    }
    l->max  = max;
    l->size = 0;

    return l;
}


int slist_append(slist_t *l, node_t *n)
{
    lnode_t *pn;

    if(!l || l->size==l->max) {
        return -1;
    }
    pn = l->pool+l->size;
    memcpy(pn->data.ptr, n->ptr, MIN(pn->data.len,n->len));
    l->size++;

    return 0;
}


int slist_get(slist_t *l, int index, node_t *n)
{
    lnode_t *pn;

    if(!l || !l->size || index>=l->size) {
        return -1;
    }

    pn = l->pool+index;
    if(n && n->ptr) {
        memcpy(n->ptr, pn->data.ptr, MIN(pn->data.len,n->len));
    }

    return 0;
}


int slist_set(slist_t *l, int index, node_t *n)
{
    lnode_t *pn;

    if(!l || index>=l->size) {
        return -1;
    }

    pn = l->pool+l->size;
    if(n && n->ptr) {
        memcpy(pn->data.ptr, n->ptr, MIN(pn->data.len,n->len));
    }

    return 0;
}


int slist_size(slist_t *l)
{
    return l->size;
}


int slist_quit(slist_t *l)
{
    l->quit = 1;
    return 0;
}


int slist_clear(slist_t *l)
{
    return 0;
}


int slist_free(slist_t **l)
{
    return 0;
}


