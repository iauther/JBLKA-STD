#include "delay.h"
#ifdef RTX
#include "cmsis_os2.h"
#endif

void delay_us(u32 us)
{
	u32 i=0;
	for(i=0;i<us;i++){
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	}
}


void delay_ms(u32 ms)
{
    u32 i;

#if 0//def RTX
    if(osKernelGetState()==osKernelRunning) {
        osDelay(ms);
    }
    else
#endif
    {
        for(i=0; i<ms; i++) {
            delay_us(1000);
        }
    }
}
