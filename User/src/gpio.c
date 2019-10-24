#include "stm32f10x.h"
#include "gpio.h"


int gpio_init(void)
{
    u32 pins_a, pins_b, pins_c;
    GPIO_InitTypeDef  init;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

    /*Configure GPIO pin Output Level */
    pins_a = GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_15;
    GPIO_ResetBits(GPIOA, pins_a);

    /*Configure GPIO pin Output Level */
    pins_b = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_12 
                      |GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_ResetBits(GPIOB, pins_b);

    GPIO_SetBits(GPIOB, GPIO_Pin_9);
    GPIO_SetBits(GPIOC, GPIO_Pin_8);

    pins_c = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5 |GPIO_Pin_6|
           GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_ResetBits(GPIOC, pins_c);

    //gpio a
    init.GPIO_Pin = GPIO_Pin_0;
    init.GPIO_Mode = GPIO_Mode_IPU;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &init);

    init.GPIO_Pin = pins_a;
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &init);

    //gpio b
    init.GPIO_Pin = GPIO_Pin_11;
    init.GPIO_Mode = GPIO_Mode_AF_PP;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &init);
    
    init.GPIO_Pin = GPIO_Pin_8;
    init.GPIO_Mode = GPIO_Mode_IPD;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &init);

    init.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    init.GPIO_Mode = GPIO_Mode_AF_PP;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &init);

    init.GPIO_Pin = pins_b;
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &init);

    //gpio c
    init.GPIO_Pin = pins_c;
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOC, &init);

    return 0;
}






