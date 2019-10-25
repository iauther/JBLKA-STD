
#include "adc.h"
#include "key.h"
#include "stm32f10x.h"


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


static void dma_init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(DMA1_Channel1); //将DMA的通道1寄存器重设为缺省值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA外设ADC基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&adcValue; //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //内存作为数据传输的目的地
    DMA_InitStructure.DMA_BufferSize = ADC_CH_NUM; //DMA通道的DMA缓存的大小
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


int adc_init(u8 mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1,ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_Init(GPIOA,&GPIO_InitStructure); 

    //第二步：复位ADC1，并设置ADC1的分频因子
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    ADC_DeInit(ADC1);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; //模数转换工作在扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //模数转换工作在连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //外部触发转换关闭
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 2; //顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure); //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 2, ADC_SampleTime_55Cycles5);

    //第四步：使能ADC1并校准
    ADC_Cmd(ADC1,ENABLE);
    ADC_GetResetCalibrationStatus(ADC1);//校准复位ADC1
    while(ADC_GetResetCalibrationStatus(ADC1));	//检测ADC1校准复位是否完成

    ADC_StartCalibration(ADC1);//，脚注复位完成，开始校准
    while(ADC_GetCalibrationStatus(ADC1));//检测是否校准完成

    return 0;
}


int adc_deinit(void)
{
    
    
    return 0;
}

static void read_adc(void)
{
    u16 i,adc_val = 0;
    u8 channel[ADC_CH_NUM]={ADC_Channel_6, ADC_Channel_7};
    
    for(i=0; i<ADC_CH_NUM; i++) {
        ADC_RegularChannelConfig(ADC1, channel[i], 1, ADC_SampleTime_55Cycles5);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )){};
        adcValue[i] = ADC_GetConversionValue(ADC1);
    }
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



