#include "amp.h"
#include "delay.h"
#include "stm32f10x.h"

static void io_config(void)      //PA0中断, 低表示异常
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;              //浮动输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* EXTI配置*/
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;                         //外部中断线
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;                //中断请求（请求模式）
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;            //下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);       //EXIT中断
}




int amp_init(void)
{
    io_config();
    
    return 0;
}


int amp_get_level(void)
{
    return GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0);
}


int amp_pwr(int on)
{
    BitAction b=on?Bit_RESET:Bit_SET;
    GPIO_WriteBit(GPIOB, GPIO_Pin_8, b);
    
    return 0;
}


u8 amp_low_flag=0;
void exti0_callback(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line0)) {   
        EXTI_ClearFlag(EXTI_Line0);
        amp_low_flag = 1;
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}









