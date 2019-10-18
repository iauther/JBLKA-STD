/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "usart.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_desc.h"
#include "tmr.h"
#include "stdint.h"  
#include "packet.h"  
#include "hid.h"
#include "paras.h"
#include "dsp.h"  
#include "usbd.h"  
#include "config.h" 
#include "queue.h"

#define E2P_POLL_TIME       50        //500ms
#define QUEUE_MAX           20

int ccc=0;
queue_t *e2p_q=0;
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
        
        case TYPE_IODAT:
        break;

        case TYPE_STATUS:
        break;

        case TYPE_ACK:
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
            usbd_send_ack(pkt, r);
        }
    }

    return r;
}

u8 retry_times = 0;
static int hid_multi_proc(packet_t *pkt)
{
    int r=0;

    switch(pkt->type) {
        case TYPE_ACK:
        {
            retry_times = 0;
            hid_pkt_step(TX);
            hid_pkt_send();
            //stop_timeout_timer();
        }
        break;

        case TYPE_PARAS:
        case TYPE_PRESET:
        case TYPE_UPGRADE:
        {
            if(pkt->dlen>0 && pkt->pkts>1) {
                hid_pkt_init(RX, 0, pkt);
                r = hid_pkt_recv(pkt);
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
    }
}

static void usb_dp_reset(void)
{
    GPIO_InitTypeDef init={0};
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Pin = GPIO_Pin_12;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &init);

    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
    delay_ms(5);
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
    delay_ms(5);
}


static void usb_init(void)
{
    USB_Interrupts_Config();
	Set_USBClock();

    usb_dp_reset();
	USB_Init();
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
            paras_write(n.ptr, n.len);
        }
        e2p_flag = 0;
    }
}


int main(void)
{
    NVIC_SetVectorTable (NVIC_VectTab_FLASH, APP_OFFSET);
    __enable_irq();

	Set_System();
	tim3_init(poll_func);
	//tim3_init();
    usb_init();

    paras_init();
    dsp_init();
    e2p_q = queue_init(QUEUE_MAX);

	while(1) {
        usb_proc();
        e2p_proc();
	}

    return 0;
}



