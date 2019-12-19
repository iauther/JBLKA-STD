#include <stdlib.h>
#include <string.h>
#include "dlist.h"



dlist_t* dlist_init(int max, int node_size)
{
    int i;
    void *pd;
    dlist_t *l=NULL;
    lnode_t *lf,*ll;

    l = (dlist_t*)malloc(sizeof(dlist_t));
    if(!l) {
        return NULL;
    }
    l->pool = malloc(max*node_size+sizeof(lnode_t));
    if(!l->pool) {
        free(l);
        return NULL;
    }
    l->max  = max;
    l->size = 0;

    pd = (u8*)l->pool+sizeof(lnode_t)*l->max;
    lf = ll = (lnode_t*)l->pool;
    l->head = l->tail = NULL;
    for(i=0; i<l->max; i++) {
        lf->data.ptr = (u8*)pd+node_size*i;
        lf->data.len = node_size;
        lf->prev = (i==0)?NULL:(ll+i-1);
        lf->next = (i==l->max-1)?NULL:(ll+i+1);

        lf = lf->next;
    }
    l->lfree = ll;

    return l;
}

static lnode_t* get_lnode(dlist_t *l, int index)
{
    int i=0;
    lnode_t *pn=l->head;

    while(pn) {
        if(i==index) {
            return pn;
        }
        pn = pn->next;
    }

    return NULL;
}
int dlist_add(dlist_t *l, int index, node_t *n)
{
    lnode_t *lf;
    lnode_t *tmp;

    if(!l || index>=l->size) {
        return -1;
    }

    lf = l->lfree;
    if(!lf) {
        return -1;
    }

    if(lf->next) {
        l->lfree = lf->next;
        lf->prev = NULL;
    }
    else {
        l->lfree = NULL;
    }

    switch(index) {
        case 0:
        {
            tmp = l->head;
            tmp->prev = lf;
            lf->prev = NULL;
            lf->next = tmp;

            l->head = lf;
        }
        break;

        case -1:
        {
            tmp = l->tail;
            if(tmp) {
                tmp->next = lf;
            }
            lf->prev = tmp;
            lf->next = NULL;

            l->tail = lf;

        }
        break;

        default:
        {
            tmp = get_lnode(l, index);
            tmp->prev = lf;
            tmp->prev->next = lf;
            lf->prev = tmp->prev;
            lf->next = tmp;
        }
        break;
    }
    memcpy(lf->data.ptr, n->ptr, MIN(lf->data.len,n->len));
    l->size++;

    return 0;
}


int dlist_remove(dlist_t *l, int index)
{
    lnode_t *pn;

    if(!l || !l->size || index>=l->size) {
        return -1;
    }
    pn = get_lnode(l, index);

    l->size--;

    return 0;
}


int dlist_get(dlist_t *l, int index, node_t *n)
{
    lnode_t *pn;

    if(!l || !l->size || index>=l->size) {
        return -1;
    }

    pn = get_lnode(l, index);
    if(n && n->ptr) {
        memcpy(n->ptr, pn->data.ptr, MIN(pn->data.len,n->len));
    }

    return 0;
}


int dlist_set(dlist_t *l, int index, node_t *n)
{
    lnode_t *pn;

    if(!l || index>=l->size) {
        return -1;
    }

    pn = get_lnode(l, index);
    if(n && n->ptr) {
        memcpy(pn->data.ptr, n->ptr, MIN(pn->data.len,n->len));
    }

    return 0;
}


int dlist_size(dlist_t *l)
{
    return l->size;
}


int dlist_quit(dlist_t *l)
{
    l->quit = 1;
    return 0;
}


int dlist_clear(dlist_t *l)
{
    return 0;
}


int dlist_free(dlist_t **l)
{
    return 0;
}


