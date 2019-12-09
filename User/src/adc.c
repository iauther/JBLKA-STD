#include "adc.h"
#include "key.h"
#include "evt.h"
#include "task.h"
#include "stm32f10x.h"


typedef struct {
    u16     code;
    u8      key;
}adc_info_t;


const adc_info_t adcTab0[] = {
    {0x004f,    KEY_ENTER},
    {0x0800,    KEY_MIC},
    {0x0a98,    KEY_EFFECT},
    {0x029f,    KEY_SAVE},
    {0x04d2,    KEY_EXIT},
    {0,         KEY_NONE},
};

const adc_info_t adcTab1[] = {
    {0x0a98,    KEY_MUSIC},
    {0x04d2,    KEY_PRESET},
    {0x0800,    KEY_SHARP},
    {0x029f,    KEY_b},
    {0,         KEY_NONE},
};


#define GAP             200
#define IS_VALUE(x,value,gap)   (value>gap)?((value-gap)<=x && x<=(value+gap)):(x<=(value+gap))

u8 adcMode=0;
u8 adcKey=KEY_NONE;
u8 adcTrigKey=KEY_NONE;
u16 adcValue[ADC_CH_MAX];
u8 adcChannel[ADC_CH_MAX]={ADC_Channel_6, ADC_Channel_7, ADC_Channel_10, ADC_Channel_11, ADC_Channel_12, ADC_Channel_13};

static u8 calc_key(u16 v0, u16 v1)
{
    u8 i,key,key0,key1;

    key=key0=key1=KEY_NONE;
    if(v0 < 0x0f00) {
        for(i=0;;i++) {
            if(adcTab0[i].key==KEY_NONE) {
                break;
            }

            if(IS_VALUE(v0, adcTab0[i].code, GAP)) {
                key0 = adcTab0[i].key;
                break;
            }
        }
    }

    if(v1 < 0x0f00) {
        for(i=0;;i++) {
            if(adcTab1[i].key==KEY_NONE) {
                break;
            }

            if(IS_VALUE(v1, adcTab1[i].code, GAP)) {
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
static u8 get_key(void)
{
    return calc_key(adcValue[ADC_CH_KEY1], adcValue[ADC_CH_KEY2]);
}


static void dma_init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMA1_Channel1); //将DMA的通道1寄存器重设为缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA外设ADC基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)adcValue; //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //内存作为数据传输的目的地
    DMA_InitStructure.DMA_BufferSize = ADC_CH_MAX; //DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //工作在循环缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); //根据DMA_InitStruct中指定的参数初始化DMA的通道

    DMA_Cmd(DMA1_Channel1, ENABLE);
}


int adc_init(void)
{
    u8 i;

    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_Init(GPIOA,&GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_Init(GPIOC,&GPIO_InitStructure); 

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; //模数转换工作在扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //模数转换工作在连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //外部触发转换关闭
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = ADC_CH_MAX; //顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure); //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

    for(i=0; i<ADC_CH_MAX; i++) {
        ADC_RegularChannelConfig(ADC1, adcChannel[i], i+1, ADC_SampleTime_55Cycles5);
    }

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1,ENABLE);

    ADC_ResetCalibration(ADC1);//校准复位ADC1
    while(ADC_GetResetCalibrationStatus(ADC1));	//检测ADC1校准复位是否完成

    ADC_StartCalibration(ADC1);//，脚注复位完成，开始校准
    while(ADC_GetCalibrationStatus(ADC1));//检测是否校准完成
    
    dma_init();

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    return 0;
}


u16 adc_read(u8 ch)
{   
    return adcValue[ch];
}


u8 adc_get_key(void)
{
    return get_key();
}

u8 adc_key_using=0;
void adc_tmr_cb(void)
{
#ifdef RTX
    {
        u8 key;

        key = get_key();
        if(key!=KEY_NONE && !adc_key_using) {
            evt_gui_t e={0};
            
            e.evt = EVT_KEY;
            e.key.src = SRC_KEY;
            e.key.value=key;
            gui_post_evt(&e);
            adc_key_using = 1;
        }
    }
#endif
}




