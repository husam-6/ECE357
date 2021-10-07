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
#include <errno.h>


int convert(int mode, char* rVal);
int printStatement(struct stat d_buf, char* path);
int treeWalk(char *directory);

int largestLength = 9;

int printStatement(struct stat d_buf, char* path)
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
    if(flag == 1)
    {
        char tmp[70];
        ssize_t len;
        if((len = readlink(path, tmp, sizeof(tmp)-1)) == -1)
        {
            fprintf(stderr, "Could not read link '%s': %s", path, strerror(errno));
            return -1; 
        }
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

    pwdD = getpwuid(uid);
    grpD = getgrgid(gid);

    off_t size = d_buf.st_size;
    int tmpLen;

    printf("%6llu", inoNum); 
    printf("%5lld", blocks); 
    printf("%11s", modeD); 
    printf("%4d ", nlink); 

    if(pwdD != NULL)
    {
        char* uid_str = pwdD->pw_name;

        tmpLen = strlen(uid_str);
        if(largestLength < tmpLen)
            largestLength = tmpLen;
        printf("%-*s", largestLength, uid_str);
    }
    else
        printf("%-*u", largestLength, uid);

    if (grpD != NULL)
    {
        char* gid_str = grpD->gr_name;

        tmpLen = strlen(gid_str);
        if(largestLength < tmpLen)
            largestLength = tmpLen;
        printf("%-s", gid_str);
    }
    else
        printf("%u", gid);
    
    if(flag == 2)
    {
        unsigned int major = major(d_buf.st_rdev); 
        unsigned int minor = minor(d_buf.st_rdev); 
        printf("%8u,", major);
        printf(" %u", minor);
    }
    else
        printf("%13lld", size);
    printf(" %s  ", time);
    printf("%s\n", path);
    return 0; 
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
        flag = 2;  // 2 indicates device (character or block device)
        break;
    case S_IFBLK:
        type = "b";     //ditto 
        flag = 2; 
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
    if((start_dir = opendir(directory)) == NULL)
    {
        fprintf(stderr, "Could not open directory '%s': %s\n" , directory, strerror(errno));
        return -1;
    }

    struct stat e_buf;
    struct stat d_buf;

    if(!start_dir)
        return 0;

    if(lstat(directory, &d_buf) == -1)
    {
        fprintf(stderr, "Could not get file status: %s\n", strerror(errno));
        return -1;
    }

    //Print Directories
    printStatement(d_buf, directory);

    struct dirent *entry;
    //Read each entry 
    errno = 0; 
    while((entry = readdir(start_dir)) != NULL)
    {
        if ((strcmp(entry->d_name, "..") != 0) && (strcmp(entry->d_name, ".") != 0))
        {
            //ret value shows error
            //Update path name 
            strcpy(path, directory);
            strcat(path, "/");
            strcat(path, entry->d_name);
            if(lstat(path, &e_buf) == -1)
            {
                fprintf(stderr, "Could not get file status: %s\n", strerror(errno));
                return -1;
            }   

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
    if (errno != 0)
    {
        fprintf(stderr, "Could not read path '%s': %s\n", path, strerror(errno));
        return -1;
    }

    if(closedir(start_dir) == -1)
    {
        fprintf(stderr, "Could not close directory stream '%s': %s", path, strerror(errno));
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if(argc == 2)
    {
        int n = strlen(argv[1]);
        treeWalk(argv[1]);
    }
    else if (argc == 1)
    {
        treeWalk(".");
    }
    else 
    {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
    }
    return 0;

}