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
int largestLength = 0;

void printStatement(struct stat d_buf, char* path)
{
    char modeD[15] = "";

    struct group *grpD;
    struct passwd *pwdD;


    char time[70];
    struct tm *rawTime = localtime(&d_buf.st_mtime);
    // strftime(time, sizeof(time), "%b %e %H:%M", rawTime);
    strftime(time, sizeof(time), "%b %e  %Y", rawTime);

    // char* time = strtok(ctime(&d_buf.st_mtime), "\n");

    int flag = convert(d_buf.st_mode, modeD);
    if(flag)
    {
        char tmp[70];
        ssize_t len;
        len = readlink(path, tmp, sizeof(tmp)-1);
        tmp[len] = '\0';
        strcat(path, " -> ");
        strcat(path, tmp);
    }
    ino_t inoNum = d_buf.st_ino;
    blkcnt_t blocks = d_buf.st_blocks/2;
    nlink_t nlink = d_buf.st_nlink;
    uid_t uid = d_buf.st_uid;
    gid_t gid = d_buf.st_gid;

    // int test = d_buf.st_blksize;
    //printf("%d\n", test);

    //char* uid = pwdD->pw_name;
    //char* gid = grpD->gr_name;

    //Need to check if these give null or a string for the name
    pwdD = getpwuid(uid);
    grpD = getgrgid(gid);

    off_t size = d_buf.st_size;
    int tmpLen;
    if((pwdD != NULL) && (grpD != NULL))
    {
        char* uid_str = pwdD->pw_name;
        char* gid_str = grpD->gr_name;

        tmpLen = strlen(uid_str);
        if(largestLength < tmpLen)
            largestLength = tmpLen;

        printf("%6llu%5lld%11s%4d %-*s%9s%13lld %s %s\n", inoNum, blocks, modeD, nlink, largestLength, uid_str, gid_str, size, time, path);
    }
    else
        printf("%6llu%5lld%11s%4d %u%9u%13lld %s %s\n", inoNum, blocks, modeD, nlink, uid, gid, size, time, path);
    //     int uid_num = pwdD->pw_uid;
    //     int gid_num = grpD->gr_gid;
    //     printf("%d%9d%11s%5d %d%17d%20d %s %s\n", inoNum, blocks, modeD, nlink, uid_num, gid_num, size, time, path);
    // }
    //printf("%llu%9lld%11s%5d %s%17s%20lld %s %s\n", d_buf.st_ino, d_buf.st_blocks, modeD, d_buf.st_nlink, pwdD->pw_name, grpD->gr_name, d_buf.st_size, time, path);

}

int convert(int mode, char* rVal)
{
    char* type;
    int flag = 0;
    //Determine type 
    switch (mode & S_IFMT)
    {
    case S_IFDIR:
        type = "d";
        break;
    case S_IFLNK:
        type = "l";
        flag = 1;   //flag to indicate type link 
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

    // Determine read permissions
    strcat(rVal, mode & S_IRUSR ? "r" : "-");
    strcat(rVal, mode & S_IWUSR ? "w" : "-");
    if (mode & S_ISUID)
        strcat(rVal, (mode & S_IXUSR) ? "s" : "S");
    else
        strcat(rVal, mode & S_IXUSR ? "x" : "-");

    //Determine write permissions
    strcat(rVal, mode & S_IRGRP ? "r" : "-");
    strcat(rVal, mode & S_IWGRP ? "w" : "-");
    if (mode & S_ISGID)
        strcat(rVal, (mode & S_IXGRP) ? "s" : "l");
    else
        strcat(rVal, mode & S_IXGRP ? "x" : "-");

    //Determine execute permissions
    strcat(rVal, mode & S_IROTH ? "r" : "-");
    strcat(rVal, mode & S_IWOTH ? "w" : "-");
    if (mode & S_ISVTX)
        strcat(rVal, (mode & S_IXOTH) ? "t" : "T");
    else
        strcat(rVal, mode & S_IXOTH ? "x" : "-");

    // return rVal;
    return flag;
}

//Recursive function to walk through filesystem 
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
    //Read each entry 
    while((entry = readdir(start_dir)) != NULL)
    {
        if ((strcmp(entry->d_name, "..") != 0) && (strcmp(entry->d_name, ".") != 0))
        {
            //ret value shows error
            //Update path name 
            strcpy(path, directory);
            strcat(path, "/");
            strcat(path, entry->d_name);
            lstat(path, &e_buf);

            //Recurse if the given type is a directory 
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
        int n = strlen(argv[1]);
        //Remove trailing '/' if present at the end of path 
        if (argv[1][n - 1] == '/')
            argv[1][n - 1] = '\0';
        treeWalk(argv[1]);
    }
    else
    {
        treeWalk(".");
    }

    return 0;

}