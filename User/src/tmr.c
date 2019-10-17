#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "tmr.h"

volatile unsigned char  uart_timeout   =0	;


/*
  TIM2定时0.2ms
*/
void tim2_init()
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  TIMER_OFF(TIM2);
  TIMER_CLEAR_CNT(TIM2);

  TIM_DeInit(TIM2);  											//定时器x相关寄存器改为默认值
  TIM_TimeBaseStructure.TIM_Period = 200;						//自动重载定时周期
  TIM_TimeBaseStructure.TIM_Prescaler= 71;						//定时器时钟预分频
  TIM_TimeBaseStructure.TIM_ClockDivision= TIM_CKD_DIV1;		//采样分频 = CK_INT
  TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;	//计数模式	向上计数
  TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

  TIM_PrescalerConfig(TIM2, 71, TIM_PSCReloadMode_Immediate);	   
  TIM_ClearFlag(TIM2,TIM_FLAG_Update);							//清除更新标记  
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);					//开定时器中断
 
  /* Enable the TIMER Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


//用于RS485接收超时，超过2个字节的时间没有收到数据认为接收完成
void tim2_callback(void)
{
   if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
   {
	  	TIMER_OFF(TIM2)			;	//关闭计数器
		TIMER_CLEAR_CNT(TIM2)	;	//计数器清零
		uart_timeout   =1		; 	//RS485接收超时
		//tmr2cnt++;
	    TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);       
   }
}

/*
  TIM3定时5ms
*/
void tim3_init()
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  TIMER_OFF(TIM3);
  TIMER_CLEAR_CNT(TIM3);

  TIM_DeInit(TIM3);  											//定时器x相关寄存器改为默认值
  TIM_TimeBaseStructure.TIM_Period = 5000;						//Auto-Reload Register
  TIM_TimeBaseStructure.TIM_Prescaler= 71;						//prescaler value	 预分频寄存器此处写0，它将在下面重新设置
  TIM_TimeBaseStructure.TIM_ClockDivision= TIM_CKD_DIV1;		//采样分频 = CK_INT
  TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;	//计数模式	向上计数
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

  TIM_PrescalerConfig(TIM3, 71, TIM_PSCReloadMode_Immediate);	   
  TIM_ClearFlag(TIM3,TIM_FLAG_Update);		//清除更新标记  
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//开定时器中断

  /* Enable the TIMER Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


//HID接收超时，4ms没有收到HID的包则认为传输结束
void tim3_callback(void)
{
   if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
   {
	  	TIMER_OFF(TIM3)			;	//关闭计数器
		TIMER_CLEAR_CNT(TIM3)	;	//计数器清零
		//tmr3cnt++;
		//USB_Received_state=2	; 	//接收完毕
	    TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);       
   }
}


void tim4_init()
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
}


u32 tim4_counter=0;
void tim4_callback(void)
{
   if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
   {
	  	TIMER_OFF(TIM4)			;	//关闭计数器
		TIMER_CLEAR_CNT(TIM4)	;	//计数器清零
		tim4_counter++;
	    TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);       
   }
}










