#include "task.h"


int task_init(void)
{
    int r=0;

#ifdef RTX
    osStatus_t st;
    //SystemCoreClockUpdate();
    st = osKernelInitialize();
    
    r = (st==osOK)?0:-1;
#endif

    return r;
}


handle_t task_new(thread_fn fn, int stack_size)
{
    handle_t id=0;
    
#ifdef RTX
     osThreadAttr_t attr = {
        .attr_bits  = 0U,
        .cb_mem     = NULL, //?
        .cb_size    = 0,
        .stack_mem  = NULL,
        .stack_size = stack_size,
        .priority   = osPriorityNormal,
        .tz_module  = 0,
    };
    
    id = osThreadNew(fn, NULL, (const osThreadAttr_t*)&attr);
#endif

    return id;
}


void task_start(void)
{
#ifdef RTX
    osKernelStart();
#endif
}

