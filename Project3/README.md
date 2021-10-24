## Operating Systems Project 3 Programming Assignment 

Michael Bentivegna and Husam Almanakly 

This program implements a simplistic shell, able to run 1 command at a time. 
The shell has 3 built in commands: cd, pwd, and exit. Aside from those, the 
shell forks a new process and calls execvp to execute the given unix command. Additionally, 
the shell supports being invoked as a script interpreter. 

After running ./a.out, the shell defaults to standard input awaiting the user to input 
a command to execute. 

Screenshots of 1 custom test case run, in addition to the two given test cases are 
attached. 