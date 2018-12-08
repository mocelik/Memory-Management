// March 15th, 2017 - due date
// updated December 7th, 2018 - came back to finish what I started
// Muhammed Orhan Celik - 100980038
// Sysc 4001 Assignment 4 - v2.2
// NOTE: THIS CODE DOES NOT PASS THE PROFESSORS TEST METHODS!!!
// THE PRINT_MEMORY FUNCTION ON LINE 520 IS COMMENTED OUT
// YOU MAY USE THAT TO CHECK IF IT PASSES TESTS.

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>

/**
 * Custom Function Declarations
 */ 
struct memoryList *findEntry(void *);
char removeEntry(struct memoryList *);
struct memoryList *newEntry(struct memoryList*, size_t);
void *firstCase(size_t);
void *bestCase(size_t);
void *worstCase(size_t);
void *nextCase(size_t);

struct memoryList
{
  // doubly-linked list
  struct memoryList *prev;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy

size_t mySize;
void *myMemory = NULL;
static struct memoryList *head;
static struct memoryList *next;


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz) {
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

	/* Release any other memory being used for bookkeeping when doing a re-initialization! */
	if (head != NULL) {
       struct memoryList* temp;
	   while (head != NULL) //this will free the memoryList
		{
		   temp = head;
		   head = head->next;
		   free(temp);
		}
	}


	myMemory = malloc(sz);

	/* Initialize memory management structure. */
	head = malloc(sizeof(struct memoryList));
	head->size = sz;
	head->alloc = 0;
	head->ptr = myMemory;
	head->prev = NULL;
	head->next = NULL;
	next = myMemory;
  return;
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested) {
	assert((int)myStrategy > 0 && requested>=1);
	
	switch (myStrategy) {
        case NotSet: 
                return NULL;
        case First:
                return firstCase(requested);
        case Best:
                return bestCase(requested);
        case Worst:
                return worstCase(requested);
        case Next:
                return nextCase(requested);
    }
	return NULL;
}

void *firstCase(size_t requested) {
	for (struct memoryList *cursor = head; cursor!= NULL; cursor = cursor->next) {
			  if (cursor->size >= requested && cursor->alloc == 0) {
				  return newEntry(cursor, requested)->ptr;
			  }//end if
	}//end for
	perror("Could not allocate that much data!");
	return NULL;
}

void *bestCase(size_t requested){
	int closest = mySize;//realistically this is the biggest it can be..
	struct memoryList *bestNode = NULL;
	for (struct memoryList *cursor = head; cursor!=NULL; cursor = cursor->next) {
		if (!cursor->alloc && (cursor->size - requested < closest) && (cursor->size >= requested) ) {
			closest = cursor->size - requested; // not sure why, but using closest = cursor->size doesn't work as well....
			bestNode = cursor;
			if (closest == 0) break; //no need to try to find anything smaller
	    }//end if
	}//end for
    if (bestNode==NULL) {
		perror("Could not allocate that much data!");
		return NULL;
	}
	return newEntry(bestNode, requested)->ptr;
}

void *worstCase(size_t requested){
	int furthest = 0;
	struct memoryList *bestNode = NULL;
	for (struct memoryList *cursor = head; cursor!=NULL; cursor = cursor->next) {
		if (!cursor->alloc && (cursor->size - requested > furthest) && (cursor->size > requested)) {
			furthest = cursor->size - requested;
			bestNode = cursor;
	  }//end if
	}//end for
    if (bestNode==NULL) {
		perror("Could not allocate that much data!");
		return NULL;
	}
	return newEntry(bestNode, requested)->ptr;
}

void *nextCase(size_t requested){
	//try finding a spot starting at 'next'
	
	for (struct memoryList *cursor = next; cursor!= NULL; cursor = cursor->next) {
			  if (cursor->size >= requested && cursor->alloc == 0) {
				  return newEntry(cursor, requested)->ptr;
			  }//end if
	}//end for

	//if none, wrap around and try again starting from 'head'
	for (struct memoryList *cursor = head; cursor!= next; cursor = cursor->next) {
			  if (cursor->size >= requested && cursor->alloc == 0) {
				  return newEntry(cursor, requested)->ptr;
			  }//end if
	}//end for

	perror("Could not allocate that much data!");
	return NULL;
}

/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block){
	if (block==NULL) return;
	if (block<myMemory || block>(myMemory+mySize)) return;
	//first, we must find the memorylist entry that points to that block
	struct memoryList *entry = findEntry(block); 
    // for readability
    struct memoryList *tempPrevious= entry->prev; 
    struct memoryList *tempNext = entry->next;

	// there are 4 common cases:
	// case 1.1: the block before is free			after is free
	// case 1.2: the block before is free, 		    after is allocated
	// case 1.3: the block before is allocated, 	after is free
	// case 1.4: the block before is allocated, 	after is allocated
	
	// there are 5 rare cases:
	// case 2.1: block is at beginning of pool, and after is free
	// case 2.2: block is at beginning of pool, and after is allocated
	// case 2.3: block is at end of pool,       and previous is free
	// case 2.4: block is at end of pool,       and previous is allocated
    // case 2.5: block spans entire pool, from beginning to end
	
    // if block isn't at beginning or end of pool
	if ( (block != myMemory) && (block + entry->size != (myMemory+mySize)) ) {
        // we know that there are nodes right before and right after entry

        
        // case 1.1: previous is free && next is free
		if ( (!tempPrevious->alloc) && (!tempNext->alloc) ) {
			// case 1: the block before is free			after is free
			// if case 1: merge the PREVIOUS with current and NEXT
			tempPrevious->size = tempPrevious->size + entry->size + tempNext->size;
			removeEntry(tempNext);
			removeEntry(entry);
		
        // case 1.2: previous is free && next is alloc
		} else if ((!tempPrevious->alloc) && (tempNext->alloc)) {

			// merge the PREVIOUS with current
			//  -> keep PREVIOUS entry, remove current entry
			//  -> but first, update the previous size
            //  -> alloc is already 0
			tempPrevious->size = tempPrevious->size + entry->size;
			removeEntry(entry);
		
        // case 1.3:  previous is alloc && next is free
		} else if ((tempPrevious->alloc) && (!tempNext->alloc)) {
			
            // merge the current with NEXT
			//  -> update size of current
            //  -> update alloc
            //  -> remove next entry
			entry->size = entry->size + tempNext->size;
			entry->alloc = 0;
			removeEntry(tempNext);

		
        // case 1.4: previous is alloc && next is alloc
		} else if ((tempPrevious->alloc) && (tempNext->alloc)) {
			
            // don't need to do anything with other nodes
            // just set alloc to 0
			entry->alloc = 0;
			
		} else {
			perror("Trying to remove a block in an unknown location (2).");
		}
        
    // case 2.5: block takes up all of memory
	} else if (block == myMemory && (entry->size == mySize)){ 
		entry->alloc = 0;	

    // case 2.1 or 2.2: block is at beginning of pool
	} else if (block == myMemory && (entry->size != mySize)) {

        // case 2.2: next block is allocated
		if (tempNext->alloc) {
			entry->alloc = 0;
        
        // case 2.1: next block is free
		} else {
			entry->size = entry->size + tempNext->size;
			entry->alloc = 0;
			removeEntry(tempNext);			
		}
    
    // case 2.3 or 2.4: block is at end of pool
	} else if (block != myMemory && (block + entry->size == myMemory+mySize)) {
		// two possibilities: the previous block is either free or allocated

        // case 2.4: previous block is allocated
		if (tempPrevious->alloc) {
			entry->alloc = 0;
        
        // case 2.3: previous block is free
		} else {
			tempPrevious->size = tempPrevious->size + entry->size;
			removeEntry(entry);		
		}
	} else {
	  perror("Trying to remove a block in an unknown location (1).");
	}

	return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when we refer to "memory" here, we mean the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
	int holes = 0;
	for (struct memoryList *cursor = head; cursor!=NULL; cursor=cursor->next)
		if (!cursor->alloc)  
			holes++;
	return holes;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	int allocated = 0;
	for (struct memoryList *cursor = head; cursor!=NULL; cursor=cursor->next)
		if (cursor->alloc) 
			allocated += cursor->size;
	return allocated;
}

/* Number of non-allocated bytes */
int mem_free()
{
	int count=0;
	for (struct memoryList *cursor = head; cursor!=NULL; cursor=cursor->next){
		if (!cursor->alloc) {
			count+=cursor->size;
		}
	}
	return count;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	int largest = 0;
	for (struct memoryList *cursor = head; cursor!=NULL; cursor=cursor->next)
		if ( cursor->size > largest && !cursor->alloc) 
			largest = cursor->size;
	return largest;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	// traverses the memoryList and records the ones with sizes less
	// than size
	int count = 0;
	for (struct memoryList *cursor = head; cursor!=NULL; cursor=cursor->next){
		if ( cursor->size <= size && (!cursor->alloc) ){ 
			count++;
		}
	}
	return count;
}       

/*
 * Returns 1 if allocated, 0 otherwise
 */
char mem_is_alloc(void *ptr) {
	if (ptr==NULL) return NULL;
	if (ptr<myMemory || ptr>(myMemory+mySize)) return NULL;
    return findEntry(ptr)->alloc;
}

// returns a pointer to the memoryList entry
// that points to the block containing the ptr
struct memoryList *findEntry(void *ptr) {
	if (ptr==NULL) return NULL;
	if (ptr<myMemory || ptr>(myMemory+mySize)) return NULL;
	
	for (struct memoryList *cursor = head; cursor != NULL; cursor= cursor->next){
		if ( (ptr >= cursor->ptr) && 
		    (ptr < ((cursor->ptr) + (cursor->size)) )) {
			return cursor;
		}
	}
	perror("There is no corresponding entry...\n");
	return NULL;	
}

// This removes a node from the doubley linked memoryList
char removeEntry(struct memoryList *entry){
	if (entry==NULL) return -1;
    // if node is in between two nodes
	if (entry->next != NULL && entry->prev != NULL) { 
		(entry->next)->prev = entry->prev; 
		(entry->prev)->next = entry->next;
        free(entry);
	
    // if node is last in list
	} else if (entry->next==NULL && entry->prev!=NULL){
		(entry->prev)->next = NULL;	
        free(entry);

    // if node is first in list
	} else if (entry->next!=NULL && entry->prev==NULL) {
		head = entry->next;
		free(entry);

    // if node is the only node in list
	} else if (entry->next==NULL && entry->prev==NULL) {
		entry->alloc = 0;
        // do not free head node
	} else {
        perror("Cannot remove node from list.");
        return -1;
    }

	return 1;
}

// This creates a new memoryList entry with a given pointer and size
// it will automatically create a second pointer that points to the
// remaining memory at the end of the block (if any)
struct memoryList *newEntry(struct memoryList *entry, size_t sz) {
	if (entry == NULL) return NULL;
  // if the requested size is the size of 'entry', give entire entry
  if (entry->size == sz) {
    entry->alloc = 1;
    return entry;
  }
    // if they didn't request all of entry, split the remaining
    // and add it to the list
  	int previousSize = entry->size;
  	entry->size = sz;
  	entry->alloc = 1;
  	//entry already points there
  	
  	// now we need to make a new entry to point to the remaining
  	// memory in that block
  	struct memoryList *remainingBlockEntry = malloc(sizeof(struct memoryList));
  	remainingBlockEntry->alloc = 0;
  	remainingBlockEntry->size = previousSize - entry->size;
  	remainingBlockEntry->ptr = entry->ptr + entry->size;
  	remainingBlockEntry->next = NULL;

    // add to the linked list in sorted order
    remainingBlockEntry->next = entry->next;
    entry->next = remainingBlockEntry;
    remainingBlockEntry->prev = entry;
    return entry;
    next = remainingBlockEntry;
    return entry;
}


/********* Useful "getters" and bookkeeping tools **********/
//Returns a pointer to the memory pool.
void *mem_pool() {
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total() {
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy) {
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy){
	if (!strcmp(strategy,"best"))
		return Best;
	
	else if (!strcmp(strategy,"worst"))
		return Worst;
	
	else if (!strcmp(strategy,"first"))
		return First;
	
	else if (!strcmp(strategy,"next"))
		return Next;
	
	else
		return 0;
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void printList() {
	printf("\nThe memory list contains the following data:\n");
    printf("%4s\t%5s\t%8s\t%12s\t%12s\n","Node","Alloc","Size","From","To");
	int i=0;
	for (struct memoryList *cursor = head; cursor != NULL; cursor = cursor->next) {
      printf("%4d\t%5d\t%8d\t%12lu\t%12lu\n",i++,cursor->alloc,cursor->size,
      (long unsigned)cursor->ptr,(long unsigned)cursor->ptr+cursor->size-1);
	}

}

/* Use this function to print out the current contents of memory. */
//currently commented out because code doesn't pass the prof's tests
void print_memory() {	
	size_t totalSize = mySize; // for a size of memory 'totalSize'
	size_t partitionSize = 25; // row size (# of columns)

	printf("\n");
	for (int i=0;i<75;i++) printf("=");
	printf("\n");
	
		printf("Memory contents are as follows:\n");
	for (size_t i=0; i < totalSize; i+=partitionSize) {
		printf("From %lu to %lu\t",i, (i+partitionSize-1));
		for (size_t j=0; j < partitionSize; j++) {
			printf("%d", mem_is_alloc(myMemory+i+j));
		}
		printf("\n");
	}
	
    printList();
    for (int i=0;i<75;i++) printf("=");
	    printf("\n\n\n");
	return;
}


/** 
 * Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status() {
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

int main(int argc, char **argv) {
    strategies strat;
    void *a, *b, *c, *d, *e;
	strat = First;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
    
	// Each algorithm should produce a different layout.
	initmem(strat,500);
	a = mymalloc(100);    
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);

	print_memory();
	print_memory_status();

    printf("DONE!\n");
    return 0;
}
