#ifndef LLIST_H
#define LLIST_H

#include <stddef.h>
#include <stdlib.h>

#include "node.h"

void *getNode(void *, char (*)(void *, void *));

void initializeList(void *);
void *getFirst(void);
void *getPredecessor(void*);
void *getSuccessor(void*);
void *getData(void*);

Node *addPredecessor(Node*, void *);
Node *addSuccessor(Node*, void *);

char removeNode(Node *);
void removeAllNodes(void);

#endif
