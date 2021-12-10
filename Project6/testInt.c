#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include "spinlock.h"


int main()
{
    struct shared{
        int incWith;
        int incWithout; 
        volatile char lock; 
    };
    
    int n = 6; 
    int numInc = 1000000;
    
    pid_t childPids[n];
    struct shared *p; 

    p = (struct shared*) mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    
    for (int i = 0; i < n; i++) 
    {   
        int ppid; 
        switch (childPids[i] = fork())
        {
            case -1:
                fprintf(stderr, "Error processing fork: %s", strerror(errno));
                break;

            case 0:
                // printf("Lock: %c\n", p->lock);
                for(int j = 0; j<numInc; j++)
                    p->incWithout++; 
                
                spin_lock(&(p->lock));
                for(int j = 0; j<numInc; j++)
                    p->incWith++; 
                spin_unlock(&(p->lock));

                return 0; 

            case 1: 
                break; 
        } 
    }
    
    int status;
    pid_t pid;

    int tmp = n; 
    while (tmp > 0) 
    {
        pid = wait(&status);
        --tmp;
    }

    printf("Value of global variable (without spinlock): %d\n", p->incWithout);
    printf("Value of global variable (with spinlock): %d\n", p->incWith);

    return 0; 
}