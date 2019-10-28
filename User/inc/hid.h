#ifndef __HID_H__
#define __HID_H__

#include "types.h"
#include "packet.h"
#include "rbuf.h"
#include "pack.h"

#define HID_LEN             64
#define HID_RETRY_TIMES     5
#define HID_TIMEOUT         5

#define PAYLOAD_LEN         (HID_LEN-sizeof(packet_t))


enum {
    RX,
    TX,
};

typedef struct {
    u8      type;
    u8      ftype;

    u8      started;
    u8      finished;
    u16     pid;
    u8      nck;        //need ack
    u32     processedLen;

    u8      index;
    u16     onceLen;
    u16     times;
    u16     pkts;

    void    *ptr;
    u32     length;
    u16     lastlen;

    rbuf_t  rb;
}hid_data_t;

typedef struct {
    hid_data_t rx;
    hid_data_t tx;
}hid_t;


void hid_pkt_init(int mode, u8 nck, packet_t *pkt);

int hid_pkt_recv(packet_t *pkt, u8 last);

int hid_pkt_send(void);

int hid_pkt_step(int mode);

int hid_pkt_started(int mode);

int hid_pkt_finished(int mode);

u8 hid_pkt_get_type(int mode);

int hid_pkt_reset(int mode);

int hid_open(void);

int hid_close(void);

int hid_read(u8* buf, int len);

int hid_write(u8* buf, int len);

int hid_send_ack(packet_t* p, int r);

int hid_send_pkt(u8 type, void* data, u16 len, u16 pkts, u16 pid, u8 nck);


#endif
