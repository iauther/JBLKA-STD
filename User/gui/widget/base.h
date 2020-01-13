#ifndef __BASE_H__
#define __BASE_H__

#include <stdlib.h>
#include "evt.h"
#include "lcd.h"
#include "color.h"

typedef struct {
    u16     v;
    u16     min;
    u16     max;
    u16     step;
    s8      *label;
    s8      *unit;
}data_t;

typedef int (*trigger_fn)(void);

#endif
