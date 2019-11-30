#include "device.h"
#include "sys.h"
#include "task.h"

//#include "stm32f10x.h"

#define E2P_POLL_TIME       10        //1000ms
#define ADC_KEY_POLL_TIME   1         //100ms
#define PWR_VOL_POLL_TIME   10


extern paras_data_t gParams;
static void reboot(void)
{
    __disable_fiq();
    NVIC_SystemReset();
}

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
    node_t n;

    switch(pkt->type) {
        case TYPE_ACK:
        {
            retry_times = 0;
            hid_pkt_step(TX);
            hid_pkt_send();
            //stop_timeout_timer();
        }
        break;

        default:
        {
            if(pkt->type==TYPE_EQRESET) {
                eq_reset_t *rst=(eq_reset_t*)pkt->data;
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


extern u8 amp_low_flag;
u8 e2p_flag=0;
u8 adc_key_flag=0;
u8 pwr_vol_flag=0;
u64 poll_counter=0;
static void amp_proc(u8 *cnt);
static void poll_cb(void)
{
    poll_counter++;

    if(poll_counter%E2P_POLL_TIME==0) {
        e2p_flag = 1;
    }

    if(poll_counter%ADC_KEY_POLL_TIME==0) {
        adc_key_flag = 1;
    }

    if(poll_counter%PWR_VOL_POLL_TIME==0) {
        pwr_vol_flag = 1;
    }

    if(amp_low_flag) {
        amp_proc(&amp_low_flag);
    }
}

static void e2p_proc(void)
{
    int r;
    node_t n;
    
    r = e2p_get(&n);
    if(r==0) {
        r = paras_write(n.ptr, n.len);
    }
}
static void adc_key_proc(void)
{
    u8 key=adc_get_key();
    if(key!=KEY_NONE) {
        switch(key) {

            case KEY_MUSIC:
            break;

            case KEY_MIC:
            break;

            case KEY_EFFECT:
            break;

            case KEY_ENTER:
            break;

            case KEY_EXIT:
            break;

            case KEY_SAVE:
            break;

            case KEY_PRESET:
            break;
        }
    }
}

static void pwr_vol_proc(void)
{
    u16 amp_pwr1,amp_pwr2;
    u16 amp_temp, pwr_temp;
    amp_pwr1 = adc_read(ADC_CH_AMP_PWR1)*3300/4096;     //20%
    amp_pwr2 = adc_read(ADC_CH_AMP_PWR2)*3300/4096;     //<500mv

    if(amp_pwr1<(1760-176*2) || amp_pwr1>(1760+176*2) || (amp_pwr2>500)) {
        power_en(DEV_AMP, 0);
    }
    else {
        power_en(DEV_AMP, 1);
    }

    amp_temp = adc_read(ADC_CH_AMP_TEMP);
    pwr_temp = adc_read(ADC_CH_PWR_TEMP);

    //do something...

}
static void amp_proc(u8 *cnt)
{
    static u32 amp_low_counter=0;
        
    if(amp_get_level()) {
        amp_low_counter = 0;
        *cnt = 0;
    }

    amp_low_counter++;
    if(amp_low_counter%500) {   //延迟500ml后，如果还是低电平就关功放
        amp_pwr(0);
        amp_low_counter = 0;
        *cnt = 0;
    }
}


static void poll_proc(void)
{
    if(e2p_flag) {
        e2p_proc();
        e2p_flag = 0;
    }

    if(adc_key_flag) {
        adc_key_proc();
        adc_key_flag = 0;
    }

    if(pwr_vol_flag) {
        pwr_vol_proc();
        pwr_vol_flag = 0;
    }

}


static void knob_proc(void)
{
    int r;
    u8 key;
    u8 tmp[20];
    u16 times;
    s16 gain;
    node_t n={0};

    key = knob_get_key(&times);
    if(key!=KEY_NONE) {
        r = dsp_gain_step(key, times, &gain, &n);
        if(r) {
            return;
        }

        e2p_put(&n);
        sprintf((char*)tmp, "%3d", gain);
        lcd_draw_string_align(0, 100, LCD_WIDTH, 60, tmp, FONT_32, LCD_FC, LCD_BC, ALIGN_MIDDLE, 0);
    }
}



int main(void)
{
    NVIC_SetVectorTable (NVIC_VectTab_FLASH, APP_OFFSET);
    __enable_irq();
    sys_init();

#ifdef RTX
    //SystemCoreClockUpdate();
    osKernelInitialize();
    
    osThreadNew(com_task, NULL, NULL);
    osThreadNew(dev_task, NULL, NULL);
    osThreadNew(gui_task, NULL, NULL);
    osKernelStart();
#else
	tim_init(TIMER3, 100, poll_cb);

	while(1) {
        usb_proc();
        knob_proc();
        poll_proc();
	}
#endif




    return 0;
}



