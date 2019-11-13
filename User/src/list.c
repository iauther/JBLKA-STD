#include <stdlib.h>
#include <string.h>
#include "list.h"


static int do_iterater(list_t *l, int index, node_t *n1, node_t *n2)
{

}

list_t* list_init(int max, int node_size)
{
    int i;
    void *pd;
    list_t *l=NULL;
    lnode_t *lf,*ll;

    l = (list_t*)malloc(sizeof(list_t));
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

    pd = l->pool+sizeof(lnode_t)*l->max;
    lf = ll = (lnode_t*)l->pool;
    l->head = l->tail = NULL;
    for(i=0; i<l->max; i++) {
        lf->data.ptr = pd+node_size*i;
        lf->data.len = node_size;
        lf->prev = (i==0)?NULL:(ll+i-1);
        lf->next = (i==l->max-1)?NULL:(ll+i+1);

        lf = lf->next;
    }

    return l;
}


int list_add(list_t *l, node_t *n, int index)
{
    lnode_t *lf;

    lf = l->lfree;
    if(!lf) {
        return -1;
    }

    if(l->lfree->next) {
        l->lfree = lf->next;
        l->lfree->prev = NULL;
    }
    else {
        l->lfree = NULL;
    }
    lf->next = NULL;
    
    if(0) {
        //ld->next = lf;
        //ld->prev = lf;
    }
    else {
        
    }

    memcpy(lf->data.ptr, n->ptr, MIN(lf->data.len,n->len));
    //ld->next = lf;

    return 0;
}


int list_append(list_t *l, node_t *n)
{

}


int list_rmv(list_t *l, node_t *n, int index)
{

}

int list_get(list_t *l, node_t *n, int index)
{

}


int list_set(list_t *l, node_t *n, int index)
{

}


int list_iterate(list_t *l, node_t *n, literater iter)
{

}


int list_clear(list_t *l)
{

}


int list_free(list_t *l)
{

}


