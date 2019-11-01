#include "power.h"
#include "stm32f10x.h"


int power_init(void)
{
    return 0;
}


int power_en(u8 dev, u8 on)
{
    BitAction b;

    switch(dev) {

        case DEV_AMP:
        b = on?Bit_RESET:Bit_SET;
        GPIO_WriteBit(GPIOB, GPIO_Pin_8, b);
        break;

        case DEV_ALL:
        b = on?Bit_RESET:Bit_SET;
        GPIO_WriteBit(GPIOA, GPIO_Pin_1, b);
        break;
    }

    return 0;
}




