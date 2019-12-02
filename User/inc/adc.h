#ifndef __ADC_H__
#define __ADC_H__

#include "types.h"

enum {
    ADC_CH_KEY1=0,
    ADC_CH_KEY2,
    ADC_CH_AMP_PWR1,
    ADC_CH_AMP_PWR2,
    ADC_CH_AMP_TEMP,
    ADC_CH_PWR_TEMP,
    
    ADC_CH_MAX
};


int adc_init(void);

u16 adc_read(u8 ch);

int adc_deinit(void);

u8 adc_get_key(void);

void adc_tmr_cb(void);

#endif
