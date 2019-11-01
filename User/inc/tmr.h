#ifndef _TMR_H
#define _TMR_H

#include <stdint.h>
#include "types.h"

enum {
    TIMER2=0,
    TIMER3,
    TIMER4,
    TIMER5,
    TIMER6,
    TIMER7,

    TIMER_MAX
};


typedef void (*tim_cb)(void);

void tim_init(u8 timer, tim_cb cb);

void tim_callback(u8 timer);

#endif








