#ifndef LLIST_H
#define LLIST_H

#include <stddef.h>
#include <stdlib.h>

#include "node.h"

size_t getSize(Node*);
char   isAlloc(Node*);
void*  getPtr(Node*);

char   setAlloc(Node*, char);
char   setSize(Node*, size_t);
char   setPtr(Node*, void *);

Node *getNode(void *);

Node *initializeList(void *, size_t, char);
Node *getFirst();
Node *getPredecessor(Node*);
Node *getSuccessor(Node*);

Node *addPredecessor(Node*, void *, size_t, char);
Node *addSuccessor(Node*, void *, size_t, char);

char removeNode(Node *);
void removeAllNodes();

#endif
