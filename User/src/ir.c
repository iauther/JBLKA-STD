#include "ir.h"
#include "key.h"
#include "task.h"

/*
                                    NEC Protocol

    Sequence:   |-------leader-------|-------address(8bits)-------|-------~address(8bits)--------|----------command(8bits)---------|---------~command(8bits)---------|
                    H(9ms)+L(4.5ms)

    Logic Bit:  |-------logic 1-------|-------logic 0-------|
                   H(560us)+L(1690us)    H(560us)+L(5600us)

    Transfer Sequence: LSB First

    RepeatPuls: |-----------------puls------------------|
                   H(9ms)+L(2.25ms)+H(560us)+L(98.19ms)

    Repeat Mode: |-----command-------|-----RepeatPlus-----|------RepeatPlus------|............|
                        110ms               110ms                  110ms               ...
*/

#define in_range(v,min,max) (v>min && v<max)
#define IR_ADDR             0x88
#define IR_TIM              TIM2
#define IR_LEVEL()          GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_11)


ircode_t irCode;
ircode_t *pIR=&irCode;

const irtab_t irTab[] = {
    {0xf3,          KEY_LOCK},
    {0xf3,          KEY_INPUT},
    {0xf3,          KEY_MUTE},
    {0xf3,          KEY_PREV},
    {0xf3,          KEY_NEXT},
    {0xf3,          KEY_PLAYPAUSE},
    {0xf3,          KEY_ONELOOP},

    {0xf3,          KEY_MODE},
    {0xf3,          KEY_M1},
    {0xf3,          KEY_M2},
    {0xf3,          KEY_M3},
    {0xf3,          KEY_SHARP},
    {0xf3,          KEY_0},
    {0xf3,          KEY_b},
    
    {0xf3,          KEY_MUSIC_UP},
    {0xf3,          KEY_MUSIC_DN},
    {0xf3,          KEY_EFFECT_UP},
    {0xf3,          KEY_EFFECT_DN},
    {0xf3,          KEY_MIC_UP},
    {0xf3,          KEY_MIC_DN},
};
static u8 get_cmd(u32 v)
{
    if( (u8)( (v >> 24) ^ (v >> 16) ) == 0xff  )
        return (u8)(v>>16);
    else
        return 0;
}
static u8 get_addr(u32 v)
{
    if( (u8)( (v ) ^ (v >> 8) ) == 0xff  )
        return (u8)(v);
    else
        return 0;
}

	 
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(IR_TIM, TIM_IT_Update)!=RESET) {
		pIR->htime = 0;
        pIR->ltime = 0;
        pIR->started = 0;
	}

    if(TIM_GetITStatus(IR_TIM, TIM_IT_CC4)!=RESET) {
		if(IR_LEVEL()) {//上升沿捕获，//红外数据输入脚
            TIM_OC4PolarityConfig(IR_TIM, TIM_ICPolarity_Falling);	
            if(pIR->started==1) {
                pIR->ltime = TIM_GetCapture4(IR_TIM);
            }
        }
        else {
            TIM_OC4PolarityConfig(IR_TIM, TIM_ICPolarity_Rising);
            pIR->htime = TIM_GetCapture4(IR_TIM);
            if(pIR->started==0) pIR->started = 1;
        }

        if(in_range(pIR->htime, 8500, 9500) && in_range(pIR->ltime, 4000, 5000) ) {         //判断是否为引导码(高9ms, 低4.5ms)
            pIR->key = 0;  //同步码
            pIR->bits = 0;
            pIR->repeat = 0;
        }
        else if(in_range(pIR->htime, 8500, 9500) && in_range(pIR->ltime, 2000, 3000) ) {    //判断是否为连发(高9ms, 低2.25ms)
             pIR->repeat = 1;
        }

        /********************************* 接收数据 *********************************/
        if(IR_LEVEL()==0) {
            if(in_range(pIR->htime, 450, 650) && in_range(pIR->ltime, 450, 650) ) {           //高低电平时间都为560us, 则该位为0
                pIR->key &= ~(1<pIR->bits++);
            }
            else if(in_range(pIR->htime, 450, 650) && in_range(pIR->ltime, 1450, 1750) ) {    //高电平时间560ms,低电平1690us, 则该位为1
                pIR->key |= 1<<pIR->bits++;
            }
        }
        
        if(pIR->bits==32 || pIR->repeat==1) {
            //send ir key to task
#ifdef RTX
            evt_gui_t e={0};
            key_t *k=&e.key;

            k->value = get_cmd(pIR->key);
            k->src = SRC_IR;
            k->updown = 0;
            k->longPress = pIR->repeat;

            e.evt = EVT_KEY;
            gui_post_evt(&e);
#endif
        }

        TIM_SetCounter(IR_TIM, 0);	
    }

	TIM_ClearITPendingBit(IR_TIM, TIM_IT_Update|TIM_IT_CC4);	 	    
}




int ir_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PORTB时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//TIM2 时钟使能 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 //PB11 输入 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		    //上拉输入 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_11);	//初始化GPIOB11

	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;//10000; //设定计数器自动重装值 最大值  
	TIM_TimeBaseStructure.TIM_Prescaler = 71; 	//预分频器,1M的计数频率,1us加1.
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(IR_TIM, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  // 选择输入端 IC4映射到TI4上
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x03;//IC4F=0011 配置输入滤波器 8个定时器时钟周期滤波
	TIM_ICInit(IR_TIM, &TIM_ICInitStructure);//初始化定时器输入捕获通道
	TIM_Cmd(IR_TIM,ENABLE ); 	//使能定时器4

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	

	TIM_ITConfig(IR_TIM, TIM_IT_Update|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC4IE捕获中断	

    return 0;
}



