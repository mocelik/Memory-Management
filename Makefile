all:
	gcc --std=c89 mymem.c -o memory

clean:
	rm memory *.o
