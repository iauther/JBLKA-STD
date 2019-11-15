#include "device.h"
#include "sys.h"


static void adda_reset(void)      //PB5, 低电平复位，时间至少1秒，(开机默认低电平)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_5);
    delay_ms(10);
    GPIO_SetBits(GPIOB, GPIO_Pin_5);
}


static void rcc_init(void)
{
    ErrorStatus HSEStartUpStatus;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    if (HSEStartUpStatus == SUCCESS) {
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

        /* Flash 2 wait state */
        FLASH_SetLatency(FLASH_Latency_2);

        /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1); 

        /* PCLK2 = HCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1); 

        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config(RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);

        /* Enable PLL */ 
        RCC_PLLCmd(ENABLE);

        /* Wait till PLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while(RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else {
        /* If HSE fails to start-up, the application will have wrong clock configuration.
        User can add here some code to deal with this error */    

        /* Go to infinite loop */
        while (1)
        {
        }
    }

    /* enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}



////////////////////////////////////////////////
static int do_config(void)
{
#if 0
    dsp_reset();
#else
    dsp_init();
#endif
    adda_reset();
    delay_ms(2000);
    //hdmi_reset(200);
    sys_mute(0);

    sys_set_iodat(0);
    sys_set_input(gParams.dsp.Array_Input.input);
    
    return 0;
}

int sys_init(void)
{
    //rcc_init();
    gpio_init();
    
    lock_init();
    paras_init();
    adc_init();
    knob_init();
    amp_init();
    hdmi_init();

    fan_init();
    lcd_init();
    ir_init();

    usbd_init();

    do_config();

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


