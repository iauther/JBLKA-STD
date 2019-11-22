#ifndef __USBD_H__
#define __USBD_H__

#include "types.h"
#include "packet.h"

int usbd_init(void);

int usbd_send(u8 *data, u16 len);

int usbd_recv(void);

int is_multipkts(packet_t *pkt);

int usbd_send_ack(packet_t *p, int r);

int usbd_send_pkt(u8 type, void *data, u16 len, u16 pkts, u16 pid, u8 nck);

int usbd_is_plugin(void);

extern u8 usbRxFlag;

#endif
