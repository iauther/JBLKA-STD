#ifndef __E2P_H__
#define __E2P_H__

#include "types.h"

int e2p_init(void);

int e2p_read_byte(u32 addr, u8* data);

int e2p_write_byte(u32 addr, u8* data);

int e2p_read(u32 addr, u8 *data, u16 len);

int e2p_write(u32 addr, u8 *data, u16 len);

void e2p_test(void);

#endif

