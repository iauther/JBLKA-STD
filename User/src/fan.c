#include "fan.h"
#include "stm32f10x.h"


#define TEMP_MIN    40
#define TEMP_MAX    60


//mv: 0~3300mv
static void set_dac_vol(u16 mv)
{
    u32 tmp;

    if(mv>3300) {
        return;
    }

    tmp = mv*4096;
    tmp /= 3300;
    DAC_SetChannel1Data(DAC_Align_12b_R, (u16)tmp);
}

int fan_init(void)
{
    DAC_InitTypeDef x1={0};
    GPIO_InitTypeDef x2={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
    x2.GPIO_Pin = GPIO_Pin_4;                 // 端口配置
    x2.GPIO_Mode = GPIO_Mode_AIN;          //模拟输入
    x2.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &x2);
    GPIO_SetBits(GPIOA, GPIO_Pin_4)    ;//PA.4 输出高

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );
    x1.DAC_Trigger = DAC_Trigger_None;  //不使用触发功能  TEN1=0
    x1.DAC_WaveGeneration = DAC_WaveGeneration_None;//不使用波形发生
    x1.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    x1.DAC_OutputBuffer = DAC_OutputBuffer_Disable ;  //DAC1 输出缓存关闭
    DAC_Init(DAC_Channel_1, &x1);
    DAC_Cmd(DAC_Channel_1, ENABLE);

    //fan_set(50);
    set_dac_vol(1600);

    return 0;
}



int fan_set(u8 temp)
{
    if(temp<=TEMP_MIN) {
        set_dac_vol(0);
    }
    else if(temp>=TEMP_MAX){
        set_dac_vol(3300);
    }
    else {
        u16 mv=3300*(temp-40)/(TEMP_MAX-TEMP_MIN);
        set_dac_vol(mv);
    }

    return 0;
}








