
/* stddef and stdlib for size_t */
#include <stddef.h>
#include <stdlib.h>

#include <stdio.h>
#include <assert.h>

#include "mymem.h"

/** GLOBAL VARIABLES */
static int currentAlgorithm = FIRSTFIT;

static size_t memorySize;
static void *baseAddress = NULL;
static Node *next; /* Multiple functions use this so it must be global */

/** Internal Functions */
static pageTableEntry *allocate(void *, size_t);
static void *firstCase(size_t);
static void *bestCase(size_t);
static void *worstCase(size_t);
static void *nextCase(size_t);



/** EXTERNAL FUNCTIONS */
/* 
 * Initializes the memory
 */
void initAlgorithms(int algorithm, size_t sz) {
	terminateAlgorithms(); /* Release previously used memory */
	currentAlgorithm = algorithm;

	/* all implementations will need an actual block of memory to use */
	memorySize = sz;

	baseAddress = malloc(sz); /* Allocate memory */

	/* Initialize memory management structure. */
	pageTableEntry *en = newEntry(baseAddress, sz, 0);
	initializeList(en);
	
	next = baseAddress;
	return;
}

void terminateAlgorithms(){
	if (baseAddress != NULL) 
		free(baseAddress); 
	removeAllNodes();
	return;
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested) {
	assert((int)currentAlgorithm > 0 && requested>=1);
	    
	
	switch (currentAlgorithm) {
		
        case FIRSTFIT:
                return firstCase(requested);
        case BESTFIT:
                return bestCase(requested);
        case WORSTFIT:
                return worstCase(requested);
        case NEXTFIT:
                return nextCase(requested);
        default:
				return NULL;
    }
}

/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* ptr){
	if (ptr==NULL) return;
	if (ptr<baseAddress || ptr>=(baseAddress+memorySize)) return;
	
	/* first, we must find the memorylist entry that points to that block */
	void *referenceNode = getNode(ptr,containsAddress);
	
	pageTableEntry *block = (pageTableEntry *) getData(referenceNode);

	Node *tn = (Node *) referenceNode;
	Node *t = getPredecessor(referenceNode);
	
    pageTableEntry *prevBlock = (pageTableEntry *) getData(getPredecessor(referenceNode)); 
    pageTableEntry *nextBlock = (pageTableEntry *) getData(getSuccessor(referenceNode));
    
    
    size_t blockSize = getSize(block),
			prevSize = getSize(prevBlock),
			nextSize = getSize(nextBlock);
	/* there are 4 common cases:
	 * case 1.1: the block before is free			after is free
	 * case 1.2: the block before is free, 		    after is allocated
	 * case 1.3: the block before is allocated, 	after is free
	 * case 1.4: the block before is allocated, 	after is allocated
	 * 
	 * there are 5 rare cases:
	 * case 2.1: block is at beginning of pool, and after is free
	 * case 2.2: block is at beginning of pool, and after is allocated
	 * case 2.3: block is at end of pool,       and previous is free
	 * case 2.4: block is at end of pool,       and previous is allocated
	 * case 2.5: block spans entire pool, from beginning to end
	 */ 
	
    /* if block isn't at beginning or end of pool */
	if ( (ptr != baseAddress) && (ptr + blockSize != (baseAddress+memorySize)) ) {
        /* we know that there are nodes right before and right after entry */

        
        /* case 1.1: previous is free && next is free */
		if ( (!isAlloc(prevBlock)) && (!isAlloc(nextBlock)) ) {
			/* case 1: the block before is free			after is free
			   if case 1: merge the PREVIOUS with current and NEXT */
			setSize(prevBlock, prevSize + blockSize + nextSize);
			removeNode(getSuccessor(referenceNode));
			removeNode(referenceNode);
			return;
		
        /* case 1.2: previous is free && next is alloc */
		} else if (!isAlloc(prevBlock) && (isAlloc(nextBlock))) {

			/* merge the PREVIOUS with current
			  -> keep PREVIOUS entry, remove current entry
			  -> but first, update the previous size
              -> alloc is already 0 
             */
            setSize(prevBlock, prevSize + blockSize);
			removeNode(referenceNode);
			return;
		
        /* case 1.3:  previous is alloc && next is free */
		} else if (isAlloc(prevBlock) && !isAlloc(nextBlock)) {
			
            /* merge the current with NEXT
			  -> update size of current
              -> update alloc
              -> remove next entry
            */ 
            setSize(block, blockSize + nextSize);
			setAlloc(block, FREE);
			removeNode(getSuccessor(referenceNode));
			return;

        /* case 1.4: previous is alloc && next is alloc */
		} else if (isAlloc(prevBlock) && isAlloc(nextBlock)) {
			
            /* don't need to do anything with other nodes
               just set alloc to 0
             */ 
            setAlloc(block, FREE);
			return;
			
		}
        
    /* case 2.5: block takes up all of memory */
	} else if (ptr == baseAddress && (blockSize == memorySize)){ 
		setAlloc(block, FREE);
		return;

    /* case 2.1 or 2.2: block is at beginning of pool */
	} else if (ptr == baseAddress && (blockSize != memorySize)) {
		
        /* case 2.2: next block is allocated */
		if (isAlloc(nextBlock)) {
			setAlloc(block, FREE);
			return;
        
        /* case 2.1: next block is free */
		} else {
			setSize(block, blockSize + nextSize);
			setAlloc(block, FREE);
			removeNode(getSuccessor(referenceNode));			
			return;
		}
    
    /* case 2.3 or 2.4: block is at end of pool */
	} else if (ptr != baseAddress && (ptr + blockSize == baseAddress+memorySize)) {
		/* two possibilities: the previous block is either free or allocated */

        /* case 2.4: previous block is allocated */
		if (isAlloc(prevBlock)) {
			setAlloc(block, FREE);
			return;
        
        /* case 2.3: previous block is free */
		} else {
			setSize(prevBlock, prevSize + blockSize);
			removeNode(referenceNode);
			return;		
		}
	}
	perror("Trying to remove a block in an unknown location...");


	return;
}
/** Internal Functions */
/* 
 * This allocates all or part of the memory block depending on
 * the size of the block and the size requested
 */ 
static pageTableEntry *allocate(void *node, size_t sz){

	if (node == NULL) return NULL;
	pageTableEntry *block = getData(node);
	/* if the requested size is the size of block, give entire block */
	if (getSize(block) == sz) {
		setAlloc(block, ALLOCATED);
		return block;
	}
	
	/* If they didn't request all of the block, split the remaining
	 and add it to the list */
	int leftoverSize = getSize(block) - sz;
	setSize(block, sz);
	setAlloc(block, ALLOCATED);

	/* now we need to make a new entry to point to the remaining
	   memory in that block */	
	next = addSuccessor(node, 
			newEntry(getPtr(block) + getSize(block), leftoverSize, 0));
							
	return block;
}

static void *firstCase(size_t requestedSize) {
	void *cursor;
	pageTableEntry *block;
	for (cursor = getFirst(); cursor!= NULL; cursor = getSuccessor(cursor)){
		block = getData(cursor);
		if (block->size >= requestedSize && !block->alloc) {
			return allocate(cursor, requestedSize)->ptr;
		}
	}
	perror("Could not allocate that much data!");
	return NULL;
}

static void *bestCase(size_t requestedSize){
	int closestSize = memorySize; /* realistically this is the biggest it can be... */
	void *bestNode = NULL;
	void *cursor;
	size_t blockSize;
	pageTableEntry *block;
	
	for (cursor = getFirst(); cursor != NULL; cursor = getSuccessor(cursor)) {
		block = getData(cursor);
		blockSize = block->size;

		if ((blockSize - requestedSize < closestSize)
				&& (blockSize >= requestedSize)) {
			closestSize = blockSize - requestedSize;
			bestNode = cursor;
			if (closestSize == 0)
				break; /* As close to the size as it can get */
		}
	}
    if (bestNode==NULL) {
		perror("Could not allocate that much data!");
		return NULL;
	}
	return allocate(bestNode, requestedSize)->ptr;
}

static void *worstCase(size_t requestedSize){
	int furthestSize = 0;
	Node *bestNode = NULL;
	Node *cursor;
	size_t blockSize;
	pageTableEntry *block;
	
	for (cursor = getFirst(); cursor!=NULL; cursor = getSuccessor(cursor)) {
		block = getData(cursor);
		blockSize = getSize(block);
		if (!isAlloc(block) && (blockSize - requestedSize > furthestSize) && (blockSize > requestedSize)) {
			furthestSize = blockSize - requestedSize;
			bestNode = cursor;
	    }
	}
    if (bestNode==NULL) {
		perror("Could not allocate that much data!");
		return NULL;
	}
	return getPtr(allocate(bestNode, requestedSize));
}

static void *nextCase(size_t requestedSize){
	/* try finding a spot starting at 'next' */
	Node *cursor;
	size_t blockSize;
	pageTableEntry *block;
	
	for (cursor = next; cursor!= NULL; cursor = getSuccessor(cursor)) {
		block = getData(cursor);
		blockSize = getSize(block);
		if (blockSize >= requestedSize && !isAlloc(block)) {
			return getPtr(allocate(cursor, requestedSize));
		}
	}

	/* if none, wrap around and try again starting from the beginning */
	for (cursor = getFirst(); cursor!= next; cursor = getSuccessor(cursor)) {
		block = getData(cursor);
		blockSize = getSize(block);
		if (blockSize >= requestedSize && !isAlloc(block)) {
			return getPtr(allocate(cursor, requestedSize));
		}
	}

	perror("Could not allocate that much data!");
	return NULL;
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
	Node *cursor;
	for (cursor = getFirst(); cursor!=NULL; cursor=getSuccessor(cursor))
		if (!isAlloc(getData(cursor)))
			holes++;
	return holes;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	int allocated = 0;
	Node *cursor;
	for (cursor = getFirst(); cursor!=NULL; cursor=getSuccessor(cursor))
		if (isAlloc(getData(cursor))) 
			allocated += getSize(getData(cursor));
	return allocated;
}

/* Number of non-allocated bytes */
int mem_free()
{
	int count=0;
	Node *cursor;
	for (cursor = getFirst(); cursor!=NULL; cursor=getSuccessor(cursor)){
		if (!isAlloc(getData(cursor))) {
			count += getSize(getData(cursor));
		}
	}
	return count;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	int largest = 0;
	Node *cursor;
	for (cursor = getFirst(); cursor!=NULL; cursor=getSuccessor(cursor))
		if ( getSize(getData(cursor)) > largest && !isAlloc(getData(cursor))) 
			largest = getSize(getData(cursor));
	return largest;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	/* traverses the memoryList and records the ones with sizes less
	   than size */
	int count = 0;
	Node *cursor;
	for (cursor = getFirst(); cursor!=NULL; cursor=getSuccessor(cursor)){
		if ( getSize(getData(cursor)) <= size && (!isAlloc(getData(cursor))) ){ 
			count++;
		}
	}
	return count;
}       

/*
 * Returns 1 if allocated, 0 otherwise
 */
char mem_is_alloc(void *ptr) {
	if (ptr==NULL) return -1;
	if (ptr<baseAddress || ptr>(baseAddress+memorySize)) return -1;
    return isAlloc(getNode(ptr, containsAddress));
}


/********* Useful "getters" and bookkeeping tools **********/
/* Returns a pointer to the memory pool. */
void *mem_pool() {
	return baseAddress;
}

/* Returns the total number of bytes in the memory pool. */
int mem_total() {
	return memorySize;
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
	void *cursor;
	pageTableEntry *block;
	for (cursor = getFirst(); cursor != NULL; cursor=getSuccessor(cursor)) {
		block = getData(cursor);
      printf("%4d\t%5d\t%8ld\t%12lu\t%12lu\n", i++, block->alloc, block->size,
        (long unsigned) block->ptr,
		(long unsigned) block->ptr + block->size - 1
	  );
	}

}

/* Use this function to print out the current contents of memory. */
void print_memory() {	
	size_t totalSize = memorySize; /* for a size of memory 'totalSize' */
	size_t partitionSize = 25; /* row size (# of columns) */

	printf("\n");
	int k;
	for (k=0;k<75;k++) printf("=");
	printf("\n");
	
	printf("Memory contents are as follows:\n");
	size_t i;
	size_t j;
	for (i=0; i < totalSize; i+=partitionSize) {
		printf("From %lu to %lu\t", i, (i+partitionSize-1));
		for (j=0; j < partitionSize; j++) {
			printf("%c", mem_is_alloc(baseAddress+i+j) == FREE ? '0' : '1');
		}
		printf("\n");
	}
	
    printList();
    for (k=0;k<75;k++) printf("=");
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


/*******************/
size_t getSize(pageTableEntry* x){
	if (x==NULL) return -1;
	return x->size;
}

char setSize(pageTableEntry* x, size_t sz){
	if (x==NULL || sz<=0) return -1;
	x->size = sz;
	return 1;
}

char isAlloc(pageTableEntry* x){
	if (x==NULL) return -1;
	return x->alloc;
}

char setAlloc(pageTableEntry* x, char a){
	if (x==NULL) return -1;
	x->alloc = a;
	return 1;
}

void  *getPtr(pageTableEntry* x){
	if (x==NULL) return NULL;
	return x->ptr;
}

char setPtr(pageTableEntry* x, void *p){
	if (x==NULL || p==NULL) return -1;
	x->ptr = p;
	return 1;
}

char containsAddress(void *e, void *ptr){
	if (e==NULL || ptr==NULL) return -1;
	pageTableEntry *en = (pageTableEntry *) e;
	return ( (ptr >= en->ptr) && 
			 (ptr < en->ptr + en->size)
			 );
}


pageTableEntry *newEntry(void *p, size_t s, char a){
	pageTableEntry *x = malloc(sizeof(pageTableEntry));
	x->size = s;
	x->alloc = a;
	x->ptr = p;
	return x;
}

void removeEntry(void *p){
	if (p != NULL) 
		free(p);
}
