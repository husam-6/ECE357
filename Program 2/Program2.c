#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h> 
#include <time.h>
#include <string.h> 
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

char* convert(int mode, char* rVal);

void printStatement(struct stat d_buf, char* path)
{
    char modeD[15] = "";

    struct group *grpD;
    struct passwd *pwdD;
    grpD = getgrgid(d_buf.st_gid);
    pwdD = getpwuid(d_buf.st_uid);

    char* time = strtok(ctime(&d_buf.st_mtime), "\n");


    printf("%llu%9lld%11s%5d %s%17s%12lld %s    %s\n", d_buf.st_ino, d_buf.st_blocks, convert(d_buf.st_mode, modeD), d_buf.st_nlink, pwdD->pw_name, grpD->gr_name, d_buf.st_size, time, path);

}

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
    //Directories
    // char modeD[10] = "";

    // struct group *grpD;
    // struct passwd *pwdD;
    // grpD = getgrgid(d_buf.st_gid);
    // pwdD = getpwuid(d_buf.st_uid);

    // printf("%llu%9lld%11s%5d %s%17s%12lld %s    %s\n", d_buf.st_ino, d_buf.st_blocks, convert(d_buf.st_mode, modeD), d_buf.st_nlink, pwdD->pw_name, grpD->gr_name, d_buf.st_size, ctime(&d_buf.st_mtime), directory);

    printStatement(d_buf, directory);

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
            
            // struct group *grp;
            // struct passwd *pwd;

            // grp = getgrgid(e_buf.st_gid);
            // pwd = getpwuid(e_buf.st_uid);

            // time_t rawtime = e_buf.st_mtime;
            // struct tm *textual = localtime(rawtime);


            //printf("username: %s\n", pwd->pw_name);

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
                printStatement(e_buf, path);
                // printf("%llu%9lld%11s%5d %s%17s%12lld %s    %s\n", e_buf.st_ino, e_buf.st_blocks, convert(e_buf.st_mode, modeE), e_buf.st_nlink, pwd->pw_name, grp->gr_name, e_buf.st_size, ctime(&e_buf.st_mtime), path);
            }
        }
    }
    closedir(start_dir);
    


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