#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>



int sendSig(ppid, sends)
{
    for(int i = 0; i<sends; i++)
    {
        kill(ppid, SIGINT);
        kill(ppid, SIGINT);
    }
}


int main(int argc, int *argv[])
{
    pid_t childPids[10];
    int n = argv[1];
    int sends = argv[2];
    
    // Start n children specified by user
    for (int i = 0; i < n; i++) 
    {
        switch (childPids[i] = fork())
        {
            case -1:
                fprintf(stderr, "Error processing command: %s", strerror(errno));
                break;
            case 0:
                int ppid = getppid();
                sendSig(ppid, sends);
                break; 
            case 1: 
                break; 
        } 
    }

    //Wait for parents to exit
    int status;
    pid_t pid;
    while (n > 0) 
    {
        pid = wait(&status);
        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
        --n;  // TODO(pts): Remove pid from the pids array.
    }

    return 0; 
}