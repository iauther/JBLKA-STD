#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "types.h"
#include "tmr.h"
#include "key.h"
#include "adc.h"
#include "fan.h"
#include "dac.h"
#include "amp.h"  
#include "dsp.h"
#include "e2p.h"
#include "key.h"
#include "hid.h"
//#include "i2c.h"
#include "lcd.h"
#include "gpio.h"
#include "mp25.h"
#include "knob.h"
#include "usbd.h"
#include "hdmi.h"
#include "usart.h"
#include "power.h"
#include "delay.h"
#include "config.h"
#include "stm32f10x.h"


enum {
    DEV_AMP,
    DEV_DSP,
    DEV_LCD,
    DEV_HDMI,
    DEV_ALL,
};


#endif
