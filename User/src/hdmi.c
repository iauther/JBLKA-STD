#include "device.h"



static void hdmi_idle_func(u8 *data, u16 data_len)
{

}

#define HDMI_BUF_LEN  100
u8 hdmiBuf[HDMI_BUF_LEN];
int hdmi_init(void)
{
    uart_paras_t p={hdmi_idle_func, hdmiBuf, HDMI_BUF_LEN};
    usart_init(HDMI_UART, &p);
    
    return 0;
}



int hdmi_reset(u16 ms)      //PA15
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);
    delay_ms(ms);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);

    return 0;
}









