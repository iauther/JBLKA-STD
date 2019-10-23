#ifndef __ADC_H__
#define __ADC_H__

#include "types.h"

int adc_init(u8 mode);

int adc_deinit(void);

u8 adc_get_key(void);

#endif
