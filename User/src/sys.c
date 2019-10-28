#include <string.h>
#include "sys.h"
#include "dsp.h"
#include "fan.h"
#include "lcd.h"
#include "e2p.h"
#include "gpio.h"
#include "delay.h"
#include "paras.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_desc.h"
#include "stm32f10x.h"
#include "config.h"


static void rca_mute(u8 on)         //PB9, 高静音，低取消静音
{
    GPIO_InitTypeDef init={0};
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Pin = GPIO_Pin_9;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &init);

    if(on) {
        GPIO_SetBits(GPIOB, GPIO_Pin_9);
    }
    else {
        GPIO_ResetBits(GPIOB, GPIO_Pin_9);
    }
}
static void adda_reset(void)      //PB5, 低电平复位，时间至少1秒，(开机默认低电平)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_5);
    delay_ms(1000);        //for test
    GPIO_SetBits(GPIOB, GPIO_Pin_5);
}

static void usb_dp_reset(void)
{
    GPIO_InitTypeDef init={0};
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Pin = GPIO_Pin_12;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &init);

    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
    delay_ms(10);
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
    delay_ms(10);
}
static void usb_init(void)
{
    USB_Interrupts_Config();
	Set_USBClock();

    usb_dp_reset();
	USB_Init();
}
////////////////////////////////////////////////
static int do_config(void)
{
    do {
        delay_ms(10);
    }while(!dsp_is_started());
    
    dsp_download();
    dsp_version();
    adda_reset();
    rca_mute(0);
    sys_set_iodat(0);
    sys_set_input(gParams.dsp.Array_Input.input);
    
    return 0;
}

int sys_init(void)
{
    NVIC_SetVectorTable (NVIC_VectTab_FLASH, APP_OFFSET);
    __enable_irq();

    Set_System();
    paras_init();

    gpio_init();
    dsp_init();
    fan_init();
    lcd_init();
    usb_init();

    do_config();

    return 0;
}





int sys_set_input(u16 input)
{
#if 0
    GPIO_InitTypeDef  init;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    init.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &init);
#endif

    switch(input) {

        case INPUT_VOD: //11
        GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_SET);
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
        break;

        case INPUT_DVD: //00
        GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_RESET);
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
        
        break;

        case INPUT_BGM: //10
        GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_SET);
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET);
        break;

        case INPUT_BTUSB://01
        GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_RESET);
        GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET);
        break;

        case INPUT_OPT:
        break;

        case INPUT_HDMI_IN:
        break;

        case INPUT_HDMI_ARC:
        break;
    }
    
    return 0;
}


int sys_set_iodat(io_data_t *io)
{
    io_data_t *pio=io?io:&gParams.iodat;


    return 0;
}


int sys_set_default(void)
{
    paras_default();
    dsp_download();
    sys_config();
    
    return 0;
}


