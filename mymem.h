#ifndef MYMEM_H
#define MYMEM_H

#include <stddef.h>
#include "llist.h"
#include "node.h"

/* Algorithms */
#define FIRSTFIT 1
#define BESTFIT  2
#define WORSTFIT 3
#define NEXTFIT  4

void initAlgorithms(int algorithm, size_t sz);
void terminateAlgorithms();
void *mymalloc(size_t requested);
Node *allocate(Node *, size_t);

void *firstCase(size_t);
void *bestCase(size_t);
void *worstCase(size_t);
void *nextCase(size_t);

void myfree(void* block);



/* Book keeping functions */
int mem_holes();
int mem_allocated();
int mem_free();
int mem_total();
int mem_largest_free();
int mem_small_free(int size);
char mem_is_alloc(void *ptr);
void* mem_pool();
void print_memory();
void print_memory_status();

#endif
