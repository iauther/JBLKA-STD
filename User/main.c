/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "usart.h"
#include "tmr.h"
#include "stdint.h"  
#include "packet.h"  
#include "hid.h"
#include "paras.h"
#include "dsp.h"
#include "sys.h"
#include "lcd.h"
#include "usbd.h"  
#include "config.h" 
#include "queue.h"

#define E2P_POLL_TIME       1000        //500ms
#define QUEUE_MAX           20

int ccc=0;
queue_t *e2p_q=0;
extern paras_data_t gParams;
typedef void (*jump_func)(void);
jump_func jump_fn;
uint32_t jump_addr;
static void jump_to(u32 addr)
{
    if(((*(__IO u32*)addr) & 0x2FFE0000) == 0x20000000) {
        
        jump_addr = *(__IO u32*)(addr+4);
        jump_fn = (jump_func)jump_addr;
        
        __disable_irq();
        __set_CONTROL(0);   //??psp????msp
        __set_MSP(*(__IO u32*)addr);
        jump_fn();
    }
}

static void jumpToBoot(void)
{
    jump_to(0);
}
static void reboot(void)
{
    __disable_fiq();
    NVIC_SystemReset();
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
    
    r = queue_put(e2p_q, &n, 1);
    if(r) {
        ccc++;
    }

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
            reboot();
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

u8 retry_times = 0;
static int hid_multi_proc(packet_t *pkt)
{
    int r=0;
    u8  last;

    switch(pkt->type) {
        case TYPE_ACK:
        {
            retry_times = 0;
            hid_pkt_step(TX);
            hid_pkt_send();
            //stop_timeout_timer();
        }
        break;
        
        case TYPE_EQRESET:
        {
#if 0
            eq_reset_t *rst=(eq_reset_t*)pkt->data;

            paras_reset_peq(rst);
            dsp_reset_peq(rst);
            hid_pkt_reset(TX);
            hid_pkt_init(TX, 1, pkt);
            hid_pkt_send();
#endif
        }
        break;

        case TYPE_PARAS:
        case TYPE_PRESET:
        case TYPE_UPGRADE:
        {
            if(pkt->dlen>0 && pkt->pkts>1) {
                hid_pkt_init(RX, 0, pkt);

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

u32 usb_rx_cnt=0;
static void usb_proc(void)
{
    extern u8 usbRxBuf[];
    packet_t *pkt=(packet_t*)usbRxBuf;
    
    if(usbRxFlag) {
        if(is_multipkts(pkt)) {
            hid_multi_proc(pkt);
        }
        else {
            hid_single_proc(pkt);
        }
        usbRxFlag = 0;
usb_rx_cnt++;
    }
}


u32 e2p_flag=0;
u64 poll_counter=0;
static void poll_func(void)
{
    poll_counter++;
    if(poll_counter%E2P_POLL_TIME==0) {
        e2p_flag = 1;
    }
}
static void e2p_proc(void)
{
    int r;
    node_t n;
    
    if(e2p_flag) {
        r = queue_get(e2p_q, &n);
        if(r==0) {
            r = paras_write(n.ptr, n.len);
        }
        e2p_flag = 0;
    }
}
 




int main(void)
{
	sys_init();

    e2p_q = queue_init(QUEUE_MAX);
	tim3_init(poll_func);

	while(1) {
        usb_proc();
        e2p_proc();
	}

    return 0;
}



