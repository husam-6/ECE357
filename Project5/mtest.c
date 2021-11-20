#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/uio.h>


//For test1
void handler(int sig)
{
    fprintf(stderr, "Signal Received (%s)\n", strsignal(sig));
    _exit(sig);
}

//Test case 1 - Writing to a read only mmap region
int test1(int fd, char buf[12])
{
    printf("Test case 1: Write to a read only mmapped region\n");

    //Loop through possible signals and set the handler for each of them
    struct sigaction sa;
        sa.sa_handler=handler;
        sa.sa_flags= 0;
        sigemptyset(&sa.sa_mask);

    for(int i = 0; i<32; i++)
        (void)sigaction(i,&sa,NULL);

    // void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);  
    size_t size = 12; 
    char* map = mmap(NULL, size, PROT_READ, 0, fd, 0);

    //Comments to explain goal to user
    printf("File contents at start: %s\n", buf);
    printf("Attempting to write to mapped area with map[1] = 'X'\n");
    
    //Write to the region and check if it succeeded
    if((map[1] = 'X') != 'X')
        return 255;
    
    close(fd);
    return 0; 
}

int test23(int fd, char buf[12], int PERM)
{
    size_t size = 4096; 

    //Map a shred
    char* map = mmap(NULL, size, PROT_READ|PROT_WRITE, PERM, fd, 0);
    printf("File contents at start: %s\n", buf);
    printf("Write to a shared region - map[1] = 'X'\n");
    map[1] = 'X';
    
    char red[12]; 
    lseek(fd, 0, SEEK_SET);
    if ((read(fd, red, sizeof(red))) == -1)
    {
        fprintf(stderr, "Error while reading: %s", strerror(errno));
        return -1; 
    }

    printf("Read in from file after: %s\n", red);

    if(red[1] == 'e')
    {
        printf("File contents did not change.\n");
        return 1; 
    }
    else if(red[1] != 'X')
    {
        printf("WE GOOFED!\n");
        return -1; 
    }
    
    printf("Success - File contents properly changed\n"); 
    return 0; 
}

int test4(int fd, char buf[12])
{
    //Increase size of file to 4100 bytes long
    char *tmp = "c\n"; 
    lseek(fd, 4100, SEEK_SET);
    write(fd, tmp, 1);
    printf("Increase size of file to 4100 bytes long.\n");
    
    size_t size = 8096;
    char* map = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    printf("Map 8096 bytes of shared memory using mmap, map[4101] = 'X'.\n");

    //Using mapped area, write 1 byte to the element right after the file ends
    map[4101] = 'X'; 

    //Lseek to 16 bytes past this point and write using system call 
    lseek(fd, 4117, SEEK_SET);
    write(fd, tmp, 1);
    printf("Write 'c' to the file at 4117 byte offset to see if 'X' becomes visible.\n");
    
    //Lseek back to 4101 point and read bytes to check the contents of the file 
    lseek(fd, 4101, SEEK_SET);
    char red[1]; 
    if ((read(fd, red, sizeof(red))) == -1)
    {
        fprintf(stderr, "Error while reading: %s", strerror(errno));
        return -1; 
    }

    //Check if 'X' is present from the read system call
    if(strcmp(red,"X")!=0)
    {
        printf("X byte at offset 4101 not visible\n");
        return 1; 
    }

    printf("Success - X byte visible at 4101 byte offset\n");
    return 0; 
}


int main(int argc, char *argv[])
{
    char* input; 
    if((input = argv[1]) == NULL)
    {
        fprintf(stderr, "Invalid input type\n");
        return -1; 
    }
    
    int fd; 
    // Create a file with the text 'Hello World' already in it (check for errors in addition)
    if ((fd = open("testfile.txt", O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0)
    {
        fprintf(stderr, "File cannot be opened for writing. Error message: %s.\n", strerror(errno));
        return -1;
    }
    
    //Write initial contents into the file
    // char buf[12] = "Hello World";
    char buf[12] = "Hello World";
    if (write(fd, buf, 11) < 0)
    {
        fprintf(stderr, "File cannot be written to. Error message: %s\n", strerror(errno));
        return -1;
    }
    
    int retVal = 0; 
    switch (*input)
    {
        case '1':
            retVal = test1(fd, buf);
            break;
        case '2':
            printf("Test case 2: Write to a MAP_SHARED region from mmap, see if file contents change\n");
            retVal = test23(fd, buf, MAP_SHARED);
            break;
        case '3':
            printf("Test case 3: Write to a MAP_PRIVATE region from mmap, see if file contents change\n");
            retVal = test23(fd, buf, MAP_PRIVATE);
            break;
        case '4':
            retVal = test4(fd, buf);
            break;
        default: 
            fprintf(stderr, "Invalid input!\n");
            return -1; 
    }
    return retVal; 
}