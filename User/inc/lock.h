#ifndef __LOCK_H__
#define __LOCK_H__

#include "types.h"
#include "cmsis_os2.h"

enum {
    LOCK_DSP=0,
    LOCK_E2P,
    
    LOCK_MAX
};


int lock_init(void);

int lock_on(int id);

int lock_off(int id);

#endif
