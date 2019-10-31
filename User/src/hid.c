#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hid.h"
#include "e2p.h"
#include "usbd.h"
//#include "flash.h"
#include "config.h"
#include "packet.h"


#define USBD_VID                            0xA9B9
#define USBD_PID                            0x8899
#define DSP_ONCE_LEN                        44//256
#define FLASH_ONCE_LEN                      2048//FLASH_PAGE_SIZE


hid_t hid;
//u8 hidTxBuf[100];
//paras_data_t gParams;
//Dsp_Paras    gPresets[PRESET_MAX];
//packet_t     *pTx = (packet_t*)&hidTxBuf[1];

#ifdef APP
static void preset_read(u8 index, Dsp_Paras *dp)
{
    u32 offset=sizeof(paras_data_t)+index*sizeof(Dsp_Paras);
    e2p_read(offset, (u8*)dp, sizeof(Dsp_Paras));
}
static void preset_write(u8 index, Dsp_Paras *dp)
{
    u32 offset=sizeof(paras_data_t)+index*sizeof(Dsp_Paras);
    e2p_write(offset, (u8*)dp, sizeof(Dsp_Paras));
}
#endif



void hid_pkt_init(int mode, u8 nck, packet_t *pkt)
{
    hid_data_t* hd = (mode == RX) ? &hid.rx : &hid.tx;

    //if(hd->started) return;
    
    hd->type = pkt->type;
    switch(pkt->type) {
#ifdef APP
        case TYPE_PARAS:
        {
            hd->length = sizeof(paras_data_t);
            if (mode == TX) {
                hd->ptr = &gParams;
                int more = (hd->length%PAYLOAD_LEN)>0?1:0;
                hd->pkts = hd->length /PAYLOAD_LEN+more;
                hd->onceLen = 0;
            }
            else {
                if(hd->ptr) free(hd->ptr);
                hd->ptr = malloc(hd->length);
                hd->pkts = pkt->pkts;
                hd->onceLen = hd->length;
            }
            rbuf_init(&hd->rb, hd->ptr, hd->length);
        }
        break;

        case TYPE_PRESET:
        {
            preset_data_t *pre = (preset_data_t*)pkt->data;
            hd->index = pre->index;
            hd->length = sizeof(Dsp_Paras);
            if(hd->ptr) free(hd->ptr);
            hd->ptr = malloc(hd->length);
            
            if (mode == TX) {
                preset_read(pre->index, (Dsp_Paras*)hd->ptr);
                int more = (hd->length%PAYLOAD_LEN)>0?1:0;
                hd->pkts = hd->length /PAYLOAD_LEN+more;
                hd->onceLen = 0;
            }
            else {
                hd->pkts = pkt->pkts;
                hd->onceLen = sizeof(Dsp_Paras);
            }
            rbuf_init(&hd->rb, hd->ptr, hd->length);
        }
        break;
        
        case TYPE_EQRESET:
        {
            eq_reset_t *rst=(eq_reset_t*)pkt->data;
            hd->length = sizeof(eq_reset_t)+sizeof(TypeS_EQ);
            if(hd->ptr) free(hd->ptr);
            hd->ptr = malloc(hd->length);
            if(hd->ptr) {
                eq_reset_t *rs=(eq_reset_t*)hd->ptr;
                rs->ch = rst->ch;
                memcpy(rs->eq, &gParams.dsp.Array_EQ[rst->ch], sizeof(TypeS_EQ));
            }
            
            int more = (hd->length%PAYLOAD_LEN)>0?1:0;
            hd->pkts = hd->length /PAYLOAD_LEN+more;
        }
        break;
#endif

        case TYPE_UPGRADE:
        {
            hdr_t *hdr=(hdr_t*)pkt->data;
            
            hd->ftype = hdr->head.ftype;
            hd->onceLen = (hd->ftype==FILE_DSP)?DSP_ONCE_LEN:FLASH_ONCE_LEN;
            if (mode == RX) {
                hd->length = hd->onceLen+0xff;
                if(hd->ptr) free(hd->ptr);
                hd->ptr = malloc(hd->length);
                hd->pkts = pkt->pkts;
            }
            else {
                //hd->ptr = fbuf;
                //hd->length = 0;
                hd->pkts = pkt->pkts;
            }
            rbuf_init(&hd->rb, hd->ptr, hd->length);
        }
        break;
    }

    hd->started = 1;
    hd->finished = 0;
    hd->pid = 0;
    hd->nck = nck;
    hd->times = 0;
    hd->processedLen = 0;
}

int hid_pkt_recv(packet_t *pkt, u8 last)
{
    int r;
    int dlen, len;
    hid_data_t* hd = &hid.rx;

    if(!hd->started) {
        return -1;
    }
    
    if(pkt->type==TYPE_UPGRADE && pkt->pid==0) {    //升级第一包，特殊处理
        hdr_t *hdr=(hdr_t*)pkt->data;
        int len = pkt->dlen-sizeof(hdr_t);
        rbuf_write(&hd->rb, hdr->data, len);
    }
    else {
        rbuf_write(&hd->rb, pkt->data, pkt->dlen);
    }
    hd->pid++;
    
    //检查数据是否缓冲够
    dlen = rbuf_get_dlen(&hd->rb);
    if (!last && dlen<hd->onceLen) {
        return -1;      //继续缓冲
    }

    switch (hd->type) {
        case TYPE_PARAS:
        {
            //memcpy(hd->ptr+hd->processedLen, hd->ptr, wlen);
        }
        break;

        case TYPE_PRESET:
        {
            //preset_write(hd->index, (Dsp_Paras*)hd->ptr);
        }
        break;

        case TYPE_UPGRADE:
        {
            while (rbuf_get_dlen(&hd->rb) > 0) {

                switch (hd->ftype) {
#ifdef APP
                    case FILE_DSP:
                    {
                        u8 tmpBuf[DSP_ONCE_LEN],last=0;
                        len = rbuf_read(&hd->rb, tmpBuf, hd->onceLen);
                        if(len<hd->onceLen || (len==hd->onceLen && rbuf_get_dlen(&hd->rb)==0)) {
                            last = 1;
                        }
                        r = dsp_upgrade(hd->times, tmpBuf, hd->onceLen, last);
                    }
                    break;
#endif

#ifdef BOOT
                    case FILE_MCU:
                    {
                        u8 tmpBuf[FLASH_ONCE_LEN];
                        len = rbuf_read(&hd->rb, tmpBuf, hd->onceLen);
                        r = flash_write(APP_OFFSET + hd->processedLen, tmpBuf, len);
                    }
                    break;

                    case FILE_BOOT:
                    {
                        u8 tmpBuf[FLASH_ONCE_LEN];
                        len = rbuf_read(&hd->rb, tmpBuf, hd->onceLen);
                        r = flash_write(hd->processedLen, tmpBuf, len);
                    }
                    break;
#endif

                    default:
                    return -1;
                }

                hd->processedLen += len;
                hd->times++;

                if(!last) {
                    break;
                }
            }
        }
    }

    if(last) {
        hid_pkt_reset(RX);
        hd->finished = last;
        return 0;
    }
    
    return 0;
}

int hid_pkt_send(void)
{
    int r,sendlen;
    hid_data_t* hd = &hid.tx;

    if(!hd->started || !hd->ptr) {
        return -1;
    }

    sendlen = (hd->processedLen +PAYLOAD_LEN>hd->length)?(hd->length- hd->processedLen):PAYLOAD_LEN;
    r = usbd_send_pkt(hd->type, (u8*)hd->ptr + hd->processedLen, sendlen, hd->pkts, hd->pid, hd->nck);
    if(r==0) {
        hd->lastlen = sendlen;
    }

    return 0;
}


int hid_pkt_step(int mode)
{
    hid_data_t* hd = (mode == RX) ? &hid.rx : &hid.tx;
    hd->pid++;
    hd->processedLen += hd->lastlen;
    if(hd->pkts==hd->pid) {
        hid_pkt_reset(mode);
        hd->finished = 1;
        return 1;
    }

    return 0;
}

int hid_pkt_reset(int mode)
{
    hid_data_t* hd = (mode == RX) ? &hid.rx : &hid.tx;

    if (!hd->started) {
        return -1;
    }

    hd->started = 0;
    //hd->times = 0;
    if(hd->ptr) {
        free(hd->ptr);
        hd->ptr = NULL;
    }
    rbuf_free(&hd->rb);

    return 0;
}

int hid_pkt_started(int mode)
{
    hid_data_t* hd = (mode == RX) ? &hid.rx : &hid.tx;
    return hd->started;
}

u8 hid_pkt_get_type(int mode)
{
    hid_data_t* hd = (mode == RX) ? &hid.rx : &hid.tx;
    return hd->type;
}

int hid_pkt_finished(int mode)
{
    hid_data_t* hd = (mode == RX) ? &hid.rx : &hid.tx;

    return hd->finished;
}


