#include "device.h"
#include "sys.h"

//#include "stm32f10x.h"

#define E2P_POLL_TIME       1000        //1000ms
#define ADC_KEY_POLL_TIME   100         //100ms
#define PWR_VOL_POLL_TIME   1000
#define QUEUE_MAX           20


queue_t *e2p_q=0;
extern paras_data_t gParams;
typedef void (*jump_func)(void);
jump_func jump_fn;
uint32_t jump_addr;
static void jump_to(u32 addr)
{
    if(((*(volatile u32*)addr) & 0x2FFE0000) == 0x20000000) {
        
        jump_addr = *(volatile u32*)(addr+4);
        jump_fn = (jump_func)jump_addr;
        
        __disable_irq();
        __set_CONTROL(0);   //??psp????msp
        __set_MSP(*(volatile u32*)addr);
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

    r = queue_get(e2p_q, &n);
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
    amp_pwr1 = adc_read(ADC_CH_AMP_PWR1);
    amp_pwr2 = adc_read(ADC_CH_AMP_PWR2);
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







static int value_adjust(s16 *ptr, int min, int max, int fac, int val, int step)
{
    int ost=val*step;

    if(fac==1) {
        if(*ptr==max) {
            return -1;
        }
        else if(*ptr+ost > max) {
            *ptr = max;
        }
        else {
            *ptr += ost;
        }
    }
    else {
        if(*ptr==min) {
            return -1;
        }
        else if(*ptr+ost< min) {
            *ptr = min;
        }
        else {
            *ptr += ost;
        }
    }

    return 0;
}



static void knob_proc(void)
{
    u8 key;
    u16 times;
    node_t n={0};
    int r,fac,val;
    dsp_data_t dd={0};
    dsp_paras_t *dsp=&uiParams.dsp;

    key = knob_get_key(&times);
    if(key!=KEY_NONE) {
        switch(key) {
            case KEY_MUSIC_UP:      //0~100
            case KEY_MUSIC_DN:
            {
                u16 *gain=&dsp->music.gain->Gain;
                fac=(key==KEY_MUSIC_UP)?1:-1;
                val=fac*times;
                r = value_adjust((s16*)gain, 0, 100, fac, val, 1);
                if(r) {
                    return;
                }

                n.ptr = dsp->music.gain;
                n.len = sizeof(TypeS_Gain);
                dd.id = CMD_ID_Gain;
                dd.ch = Gain_CH_Music;
            }
            break;

            case KEY_EFFECT_UP:
            case KEY_EFFECT_DN:
            {
                u16 *gain=&dsp->effGain->Gain;
                fac=(key==KEY_EFFECT_UP)?1:-1;
                val=fac*times;
                r = value_adjust((s16*)gain, 0, 100, fac, val, 1);
                if(r) {
                    return;
                }

                n.ptr = dsp->effGain;
                n.len = sizeof(TypeS_Gain);
                dd.id = CMD_ID_Gain;
                dd.ch = Gain_CH_Eff;
            }
            break;

            case KEY_MIC_UP:
            case KEY_MIC_DN:
            {
                u16 *gain=&dsp->mic.gain->Gain;
                fac=(key==KEY_MIC_UP)?1:-1;
                val=fac*times;
                r = value_adjust((s16*)gain, 0, 100, fac, val, 1);
                if(r) {
                    return;
                }
                
                n.ptr = dsp->mic.gain;
                n.len = sizeof(TypeS_Gain);
                dd.id = CMD_ID_Gain;
                dd.ch = Gain_CH_Mic;
            }
            break;
            
            case KEY_MUSIC_TREBLE_UP:
            case KEY_MUSIC_TREBLE_DN:
            {
                s16 *gain=&dsp->music.geq[1]->Gain;
                fac=(key==KEY_MUSIC_TREBLE_UP)?1:-1;
                val=fac*times;
                r = value_adjust(gain, -240, 120, fac, val, 1);
                if(r) {
                    return;
                }

                n.ptr = dsp->music.geq[1];
                n.len = sizeof(TypeS_EQBand);
                dd.id = CMD_ID_EQ;
                dd.ch = EQ_CH_Music;
                dd.n  = 1;
            }
            break;
            
            case KEY_MUSIC_BASS_UP:
            case KEY_MUSIC_BASS_DN:
            {
                s16 *gain=&dsp->music.geq[0]->Gain;
                fac=(key==KEY_MUSIC_BASS_UP)?1:-1;
                val=fac*times;
                r = value_adjust(gain, -240, 120, fac, val, 1);
                if(r) {
                    return;
                }

                n.ptr = dsp->music.geq[0];
                n.len = sizeof(TypeS_EQBand);
                dd.id = CMD_ID_EQ;
                dd.ch = EQ_CH_Music;
                dd.n  = 0;
            }
            break;
            
            case KEY_ECHO_LEVEL_UP:
            case KEY_ECHO_LEVEL_DN:
            {
                u16 *gain=&dsp->echo.effVol->Vol;
                fac=(key==KEY_ECHO_LEVEL_UP)?1:-1;
                val=fac*times;
                r = value_adjust((s16*)gain, 0, 100, fac, val, 1);
                if(r) {
                    return;
                }
                
                n.ptr = dsp->echo.effVol;
                n.len = sizeof(TypeS_Vol);
                dd.id = CMD_ID_Vol;
                dd.ch = Vol_CH_Echo_EffVol;
            }
            break;

            case KEY_ECHO_DELAY_UP:
            case KEY_ECHO_DELAY_DN:
            {
                u16 *gain=&dsp->echo.delay->Delay;
                fac=(key==KEY_ECHO_DELAY_UP)?1:-1;
                val=fac*times;
                r = value_adjust((s16*)gain, 0, 14400, fac, val, 48);
                if(r) {
                    return;
                }

                n.ptr = dsp->echo.delay;
                n.len = sizeof(TypeS_Delay);
                dd.id = CMD_ID_Delay;
                dd.ch = Delay_CH_Echo_Delay;
            }
            break;

            case KEY_ECHO_REPEAT_UP:
            case KEY_ECHO_REPEAT_DN:
            {
                u16 *gain=&dsp->echo.repeat->Vol;
                fac=(key==KEY_ECHO_REPEAT_UP)?1:-1;
                val=fac*times;
                r = value_adjust((s16*)gain, 0, 90, fac, val, 1);
                if(r) {
                    return;
                }

                n.ptr = dsp->echo.repeat;
                n.len = sizeof(TypeS_Vol);
                dd.id = CMD_ID_Vol;
                dd.ch = Vol_CH_Echo_Repeat;
            }
            break;
            
            case KEY_REVERB_LEVEL_UP:
            case KEY_REVERB_LEVEL_DN:
            {
                u16 *gain=&dsp->reverb.effVol->Vol;
                fac=(key==KEY_REVERB_LEVEL_UP)?1:-1;
                val=fac*times;
                r = value_adjust((s16*)gain, 0, 100, fac, val, 1);
                if(r) {
                    return;
                }
                
                n.ptr = dsp->reverb.effVol;
                n.len = sizeof(TypeS_Vol);
                dd.id = CMD_ID_Vol;
                dd.ch = Vol_CH_Rev_EffVol;
            }
            break;

            case KEY_REVERB_TIME_UP:
            case KEY_REVERB_TIME_DN:
            {
                u16 *gain=&dsp->reverb.time->Delay;
                fac=(key==KEY_REVERB_TIME_UP)?1:-1;
                val=fac*times;
                r = value_adjust((s16*)gain, 0, 8000, fac, val, 1);
                if(r) {
                    return;
                }

                n.ptr = dsp->reverb.time;
                n.len = sizeof(TypeS_Delay);
                dd.id = CMD_ID_Delay;
                dd.ch = Delay_CH_Rev_Time;
            }
            break;
            
            case KEY_MIC_TREBLE_UP:
            case KEY_MIC_TREBLE_DN:
            {
                s16 *gain=&dsp->mic.geq[2]->Gain;
                fac=(key==KEY_MIC_TREBLE_UP)?1:-1;
                val=fac*times;
                r = value_adjust(gain, -240, 120, fac, val, 1);
                if(r) {
                    return;
                }

                n.ptr = dsp->mic.geq[2];
                n.len = sizeof(TypeS_EQBand);
                dd.id = CMD_ID_EQ;
                dd.ch = EQ_CH_Mic;
                dd.n  = 2;
            }
            break;

            case KEY_MIC_MIDDLE_UP:
            case KEY_MIC_MIDDLE_DN:
            {
                s16 *gain=&dsp->mic.geq[1]->Gain;
                fac=(key==KEY_MIC_MIDDLE_UP)?1:-1;
                val=fac*times;
                r = value_adjust(gain, -240, 120, fac, val, 1);
                if(r) {
                    return;
                }

                n.ptr = dsp->mic.geq[1];
                n.len = sizeof(TypeS_EQBand);
                dd.id = CMD_ID_EQ;
                dd.ch = EQ_CH_Mic;
                dd.n  = 1;
            }
            break;

            case KEY_MIC_BASS_UP:               //-24~+12dB
            case KEY_MIC_BASS_DN:
            {
                s16 *gain=&dsp->mic.geq[0]->Gain;
                fac=(key==KEY_MIC_BASS_UP)?1:-1;
                val=fac*times;
                r = value_adjust(gain, -240, 120, fac, val, 1);
                if(r) {
                    return;
                }

                n.ptr = dsp->mic.geq[0];
                n.len = sizeof(TypeS_EQBand);
                dd.id = CMD_ID_EQ;
                dd.ch = EQ_CH_Mic;
                dd.n  = 0;
            }
            break;
        }

        dsp_send(&dd);
        queue_put(e2p_q, &n, 1);
    }
}



int main(void)
{
	sys_init();
    e2p_q = queue_init(QUEUE_MAX);
	tim_init(TIMER2, poll_cb);

	while(1) {
        usb_proc();
        knob_proc();
        poll_proc();
	}

    return 0;
}



