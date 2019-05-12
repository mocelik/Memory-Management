OPT = --std=c89

link: main.o mymem.o llist.o
	gcc $(OPT) -o memory main.o mymem.o llist.o

all:
	gcc $(OPT) -o memory main.c mymem.c llist.c 

llist: llist.c llist.h node.h
	gcc $(OPT) -c llist.c -o llist.o

mymem: mymem.c mymem.h llist.h node.h
	gcc $(OPT) -c mymem.c -o mymem.o

main: main.c
	gcc $(OPT) -c main.c -o main.o

clean:
	rm memory *.o
