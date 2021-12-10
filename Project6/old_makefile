dumbBitch.exe: testInt.o spinlock.o tas64.o
	gcc -o dumbBitch.exe testInt.o spinlock.o tas64.o

testInt.o: testInt.c
	gcc -c testInt.c

spinlock.o: spinlock.c spinlock.h
	gcc -c spinlock.c

tas64.o: tas64.S
	gcc -c tas64.S

clean:
	rm -f *.exe *.o *.stackdump *~

backup:
	test -d backups || mkdir backups
	cp *.c backups
	cp *.h backups
	cp Makefile backups