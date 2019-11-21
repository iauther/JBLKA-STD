#ifndef __PARAS_H__
#define __PARAS_H__

#include "types.h"
#include "packet.h"

int paras_init(void);

int paras_reset_peq(eq_reset_t *rst);

int paras_default(void);

int paras_read(void *p, int len);

int paras_write(void *p, int len);

int paras_reset(void);

int paras_read_preset(u8 index, Dsp_Paras *dsp);

int paras_write_preset(u8 index, Dsp_Paras *dsp);

int paras_update(packet_t *pkt, node_t *node);

#endif
