#ifndef __DAC_H__
#define __DAC_H__

#include "types.h"

enum {
    CHANNEL1,
    CHANNEL2,
};


int dac_init();

int dac_set(u8 ch, u16 v);

#endif
