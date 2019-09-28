#ifndef MYMEM_H
#define MYMEM_H

#include <stddef.h>
#include "llist.h"
#include "node.h"
#include "defs.h"


/** External Interface Functions */
void  initAlgorithms(int algorithm, size_t sz);
void  terminateAlgorithms(void);
void *mymalloc(size_t requested);
void  myfree(void* block);

/** External Statistic or Diagnostic Functions */
int   mem_holes(void);
int   mem_allocated(void);
int   mem_free(void);
int   mem_total(void);
int   mem_largest_free(void);
int   mem_small_free(int size);
char  mem_is_alloc(void *ptr);
void* mem_pool(void);
void  print_memory(void);
void  print_memory_status(void);




/** Internal Trivial Functions*/
size_t getSize(pageTableEntry*);
char   isAlloc(pageTableEntry*);
void*  getPtr(pageTableEntry*);

char   setAlloc(pageTableEntry*, char);
char   setSize(pageTableEntry*, size_t);
char   setPtr(pageTableEntry*, void *);

char containsAddress(void *, void *);

pageTableEntry *newEntry(void *, size_t , char );
void removeEntry(void *);

#endif
