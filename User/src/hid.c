#include <stdio.h>
#include <string.h>
#include "hid.h"
#include "usbd.h"
//#include "flash.h"
#include "config.h"
#include "packet.h"


#define USBD_VID                            0xA9B9
#define USBD_PID                            0x8899
#define DSP_ONCE_LEN                        256
#define E2P_ONCE_LEN                        512


hid_t hid;
//u8 hidTxBuf[100];
//paras_data_t gParams;
//Dsp_Paras    gPresets[PRESET_MAX];
//packet_t     *pTx = (packet_t*)&hidTxBuf[1];

#ifdef APP
static void preset_read(u8 index, Dsp_Paras *dp)
{
    //e2p_read();
}
static void preset_write(u8 index, Dsp_Paras *dp)
{
    //e2p_write();
}
#endif



void hid_pkt_init(int mode, u8 nck, packet_t *pkt)
{
    int more;
    hid_data_t* hd = (mode == RX) ? &hid.rx : &hid.tx;
    if(hd->started) {
        return;
    }
    
    hd->type = pkt->type;
    switch(pkt->type) {
#ifdef APP
        case TYPE_PARAS:
        {
            hd->ptr = (u8*)&gParams;
            hd->length = sizeof(paras_data_t);
            
            if (mode == TX) {
                more = (hd->length%PAYLOAD_LEN)>0?1:0;
                hd->pkts = hd->length /PAYLOAD_LEN+more;
                hd->onceLen = 0;
            }
            else {
                hd->pkts = pkt->pkts;
                hd->onceLen = hd->length;
            }
        }
        break;

        case TYPE_PRESET:
        {
            preset_data_t *pre = (preset_data_t*)pkt->data;
            hd->index = pre->index;
            hd->ptr = hd->buf;
            hd->length = sizeof(Dsp_Paras);
            
            if (mode == TX) {
                preset_read(pre->index, (Dsp_Paras*)hd->ptr);
                more = (hd->length%PAYLOAD_LEN)>0?1:0;
                hd->pkts = hd->length /PAYLOAD_LEN+more;
                hd->onceLen = 0;
            }
            else {
                hd->pkts = pkt->pkts;
                hd->onceLen = sizeof(Dsp_Paras);
            }
        }
        break;
#endif

        case TYPE_UPGRADE:
        {
            hdr_t *hdr=(hdr_t*)pkt->data;
            upg_t *upg=(upg_t*)hdr->data;

            hd->ftype = upg->ftype;
            hd->onceLen = (hd->ftype==FILE_DSP)?DSP_ONCE_LEN:E2P_ONCE_LEN;

            //hd->ptr = fbuf;
            hd->pkts = pkt->pkts;
        }
        break;
    }

    hd->started = 1;
    hd->finished = 0;
    hd->pid = 0;
    hd->nck = nck;
    hd->processedLen = 0;
    rbuf_init(&hd->rb, hd->ptr, hd->length);
}

u8 tmpBuf[BUF_LEN];
static int hid_pkt_rx_proc(u8 type, u16 pid, u8 finished)
{
    int data_len, once_len, wlen,j=0;
    hid_data_t *hd = &hid.rx;

    data_len = rbuf_get_data_len(&hd->rb);
    if (!finished) {//非最后一包数据
        if (data_len >= hd->onceLen) {  //继续等待数据缓冲
            data_len = hd->onceLen;
        }
        else {
            return -1;
        }
    }
    else {
        j++;
    }

    
    switch (hd->type) {
        case TYPE_PARAS:
        {
            //memcpy(hd->ptr+hd->processedLen, tmpBuf, wlen);
        }
        break;

        case TYPE_PRESET:
        {
            //preset_write(hd->index, (Dsp_Paras*)tmpBuf);
            //memcpy(&gPresets[hd->index], tmpBuf, hd->onceLen);
        }
        break;

        case TYPE_UPGRADE:
        {
            while (data_len > 0) {
                if (data_len >= hd->onceLen) {
                    once_len = hd->onceLen;
                }
                else {
                    once_len = data_len;
                }

                wlen = rbuf_read(&hd->rb, tmpBuf, once_len);
                switch (hd->ftype) {
#ifdef APP
                    case FILE_DSP:
                    {
                        dsp_upgrade(pid, tmpBuf, wlen);
                    }
                    break;
#endif

                    case FILE_APP:
                    {
                        //flash_write(APP_OFFSET + hd->processedLen, tmpBuf, wlen);
                    }
                    break;

#ifdef BOOT
                    case FILE_BOOT:
                    {
                        flash_write(hd->processedLen, tmpBuf, wlen);
                    }
                    break;
#endif
                }
				data_len -= wlen;
				hd->processedLen += wlen;
            }
        }
    }

    return 0;
}

static int hid_pkt_rx_postproc(u8 type)
{
    hid_data_t *hd = &hid.rx;

    switch (hd->type) {
        case TYPE_PRESET:
        //dsp_download();
        break;
        
        case TYPE_UPGRADE:
        {
            switch (hd->ftype) {
#ifdef APP
                case FILE_DSP:
                    //dsp_reset();      //soft reset, load dsp paras
                    break;
#endif

                case FILE_APP:
                    //jump to APP_OFFSET run
                    break;

                case FILE_BOOT:
                    //reboot or not?
                    break;
            }
        }
        break;
    }
    
    return 0;
}

int hid_pkt_recv(packet_t *pkt)
{
    int finished=0;
    hid_data_t* hd = &hid.rx;

    if(!hd->started) {
        return -1;
    }
    
    rbuf_write(&hd->rb, pkt->data, pkt->dlen);
    finished = (pkt->pkts==pkt->pid+1)?1:0;
    hid_pkt_rx_proc(hd->type, hd->pid, finished);
    if(finished) {
        hid_pkt_reset(RX);
        hd->finished = finished;
        return 0;
    }
    
    hd->pid++;
    
    return 0;
}

int hid_pkt_send(void)
{
    int r,sendlen;
    hid_data_t* hd = &hid.tx;

    if(!hd->started) {
        return -1;
    }

    sendlen = (hd->processedLen +PAYLOAD_LEN>hd->length)?(hd->length- hd->processedLen):PAYLOAD_LEN;
    r = usbd_send_pkt(hd->type, hd->ptr+ hd->processedLen, sendlen, hd->pkts, hd->pid, hd->nck);
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

    if (hd->started == 0) {
        return -1;
    }

    hd->started = 0;
    hd->finished = 0;
    hd->type = 0;
    hd->pid  = 0;
    hd->nck  = 0;
    hd->processedLen = 0;
    hd->pkts = 0;
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


