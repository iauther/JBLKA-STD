#ifndef __GBL_H__
#define __GBL_H__

#include "types.h"
#include "listitem.h"

typedef struct {
    u8              state;
    listitem_t      *pl;
}gbl_t;


extern gbl_t gb;

#endif
