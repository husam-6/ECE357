#ifndef _SEM_H
#define _SEM_H
#include "spinlock.h"

int my_procnum; 

struct node
{
    int pid; 
    struct node *next; 
};

//Struct for a semaphore 
struct sem
{
    volatile char lock; 
    int count;
    
    //Linked list to keep track of waiting proccesses
    struct node items[N_PROC];

    struct node *start; 
    struct node *end; 
    int length; 
};



//insert struct node
void insert(struct node *n, struct sem *s);

//remove from the linked list
int pop(struct sem *s);

//Function to declare the sempahore with the specified value
void sem_init(struct sem *s, int count);

int sem_try(struct sem *s);

void sem_wait(struct sem *s);

void sem_inc(struct sem *s);

#endif