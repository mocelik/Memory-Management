/* 
 * This program was written to demonstrate different memory allocation
 * algorithms, namely: first fit, best fit, worst fit, next fit.
 * It does not use paging or formatting.
 * 
 * Updated December 7th, 2018 - came back to finish what I started
 * Updated May 12th, 2019 - cleaned up and made C89 compatible
 * 							Separated linked list interface
 *  
 * Author: Muhammed Orhan Celik
 */
 
#include <stdio.h>
#include <string.h>
#include "mymem.h"

int main(int argc, char **argv) {
    int algorithm = FIRSTFIT;
    void *a, *b, *c, *d, *e;
    
	if (argc > 1) {
		if (!strcmp(argv[1], "first"))
			algorithm = FIRSTFIT;

		else if (!strcmp(argv[1], "best"))
			algorithm = BESTFIT;

		else if (!strcmp(argv[1], "worst"))
			algorithm = WORSTFIT;

		else if (!strcmp(argv[1], "next"))
			algorithm = NEXTFIT;
	} else {
		algorithm = FIRSTFIT;
	}
    
	/* The following is a demo to show the algorithm differences. */
	initAlgorithms(algorithm,500);
	a = mymalloc(101);
	b = mymalloc(102);
	c = mymalloc(103);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);

	print_memory();
	print_memory_status();

	terminateAlgorithms();
	
    printf("DONE!\n");
    return 0;
}
