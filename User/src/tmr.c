#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "tmr.h"

volatile unsigned char  uart_timeout   =0	;


tim_callback tim2_callback_fn=0;
tim_callback tim3_callback_fn=0;
tim_callback tim4_callback_fn=0;

#if 0
#define TIM_MAX  17
TIM_TypeDef *timDef[17]={TIM1,TIM2,TIM3,TIM4,TIM5,TIM6,TIM7,TIM8,TIM9,TIM10,TIM11,TIM12,TIM13,TIM14,TIM15,TIM16,TIM17};
IRQn_Type   timIRQn[17]={0,TIM2_IRQn,TIM3_IRQn,TIM4_IRQn,TIM5_IRQn,TIM6_IRQn,TIM7_IRQn,TIM8_IRQn,TIM9_IRQn,TIM10_IRQn,
                          TIM11_IRQn,TIM12_IRQn,TIM13_IRQn,TIM14_IRQn,TIM15_IRQn,TIM16_IRQn,TIM17_IRQn};

u32 rccPeriphClock[17]={0, RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3, RCC_APB1Periph_TIM4, RCC_APB1Periph_TIM5,RCC_APB1Periph_TIM6,
                           RCC_APB1Periph_TIM7, RCC_APB1Periph_TIM8, RCC_APB1Periph_TIM9, RCC_APB1Periph_TIM10, RCC_APB1Periph_TIM11,
                           RCC_APB1Periph_TIM12, RCC_APB1Periph_TIM13, RCC_APB1Periph_TIM14, RCC_APB1Periph_TIM15, RCC_APB1Periph_TIM16, 
                           RCC_APB1Periph_TIM17};
#endif

u32 ms_counter=0;
void tim3_init(tim_callback cb)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = 1000; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值     计数到5000为500ms
    TIM_TimeBaseStructure.TIM_Prescaler = 71; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
    tim3_callback_fn = cb;
}


void tim3_callback(void)   //TIM3中断
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
    }
    
    ms_counter++;
    if(tim3_callback_fn) {
        tim3_callback_fn();
    }
}

    


//systick
void tim4_init(tim_callback cb)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    TIMER_OFF(TIM4);
    TIMER_CLEAR_CNT(TIM4);

    TIM_DeInit(TIM4);  											//定时器x相关寄存器改为默认值
    TIM_TimeBaseStructure.TIM_Period = 1000;						//自动重载定时周期
    TIM_TimeBaseStructure.TIM_Prescaler= 71;						//定时器时钟预分频
    TIM_TimeBaseStructure.TIM_ClockDivision= TIM_CKD_DIV1;		//采样分频 = CK_INT
    TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;	//计数模式	向上计数
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);

    TIM_PrescalerConfig(TIM4, 71, TIM_PSCReloadMode_Immediate);	   
    TIM_ClearFlag(TIM4,TIM_FLAG_Update);							//清除更新标记  
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);					//开定时器中断

    /* Enable the TIMER Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    tim4_callback_fn = cb;
}



void tim4_callback(void)
{
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
    {
        TIMER_OFF(TIM4)			;	//关闭计数器
        TIMER_CLEAR_CNT(TIM4)	;	//计数器清零
        TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);       
    }
    if(tim4_callback_fn) {
        tim4_callback_fn();
    }
}










