//
// Created by jordan on 2022-03-16.
//

#ifndef CPT_LINKED_LIST_TYPES_H
#define CPT_LINKED_LIST_TYPES_H

/* 'object' typedefs */
typedef struct node_struct Node;
typedef struct linked_list LinkedList;
typedef struct filter_query FilterQuery;


/**
 * A generic Node object.
 */
struct node_struct
{
    void * data;
    Node * next;
};


/**
 * A generic linked list object.
 */
struct linked_list
{
    // properties
    int length;
    size_t node_size;
    Node ** head;
    Node ** tail;
};


/**
 * A FilterQuery object.\n\n
 *
 * Can be passed to a Predicate
 * when used in conjunction with
 * filter() to test multiple parameters.
 *
 * @property num_params: Number of elements in <params>
 * @property params:     Pointer to data structure holding
 *                       search parameters.
 */
struct filter_query {
    int num_params;
    void * params;
};


/**
 * A Predicate function pointer.\n\n
 *
 * A generic function pointer that
 * accepts two arguments and returns
 * a boolean value.
 *
 * @param1  Data to check condition on.
 * @param2  Additional 'test' params.
 * @return true or false.
 */
typedef bool (*Predicate)(void *, void *);


/**
 * A Consumer function pointer.\n\n
 *
 * A generic function pointer that
 * accepts one argument and returns
 * a nothing.
 *
 * @param1  Data to to execute function on.
 */
typedef void (*Consumer)(void *);


/**
 * A Supplier function pointer.\n\n
 *
 * A generic function pointer that
 * accepts one argument and returns
 * one result.
 *
 * @param1  Data to to execute function on.
 * @return  A value derived from <param1>
 */
typedef void * (*Supplier)(void *);


#endif //CPT_LINKED_LIST_TYPES_H
