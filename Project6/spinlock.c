#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

#include "spinlock.h"

int spin_lock(volatile char *lock)
{
    while(tas(lock)!=0)
        sched_yield();
        
    return 1; 
}

int spin_unlock(volatile char *lock)
{   
    *lock = 0;
    return 1; 
}
