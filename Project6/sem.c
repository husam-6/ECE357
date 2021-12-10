#include "sem.h"
#include "spinlock.h"
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>


void sem_init(struct sem *s, int count)
{
    s->count = count;
    s->start = NULL; 
    s->end = NULL; 
    s->length = 0; 
}

int sem_try(struct sem *s)
{
    spin_lock(&(s->lock));
    if(s->count>=0)
    {
        s->count--; 
        spin_unlock(&(s->lock));
        return 1;
    }
    spin_unlock(&(s->lock));
    return 0; 
}

void sem_wait(struct sem *s)
{ 
    s->items[my_procnum].pid = getpid();
    while(!(sem_try(s)))
    {
        //add to waitlist

        //not sure ab syntax rn
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set,SIGUSR1); 

        sigprocmask(SIG_BLOCK, &set, NULL);
        insert(&(s->items[my_procnum]), s);

        //Block the caller
        //Let the signal sleep, no signals being blocked
        sigset_t mask;
        sigemptyset (&mask);
        sigsuspend(&mask);

        sigprocmask(SIG_UNBLOCK, &set, NULL);
        
    }
}


void sem_inc(struct sem *s)
{
    spin_lock(&(s->lock));
    
    s->count++; 

    if(s->length > 0 && s->count>0)
    {
        for(int i=0; i<s->length; i++)
        {
            int pid = pop(s);
            kill(pid, SIGUSR1);
        }
    }
}

//insert struct node
void insert(struct node *n, struct sem *s)
{
    spin_lock(&(s->lock));
    if(s->length == 0)
    {
        s->start = n; 
        s->end = n; 
    }
    else
    {
        s->end->next = n; 
        s->end = n; 
    }
    
    s->end->next = NULL;
    s->length++; 
    spin_unlock(&(s->lock));
}

//remove from the linked list
// -1 -> empty linked list 
// otherwise returns the removed element
int pop(struct sem *s)
{
    spin_lock(&(s->lock));
    if(s->length == 0)
    {
        spin_unlock(&(s->lock));
        return -1; 
    }
    
    struct node* tmp; 
    tmp = s->start;
    s->start = s->start->next;
    s->length--; 
    spin_unlock(&(s->lock));

    return tmp->pid; 
}
