#   Operating Systems Program 1 Assignment

Husam Almanakly and Michael Bentivegna 
OS Problem Set 1 - Problem 3

To build the executable, run 'make' in the same directory as 
the source code file. Then run the program using './kitty' with 
the specified arguments. 

This program concatenates inputted N text files stated at the command line.
ie... 

./kitty [-o output.txt] [file1.txt] ... [fileN.txt]

If an output is given, the file is created and emptied if a file with that name
already exists. 
If no output file is specified, stdout is defaulted.
If a hyphen is inputted as an infile, stdin is used as the input
If no infiles are given, stdin is defaulted