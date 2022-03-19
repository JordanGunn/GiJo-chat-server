//
// Created by jordan on 2022-03-08.
//

#include "../include/linked_list.h"
#include "../include/common.h"


void for_each(LinkedList * list, Consumer consumer)
{
    Node * node;

    node = get_head_node(list);
    if ( node->next ) /* check if only head node */
    {
        while ( node->next )
        {
            consumer( node->data );
            node = node->next;
        }
    }
    else
    {
        if (node->data) {consumer(node->data);}
    }
}


Node * create_node(void * data, size_t data_size)
{
    Node * node;

    if ( !(node = malloc(sizeof(Node)) ))
    {
        printf("Failed to allocate memory of size %lud\n", sizeof(Node));
        return NULL;
    }

    if ( !(node->data = malloc(data_size)) )
    {
        printf("Failed to allocate memory of size %lud\n", sizeof(data_size));
        return NULL;
    }

    memset(node->data, 0, data_size);
    memcpy(node->data, data, data_size);
    node->next = NULL;

    return node;
}


void destroy_node(Node * node)
{
    if ( node )
    {
        if ( node->data )
        {
            free(node->data);
            node->data = NULL;
        }
        free(node);
        node = NULL;
    }
}


LinkedList * init_list(void * data, size_t data_size)
{
    Node * head;
    LinkedList * list;

    if ( !(list = malloc(sizeof(LinkedList))) )
    {
        printf("Failed to allocate memory of size %lud\n", sizeof(LinkedList));
        return NULL;
    }

    if ( !(head = create_node(data, data_size)) )
    {
        printf("Failed to allocate memory of size %lud\n", sizeof(*head));
        return NULL;
    }

    if ( !(list->head = (Node **)malloc(sizeof(Node *))) ) { return NULL; }
    if ( !(list->tail = (Node **)malloc(sizeof(Node *))) ) { return NULL; }

    list->node_size = data_size;
    (*list->head) = head;
    (*list->tail) = head;
    list->length++;

    return list;
}


void destroy_list(LinkedList * list)
{
    Node * node_iterator;
    Node * next_iterator;

    if ( (node_iterator = *(list->head)) )
    {
        while ((next_iterator = node_iterator->next))
        {
            free(node_iterator);
            node_iterator = NULL;
            node_iterator = next_iterator;
        }

        free(list);
        list = NULL;
    }
}


void push_node(LinkedList * list, void * data, size_t data_size)
{
    Node * new_node;
    Node * next_node;

    new_node = create_node(data, data_size);
    next_node = *(list->head);
    (*list->head) = new_node;
    (*list->head)->next = next_node;

    if ( list->length == 1 )
    {
        (*list->tail) = next_node;   // set the first tail
    }

    list->length++;
}


LinkedList * filter(LinkedList * list, Comparator comparator, void * params, size_t num_params)
{
    size_t passes, found;
    Node * node_iterator;
    LinkedList * filtered;

    filtered = NULL; passes = 0; found = 0;
    node_iterator = get_head_node(list);
    while (passes < num_params)
    {
        if (found == num_params) { break; } /* if we have everything, let's blow this popsicle stand */
        if ( comparator(node_iterator->data, params) ) /* found a match */
        {
            if (!filtered) /* check for first find */
            {
                filtered = init_list(node_iterator->data, list->node_size);
            }
            else /* other-wise push the node */
            {
                push_node(filtered, node_iterator->data, list->node_size);
            }
            found++;
        }

        if ( !(node_iterator->next) ) /* check if we need to reset loop */
        {
            node_iterator = get_head_node(list);
            passes++;
        }
        node_iterator = node_iterator->next;
    }

    return filtered;
}


Node * get_head_node(LinkedList * list)
{
    Node * head_node;

    head_node = ( *(list->head) )
        ? *(list->head)
        : NULL;

    return head_node;
}


Node * find_node(LinkedList * list, Comparator comparator, void * test_param)
{
    Node * node_iterator;
    Node * next_iterator;

    node_iterator = get_head_node(list);
    while ( (next_iterator = node_iterator->next) )
    {
        if ( comparator(node_iterator->data, test_param) )
        {
            return node_iterator;
        }

        node_iterator = next_iterator;
    }

    return NULL;
}


int delete_node(LinkedList * list, Comparator comparator, void * test_param)
{
    bool found;
    Node * previous, * middle, * next;

    previous = (*list->head);
    middle = previous->next;
    if (!middle) { return -1; }

    while ( (next = middle->next) )
    {
        if (( found = (comparator(middle, test_param)) )) { break; }
        previous = middle;
        middle = previous->next;
    }

    if ( found ) /* Check if any were found */
    {
        previous->next = next;
        destroy_node(middle);
        list->length--;
    }
    else { return -1; }

    if ( !next ) { (*list->tail) = previous; } /* point to new tail if needed */

    return 0;
}
