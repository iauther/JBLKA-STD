#include "device.h"
#include "sys.h"
#include "task.h"


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
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        FLASH_SetLatency(FLASH_Latency_2);
        RCC_HCLKConfig(RCC_SYSCLK_Div1); 
        RCC_PCLK2Config(RCC_HCLK_Div1); 
        RCC_PCLK1Config(RCC_HCLK_Div2);
        
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6); //PLLCLK = 8MHz*9 = 72 MHz
        RCC_PLLCmd(ENABLE);

        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while(RCC_GetSYSCLKSource() != 0x08){}
    }
    else {
        while(1){}
    }

    /* enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}
static void vec_init(void)
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, APP_OFFSET);
    __enable_irq();
}
static void hw_init(void)
{
    vec_init();
    rcc_init();
}


int main(void)
{
    hw_init();
    task_init();

    sys_init();
    task_new(com_task, 1024);
    task_new(dev_task, 1024);
    task_new(gui_task, 1024);
    task_new(eep_task, 256);
    task_start();
    while(1);

    return 0;
}



