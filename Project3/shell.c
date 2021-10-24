#include <stdio.h>
#include <stdlib.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>

//Function to display arguments and redirection operators
//Used for testing purposes
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

//Built in exit function of shell
//returns 1 on invalid syntax
int customExit(int argIdx, int prevError, char* args[100])
{
    if (argIdx>2)
    {
        printf("exit: invalid syntax\n");
        return 1;
    }
    //Need to exit with return code of last command
    else if (argIdx == 1)
        exit(prevError);
    else
        exit(atoi(args[1]));
}

//Built in cd function using chdir
int customCD(int argIdx, char* args[100])
{
    if (argIdx > 2)
        fprintf(stderr, "cd: too many arguments\n");
    else if(argIdx == 1)
        chdir(getenv("HOME"));
    else
        chdir(args[1]);
    return 0; 
}

//Built in pwd
int customPWD(int argIdx)
{
    char path[100000];
    if (argIdx != 1)
        fprintf(stderr, "pwd: too many arguments\n");
    else if(getcwd(path, sizeof path) != NULL)
        printf("%s\n", path);
    else
        fprintf(stderr, "Error using command pwd: %s\n", strerror(errno));
    return 0; 
}

//Function to fork and exec a given command
//returns the prev return code (to be saved for exit)
int customExecute(int redirIdx, char* redir[100], char* args[100], const char* command)
{
    int pid; 
    struct rusage ru; 
    int childPid;
    int status; 
    int cpid; 
    int prevError = 0; 
    struct timeval start, end; 
    gettimeofday(&start, NULL);

    //Fork a new child process
    switch (pid = fork())
    {
        case -1:
            printf("Error processing command: %s", strerror(errno));
            break;
        case 0:
            //Parse redirection operators
            for(int i = 0; i<redirIdx; i++)
            {
                char* filename = redir[i];
                int fd; 
                switch(filename[0])
                {
                    //redirect standard out
                    case '>':
                        //check for redirect with append
                        if (filename[1] == '>')
                        {   
                            if((fd = open(filename+2, O_WRONLY|O_CREAT|O_APPEND, 0666))<0)
                            {
                                fprintf(stderr, "Input file %s cannot be opened for reading. Error message: %s\n", filename+2, strerror(errno));
                                _exit(1);
                            }
                            //Dup standard output to the file descriptor
                            dup2(fd, 1); 
                            if(close(fd) < 0)
                            {
                                fprintf(stderr, "Input file %s cannot be closed.  Error message: %s.\n", filename+2, strerror(errno));
                            }
                            break; 
                        }
                        else
                        {
                            if((fd = open(filename+1, O_WRONLY|O_CREAT|O_TRUNC, 0666))<0)
                            {
                                fprintf(stderr, "Input file %s cannot be opened for reading. Error message: %s\n", filename+1, strerror(errno));
                                _exit(1);
                            }
                            //Dup standard output to the file descriptor
                            dup2(fd, 1);
                            if(close(fd) < 0)
                            {
                                fprintf(stderr, "Input file %s cannot be closed.  Error message: %s.\n", filename+1, strerror(errno));
                            }
                            break; 
                        }
                    //check for standard error redirection
                    case '2':
                        if (filename[1] == '>')
                        {
                            if (filename[2] == '>')
                            {
                                if((fd = open(filename+3, O_WRONLY|O_CREAT|O_APPEND, 0666))<0)
                                {
                                    fprintf(stderr, "Input file %s cannot be opened for reading. Error message: %s\n", filename+3, strerror(errno));
                                    _exit(1);
                                }
                                //Dup standard error to the file descriptor
                                dup2(fd, 2);
                                if(close(fd) < 0)
                                {
                                    fprintf(stderr, "Input file %s cannot be closed.  Error message: %s.\n", filename+3, strerror(errno));
                                }
                                break; 
                            }
                            else
                            {
                                if((fd = open(filename+2, O_WRONLY|O_CREAT|O_TRUNC, 0666))<0)
                                {
                                    fprintf(stderr, "Input file %s cannot be opened for reading. Error message: %s\n", filename+2, strerror(errno));
                                    _exit(1);
                                }
                                //Dup standard error to the file descriptor
                                dup2(fd, 2);
                                if(close(fd) < 0)
                                {
                                    fprintf(stderr, "Input file %s cannot be closed.  Error message: %s.\n", filename+2, strerror(errno));
                                }
                                break; 
                            }
                        }
                        else
                            fprintf(stderr, "Invalid use of redirection\n");
                    //Check for standard input redirection
                    case '<':
                        if((fd = open(filename+1, O_RDONLY))<0)
                        {
                            fprintf(stderr, "Input file %s cannot be opened for reading. Error message: %s\n.", filename+1, strerror(errno));
                            _exit(1);
                        }
                        dup2(fd, 0);
                        if(close(fd) < 0)
                            {
                                fprintf(stderr, "Input file %s cannot be closed.  Error message: %s.\n", filename+1, strerror(errno));
                            }
                        break; 
                    default: 
                        fprintf(stderr, "Invalid use of redirection\n");
                        _exit(1); 
                }
            }
            int err; 
            //Execute 
            if((err = execvp(command, args)) == -1)
            {
                fprintf(stderr, "Error executing command %s: %s\n", command, strerror(errno));
                _exit(127);
            }
        default:
            //'claim' child process 
            if((cpid = wait3(&status, 0, &ru)) == -1)
                fprintf(stderr, "Error waiting\n");
            gettimeofday(&end, NULL);       //for timing 
            fprintf(stderr, "Child process: %d ", pid);
            if (status==0)
                fprintf(stderr, "exited normally.\n");
            else
            {
                if (WIFSIGNALED(status))
                    fprintf(stderr, "exited with signal %d\n", WTERMSIG(status));
                else
                {
                    prevError = WEXITSTATUS(status);
                    fprintf(stderr, "exited with return value %d\n", prevError);
                }                             
            }
            fprintf(stderr, "User time: %ld.%06d seconds, ", ru.ru_utime.tv_sec, ru.ru_utime.tv_usec );
            fprintf(stderr, "System time: %ld.%06d seconds, ", ru.ru_stime.tv_sec, ru.ru_stime.tv_usec);
            fprintf(stderr, "Real time: %ld.%06d seconds\n", (end.tv_sec - start.tv_sec), (end.tv_usec - start.tv_usec));

            break; 
    }
    return prevError; 
}

//Function to read in commands of script or standard input
int readCommands(FILE* readFile)
{
    //Built in - cd, pwd, and exit, other commands get are forked and executed 
    int exitT = 0;   
    char *args[100];
    char *redir[100];
    int prevError = 0; 

    while(!feof(readFile))
    {
        //Reset args and redir array for every command
        memset(args, '\0', sizeof args); 
        memset(redir, '\0', sizeof args); 
        
        //Prepare variables to parse input
        char inStr[1000];
        fgets(inStr, 1000, readFile);

        //Skip input if its simply a newline
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
            //Check if the argument is part of I/O redirection
            if (token[0] == '<' || token[0] == '>' || (token[0] == '2' && token[1] == '>'))
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
            token = strtok(NULL, " ");
        }
        
        // checkArrs(args, redir, argIdx, redirIdx);

        //Choose command to conduct 
        const char* command = args[0];
        if(command[0] == '#')
            continue;           //Skip comments
        else if (strcmp(command, "exit") == 0)
        {   
            if(customExit(argIdx, prevError, args))
            continue; 
        }
        else if (strcmp(command, "cd") == 0)
        {
            customCD(argIdx, args);
            continue;
        }
        else if (strcmp(command, "pwd") == 0)
        {
            customPWD(argIdx);
            continue; 
        }
        else
            prevError = customExecute(redirIdx, redir, args, command);
    }
    exit(prevError); 
}

int main(int argc, char *argv[])
{
    //Defaults to stdin
    if(argc == 1)
        readCommands(stdin);
    //For the case of running the shell as an interpreter
    else
    {
        FILE* inpFile; 
        inpFile = fopen(argv[1], "r");
        readCommands(inpFile);
    }
    
    return 0;
}