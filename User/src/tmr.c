#include "stm32f10x.h"
#include "tmr.h"


tim_cb tim_cb_fn[TIMER_MAX]={0};

#if 1
#define TIM_MAX  7
TIM_TypeDef *timDef[TIMER_MAX]={TIM2,TIM3,TIM4,TIM5,TIM6,TIM7};
IRQn_Type   timIRQn[TIMER_MAX]={TIM2_IRQn,TIM3_IRQn,TIM4_IRQn,TIM5_IRQn,TIM6_IRQn,TIM7_IRQn};

u32 rccPeriphClock[TIMER_MAX]={RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3, RCC_APB1Periph_TIM4, 
                        RCC_APB1Periph_TIM5,RCC_APB1Periph_TIM6, RCC_APB1Periph_TIM7};
#endif

u32 ms_counter=0;
void tim_init(u8 timer, u16 ms, tim_cb cb)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(rccPeriphClock[timer], ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = ms; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler = 36000-1; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2; //TIM_CKD_DIV1
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(timDef[timer], &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(timDef[timer], TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

    NVIC_InitStructure.NVIC_IRQChannel = timIRQn[timer];  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(timDef[timer], ENABLE);  //使能TIMx外设
    tim_cb_fn[timer] = cb;
}


void tim_callback(u8 timer)   //TIM中断
{
    if (TIM_GetITStatus(timDef[timer], TIM_IT_Update) != RESET) {//检查指定的TIM中断发生与否:TIM 中断源 
        TIM_ClearITPendingBit(timDef[timer], TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
    }

    if(tim_cb_fn[timer]) {
        tim_cb_fn[timer]();
    }
}

    












