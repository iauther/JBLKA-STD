#include "dac.h"
#include "stm32f10x.h"

static void set_value(float Voltage)
{
  uint16_t data;

  data = (uint16_t)((Voltage/3.3) * 4096);                           //换算数值

  //DAC_SetChannel1Data(DAC_Align_12b_R, data);                        //12位数 右对齐
  //DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);                     //启动转换
}

int dac_init(void)
{
    GPIO_InitTypeDef x1={0};
    DAC_InitTypeDef x2={0};
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    x1.GPIO_Pin = GPIO_Pin_4;                 // 端口配置
    x1.GPIO_Mode = GPIO_Mode_AIN;          //模拟输入
    x1.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &x1);
    //DAC_DeInit();
    
    x2.DAC_Trigger = DAC_Trigger_Software;
    x2.DAC_WaveGeneration = DAC_WaveGeneration_None;//不使用波形发生
    x2.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits11_0;
    x2.DAC_OutputBuffer = DAC_OutputBuffer_Enable ;  //DAC1 输出缓存关闭
    DAC_Init(DAC_Channel_1, &x2);
    DAC_Cmd(DAC_Channel_1, ENABLE);

    return 0;
}


int dac_set(u8 ch, u16 v)
{
    u8  chn;
    u32 tmp;

    if(v>3300) {
        return -1;
    }

    tmp = v*4096;
    tmp /= 3300;

    chn = (ch==CHANNEL1)?DAC_Channel_1:DAC_Channel_2;
    DAC_SetChannel1Data(DAC_Align_12b_R, (u16)tmp);
    DAC_SoftwareTriggerCmd(chn, ENABLE);
    return 0;
}





