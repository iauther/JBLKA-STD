#ifndef _TMR_H
#define _TMR_H

#include "stdint.h"

//开关定时器
#define TIMER_ON(TIMx)  ((TIM_TypeDef*)TIMx)->CR1 |= TIM_CR1_CEN
#define TIMER_OFF(TIMx)  ((TIM_TypeDef*)TIMx)->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN))

//开关更新中断
#define TIMER_IT_ON(TIMx,TIM_IT)	  ((TIM_TypeDef*)TIMx)->DIER |= TIM_IT
#define TIMER_IT_OFF(TIMx,TIM_IT)   ((TIM_TypeDef*)TIMx)->DIER &= (uint16_t)~TIM_IT

//读取定时器开关状态
#define TIMER_STATE(TIMx)			  ((TIM_TypeDef*)TIMx)->CR1 & TIM_CR1_CEN

//定时器计数值清零
#define TIMER_CLEAR_CNT(TIMx)			  ((TIM_TypeDef*)TIMx)->CNT =((uint16_t)0)

void tim2_init( void );
void tim3_init( void );
void tim2_callback(void);
void tim3_callback(void);
void tim4_init( void );
void tim4_callback(void);
#endif








