#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int handlerCount;
int handlerCountRT;

//Function for child processes to send each signal 
//the specified number of times 
int sendSig(int ppid, int sends)
{
    for(int i = 0; i<sends; i++)
    {
        kill(ppid, SIGINT);
        kill(ppid, SIGRTMIN+1);
        // printf("testing\n");
    }
    return 0; 
}

//Simplistic handler to demonstrate the number of 
//times the signal is handled
void handler(int sig)
{
    ++handlerCount;
}

//Handler foro the real time signal
void handlerRT(int sig)
{
    ++handlerCountRT;
}

int main(int argc, char *argv[])
{
    //Set up signal handlers for SIGINT and real time signal 35
    struct sigaction saINT;
    saINT.sa_handler=handler;
    saINT.sa_flags=SA_NODEFER;
    sigemptyset(&saINT.sa_mask);
    
    struct sigaction saRT; 
    saRT.sa_handler = handlerRT; 
    saRT.sa_flags = SA_NODEFER; 
    sigemptyset(&saRT.sa_mask);

    (void)sigaction(SIGINT,&saINT,NULL);
    (void)sigaction(SIGRTMIN+1, &saRT, NULL);

    // set up array to store all the child pid's  
    pid_t childPids[100000];
    int n, sends; 
    if(argv[1] && argv[2])
    {
        n = atoi(argv[1]);
        sends = atoi(argv[2]);
    }
    // default to 1 child and 1 send
    else
    {
        n = 1; 
        sends = 1;
    }

    //printf("%d          %d\n", n, sends);

    // Start n children specified by user
    for (int i = 0; i < n; i++) 
    {   
        int ppid; 
        switch (childPids[i] = fork())
        {
            case -1:
                fprintf(stderr, "Error processing command: %s", strerror(errno));
                break;
            case 0:
                // printf("CHILD PROCESS\n");
                ppid = getppid();
                sendSig(ppid, sends);
                return 0; 
            case 1: 
                break; 
        } 
    }

    //Wait for each child to exit
    int status;
    pid_t pid;

    int tmp = n; 
    while (tmp > 0) 
    {
        pid = wait(&status);
        // printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
        --tmp;  // TODO(pts): Remove pid from the pids array.
    }

    // Print results: number of times the signal was sent and 
    // how many times it was handled
    printf("Signals delivered: %d, signals handled: %d\n", n*sends, handlerCount);
    printf("Real time signals delivered: %d, signals handled: %d\n", n*sends, handlerCountRT);

    return 0; 
}
