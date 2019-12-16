#include "device.h"
#include "sys.h"
#include "task.h"



int main(void)
{
    NVIC_SetVectorTable (NVIC_VectTab_FLASH, APP_OFFSET);
    __enable_irq();
    sys_init();

#ifdef RTX
    //SystemCoreClockUpdate();
    osKernelInitialize();
    
    osThreadNew(com_task, NULL, NULL);
    osThreadNew(dev_task, NULL, NULL);
    osThreadNew(gui_task, NULL, NULL);
    osKernelStart();
#endif

    return 0;
}



