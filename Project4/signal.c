#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int handlerCount;

//Function for child processes to send each signal 
//the specified number of times 
int sendSig(int ppid, int sends, int sigNum)
{
    for(int i = 0; i<sends; i++)
    {
        kill(ppid, sigNum);
    }
    return 0; 
}

//Simplistic handler to demonstrate the number of 
//times the signal is handled
void handler(int sig)
{
    ++handlerCount;
}

int main(int argc, char *argv[])
{
    // set up array to store all the child pid's  
    pid_t childPids[100000];
    int n, sends, sigNum; 

    if(argv[1] && argv[2] && argv[3])
    {
        n = atoi(argv[1]);
        sends = atoi(argv[2]);
        sigNum = atoi(argv[3]); 
    }
    // default to 5 child, 5 send, SIGINT
    else
    {
        n = 5; 
        sends = 5;
        sigNum = SIGINT;
    }

    //Set up signal handlers for SIGINT and real time signal 35
    struct sigaction saINT;
    saINT.sa_handler=handler;
    saINT.sa_flags=SA_NODEFER;
    sigemptyset(&saINT.sa_mask);
    
    if(sigaction(sigNum,&saINT,NULL) < 0)
    {
        fprintf(stderr, "Error processing sigaction for conventional signal: %s", strerror(errno));
    }

    // Start n children specified by user
    for (int i = 0; i < n; i++) 
    {   
        int ppid; 
        switch (childPids[i] = fork())
        {
            case -1:
                fprintf(stderr, "Error processing fork: %s", strerror(errno));
                break;
            case 0:
                ppid = getppid();
                sendSig(ppid, sends, sigNum);
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
        --tmp;
    }

    // Print results: number of times the signal was sent and 
    // how many times it was handled
    printf("Signals delivered: %d, signals handled: %d\n", n*sends, handlerCount);

    return 0; 
}
