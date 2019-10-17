#ifndef __PARAS_H__
#define __PARAS_H__

#include "types.h"

void paras_init(void);

void paras_read(void *p, int len);

void paras_write(void *p, int len);

void paras_reset(void);

void paras_save_preset(u8 index);

#endif
