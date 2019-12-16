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
    TIM_TypeDef *tmr=timDef[timer];

    RCC_APB1PeriphClockCmd(rccPeriphClock[timer], ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = ms-1;
    TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(tmr, &TIM_TimeBaseStructure);

    TIM_ITConfig(tmr, TIM_IT_Update,ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = timIRQn[timer];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(tmr, ENABLE);  //使能TIMx外设
    tim_cb_fn[timer] = cb;
}


void tim_callback(u8 timer)   //TIM中断
{
    TIM_TypeDef *tmr=timDef[timer];
    
    if (TIM_GetITStatus(tmr, TIM_IT_Update) != RESET) {//检查指定的TIM中断发生与否:TIM 中断源 
        TIM_ClearITPendingBit(tmr, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 

        if(tim_cb_fn[timer]) {
            tim_cb_fn[timer](0);
        }
    }
    TIM_ClearFlag(tmr, TIM_FLAG_Update);  
}

    












