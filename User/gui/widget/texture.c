#include "texture.h"


texture_t *texture_init(rect_t *rect, s8 *label, data_t *dat)
{
    texture_t *t=(texture_t*)malloc(sizeof(texture_t));
    if(!t) {
        return NULL;
    }
    
    //t->label = label;
    //t->rect  = *rect;
    //t->data  = *dat;
    
    return t;
}



int texture_free(texture_t *t)
{
    return 0;
}


int texture_refresh(texture_t *t)
{
    return 0;
}


int texture_handle(texture_t *t, key_t *k)
{
    return 0;
}




