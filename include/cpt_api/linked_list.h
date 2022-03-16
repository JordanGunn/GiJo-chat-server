//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_LINKED_LIST_H
#define CPT_LINKED_LIST_H

#include "common.h"

typedef struct
{
    void * data;
    Node * next;
} Node;

typedef struct
{
    int node_count;
    Node * current_node;
    Node ** head;
    Node ** tail;
} LinkedList;



Node * create_node(void * data, size_t data_size);
void destroy_node(Node * node);

LinkedList * init_linked_list(void * data, size_t data_size);
void destroy_linked_list(LinkedList * linked_list);
void print_linked_list(Node * node, void (*consumer)(void *));

void push_node(LinkedList * linked_list, void * data, size_t data_size);
Node * filter_linked_list(Node * node, bool (*predicate)(void *));
int delete_node(LinkedList * linked_list, Node * target, void *(*getter)(Node *));


#endif //CPT_LINKED_LIST_H
