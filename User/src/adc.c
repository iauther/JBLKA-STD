
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
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    u8 key;
    //HAL_ADC_Stop_DMA(&hadc1);

    key = get_key();
#ifdef APP
{
    evt_t e;
    key_t *k=(key_t*)e.data;
    
    k->key = key;
    k->type = PANEL;
    if(k->key == KEY_NONE) {    //press up
        k->updown = UP;
    }
    else {      //press down
        if(k->key != adcTrigKey) {
            k->updown = DOWN;
        }
    }
    k->longPress = 0;

    e.evt = EVT_KEY;
    ui_send_evt(&e);
    adcTrigKey = k->key;
}
#endif

    HAL_ADC_Start_DMA(&hadc1,(u32*)adcValue, ADC_CH_NUM);
}



int adc_init(u8 mode)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    
    adcMode = mode;
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
    hadc1.Init.ContinuousConvMode = (adcMode==MODE_DMA)?ENABLE:DISABLE;
    hadc1.Init.DiscontinuousConvMode = (adcMode==MODE_DMA)?DISABLE:ENABLE;
    hadc1.Init.NbrOfConversion = ADC_CH_NUM;
    hadc1.Init.NbrOfDiscConversion = 0;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
    //hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        return -1;
    }
    /** Configure Regular Channel 
    */
    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        return -1;
    }

    sConfig.Channel = ADC_CHANNEL_7;
    sConfig.Rank = ADC_REGULAR_RANK_2;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        return -1;
    }
    
    HAL_ADCEx_Calibration_Start(&hadc1);
    if(adcMode==MODE_POLL) {
        adc_get_key();
        //HAL_ADC_Start(&hadc1);
    }
    else if(adcMode==MODE_IT) {
        HAL_ADC_Start_IT(&hadc1);
    }
    else if(adcMode==MODE_DMA) {
        dma_init();
        //adc_tim_init();
        HAL_ADC_Start_DMA(&hadc1,(u32*)adcValue, ADC_CH_NUM);
    }

    return 0;
}


int adc_deinit(void)
{
    if(adcMode==MODE_DMA) {
        dma_deinit();
    }
    HAL_ADC_DeInit(&hadc1);
    
    return 0;
}

static void read_adc(void)
{
    u8 i;
    
    //HAL_ADC_Stop(&hadc1);
    for(i=0; i<ADC_CH_NUM; i++) {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 0xffff);
        adcValue[i] = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
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



void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(adcHandle->Instance==ADC1)
    {
        /* ADC1 clock enable */
        __HAL_RCC_ADC1_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        dma_config();
        
        /* ADC1 interrupt Init */
        HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
    if(adcHandle->Instance==ADC1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_ADC1_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2|GPIO_PIN_3);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6|GPIO_PIN_7);
        
        HAL_DMA_DeInit(adcHandle->DMA_Handle);

        /* ADC1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
    }
} 

