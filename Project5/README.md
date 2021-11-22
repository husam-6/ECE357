# Operating Systems Project 4 Programming Assignment 

Michael Bentivegna and Husam Almanakly 

This program aims to illustrate different aspects of virtual memory using 
the mmap system call. A series of test cases are provided in this program
(specified by the user). 

Each testcase can be ran by executing 

## ./a.out <# test case number>


The first test case illustrates what happens when a region is mapped
with Read only protections, and a write is attempted using the mmapped
region. All signals are set to be handled by a common handler, 
which displays the signal number and signal name if a signal 
is recieved. Ultimately, this results in a SIGSEGV being received
by the process (invalid protections for the given operation)

The second test case tests to see if changes made through a region
mapped (using mmap) with the MAP_SHARED flag would be seen in the file. 
The results show that this in fact is 'successful' - changes are 
seen through the region

The third case is identical to the second, except the flag MAP_PRIVATE
is used. And as expected, changes are not seen in the file. 

The final test case experiments with mapping a file to a region that is 
larger than it. Writes are made beyond the bounds of the file (using 
mmap), and then subsequently using the write system call about 16 bytes
past the first write. Before the second write, no changes are visible. 
However after both, both changes can be seen in the file at the specified
offsets. 


