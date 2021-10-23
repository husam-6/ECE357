#include <stdio.h>
#include <stdlib.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>


int checkArrs(char *args[100], char *redir[100], int argIdx, int redirIdx)
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
    int prevError = 0; 

    while(1)
    {
        //Reset args and redir array for every command
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
                // strcat(redir[redirIdx], "\0\0\0\0\0\0");
                redirIdx++; 
            }
            //printf("%s\n", token);
            token = strtok(NULL, " ");
        }
        
        // checkArrs(args, redir, argIdx, redirIdx);

        const char* command = args[0];
        if(command[0] == '#')
        {
            // printf("----This is a comment----\n");
            continue;
        }
        //throw in a function maybe
        else if (strcmp(command, "exit") == 0)
        {
            if ((argIdx>1) | (redirIdx>1))
            {
                printf("exit: invalid syntax");
                continue;
            }
            //Need to exit with return code of last command
            else if (redirIdx == 0)
                exit(prevError);
            else
                exit(atoi(redir[0]));
        }
        else if (strcmp(command, "cd") == 0)
        {
            if (argIdx > 2)
                fprintf(stderr, "cd: too many arguments\n");
            else if(argIdx == 1)
                chdir(getenv("HOME"));
            else
                chdir(args[1]);
            continue; 
        }
        else if (strcmp(command, "pwd") == 0)
        {
            char path[100000];
            if (argIdx != 1)
                fprintf(stderr, "pwd: too many arguments\n");
            else if(getcwd(path, sizeof path) != NULL)
                printf("%s\n", path);
            else
                fprintf(stderr, "Error using command pwd: %s\n", strerror(errno));
            continue; 
        }
        else
        {
            int pid; 
            struct rusage ru; 
            int childPid;
            int status; 
            // printf("%s\n", command); 
            // execvp(command, args);
            struct timeval start, end; 
            gettimeofday(&start, NULL);
            switch (pid = fork())
            {
                case -1:
                    printf("Error processing command: %s", strerror(errno));
                    break;
                case 0:
                    if (redirIdx > 0)
                    {
                        char* i = redir[0];
                        // char tmp[strlen(i)-1];
                        char filename[strlen(i)-2];
                        switch(i[0])
                        {
                            case '>':
                                if (i[1] == '>')
                                {   
                                    int fd = open(i+2, O_WRONLY|O_CREAT|O_APPEND, 0666);
                                    dup2(fd, 1);
                                }
                                else
                                {
                                    // memcpy(filename, &i[1], strlen(i)-1);
                                    // printf("%lu\n", strlen(filename));
                                    int fd = open(i+1, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                                    dup2(fd, 1);
                                    break; 
                                }
                        }
                    }
                    execvp(command, args);
                    break; 
                default: 
                    wait3(&status, 0, &ru);
                    gettimeofday(&end, NULL);
                    fprintf(stderr, "Child process: %d ", pid);
                    if (status==0)
                        fprintf(stderr, "exited normally.\n");
                    else
                    {
                        prevError = WEXITSTATUS(status);
                        // not sure ab signal num
                        fprintf(stderr, "exited with %s\n", strsignal(prevError));
                    }
                    fprintf(stderr, "User time: %ld.%06d seconds, ", ru.ru_utime.tv_sec, ru.ru_utime.tv_usec );
                    fprintf(stderr, "System time: %ld.%06d seconds, ", ru.ru_stime.tv_sec, ru.ru_stime.tv_usec);
                    fprintf(stderr, "Real time: %ld.%06d seconds\n", (end.tv_sec - start.tv_sec), (end.tv_usec - start.tv_usec));
                    
                    break; 
            }
        }
    }

    return 0;
}