//
// Created by jordan on 2022-03-16.
//

#ifndef CPT_LINKED_LIST_TYPES_H
#define CPT_LINKED_LIST_TYPES_H

// 'object' typedefs
typedef struct node_struct Node;
typedef struct linked_list LinkedList;

struct node_struct
{
    void * data;
    Node * next;
};

struct linked_list
{
    // properties
    int length;
    size_t node_size;
    Node * current_node;
    Node ** head;
    Node ** tail;
};

// declare generic function pointers
typedef bool (*Predicate)(void *, void *);
typedef void (*Consumer)(void *);
typedef void * (*Supplier)(void *);

// typedefs for linked list 'methods'
typedef Node * (*GetNode)(LinkedList * linked_list);
typedef Node * (*FindNode)(LinkedList * linked_list, Predicate predicate);

#endif //CPT_LINKED_LIST_TYPES_H
