#include "dac.h"
#include "stm32f10x.h"


int dac_init(u8 ch)
{
    u32 chn;
    GPIO_InitTypeDef x1={0};
    DAC_InitTypeDef  x2={0};
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    x1.GPIO_Pin = (ch==DAC_CH1)?GPIO_Pin_4:GPIO_Pin_5;
    x1.GPIO_Mode = GPIO_Mode_AIN;
    x1.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &x1);
    //DAC_DeInit();
    
    x2.DAC_Trigger = DAC_Trigger_None;//DAC_Trigger_Software;
    x2.DAC_WaveGeneration = DAC_WaveGeneration_None;
    x2.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;//DAC_LFSRUnmask_Bits11_0;
    x2.DAC_OutputBuffer = DAC_OutputBuffer_Enable ;

    chn = (ch==DAC_CH1)?DAC_Channel_1:DAC_Channel_2;
    DAC_Init(chn, &x2);
    DAC_Cmd(chn, ENABLE);

    return 0;
}


int dac_set(u8 ch, u16 v)
{
    if(v>0xfff) {
        return -1;
    }

    if(ch==DAC_CH1) {
        DAC_SetChannel1Data(DAC_Align_12b_R, v);
    }
    else {
        DAC_SetChannel2Data(DAC_Align_12b_R, v);
    }
    //DAC_SoftwareTriggerCmd(chn, ENABLE);
    return 0;
}





