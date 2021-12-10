#ifndef _SPINLOCK_H
#define _SPINLOCK_H
#define N_PROC 64

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

extern int tas(volatile char *lock);

//Spinlock and unlock functions
int spin_lock(volatile char *lock);
int spin_unlock(volatile char *lock);


#endif