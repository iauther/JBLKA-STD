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
#define IR_TIM_CCx          TIM_IT_CC4


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

static void ir_reset(void)
{
    pIR->htime = 0;
    pIR->ltime = 0;
    pIR->started = 0;
    pIR->polarity = TIM_ICPolarity_Falling;
}

	 
void TIM2_IRQHandler(void)
{
    u16 polarity;

    if(TIM_GetITStatus(IR_TIM, TIM_IT_Update)!=RESET) {
		ir_reset();
	}

    if(TIM_GetITStatus(IR_TIM, IR_TIM_CCx)!=RESET) {
    //if(TIM_GetITStatus(IR_TIM, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4) == SET) {

		if(pIR->polarity==TIM_ICPolarity_Falling) {
            polarity = TIM_ICPolarity_Rising;
            if(pIR->started==1) {
                pIR->htime = TIM_GetCapture4(IR_TIM);
            }
        }
        else {
            polarity = TIM_ICPolarity_Falling;
            if(pIR->started==0) pIR->started = 1;
            pIR->ltime = TIM_GetCapture4(IR_TIM);
        }
        TIM_SetCounter(IR_TIM, 0);
        //TIM_OC4PolarityConfig(IR_TIM, polarity);

        if(in_range(pIR->htime, 8500, 9500) && in_range(pIR->ltime, 4000, 5000) ) {         //判断是否为引导码(高9ms, 低4.5ms)
            pIR->key = 0;  //同步码
            pIR->bits = 0;
            pIR->repeat = 0;
        }
        else if(in_range(pIR->htime, 8500, 9500) && in_range(pIR->ltime, 2000, 3000) ) {    //判断是否为连发(高9ms, 低2.25ms)
             pIR->repeat = 1;
        }

        /********************************* 处理数据 *********************************/
        if(in_range(pIR->htime, 450, 650) && in_range(pIR->ltime, 450, 650) ) {           //高低电平时间都为560us, 则该位为0
            pIR->key &= ~(1<pIR->bits++);
        }
        else if(in_range(pIR->htime, 450, 650) && in_range(pIR->ltime, 1450, 1750) ) {    //高电平时间560ms,低电平1690us, 则该位为1
            pIR->key |= 1<<pIR->bits++;
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
    }

    pIR->polarity = polarity;
	TIM_ClearITPendingBit(IR_TIM, TIM_IT_Update|IR_TIM_CCx);	 	    
}




int ir_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    ir_reset();
    //TIM_DeInit(IR_TIM);
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//GPIO_SetBits(GPIOB, GPIO_Pin_11);

	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = 719;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(IR_TIM, &TIM_TimeBaseStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;//TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(IR_TIM, &TIM_ICInitStructure);

    //TIM_SelectInputTrigger(IR_TIM, TIM_TS_TI1FP1);
    //TIM_SelectSlaveMode(IR_TIM, TIM_SlaveMode_Reset);
    //TIM_SelectMasterSlaveMode(IR_TIM, TIM_MasterSlaveMode_Enable);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    //TIM_ClearFlag(IR_TIM, TIM_FLAG_Update|TIM_IT_CC4);
    //TIM_ClearITPendingBit(IR_TIM, TIM_IT_Update|TIM_IT_CC4);
	//TIM_ITConfig(IR_TIM, TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4, ENABLE);
    TIM_ITConfig(IR_TIM, TIM_IT_Update|TIM_IT_CC4, ENABLE);
	TIM_Cmd(IR_TIM, ENABLE);
    
    return 0;
}



