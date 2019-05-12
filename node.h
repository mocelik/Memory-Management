#ifndef NODE_H
#define NODE_H

/* For size_t */
#include <stddef.h>
#include <stdlib.h>

typedef struct memlist
{
  /* doubly-linked list */
  struct memlist *prev;
  struct memlist *next;

  size_t size;         /* How many bytes in this block? */
  char alloc;          /* 1 if this block is allocated, */
                       /* 0 if this block is free. */
  void *ptr;           /* location of block in memory pool. */
}Node;
#endif
