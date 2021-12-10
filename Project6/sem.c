#include "sem.h"

void handler()
{
    handlerCount++; 
}

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
    if(s->count>0)
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
        spin_lock(&(s->lock));
        signal(SIGUSR1, handler);

        //add to waitlist
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set,SIGUSR1); 
        sigprocmask(SIG_BLOCK, &set, NULL);
        
        s->items[my_procnum].procnum = my_procnum;
        insert(&(s->items[my_procnum]), s);
        spin_unlock(&(s->lock));
        
        //Block the caller
        //Let the signal sleep, no signals being blocked
        sigset_t mask;
        sigemptyset (&mask);
        
        s->sleep[my_procnum]++; 
        sigsuspend(&mask);
        s->woke[my_procnum]++;

        sigprocmask(SIG_UNBLOCK, &set, NULL);
        
    }
}

void sem_inc(struct sem *s)
{
    spin_lock(&(s->lock));
    
    s->count++; 
    
    // printf("COUNT: %d\n", s->count);

    if(s->length > 0 && s->count>0)
    {
        for(int i=0; i<s->length; i++)
        {
            struct node *tmp = pop(s);
            kill(tmp->pid, SIGUSR1);
            s->woke[tmp->procnum]++;

        }
    }
    spin_unlock(&(s->lock));
}

//insert struct node
void insert(struct node *n, struct sem *s)
{
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
}

//remove from the linked list
// -1 -> empty linked list 
// otherwise returns the removed element
struct node* pop(struct sem *s)
{
    if(s->length == 0)
    {
        return NULL; 
    }
    
    struct node* tmp; 
    tmp = s->start;
    s->start = s->start->next;
    s->length--; 

    return tmp; 
}


void showList(struct sem *s)
{
    for(int i = 0; i<7; i++)
    {
        printf("PIDS: %d\n", s->items[i].pid);
    }
    printf("\n\n\n\n\n");
}