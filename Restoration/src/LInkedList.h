#ifndef LINKED_LIST_H
#define LINDED_LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Pixel.h"

struct node {
   int data;
   struct pixel key;
   struct node *next;
};

void printList();
void insertFirst(struct pixel key, int data);
struct node* deleteFirst();
bool isEmpty();
int length();
struct node* find(struct pixel key);
struct node* delete(struct pixel key);
struct node* getHead(void);
//void sort();
//void reverse(struct node** head_ref);
#endif
