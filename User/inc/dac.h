#ifndef __DAC_H__
#define __DAC_H__

#include "types.h"

enum {
    DAC_CH1,
    DAC_CH2,
};


int dac_init(u8 ch);

int dac_set(u8 ch, u16 v);

#endif
