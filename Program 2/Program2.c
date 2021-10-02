#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h> 
#include <string.h> 
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


char* convert(int mode, char* rVal)
{

    if((mode & S_IFMT) == S_IFDIR)
        strcat(rVal, "d");
    else if ((mode & S_IFMT) == S_IFLNK)
        strcat(rVal, "l");
    else  
        strcat(rVal, "-");

    //Read
    strcat(rVal, mode & S_IRUSR ? "r" : "-");
    strcat(rVal, mode & S_IWUSR ? "w" : "-");
    strcat(rVal, mode & S_IXUSR ? "x" : "-");
    
    //Write
    strcat(rVal, mode & S_IRGRP ? "r" : "-");
    strcat(rVal, mode & S_IWGRP ? "w" : "-");
    strcat(rVal, mode & S_IXGRP ? "x" : "-");

    //Execute
    strcat(rVal, mode & S_IROTH ? "r" : "-");
    strcat(rVal, mode & S_IWOTH ? "w" : "-");
    strcat(rVal, mode & S_IXOTH ? "x" : "-");

    return rVal; 
}

int treeWalk(char *directory)
{

    DIR* start_dir;
    // char contents[4086];
    char path[1024];
    start_dir = opendir(directory);
    struct stat e_buf;
    struct stat d_buf;

    if(!start_dir)
        return 0; 

    lstat(directory, &d_buf);
    //Print out initial directory
    char modeD[10] = "";
    printf("%llu%8lld%11s     %s\n", d_buf.st_ino, d_buf.st_blocks, convert(d_buf.st_mode, modeD), directory);

    struct dirent *entry;
    while((entry = readdir(start_dir)) != NULL)
    {
        
        if ((strcmp(entry->d_name, "..") != 0) && (strcmp(entry->d_name, ".") != 0))
        {
            //ret value shows error
            strcpy(path, directory);
            strcat(path, "/");
            strcat(path, entry->d_name);
            lstat(path, &e_buf); 
            
            //int blocks = buf.st_blocks/1000;
            //printf("%d\t%lld\n", buf.st_blksize, buf.st_blocks);

            //printf("%s\n",strcat(directory,entry->d_name));
            if(entry->d_type == DT_DIR)
            {
                //printf("%s\n", path);
                treeWalk(path);
            }
            else
            {
                char modeE[10] = "";
                printf("%llu%8lld%11s     %s\n", e_buf.st_ino, e_buf.st_blocks, convert(e_buf.st_mode, modeE), path);
            }
        }
    }
    //closedir(start_dir);
    


    return 0;
}

int main(int argc, char *argv[])
{
    if(argc > 1)
    {
        treeWalk(argv[1]);
        //start_dir = opendir(argv[1]);
    }
    else
    {
        treeWalk("./");
        //start_dir = opendir("./");
    }

    return 0; 

}