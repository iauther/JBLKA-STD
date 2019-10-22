#include "sys.h"
#include "dsp.h"
#include "delay.h"
#include "paras.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_desc.h"
#include "stm32f10x.h"
#include "config.h"

static void gpio_init(void)
{
    u32 pins_a, pins_b, pins_c;
    GPIO_InitTypeDef  init;
    
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

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
}
static void rca_mute(u8 on)         //PB9, 高静音，低取消静音
{
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
    delay_ms(50);
    GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
    delay_ms(50);
}
static void usb_init(void)
{
    USB_Interrupts_Config();
	Set_USBClock();

    usb_dp_reset();
	USB_Init();
}
////////////////////////////////////////////////


int sys_init(void)
{
    NVIC_SetVectorTable (NVIC_VectTab_FLASH, APP_OFFSET);
    __enable_irq();

    Set_System();
    paras_init();

    gpio_init();
    dsp_init();
    //lcd_init();
    usb_init();

    return 0;
}


int sys_config(void)
{
    do {
        delay_ms(10);
    }while(!dsp_is_started());

    dsp_version();
    adda_reset();
    rca_mute(0);
    sys_set_iodat(0);
    sys_set_input(gParams.dsp.Array_Input.input);
    
    return 0;
}


int sys_set_input(u16 input)
{
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
    dsp_reset();
    sys_config();
    
    return 0;
}


