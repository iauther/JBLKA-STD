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


static int hid_single_proc(packet_t *pkt)
{
    int r=0;
    
    switch(pkt->type) {
        case 0xff:
        {
            dsp_set_started();
        }
        break;
        
        case TYPE_DSP:
        {
            dsp_data_t *dsp=(dsp_data_t*)pkt->data;

            if(dsp_is_started()) {
                r = dsp_send(dsp->id, dsp->ch, dsp->n);
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
                //...
            }
        }
        break;
        
        case TYPE_AMP:
        break;           
        
        case TYPE_IODAT:
        break;

        case TYPE_STATUS:
        break;

        case TYPE_ACK:
        break;
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


extern u8 usbRxBuf[];
static void usbhid_proc(void)
{
    u32 len;
    packet_t *pkt=(packet_t*)usbRxBuf;
    
    if(usbRxFlag) {
        if(len>0) {
            if(is_multipkts(pkt)) {
                hid_multi_proc(pkt);
            }
            else {
                hid_single_proc(pkt);
            }
        }
        usbRxFlag = 0;
    }
}
static void usb_init(void)
{
    USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
}


int main(void)
{
    NVIC_SetVectorTable (NVIC_VectTab_FLASH, APP_OFFSET);
    __enable_irq();

	Set_System();//系统时钟初始化
	//tim2_init();
	//tim3_init();
    usb_init();

    paras_init();
    dsp_init();

	while(1) {
        usbhid_proc();
	}

    return 0;
}



