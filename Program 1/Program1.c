#include<stdio.h>
#include<stdlib.h> 
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

//Husam Almanakly and Michael Bentivegna 
//OS Problem Set 1 - Problem 3

int main(int argc, char *argv[])
{
    int opt;
    int check = 0;
    int d = 1;

    //Loop through command line arguments to see if an output file is provided
    while ((opt = getopt(argc, argv, "o:")) != -1) 
    {
        switch (opt)
        {
            case 'o':
                //Open output file and check for error 
                if((d = open(optarg, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0)
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

    //Loops through input files
    while (optind < argc)
    { 
        fd = 0;     //0 for stdin


        //Open requested file name, read info into buf, write to output file
        if(strcmp(argv[optind],"-") != 0)
            {
            if ((fd = open(argv[optind], O_RDONLY, 0666)) < 0)
            {
                fprintf(stderr, "Input file %s cannot be opened for reading. Error message: %s\n.", argv[optind], strerror(errno));
                return -1;
            }   
        }    
        
        //Counter declarations
        int total = 0;
        int rwCalls = 0; 
        int binary = 0; 
        int written = 0; 


        //Loop read and write procedures encase file is above 4096 bytes
        while((tmp = read(fd, buf, sizeof(buf))) != 0)
        {

            //Binary check
            for(int i = 0; i<tmp; i++)
            {
                if((buf[i]>=127) | (buf[i]>0 & buf[i]<9) | (buf[i]>13 & buf[i]<32))
                {
                    binary = 1; 
                    break;
                }
            }

            //Read error check
            if (tmp < 0)
            {
                fprintf(stderr, "Input file %s cannot be read.  Error message: %s\n", argv[optind], strerror(errno));
                return -1;
            }


            if ((written = write(d, buf, tmp)) < 0)
            {
                fprintf(stderr, "Output file %s cannot be written to.  Error message: %s\n", argv[optind], strerror(errno));
                return -1;
            }

            //partial write check 
            //retry the write starting at buf+number of bytes already correctly written
            //(Only if not all the read in bytes were writte...)
            if(written < tmp)
            {
                write(d, buf+written, tmp);
            }
            
            rwCalls+=2;
            total+=tmp; 
        }

        if (strcmp(argv[optind],"-") == 0)
            argv[optind] = "<standard input>";

        if(binary == 1)
        {
            fprintf(stderr, "Warning: Inputted file %s is a binary file.\n", argv[optind]);
        }
        
        fprintf(stderr, "%d bytes transferred from '%s', with %d read and write system calls made.\n", total, argv[optind], rwCalls);


        if (close(fd) < 0)
        {
            fprintf(stderr, "Input file %s cannot be closed.  Error message: %s.\n", argv[optind], strerror(errno));
            return -1;
        }

            optind++; 
    }

    if(close(d) < 0)
    {
            fprintf(stderr, "Output file %s cannot be closed.  Error message: %s.\n", optarg, strerror(errno));
            return -1;
    }

    //partial writes, binary files check still required
    return 0;
}
