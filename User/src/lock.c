#include "lock.h"

osMutexId_t mutex[LOCK_MAX];

int lock_init(void)
{
    int i;
    for(i=0; i<LOCK_MAX; i++) {
        mutex[i] = osMutexNew(NULL);
    }
    
    return 0;
}


int lock_on(int id)
{
    return osMutexAcquire(mutex[id], 0);
}


int lock_off(int id)
{
    return osMutexRelease(mutex[id]);
}

