// March 15th, 2017
// Muhammed Orhan Celik - 100980038
// Sysc 4001 Assignment 4 - v2.1
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
  struct memoryList *last;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;
char debug = 0, debug2 = 0, debug3 = 0;
/* copy paste the line below beside debug printf statements
 if (debug) 
 */ 
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

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
	
	if (head != NULL) {
       struct memoryList* temp;
	   while (head != NULL) //this will free the memoryList
		{
		   temp = head;
		   head = head->next;
		   free(temp);
		}
	}
	//head = NULL;
	//next = NULL;

	myMemory = malloc(sz);

	/* TODO: Initialize memory management structure. */
	head = malloc(sizeof(struct memoryList));
	head->size = sz;
	head->alloc = 0;
	head->ptr = myMemory;
	head->last = NULL;
	head->next = NULL;
	next = myMemory;
  return;
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0 && requested>=1);
	
	switch (myStrategy)
	  {
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
	if (debug) printf("myMalloc couldn't fit it anywhere probably...\n");
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
	return newEntry(bestNode, requested)->ptr;
}
void *nextCase(size_t requested){
	//try finding a spot starting at 'next'
	
	for (struct memoryList *cursor = next; cursor!= NULL; cursor = cursor->next) {
			  if (cursor->size >= requested && cursor->alloc == 0) {
				  return newEntry(cursor, requested)->ptr;
			  }//end if
	}//end for
	if (debug3) printf("We got into the nextCase function.\n");
	//if none, try again starting at 'head'
	for (struct memoryList *cursor = head; cursor!= NULL; cursor = cursor->next) {
			  if (cursor->size >= requested && cursor->alloc == 0) {
				  return newEntry(cursor, requested)->ptr;
			  }//end if
	}//end for
	if (debug3) printf("myMalloc couldn't fit it anywhere probably...\n");
	return NULL;
}

/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
	//first, we must find the memorylist entry that points to that block

	struct memoryList *entry = findEntry(block); 
	//does findEntry finally work?

    //entry should now point to the block starting with ptr

	// there are 4 cases:
	// case 1: the block before is free			after is free
	// case 2: the block before is free, 		after is allocated
	// case 3: the block before is allocated, 	after is free
	// case 4: the block before is allocated, 	after is allocated
	
	//there are 4 more rare cases:
	// case 2.1: block is at beginning of pool, and after is free
	// case 2.2: block is at beginning of pool, and after is allocated
	// case 2.3: block is at end of pool,       and previous is free
	// case 2.4: block is at end of pool,       and previous is allocated
	
	// There is one last case where it takes up all of memory
	// what if it encapsulates the whole memory?
	if (debug)printf("\tmyFree Function: ");
	if ( (block != myMemory) && (block + entry->size != (myMemory+mySize)) ) {
		// ensure that the block isn't at the beginning or the end
		// of the memory pool. if they are... we'll handle it later...
		if (!mem_is_alloc(block - 1) && !mem_is_alloc(block+entry->size)) {
			// case 1: the block before is free			after is free
			// if case 1: merge the PREVIOUS with current and NEXT
			if (debug)printf("The block is between two free blocks.\n");
			//find PREVIOUS and NEXT entry
			struct memoryList *tempPrevious = findEntry(block -1);
			struct memoryList *tempNext = findEntry(block + entry->size);
			tempPrevious->size = tempPrevious->size + entry->size + tempNext->size;
			removeEntry(tempNext);
			removeEntry(entry);
			
		} else if (!mem_is_alloc(block - 1) && mem_is_alloc(block+entry->size)) {
			// case 2: the block before is free, 		after is allocated
			// if case 2: merge the PREVIOUS with current
			if (debug)printf("The block is after a free block, and before an allocated one.\n");
			// find the entry pointing to PREVIOUS
			struct memoryList *temp = findEntry(block -1);
			
			// keep PREVIOUS entry, remove current entry
			// but first, update the previous size
			temp->size = temp->size + entry->size;
			// remember that temp->alloc is already 0
			
			//now, remove the CURRENT entry from the memoryList
			removeEntry(entry);
			//done
			
		} else if (mem_is_alloc(block - 1) && !mem_is_alloc(block+entry->size)) {
			// case 3: the block before is allocated, 	after is free
			// if case 3: merge the current with NEXT
			if (debug)printf("The block is after an allocated block, but before a free one.\n");
			// find the entry pointing to the NEXT block
			struct memoryList *temp = findEntry(block + entry->size); //don't need +1
			
			// now need to update size of the entry
			entry->size = entry->size + temp->size;
			// and update alloc
			entry->alloc = 0;
			
			removeEntry(temp);
			// now we're done		
			
		} else if (mem_is_alloc(block - 1) && mem_is_alloc(block+entry->size)) {
				// case 4: the block before is allocated, 	after is allocated
			// if case 4: just change the alloc to 0
			if (debug)printf("The block is between two allocated blocks.\n");
			entry->alloc = 0;
			
		} else {
			if (debug)printf ("I don't really know what's going on\n");
		}
	} else if (block == myMemory && (entry->size == mySize)){ //block takes up all of memory
		entry->alloc = 0;	
		if (debug)printf("The block takes up all of the memory (from beginning to end).\n");
	} else if (block == myMemory && (entry->size != mySize)) {//block is at beginning
		// two possibilities: the next block is either free or allocated
    if (debug)printf("The block is at the beginning and ");
		if (mem_is_alloc(block + entry->size)) {
		  if (debug)printf("the next block is allocated.\n");
			entry->alloc = 0;
		} else {
		  if (debug)printf("the next block is free.\n");
			struct memoryList *temp = findEntry(block + entry->size); //don't need +1
			entry->size = entry->size + temp->size;
			entry->alloc = 0;
			removeEntry(temp);			
		}
	} else if (block != myMemory && (block + entry->size == myMemory+mySize)) {//block is at end
		// two possibilities: the previous block is either free or allocated
		if (debug)printf("The block is at the end and ");
		if (mem_is_alloc(block-1)) {
		  		if (debug)printf("the previous block is allocated.\n");
			entry->alloc = 0;
		} else {
		  if (debug)printf("the previous block is free.\n");
			struct memoryList *temp = findEntry(block -1);
			temp->size = temp->size + entry->size;
			removeEntry(entry);			
		}
	} else {
	  if (debug)printf("I got no clue where this block is.\n");
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
		if ( cursor->size < size && (!cursor->alloc) ){ 
			count++;
		}
	}
	return count;
}       

/*
 * Returns 1 if allocated, 0 otherwise
 */
char mem_is_alloc(void *ptr)
{
	for (struct memoryList *cursor = head; cursor != NULL; cursor=cursor->next) {
		if ( (ptr >= cursor->ptr) && 
				(ptr< (cursor->ptr + (cursor->size))) &&
				(cursor->alloc == 1) ) {
			return 1;
		}
	}
    return 0;
}

 
// returns a pointer to the memoryList entry
// that points to the block containing the ptr
struct memoryList *findEntry(void *ptr) {
	for (struct memoryList *cursor = head; cursor != NULL; cursor= cursor->next){
		if ( (ptr >= cursor->ptr) && 
		    (ptr < ((cursor->ptr) + (cursor->size)) )) {
			return cursor;
		}
	}
	if (debug2) printf("There is no corresponding entry...\n");
	return NULL;	
}

// This removes an entry from the doubley linked memoryList
// 
char removeEntry(struct memoryList *entry){
	char flag = 1;
	if (debug)printf("\t\tRemove Entry Function: ");
	if (entry->next != NULL && entry->last != NULL) { //if its in the middle
		(entry->next)->last = entry->last; 
		(entry->last)->next = entry->next;
		
	} else if (entry->next==NULL && entry->last!=NULL){//if this is last entry
		(entry->last)->next = NULL;	
		if (debug)printf("this was the last node in the list.\n");
		
	} else if (entry->next!=NULL && entry->last==NULL) {//if this is first entry
		head = entry->next;
		
	} else if (entry->next==NULL && entry->last==NULL) {//if this is the only entry
		flag = 0;
		entry->alloc = 0;
		//equivalent to head->alloc = 0;
	}
	
	if (flag) { //DO NOT FREE HEAD!!!
		free(entry);
	}
	return 1;
}

// This creates a new memoryList entry with a given pointer and size
// it will automatically create a second pointer that points to the
// remaining memory at the end of the block (if any)
struct memoryList *newEntry(struct memoryList *entry, size_t sz) {
  if (entry->size == sz) {
    entry->alloc = 1;
    return entry;
  } else {
    // there will always be an entry pointing there
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
  	
  	struct memoryList *cursor = head;
  	while (cursor->next != NULL) {
  	  cursor = cursor->next;
  	} 
  	for (cursor = head; cursor->next != NULL; cursor = cursor->next) {}
  	//nothing inside the for loop. Just need to get to last entry in list
  	remainingBlockEntry->last = cursor;
  	cursor->next = remainingBlockEntry;
  	next = remainingBlockEntry;
  	return entry;
  }
}


/** 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may ind them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
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
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
//currently commented out because code doesn't pass the prof's tests
void print_memory()
{	/**
	size_t totalSize = mySize;//for a size of memory 'totalSize'
	//in blocks/partitions of 'partitionSize'
	size_t partitionSize = 25; //keep it 25 for now...

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
	
	printf("\nThe memory list contains the following data:\n");
	printf("Node\tAlloc\tFrom\t\tTo\t\tSize\n");
	int i=0;
	for (struct memoryList *cursor = head; cursor != NULL; cursor = cursor->next) {
	  printf("%d \t%d \t%lu \t%lu \t%d\n",i++,cursor->alloc,(long unsigned)cursor->ptr,(long unsigned)cursor->ptr+cursor->size-1,cursor->size);  
	}
	for (int i=0;i<75;i++) printf("=");
	printf("\n\n\n");
	*/
	return;
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}
/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
    strategies strat;
    void *a, *b, *c, *d, *e;
	strat = First;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	  
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	/** profs code*/
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
	
	/** my testing code **/
	/*
	strat = First;
	initmem(strat,500);
	printf("initmem has completed. We are testing First.\n");
  printf("There should be no memory allocated right now. Everything should be empty.\n");
  print_memory();
  
	a = mymalloc(100);
  printf("Just allocated a=100 bytes. 0 to 99 should be full.\n");
  print_memory();
  
	b = mymalloc(100);
	printf("Just allocated b=100 bytes. 100 to 199 should be full.\n");
	print_memory(); 
	
	c = mymalloc(100);
	printf("Just allocated c=100 bytes. 200 to 299 should be full.\n");
	print_memory();
	
	myfree(b);
	printf("Just freed b. 100 to 199 should be empty.\n");
	print_memory();
	
	d = mymalloc(50);
	printf("Just allocated d=50 bytes. 100 to 149 should be full.\n");
  print_memory();

	myfree(a);
  printf("Just freed a. 0 to 99 should be free.\n");
  print_memory();
  
	e = mymalloc(25);
	printf("Just allocated e=25 bytes. 0 to 24 should be full.");
	print_memory();
	
	//the printf below is to avoid warnings all the time
	printf("a=%p, b=%p, c=%p, d=%p, e=%p, next=%p \n",a,b,c,d,e,next);
//	print_memory();
	print_memory_status();
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////END OF FIRST////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  strat = Best;
	initmem(strat,500);
	printf("initmem has completed. We are testing Best.\n");
  printf("There should be no memory allocated right now. Everything should be empty.\n");
  print_memory();
  
	a = mymalloc(100);
  printf("Just allocated a=100 bytes. 0 to 99 should be full.\n");
  print_memory();
  
	b = mymalloc(50);
	printf("Just allocated b=50 bytes. 100 to 149 should be full.\n");
	print_memory(); 
	
	c = mymalloc(75);
	printf("Just allocated c=75 bytes. 150 to 224 should be full.\n");
	print_memory();
	
	myfree(b);
	printf("Just freed b. 100 to 149 should be empty.\n");
	print_memory();
	
	d = mymalloc(50);
	printf("Just allocated d=50 bytes. 100 to 149 should be full.\n");
  print_memory();

	myfree(a);
  printf("Just freed a. 0 to 99 should be free.\n");
  print_memory();
  
	e = mymalloc(25);
	printf("Just allocated e=25 bytes. 0 to 24 should be full.");
	print_memory();
	
	//the printf below is to avoid warnings all the time
	printf("a=%p, b=%p, c=%p, d=%p, e=%p, next=%p \n",a,b,c,d,e,next);
	print_memory_status();
	
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////END OF BEST////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  strat = Worst;
	initmem(strat,500);
	printf("initmem has completed. We are testing Worst.\n");
  printf("There should be no memory allocated right now. Everything should be empty.\n");
  print_memory();
  
	a = mymalloc(100);
  printf("Just allocated a=100 bytes. 0 to 99 should be full.\n");
  print_memory();
  
	b = mymalloc(75);
	printf("Just allocated b=75 bytes. 100 to 174 should be full.\n");
	print_memory(); 
	
	c = mymalloc(125);
	printf("Just allocated c=125 bytes. 175 to 300 should be full.\n");
	print_memory();
	
	myfree(b);
	printf("Just freed b. 100 to 174 should be empty.\n");
	print_memory();
	
	d = mymalloc(50);
	printf("Just allocated d=50 bytes. 300 to 349 should be full.\n");
  print_memory();

	myfree(a);
  printf("Just freed a. 0 to 99 should be free.\n");
  print_memory();
  
	e = mymalloc(25);
	printf("Just allocated e=25 bytes. 0 to 24 should be full.");
	print_memory();
	
	//the printf below is to avoid warnings all the time
	printf("a=%p, b=%p, c=%p, d=%p, e=%p, next=%p \n",a,b,c,d,e,next);
	print_memory_status();
	
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////END OF WORST////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  strat = Next;
	initmem(strat,500);
	printf("initmem has completed. We are testing Next.\n");
  printf("There should be no memory allocated right now. Everything should be empty.\n");
  print_memory();
  
	a = mymalloc(100);
  printf("Just allocated a=100 bytes. 0 to 99 should be full.\n");
  print_memory();
  
	b = mymalloc(250);
	printf("Just allocated b=250 bytes. 100 to 349 should be full.\n");
	print_memory(); 
	
	c = mymalloc(125);
	printf("Just allocated c=125 bytes. 350 to 474 should be full.\n");
	print_memory();
	
	myfree(b);
	printf("Just freed b. 100 to 349 should be empty.\n");
	print_memory();
	
	d = mymalloc(25);
	printf("Just allocated d=25 bytes. 475 to 499 should be full.\n");
  print_memory();

	myfree(a);
  printf("Just freed a. 0 to 99 should be free.\n");
  print_memory();
  
	e = mymalloc(25);
	printf("Just allocated e=25 bytes. 0 to 24 should be full.\n");
	print_memory();
	
	myfree(d);
	printf("Just freed d. 475 to 499 should be empty.\n");
	print_memory();
	
	d = mymalloc(175);
	printf("Just allocated d=175 bytes. 25 to 199 should be full.\n");
	print_memory();
	
	myfree(e);
	printf("Just freed e. 0 to 25 should be empty.\n");
	print_memory();
	
	e = mymalloc(50);
	printf("Just allocated e=50 bytes. 200 to 250 should be full.\n");
	print_memory();
	
	//the printf below is to avoid warnings all the time
	printf("a=%p, b=%p, c=%p, d=%p, e=%p, next=%p \n",a,b,c,d,e,next);
	print_memory_status();
	*/
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////END OF NEXT////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
}

