#ifndef __GPIO_H__
#define __GPIO_H__

#include "types.h"


#define POWER_PIN           GPIO_Pin_1          //PA1       高关   低开
#define HDMI_RESET_PIN      GPIO_Pin_15         //PA16      低复位

#define SOUND_RESET_PIN     GPIO_Pin_5          //PB5       低复位
#define AMP_POWER_PIN       GPIO_Pin_8          //PB8       高关   低开
#define RCA_MUTE_PIN        GPIO_Pin_9          //PB9       高静音 低取消静音

#define DSP_RESET_PIN       GPIO_Pin_8          //PC8       低复位

int gpio_init(void);


#endif
