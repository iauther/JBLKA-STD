#ifndef __POWER_H__
#define __POWER_H__

#include "device.h"


int power_init(void);

int power_en(u8 dev, u8 on);

#endif
