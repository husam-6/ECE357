.text                           
			                    
    .global _main              	
			                    

_main:

loop: 
	movl 	$4096,%edx
	movl 	$buf,%ecx			# buf - temp string
	movl 	$0,%ebx				# Read from stdin
	movl 	$3,%eax 			# System call read - 3
	int     $0x80               # make system call

	movl $0, %esi

	cmpl %esi, %eax
	jle ex

	movl    %eax,%edx           # length of previous read
	movl    $buf,%ecx           # buffer which contains the string
	movl    $1,%ebx             # write to stdout
	movl    $4,%eax             # System call write - 4
	int     $0x80 				# make system call
	
	jmp loop              			
ex:

	movl    $0,%ebx             # Exit code 0
	movl    $1,%eax             # System call exit - 1
	int     $0x80               # make system call

.data                          

#Temporary buffer for read and write 
buf:
	.ascii    "\n"   # our dear string
