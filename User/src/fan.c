#include "fan.h"
#include "dac.h"
#include "stm32f10x.h"


#define TEMP_MIN    40
#define TEMP_MAX    60

int fan_init(void)
{
    dac_init(DAC_CH1);
    fan_set(50);

    return 0;
}


int fan_set(u8 temp)
{
    if(temp<=TEMP_MIN) {
        dac_set(DAC_CH1, 0);
    }
    else if(temp>=TEMP_MAX){
        dac_set(DAC_CH1, 3300);
    }
    else {
        u16 mv=3300*(temp-40)/(TEMP_MAX-TEMP_MIN);
        dac_set(DAC_CH1, mv);
    }

    return 0;
}


