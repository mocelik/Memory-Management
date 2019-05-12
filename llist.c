
#include <stdio.h> /* for perror */
#include "llist.h"

static Node *head;

size_t getSize(Node* x){
	if (x==NULL) return -1;
	return x->size;
}

char setSize(Node* x, size_t sz){
	if (x==NULL || sz<=0) return -1;
	x->size = sz;
	return 1;
}

char isAlloc(Node* x){
	if (x==NULL) return -1;
	return x->alloc;
}

char setAlloc(Node* x, char a){
	if (x==NULL) return -1;
	x->alloc = a;
	return 1;
}

void  *getPtr(Node* x){
	if (x==NULL) return NULL;
	return x->ptr;
}

char setPtr(Node* x, void *p){
	if (x==NULL || p==NULL) return -1;
	x->ptr = p;
	return 1;
}

/*
 * This initializes the list to start at address at size sz
 */
Node *initializeList(void *address, size_t sz, char allocated){

	head = malloc(sizeof(Node));
	head->size = sz;
	head->alloc = allocated;
	head->ptr  = address;

	head->prev = NULL;
	head->next = NULL;
}

/*
 * This returns head so the caller can iterate themselves
 */
Node *getFirst(){
	return head;
}

Node *getPredecessor(Node* x){
	if (x==NULL) return NULL;
	return x->prev;
}

Node *getSuccessor(Node* x){
	if (x==NULL) return NULL;
	return x->next; 
}

/*
 * Honestly I don't think this is ever called, but I want this to be complete-ish
 */ 
Node *addPredecessor(Node* x, void *ad, size_t sz, char alloc){
	if (x==NULL) return NULL;
	Node *new =  malloc(sizeof(Node));
	new->ptr = ad;
	new->size = sz;
	new->alloc= alloc;
	
	/* add to the linked list in sorted order */
	new->next = x;
	new->prev = x->prev;
	x->prev = new;
	return new;
}

Node *addSuccessor(Node* x, void *ad, size_t sz, char alloc){
	if (x==NULL) return NULL;
	Node *new =  malloc(sizeof(Node));
	new->ptr = ad;
	new->size = sz;
	new->alloc= alloc;
	
	/* add to the linked list in sorted order */
	new->next = x->next;
	new->prev = x;
	x->next = new;
}

/* 
 * Returns the node containing ptr
 */
Node *getNode(void *ptr) {
	if (ptr==NULL) return NULL;
	/* if (ptr<baseAddress || ptr>(baseAddress+mySize)) return NULL; */
	Node *cursor;
	for (cursor = head; cursor != NULL; cursor= cursor->next){
		if ( (ptr >= cursor->ptr) && 
		    (ptr < ((cursor->ptr) + (cursor->size)) )) {
			return cursor;
		}
	}
	perror("There is no corresponding entry...\n");
	return NULL;	
}

/* This removes a node from the doubley linked memoryList */
char removeNode(Node *entry){
	if (entry==NULL) return -1;
	
    /* if node is in between two nodes */
	if (entry->next != NULL && entry->prev != NULL) { 
		(entry->next)->prev = entry->prev; 
		(entry->prev)->next = entry->next;
        free(entry);
	
    /* if node is last in list */
	} else if (entry->next==NULL && entry->prev!=NULL){
		(entry->prev)->next = NULL;	
        free(entry);

    /* if node is first in list */
	} else if (entry->next!=NULL && entry->prev==NULL) {
		head = entry->next;
		free(entry);

    /* if node is the only node in list... */
	} else if (entry->next==NULL && entry->prev==NULL) {
		free(entry); /* equivalent to free(head); (I hope) */
        
	} else {
        perror("Cannot remove node from list.");
        return -1;
    }

	return 1;
}

/*
 * This clears the list, freeing all the nodes
 */
void removeAllNodes(){
	if (head != NULL) {
	   Node* temp;
	   while (head != NULL){
		   temp = head;
		   head = head->next;
		   free(temp);
	   }
	}
}
