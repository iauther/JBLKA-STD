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
#include "lcd.h"
#include "usbd.h"  
#include "config.h" 
#include "queue.h"

#define E2P_POLL_TIME       100        //500ms
#define QUEUE_MAX           20

int ccc=0;
queue_t *e2p_q=0;
u8 adda_reseted=0;


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
    delay_ms(50);
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
    delay_ms(50);
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
            r = paras_write(n.ptr, n.len);
        }
        e2p_flag = 0;
    }
}

static void gpio_init(void)
{
    u32 pins_a, pins_b, pins_c;
    GPIO_InitTypeDef  init;
    
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /*Configure GPIO pin Output Level */
    pins_a = GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_15;
    GPIO_ResetBits(GPIOA, pins_a);

    /*Configure GPIO pin Output Level */
    pins_b = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_12 
                      |GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_ResetBits(GPIOB, pins_b);

    GPIO_SetBits(GPIOB, GPIO_Pin_9);
    GPIO_SetBits(GPIOC, GPIO_Pin_8);

    pins_c = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5 |GPIO_Pin_6|
           GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_ResetBits(GPIOC, pins_c);

    //gpio a
    init.GPIO_Pin = GPIO_Pin_0;
    init.GPIO_Mode = GPIO_Mode_IPU;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &init);

    init.GPIO_Pin = pins_a;
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &init);

    //gpio b
    init.GPIO_Pin = GPIO_Pin_11;
    init.GPIO_Mode = GPIO_Mode_AF_PP;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &init);
    
    init.GPIO_Pin = GPIO_Pin_8;
    init.GPIO_Mode = GPIO_Mode_IPD;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &init);

    init.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    init.GPIO_Mode = GPIO_Mode_AF_PP;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &init);

    init.GPIO_Pin = pins_b;
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &init);

    //gpio c
    init.GPIO_Pin = pins_c;
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOC, &init);
}

static void rca_mute(u8 on)         //PB9, 高静音，低取消静音
{
    if(on) {
        GPIO_SetBits(GPIOB, GPIO_Pin_9);
    }
    else {
        GPIO_ResetBits(GPIOB, GPIO_Pin_9);
    }
}

static void adda_reset(void)      //PB5, 低电平复位，时间至少1秒，(开机默认低电平)
{
    if(dsp_is_started()) {
        if(!adda_reseted) {
            GPIO_ResetBits(GPIOB, GPIO_Pin_5);
            delay_ms(1000);        //for test
            GPIO_SetBits(GPIOB, GPIO_Pin_5);

            rca_mute(0);
            adda_reseted = 1;
        }
    }
}



int main(void)
{
    NVIC_SetVectorTable (NVIC_VectTab_FLASH, APP_OFFSET);
    __enable_irq();

	Set_System();
    e2p_q = queue_init(QUEUE_MAX);

    gpio_init();
	tim3_init(poll_func);
    usb_init();

    paras_init();
    dsp_init();
    //lcd_init();

	while(1) {
        adda_reset();
        usb_proc();
        e2p_proc();
	}

    return 0;
}



