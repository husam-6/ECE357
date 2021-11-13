## Operating Systems Project 4 Programming Assignment 

Michael Bentivegna and Husam Almanakly 

This program aims to explore and demonstrate key principles regarding 
signals and pipes in a linux system. The file included labelled 
'pipe.c' determines the capacity of a Linux pipe to the nearest 256 bytes. 
This is done by first creating a pipe using the pipe system call, 
and repeatedly writing to the 'write side' of the pipe until an error
has occured in write (ie the pipe is full). 

The file labelled signal.c demonstrates the idea of a 'real time' signal 
(signal numbers >32) vs the traditional signals (signal numbers <=32). 
This is done by spawning child processes to send each specified signal 
a certain number of times (to bombard the process). In the case
of the traditional signal, only 1 instance of the signal is 
processed when the process becomes bombarded with the same signal. 
Whereas, in the real time signal case, each and every signal is processed, 
even in the case of duplicates. The program allows the user to input 
a specified number of children and the number of times the signal should 
be sent for each child. Ie, after compiling "gcc signal.c"

./a.out <number of children> <number of signals> <signal #>


Example:

./a.out 4 5 2

creates 4 child processes that each send signal number 2, 5 times. 
The results are printed to the terminal (number of signals sent vs number 
of signals handled). The sigaction interface is used to set the signal 
handler of each signal. 