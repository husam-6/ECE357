#include "spinlock.h"
#include "sem.h"

//Function to make a number of 'moves' (P and V iterations)
//For each task with dedicated roles ie Task 1 A->B
int makeMoves(struct sem *from, struct sem *to, int moves)
{
    for(int i = 0; i<moves; i++)
    {
        sem_wait(from);
        sem_inc(to);
    }
    return 0; 
}

//Function to display the info at the end, after each child terminates
void displayInfo(struct sem *tmp, int num)
{
    printf("%d\t\t%d\n", num, tmp->count);
    for(int i = 0; i<6; i++)
        printf("VCPU: %d\t\t\t\t%d\t\t%d\n", i, tmp->sleep[i], tmp->woke[i]);
    printf("\n");
}

//Function to display the info regarding the child, ie pid and 'task' number
//and number of times handler is invoked
void childInfo(int ret)
{
    printf("Child %d (pid %d) done - Signal handler was invoked %d times\n", my_procnum, getpid(), handlerCount);
    printf("VCPU: %d done\n", my_procnum);
    printf("Child pid %d exited with -> %d\n", getpid(), ret);
} 

int main(int argc, char *argv[])
{
    if(argc <= 1)
    {
        printf("Incorrect Usage: ./shellgame.exe <pebbles> <moves>\n");
        return 1;
    }
    int pebbles, moves; 

    pebbles = atoi(argv[1]);
    moves = atoi(argv[2]);

    // printf("%d\t%d\n", pebbles, moves);

    //Initialize 3 semaphores in shared memory
    struct sem *A = (struct sem*) mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    struct sem *B = (struct sem*) mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    struct sem *C = (struct sem*) mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

    //Initialize each semaphore using sem_init
    sem_init(A, pebbles);
    sem_init(B, pebbles);
    sem_init(C, pebbles);


    //Fork 6 processes to simulate different threads
    int n = 6; 
    pid_t childPids[n];

    for (int i = 0; i < n; i++) 
    {   
        int ppid; 
        switch (childPids[i] = fork())
        {
            case -1:
                fprintf(stderr, "Error processing fork: %s", strerror(errno));
                break;

            case 0:
                printf("VCPU %d starting, pid %d\n", i, getpid());
                my_procnum = i; 
                int ret; 
                switch (i)
                {
                    case 0: 
                        ret = makeMoves(A, B, moves);
                        childInfo(ret);
                        break;
                    case 1: 
                        ret = makeMoves(A, C, moves);
                        childInfo(ret);
                        break;
                    case 2:
                        ret = makeMoves(B, A, moves);
                        childInfo(ret);
                        break;
                    case 3: 
                        ret = makeMoves(B, C, moves);
                        childInfo(ret);
                        break;
                    case 4: 
                        ret = makeMoves(C, A, moves);
                        childInfo(ret);
                        break;
                    case 5:
                        ret = makeMoves(C, B, moves);
                        childInfo(ret);
                        break;
                }
                return 0; 

            case 1: 
                break; 
        } 
    }

    printf("Main process waiting for spawned children...\n");

    int status;
    pid_t pid;

    //Wait for each task to terminate
    int tmp = n; 
    while (tmp > 0) 
    {
        pid = wait(&status);
        --tmp;
    }
    
    //Print final info
    printf("Sem#\t\tval\t\tSleeps\t\tWakes\n");
    displayInfo(A, 0);
    displayInfo(B, 1);
    displayInfo(C, 2);

}