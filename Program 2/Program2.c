#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h> 
#include <string.h> 
#include <sys/types.h>
#include <dirent.h>

int treeWalk(char *directory)
{
    DIR* start_dir;
    char contents[4086];
    char path[1024];
    start_dir = opendir(directory);

    if(!start_dir)
        return 0; 

    struct dirent *entry;
    while((entry = readdir(start_dir)) != NULL)
    {
        
        if ((strcmp(entry->d_name, "..") != 0) && (strcmp((*entry).d_name, ".") != 0))
        {
            


            printf("%s\n", entry->d_name);
            //printf("%s\n",strcat(directory,entry->d_name));
            if(entry->d_type == DT_DIR)
            {
                strcpy(path, directory);
                strcat(path, "/");
                strcat(path, entry->d_name);
                //printf("%s\n", path);
                treeWalk(path);
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
        treeWalk(argv[1]);
        //start_dir = opendir("./");
    }

    return 0; 

}