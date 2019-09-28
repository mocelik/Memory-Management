#ifndef NODE_H
#define NODE_H

/* For size_t */
#include <stddef.h>
#include <stdlib.h>

#include "defs.h"

typedef struct memlist
{
  /* doubly-linked list */
  struct memlist *prev;
  struct memlist *next;

  void *data;
}Node;
#endif
