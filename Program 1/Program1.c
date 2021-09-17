#include<stdio.h>
#include<stdlib.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

//Husam Almanakly and Michael Bentivegna 
//OS Problem Set 1 - Problem 3
//sterr, perror, getopt, fprintf

int main(int argc, char *argv[])
{
    int opt;
    int check = 0;
    int d = 1;
    while ((opt = getopt(argc, argv, "o:")) != -1) 
    {
        switch (opt)
        {
            case 'o':
                //create and/or empty output file if output file is specified 
                if((d = open(optarg, O_WRONLY|O_CREAT|O_TRUNC, 0666)) == -1)
                {
                    fprintf(stderr, "Output file %s cannot be opened for writing. Error message: %s.\n", optarg, strerror(errno));
                    return -1;
                }

                check = 1; 
                break;
        }
    }    

    int fd; 
    int tmp;
    char buf[4096];

    //Checks if no infiles were specified
    if((argc == 1) | (check == 1 & argc == 3))
    {
        argv[argc] = "-";
        argc++;
    }

    //printf("ArgC: %d, argv %s \n", argc, argv[2]);
    if (optind < argc) 
    {
        while (optind < argc)
        { 
            fd = 0;     //0 for stdin
            //Open requested file name, read info into buf, write to output file
            if(strcmp(argv[optind],"-") != 0)
            {
                if ((fd = open(argv[optind], O_RDONLY, 0666)) == -1)
                {
                    fprintf(stderr, "Input file %s cannot be opened for reading. Error message: %s\n.", argv[optind], strerror(errno));
                    return -1;
                }    // 0 fd for STDIN
            }    
            
            //Print while reading in from Stdin
            int total = 0;
            while((tmp = read(fd, buf, sizeof(buf))) != 0)
            {
                if (tmp == -1)
                {
                    fprintf(stderr, "Input file %s cannot be read.  Error message: %s\n", argv[optind], strerror(errno));
                    return -1;
                }

                if (write(d, buf, tmp) == -1)
                {
                    fprintf(stderr, "Output file %s cannot be written to.  Error message: %s\n", argv[optind], strerror(errno));
                    return -1;
                }
                total+=tmp; 
            }
            //fprintf(stderr, "Number of bytes transferred: %d\n", total);
            if (close(fd))
            close(fd);
            optind++; 
        }
        close(d);
    }

    //partial writes, binary files check still required
    return 0;
}
