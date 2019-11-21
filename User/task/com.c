#include "task.h"

#define MSG_MAX         6

#ifdef RTX
msg_t *com_msg=NULL;
static int set_default(void)
{
    node_t n;
    paras_default();
    //dsp_reset();
    dsp_download();
    sys_set_input(gParams.dsp.Array_Input.input);
    
    n.ptr = &gParams;
    n.len = sizeof(gParams);
    e2p_put(&n);
    
    return 0;
}

static int hid_single_proc(packet_t *pkt)
{
    int r=0;
    node_t n;
    
    if(!dsp_is_started()) {
        return -1;
    }

    r = paras_update(pkt, &n);
    if(r) {
        return r;
    }
    e2p_put(&n);

    switch(pkt->type) {        
        case TYPE_DSP:
        {
            dsp_data_t *dsp=(dsp_data_t*)pkt->data;
            r = dsp_send(dsp);
        }
        break;   
        
        case TYPE_DEFAULT:
        {
            r = set_default();
        }
        break;

        case TYPE_VERSION:
        {
            fw_info_t fw=gParams.fw;
            fw.mode = 2;
            delay_ms(10);
            r = usbd_send_pkt(TYPE_VERSION, &fw, sizeof(fw), 1, 0, 0);
        }
        break;

        case TYPE_REBOOT:
        {
            sys_reboot();
        }
        break;

        case TYPE_UPGRADE_REQ:
        {
            //*(vu32*)UPG_FLAG_SRAM_OFFSET = UPG_FLAG;
            //jumpToBoot();
        }
        break;

        case TYPE_IODAT:
        {
            r = sys_set_iodat((io_data_t*)pkt->data);
        }
        break;

        case TYPE_STATUS:
        break;

        case TYPE_ACK:
        {
            
        }
        break;

        default:
        r = -1;
        break;
    }
    
    if(pkt->nck) {
        usbd_send_ack(pkt, r);
    }
    else {
        if(!r) {
            //usbd_send_ack(pkt, r);
        }
    }

    return r;
}
static int hid_multi_proc(packet_t *pkt)
{
    int r=0;
    u8  last;
    node_t n;

    switch(pkt->type) {
        case TYPE_ACK:
        {
            hid_pkt_step(TX);
            hid_pkt_send();
            //stop_timeout_timer();
        }
        break;

        default:
        {
            if(pkt->type==TYPE_EQRESET) {
                eq_reset_t *rst=(eq_reset_t*)pkt->data;
                paras_reset_peq(rst);
                r = dsp_reset_peq(rst, &n);
                if(r==0) {
                    e2p_put(&n);
                }
            }

            if(pkt->dlen>0 && pkt->pkts>1) {
                if(pkt->pid==0) {
                    hid_pkt_init(RX, 0, pkt);
                }

                last = (pkt->pkts==pkt->pid+1)?1:0;
                r = hid_pkt_recv(pkt, last);
                if(pkt->nck) {
                    usbd_send_ack(pkt, r);
                }
                else {
                    if(!r) {
                        usbd_send_ack(pkt, r);
                    }
                }
            }
            else {
                hid_pkt_reset(TX);
                hid_pkt_init(TX, 1, pkt);
                hid_pkt_send();
                //start_timeout_timer();
            }
        }
        break;
    }

    return 0;
}
static void hid_proc(packet_t *pkt)
{
    if(is_multipkts(pkt)) {
        hid_multi_proc(pkt);
    }
    else {
        hid_single_proc(pkt);
    }
}



void com_task(void *arg)
{
    int r;
    evt_com_t e;
    osStatus_t st;

    //menu_init();
    com_msg = msg_init(MSG_MAX, sizeof(e));
    if(!com_msg) {
        return;
    }

    while(1) {
        r = msg_recv(com_msg, &e, NULL);
        if(r==0) {
            switch(e.evt) {
                
                case EVT_HID:
                {
                    packet_t *pkt=(packet_t*)e.param.hid;
                    hid_proc(pkt);
                }
                break;

                case EVT_RS485:
                {
                    //
                }
                break;
            }
        }
    }

    msg_free(&com_msg);
}


void com_post_evt(evt_com_t *e)
{
    msg_post(com_msg, e, sizeof(*e));
}

#endif




