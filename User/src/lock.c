#include "lock.h"
#include "cmsis_os2.h"

osMutexId_t mutex[LOCK_MAX];

int lock_init(void)
{
    int i;

#ifdef RTX
    for(i=0; i<LOCK_MAX; i++) {
        mutex[i] = osMutexNew(NULL);
    }
#endif
   
    return 0;
}


int lock_on(int id)
{
    int r=0;

#ifdef RTX
    r = osMutexAcquire(mutex[id], 0);
#endif

    return r;
}


int lock_off(int id)
{
    int r=0;

#ifdef RTX
    r = osMutexRelease(mutex[id]);
#endif

    return r;
}

