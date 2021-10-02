#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <time.h>
#include <string.h> 
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

int convert(int mode, char* rVal);

void printStatement(struct stat d_buf, char* path)
{
    char modeD[15] = "";

    struct group *grpD;
    struct passwd *pwdD;
    grpD = getgrgid(d_buf.st_gid);
    pwdD = getpwuid(d_buf.st_uid);

    char* time = strtok(ctime(&d_buf.st_mtime), "\n");

    int flag = convert(d_buf.st_mode, modeD);
    if(flag)
    {
        char tmp[1024];
        ssize_t len;  
        len = readlink(path, tmp, sizeof(tmp)-1);
        tmp[len] = '\0';
        strcat(path, " -> ");
        strcat(path, tmp);
    }

    printf("%llu%9lld%11s%5d %s%17s%20lld %s\t%s\n", d_buf.st_ino, d_buf.st_blocks, modeD, d_buf.st_nlink, pwdD->pw_name, grpD->gr_name, d_buf.st_size, time, path);

}

int convert(int mode, char* rVal)
{
    char* type;
    int flag = 0; 
    //Type
    switch (mode & S_IFMT)
    {
    case S_IFDIR:
        type = "d";
        break;
    case S_IFLNK:
        type = "l";
        flag = 1;
        break;
    case S_IFCHR:
        type = "c";
        break;
    case S_IFBLK:
        type = "b";
        break;
    case S_IFIFO:
        type = "p";
        break;
    case S_IFSOCK:
        type = "s";
        break;
    case S_IFREG:
        type = "-";
        break;
    default:
        type = "?";
        break;
    }

    strcat(rVal, type);

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

    // return rVal;
    return flag;  
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

    //Print Directories
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
        
            if(entry->d_type == DT_DIR)
            {
                treeWalk(path);
            }
            else
            {
                char modeE[10] = "";
                //Print directory entries
                printStatement(e_buf, path);
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