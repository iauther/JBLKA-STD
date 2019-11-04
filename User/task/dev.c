#include "task.h"


#define MSG_MAX         6

osTimerId_t dev_tmr;

osMessageQueueId_t dev_mq;
status_data_t gStatus;
queue_t *q_e2p=0;

extern paras_data_t gParams;
typedef void (*j_func)(void);
j_func j_fn;
u32 j_addr;
static void jump_to(u32 addr)
{
    if(((*(volatile u32*)addr) & 0x2FFE0000) == 0x20000000) {
        
        j_addr = *(volatile u32*)(addr+4);
        j_fn = (j_func)j_addr;
        
        __disable_irq();
        __set_CONTROL(0);   //??psp????msp
        __set_MSP(*(volatile u32*)addr);
        j_fn();
    }
}
static void jump2Boot(void)
{
    jump_to(0);
}
static void re_boot(void)
{
    __disable_fiq();
    NVIC_SystemReset();
}

//osMutexId_t mutex_id = osMutexNew (NULL);
//osStatus_t status = osMutexAcquire (mutex_id, NULL);
//osMutexRelease (mutex_id);

static void tmr1_fun (void *arg)
{
    evt_dev_t e={0};
    e.evt = EVT_TIMER;
    osMessageQueuePut(dev_mq, &e, NULL, 0);
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
    
    r = queue_put(q_e2p, &n, 1);
    switch(pkt->type) {        
        case TYPE_DSP:
        {
            dsp_data_t *dsp=(dsp_data_t*)pkt->data;
            r = dsp_send(dsp);
        }
        break;   
        
        case TYPE_DEFAULT:
        {
            r = sys_set_default();
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
            re_boot();
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
                dsp_reset_peq(rst);
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
static void e2p_proc(void)
{
    int r;
    node_t n;
    
    r = queue_get(q_e2p, &n);
    if(r==0) {
        r = paras_write(n.ptr, n.len);
    }
}
//////////////////////////////////////////////////////////


void dev_task(void *arg)
{
    int r;
    u8  i;
    evt_dev_t e;
    osStatus_t st;
    
    dev_mq = osMessageQueueNew(MSG_MAX, sizeof(evt_com_t), NULL);
    dev_tmr = osTimerNew(tmr1_fun, osTimerPeriodic, NULL, NULL);
    osTimerStart(dev_tmr, 1000);

    while(1) {
        st = osMessageQueueGet(dev_mq, &e, NULL, osWaitForever);
        if(st==osOK) {
            switch(e.evt) {
                
                case EVT_TIMER:
                {
                    //printf("_____haha_____\n");
i++;
                    //e2p_proc();
                }
                break;
                
                case EVT_DSP:
                {
                    //
                    i++;
                }
                break;
            }
        }
    }

    osTimerDelete(dev_tmr);
    osMessageQueueDelete(dev_mq);
}


void dev_post_evt(evt_dev_t *e)
{
    osMessageQueuePut(dev_mq, e, 0, 0);
}





