#include "task.h"


#define MSG_MAX         6
//static const osThreadAttr_t devAttr = {
//  .stack_size = 400U
//};



status_data_t gStatus;
#ifdef RTX
osTimerId_t dev_tmr;
msg_t *dev_msg=NULL;

extern paras_data_t gParams;

static void pwr_check(void)
{
    u16 amp_pwr1,amp_pwr2;
    
    amp_pwr1 = adc_read(ADC_CH_AMP_PWR1)*3300/4096;     //20%
    amp_pwr2 = adc_read(ADC_CH_AMP_PWR2)*3300/4096;     //<500mv

    if(amp_pwr1<(1760-176*2) || amp_pwr1>(1760+176*2) || (amp_pwr2>500)) {
        power_en(DEV_AMP, 0);
    }
    else {
        power_en(DEV_AMP, 1);
    }

    

}
static void temp_check(void)
{
    u16 amp_temp, pwr_temp;
    amp_temp = adc_read(ADC_CH_AMP_TEMP);
    pwr_temp = adc_read(ADC_CH_PWR_TEMP);

    //do something...
    //topbar_set();
}
static void amp_check(void)
{
    static u8 low_cnt=0;
        
    if(amp_get_level()) {
        low_cnt = 0;
    }

    low_cnt++;
    if(low_cnt%2) {
        amp_pwr(0);
    }
}
static void dev_check(void)
{
    amp_check();
    pwr_check();
    temp_check();
}
static void eep_trigger(void)
{
    evt_eep_t e={EVT_E2P};
    eep_post_evt(&e);
}



static void tmr1_fun (void *arg)
{
    evt_dev_t e={0};
    e.evt = EVT_TIMER;
    msg_post(dev_msg, &e, sizeof(e));
}
//////////////////////////////////////////////////////////

void dev_task(void *arg)
{
    int r;
    u8  i;
    evt_dev_t e;
    osStatus_t st;
    
    dev_msg = msg_init(MSG_MAX, sizeof(e));
    if(!dev_msg) {
        return;
    }

    dev_tmr = osTimerNew(tmr1_fun, osTimerPeriodic, NULL, NULL);
    osTimerStart(dev_tmr, 1000);

    while(1) {
        r = msg_recv(dev_msg, &e, sizeof(e));
        if(r==0) {
            switch(e.evt) {
                
                case EVT_TIMER:
                {
                    eep_trigger();
                    dev_check();
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
    msg_free(&dev_msg);
}


void dev_post_evt(evt_dev_t *e)
{
    msg_post(dev_msg, e, sizeof(*e));
}

#endif




