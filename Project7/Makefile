cat.exe: cat.o
	ld -m elf_i386 cat.o -e _main -o cat.exe

cat.o: cat.S
	as --32 cat.S -o cat.o

clean:
	rm -f *.exe *.o *.stackdump *~

backup:
	test -d backups || mkdir backups
	cp *.c backups
	cp *.h backups
	cp Makefile backups