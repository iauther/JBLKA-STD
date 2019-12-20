#include "device.h"
#include "sys.h"
#include "task.h"



int main(void)
{
    NVIC_SetVectorTable (NVIC_VectTab_FLASH, APP_OFFSET);
    __enable_irq();
    sys_init();

    task_init();
    task_new(com_task, 1024);
    task_new(dev_task, 1024);
    task_new(gui_task, 1024);
    task_new(eep_task, 512);
    task_start();
    while(1);

    return 0;
}



