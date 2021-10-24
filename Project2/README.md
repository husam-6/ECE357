## Operating Systems Project 2 Programming Assignment

Michael Bentivegna and Husam Almanakly 

This program essentially mimics the "find <path> -ls" and recursively walks through 
a file system, displaying all the same information specific to each file/directory
(including Inode Number, disk usage, mode string, uid, gid, file size, modified date, and file path)

For symlinks, the file path includes <path> -> filename

If no path is given, the default working directory is used, same as the find command. 

Usage example: 

gcc Program2.c
./a.out <path>
