#include <stdlib.h>
#include <string.h>
#include "slist.h"



slist_t* slist_init(int max, int node_size)
{
    u8 *ps;
    int i,hlen,nlen;
    slist_t *l=NULL;
    lnode_t *pn;

    if(max<=0 || node_size<=0) {
        return NULL;
    }

    l = (slist_t*)malloc(sizeof(slist_t));
    if(!l) {
        return NULL;
    }
    
    hlen = max*sizeof(lnode_t);
    nlen = max*node_size;
    l->pool = (lnode_t*)malloc(hlen+nlen);
    if(!l->pool) {
        free(l);
        return NULL;
    }
    
    ps = (u8*)l->pool+hlen;
    for(i=0; i<max; i++) {
        pn = l->pool+i;
        pn->data.ptr = ps+node_size*i;
        pn->data.len = node_size;
        pn->prev = (i==0)?NULL:(pn-1);
        pn->next = (i==max-1)?NULL:(pn+1);
    }

    l->max  = max;
    l->size = 0;
    l->node_size = node_size;

    return l;
}


int slist_append(slist_t *l, node_t *n)
{
    lnode_t *pn;

    if(!l || l->size>=l->max) {
        return -1;
    }

    pn = l->pool+l->size;
    if(n && n->ptr) {
        memcpy(pn->data.ptr, n->ptr, MIN(l->node_size,n->len));
    }
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
        memcpy(n->ptr, pn->data.ptr, MIN(l->node_size,n->len));
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
        memcpy(pn->data.ptr, n->ptr, MIN(l->node_size,n->len));
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


