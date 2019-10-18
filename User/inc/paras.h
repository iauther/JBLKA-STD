#ifndef __PARAS_H__
#define __PARAS_H__

#include "types.h"
#include "packet.h"

int paras_init(void);

int paras_read(void *p, int len);

int paras_write(void *p, int len);

int paras_reset(void);

int paras_save_preset(u8 index);

int paras_update(packet_t *pkt, node_t *node);

#endif
