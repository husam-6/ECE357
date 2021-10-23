#include <stdio.h>
#include <stdlib.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>


int checkArrs(const char *args[100], const char *redir[100], int argIdx, int redirIdx)
{
    for(int i =0; i < argIdx; i++)
        printf("%s     ", args[i]);
    printf("\n");
    for(int i = 0; i<redirIdx; i++)
        printf("%s      ", redir[i]);
    printf("\n");
    return 0;
}


int main()
{
    int exitT = 0;    //Need to execute cd, pwd, and exit built in to our shell, other commands get forked and exec'ed
    char *args[100];
    char *redir[100];
    while(1)
    {
        memset(args, '\0', sizeof args); 
        memset(redir, '\0', sizeof args); 


        char inStr[1000];
        fgets(inStr, 1000, stdin);
        if (strcmp(inStr, "\n") == 0)
            continue;
        inStr[strcspn(inStr, "\n")] = 0; 
        
        char *token = strtok(inStr, " ");
        args[0] = token; 
        int argIdx = 1;
        int redirIdx = 0; 
        
        token = strtok(NULL, " ");
        int ioReached = 0; 
        while(token != NULL)
        {
            if (token[0] == '<' || token[0] == '>' || (token[0] == 2 && token[1] == '>'))
                ioReached = 1; 
            if (!ioReached)
            {
                args[argIdx] = token;
                argIdx++; 
            }
            else
            {
                redir[redirIdx] = token; 
                redirIdx++; 
            }
            //printf("%s\n", token);
            token = strtok(NULL, " ");
        }

        // checkArrs(args, redir, argIdx, redirIdx);

        const char* command = args[0];
        if(command[0] == '#')
        {
            printf("----This is a comment----\n");
            continue;
        }
        //throw in a function
        else if (strcmp(command, "exit") == 0)
        {
            if ((argIdx>1) | (redirIdx>1))
            {
                printf("exit: invalid syntax");
                continue;
            }
            //Need to exit with return code of last command
            else if (redirIdx == 0)
                exit(0);
            else
                exit(atoi(redir[0]));
        }
        else if (strcmp(command, "cd") == 0)
            continue; 
        else if (strcmp(command, "pwd") == 0)
            continue; 
        else
        {
            int pid; 
            struct rusage ru; 
            int childPid;
            int status; 
            
            // printf("%s\n", command); 
            // execvp(command, args);
            switch (pid = fork())
            {
                case -1:
                    printf("Error processing command: %s", strerror(errno));
                    break;
                case 0:
                    execvp(command, args);
                    break; 
                default: 
                    wait3(&status, 0, &ru);
                    break; 
            }
        }
    }

    return 0;
}