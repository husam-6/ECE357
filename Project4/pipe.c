#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    int pipefds[2]; 
    if(pipe(pipefds) == -1)
    {
        fprintf(stderr, "Pipe error: %s\n ", strerror(errno));
        return -1; 
    }
    int writeSize = 256;
    int count = 0;  
    fcntl(pipefds[1], F_SETFL, O_NONBLOCK);
    char* buf[writeSize]; 
    
    while(write(pipefds[1], buf, writeSize) != -1)
        count++;

    if(errno == EAGAIN)
        printf("Length of pipe: %d\n", count*writeSize); 
    else
        fprintf(stderr, "Error while writing to pipe: %s\n", strerror(errno));
        
    return 0; 
}