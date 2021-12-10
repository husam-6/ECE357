#ifndef _SEM_H
#define _SEM_H

#include "spinlock.h"
#include "spinlock.h"
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>


int my_procnum; 
int handlerCount;

struct node
{
    int pid; 
    int procnum; 
    struct node *next; 
};

//Struct for a semaphore 
struct sem
{
    volatile char lock; 
    int count;
    
    //went to sleep waiting for semaphore
    int sleep[6];

    //woken up for that semaphore
    int woke[6];

    //handler invoked
    int handlerCount[6];

    //Linked list to keep track of waiting proccesses
    struct node items[N_PROC];
    struct node *start; 
    struct node *end; 
    int length; 
};



//insert struct node
void insert(struct node *n, struct sem *s);

//remove from the linked list
struct node* pop(struct sem *s);

//Function to declare the sempahore with the specified value
void sem_init(struct sem *s, int count);

int sem_try(struct sem *s);

void sem_wait(struct sem *s);

void sem_inc(struct sem *s);

void showList(struct sem *s);

#endif