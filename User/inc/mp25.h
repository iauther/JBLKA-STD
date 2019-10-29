#ifndef __MP25_H__
#define __MP25_H__

#include "types.h"

int mp25_init(void);

int mp25_read(u32 addr, u8 *data, int len);

int mp25_write(u32 addr, u8 *data, int len);


#endif
