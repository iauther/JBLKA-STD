#include "hdmi.h"
#include "config.h"
#include "stm32f10x.h"



static void hdmi_idle_func(u8 *data, u16 data_len)
{

}

#define HDMI_BUF_LEN  100
u8 hdmiBuf[HDMI_BUF_LEN];
int amp_init(void)
{
    uart_paras_t p={hdmi_idle_func, hdmiBuf, HDMI_BUF_LEN};
    usart_init(HDMI_UART, &p);
    
    return 0;
}












