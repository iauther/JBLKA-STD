
#include "adc.h"
#include "key.h"


typedef struct {
    u16     code;
    u8      key;
}adc_info_t;


const adc_info_t adcTab0[5] = {
    {0x004e,    KEY_ENTER},
    {0x07d8,    KEY_MIC},
    {0x0a67,    KEY_EFFECT},
    {0x0297,    KEY_SAVE},
    {0x04c5,    KEY_EXIT},
};

const adc_info_t adcTab1[2] = {
    {0x0a68,    KEY_MUSIC},
    {0x04c4,    KEY_PRESET},
};


#define GAP             10
#define ADC_CH_NUM      2
#define in_range(x,v)   ((v-GAP)<=x && x<=(v+GAP))

u8 adcMode=0;
u8 adcKey=KEY_NONE;
u8 adcTrigKey=KEY_NONE;
u16 adcValue[ADC_CH_NUM];


static u8 get_key(void)
{
    u8 i,key,key0,key1;

    key=key0=key1=KEY_NONE;
    if(adcValue[0] < 0x0f00) {
        for(i=0; i<5; i++) {
            if(in_range(adcValue[0], adcTab0[i].code)) {
                key0 = adcTab0[i].key;
                break;
            }
        }
    }

    if(adcValue[1] < 0x0f00) {
        for(i=0; i<2; i++) {
            if(in_range(adcValue[1], adcTab1[i].code)) {
                key1 = adcTab1[i].key;
                break;
            }
        }
    }
    
    if(key0!=KEY_NONE && key1!=KEY_NONE) {
        return KEY_NONE;
    }

    if(key0!=KEY_NONE) {
        key = key0;
    }
    else if(key1!=KEY_NONE) {
        key = key1;
    }
    adcKey = key;

    return key;
}



int adc_init(u8 mode)
{
    

    return 0;
}


int adc_deinit(void)
{
    
    
    return 0;
}

static void read_adc(void)
{
    
}
u8 adc_get_key(void)
{
    if(adcMode==MODE_POLL) {
        read_adc();
        return get_key();
    }
    else if(adcMode==MODE_IT) {
        
    }
    else if(adcMode==MODE_DMA) {
        return adcKey;
    }
    return KEY_NONE;
}



