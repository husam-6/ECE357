#include "spinlock.h"
#include "sem.h"


void makeMoves(struct sem *from, struct sem *to, int moves)
{
    for(int i = 0; i<moves; i++)
    {
        sem_wait(from);
        // printf("GETS PAST WAIT\n");
        sem_inc(to);
    }
}


void displayInfo(struct sem *tmp)
{
    for(int i = 0; i<6; i++)
    {
        printf("Process %d went to sleep: %d times\n", i, tmp->sleep[i]);
        printf("Process %d woke up: %d times\n", i, tmp->woke[i]);
    }
    printf("\n\n\n");
}

int main(int argc, char *argv[])
{
    int pebbles, moves; 

    pebbles = atoi(argv[1]);
    moves = atoi(argv[2]);

    // printf("%d\t%d\n", pebbles, moves);

    //Initialize 3 semaphores in shared memory
    struct sem *A = (struct sem*) mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    struct sem *B = (struct sem*) mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    struct sem *C = (struct sem*) mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);


    sem_init(A, pebbles);
    sem_init(B, pebbles);
    sem_init(C, pebbles);


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
                my_procnum = i; 
                switch (i)
                {
                    case 0: 
                        makeMoves(A, B, moves);
                        printf("Finish: %d\n", my_procnum);
                        break;
                    case 1: 
                        makeMoves(A, C, moves);
                        printf("Finish: %d\n", my_procnum);
                        break;
                    case 2:
                        makeMoves(B, A, moves);
                        printf("Finish: %d\n", my_procnum);
                        break;
                    case 3: 
                        makeMoves(B, C, moves);
                        printf("Finish: %d\n", my_procnum);
                        break;
                    case 4: 
                        makeMoves(C, A, moves);
                        printf("Finish: %d\n", my_procnum);
                        break;
                    case 5:
                        makeMoves(C, B, moves);
                        printf("Finish: %d\n", my_procnum);
                        break;
                }
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

    displayInfo(A);
    displayInfo(B);
    displayInfo(C);

}