/** Contains definitions to be used throughout this program. */
#ifndef DEFS_H
#define DEFS_H

/* Algorithms */
#define FIRSTFIT 1
#define BESTFIT  2
#define WORSTFIT 3
#define NEXTFIT  4

typedef enum Allocation{
	FREE = 0,
	ALLOCATED = 1
} Allocation;

/* The data structure to keep track of the memory allocations */
typedef struct pageTableEntry
{
  size_t size;         /* How many bytes in this block? */
  Allocation alloc;    /* 1 if this block is allocated, */
                       /* 0 if this block is free. */
  void *ptr;           /* location of block in memory pool. */
} pageTableEntry;

#endif
