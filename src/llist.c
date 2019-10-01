
#include <stdio.h> /* for perror */
#include "llist.h"

static Node *head;



/*
 * This initializes the list to start at address at size sz
 */
void initializeList(void *p){

	head = malloc(sizeof(Node));
	head->data = p;
	
	head->prev = NULL;
	head->next = NULL;
}

void *getFirst(){
	return head;
}

void *getPredecessor(void* x){
	if (x==NULL) return NULL;
	Node *xx = (Node *) x;
	
	return xx->prev; 
}

void *getSuccessor(void* x){
	if (x==NULL) return NULL;
	Node *xx = (Node *) x;
	return xx->next; 
}

/*
 * Honestly I don't think this is ever called, but I want this to be complete-ish
 */ 
Node *addPredecessor(Node* x, void *p){
	if (x==NULL) return NULL;
	Node *new =  malloc(sizeof(Node));
	new->data = p;
	
	/* add to the linked list in sorted order */
	new->next = x;
	new->prev = x->prev;
	x->prev = new;
	return new;
}

Node *addSuccessor(Node* x, void *p){
	if (x==NULL) return NULL;
	Node *new =  malloc(sizeof(Node));
	new->data = p;
	
	/* add to the linked list in sorted order */
	new->next = x->next;
	new->prev = x;
	x->next = new;
}

void *getData(void *p){
	if (p==NULL) return NULL;
	Node *pp = (Node *) p;
	return pp->data;
}

/*
 * Returns the data that returns true to f(ptr, data);
 */ 
void *getNode(void *ptr, char (*f)(void*, void *)){
	if (ptr==NULL) return NULL;
	/* if (ptr<baseAddress || ptr>(baseAddress+mySize)) return NULL; */
	Node *cursor;
	for (cursor = head; cursor != NULL; cursor= cursor->next){
		if ( f(cursor->data, ptr) ) {
			return cursor;
		}
	}
	perror("There is no corresponding entry... Returning null.\n");
	return NULL;	
}


/* This removes a node from the doubley linked memoryList */
char removeNode(Node *entry){
	if (entry==NULL) return -1;
	
    /* if node is in between two nodes */
	if (entry->next != NULL && entry->prev != NULL) { 
		(entry->next)->prev = entry->prev; 
		(entry->prev)->next = entry->next;
		free(entry->data);
        free(entry);
	
    /* if node is last in list */
	} else if (entry->next==NULL && entry->prev!=NULL){
		(entry->prev)->next = NULL;	
		free(entry->data);
        free(entry);

    /* if node is first in list */
	} else if (entry->next!=NULL && entry->prev==NULL) {
		head = entry->next;
		free(entry->data);
		free(entry);

    /* if node is the only node in list... */
	} else if (entry->next==NULL && entry->prev==NULL) {
		free(entry->data);
		free(entry); /* equivalent to free(head); */
        
	} else {
        perror("Cannot remove node from list.");
        return -1;
    }

	return 1;
}

/*
 * This clears the list, freeing all the nodes
 */
void removeAllNodes() {
	if (head != NULL) {
		Node *temp;
		while (head != NULL) {
			temp = head;
			head = head->next;
			free(temp->data);
			free(temp);
		}
	}
}
