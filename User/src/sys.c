#include "device.h"
#include "sys.h"




static void adda_reset(void)      //PB5, 低电平复位，时间至少1秒，(开机默认低电平)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_5);
    delay_ms(10);
    GPIO_SetBits(GPIOB, GPIO_Pin_5);
}
///////////////////////////////////////////////

static void show_startup(rect_t r1, rect_t r2)
{
    char *txt="STARTTING...";
    
    lcd_draw_string_align(r1.x, r1.y, r1.w, r1.h, (u8*)txt, FONT_24, LCD_FC, LCD_BC, ALIGN_MIDDLE);
    lcd_draw_rect(r2.x, r2.y, r2.w, r2.h, BLUE);
}
static void show_progress(rect_t rect, u8 cur, u8 max)
{
    u16 total,ox=0;
    static u16 prev_x=0;
    rect_t r={rect.x+1, rect.y+1, rect.w, rect.h-1};
    
    total = r.x+rect.w*cur/max;
    if(prev_x && prev_x<total) {
        ox = total-prev_x;
    }

    r.x = prev_x?prev_x:r.x;
    r.w = r.w/max+ox;
    lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_FC);
    prev_x = r.x+r.w;
    
}
static void show_clear(rect_t r)
{
    lcd_fill_rect(r.x, r.y, r.w, r.h, LCD_BC);
}
static void delay_show(int ms)
{
    int max=100;
    int i,time=ms/max;

    rect_t r1={40, 80,  240,  40};
    rect_t r2={r1.x, r1.y+r1.h, r1.w,  20};
    rect_t r3={r1.x, r1.y,  r1.w+4,  r1.h+r2.h+2};

    show_startup(r1, r2);
    for(i=1; i<=max; i++) {
        delay_ms(time);
        show_progress(r2, i, max);
    }
    show_clear(r3);
}
int audio_init(void)
{
    
#if 0
    dsp_reset();
#else
    dsp_init();
#endif
    adda_reset();
    delay_show(2000);
    //hdmi_reset(200);
    sys_mute(0);

    sys_set_iodat(0);
    sys_set_input(gParams.dsp.Array_Input.input);
    
    return 0;
}

int sys_init(void)
{
    gpio_init();
    
    lock_init();
    paras_init();
    adc_init();
    knob_init();
    amp_init();
    hdmi_init();

    fan_init();
    lcd_init();
    usbd_init();
    ir_init();

    audio_init();

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
    
    power_en(DEV_ALL, 1);
    power_en(DEV_AMP, 1);
    
#if 0
    power_en(DEV_ALL, io->pwr_en);  //????
    if(io->pwr_en) {
        if(adc_read(ADC_CH_AMP_PWR1)>0x55) {
            power_en(DEV_AMP, 1);
        }
        else {
            power_en(DEV_AMP, 0);
        }
    }
#endif
    //sys_mute(0);

    return 0;
}


int sys_mute(u8 on)         //PB9, 高静音，低取消静音
{
    BitAction b;
    GPIO_InitTypeDef init={0};

    init.GPIO_Mode = GPIO_Mode_Out_PP;
    init.GPIO_Pin = GPIO_Pin_9;
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &init);

    b = on?Bit_SET:Bit_RESET;
    GPIO_WriteBit(GPIOB, GPIO_Pin_8, b);
    return 0;
}



int sys_standby(u8 on)
{
    if(on) {
        //mic mute
        //music mute
        //effect mute
        //HAL_Delay(500);
        //power_en(0);
        //lcd_dark();
    }
    else {
        //lcd_on();
        power_en(DEV_ALL, 1);
        power_en(DEV_AMP, 1);      //depend on get_amp_pwr()?
        //mic mute off
        //music mute off
        //effect mute off
    }

    return 0;
}

#if 0
typedef void (*j_func)(void);
j_func j_fn;
u32 j_addr;
static void jump_to(u32 addr)
{
    if(((*(volatile u32*)addr) & 0x2FFE0000) == 0x20000000) {
        
        j_addr = *(volatile u32*)(addr+4);
        j_fn = (j_func)j_addr;
        
        __disable_irq();
        __set_CONTROL(0);   //??psp????msp
        __set_MSP(*(volatile u32*)addr);
        j_fn();
    }
}
static void jump2Boot(void)
{
    jump_to(0);
}
#endif

void sys_reboot(void)
{
    __disable_fiq();
    NVIC_SystemReset();
}


