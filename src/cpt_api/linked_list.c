//
// Created by jordan on 2022-03-08.
//

#include "cpt_api/linked_list.h"
#include "common.h"

void print_linked_list(Node * node, void (*consumer)(void *))
{
    while ( node->next )
    {
        (*consumer)(node->data);
        node = node->next;
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


LinkedList * init_linked_list(void * data, size_t data_size)
{
    Node * head;
    LinkedList * linked_list;

    if ( !(linked_list = malloc(sizeof(LinkedList))) )
    {
        printf("Failed to allocate memory of size %lud\n", sizeof(LinkedList));
        return NULL;
    }

    if ( !(head = create_node(data, data_size)) )
    {

        if ( !(linked_list->head = malloc(sizeof(Node **))) ) { return NULL; }
        if ( !(linked_list->tail = malloc(sizeof(Node **))) ) { return NULL; }


        printf("Failed to allocate memory of size %lud\n", sizeof(*head));
        return NULL;
    }

    (*linked_list->head) = head;
    (*linked_list->tail) = head;
    linked_list->current_node = NULL;
    linked_list->node_count++

    return linked_list;
}


void destroy_linked_list(LinkedList * linked_list)
{
    Node * node_iterator;
    Node * next_iterator;

    node_iterator = *(linked_list->head)
    if (linked_list)
    {
        while ( (next_iterator = node_iterator->next) )
        {
            if ( node_iterator )
            {
                free(node_iterator);
                node_iterator = NULL;
            }
            node_iterator = next_iterator;
        }

        free(linked_list);
        linked_list = NULL;
    }
}


void push_node(LinkedList * linked_list, void * data, size_t data_size)
{
    Node * new_node;
    Node * next_node;

    new_node = create_node(data, data_size);
    next_node = *(linked_list->head);
    (*linked_list->head) = new_node;
    (*linked_list->head)->next = next_node;

    if ( linked_list->node_count == 1 )
    {
        (*linked_list->tail) = next_node;   // set the first tail
    }

    linked_list->node_count++;
}


Node * filter_linked_list(Node * node, bool (*predicate)(void *))
{
    Node * node_iterator;
    Node * next_iterator

    node_iterator = node;
    while ( (next_iterator = node_iterator->next) )
    {
        if ( (*predicate)(node_iterator->data) )
        {
            return node_iterator;
        }

        node_iterator = next_iterator;
    }

    return NULL;
}


int delete_node(LinkedList * linked_list, Node * target, void * (*supplier)(Node *))
{
    // TODO Find a way to write this function without using the stupid flag
    bool found;
    Node * previous, * middle, * next;

    previous = (*linked_list->head);
    middle = previous->next;

    if (!middle) { return -1; }

    while ( (next = middle->next) )
    {
        if (supplier)
        {
            if ( (found = supplier(middle) == supplier(target))) ) { break; }
        }
        else
        {
            if ( (found = (middle == target)) ) { break; }
        }
        previous = middle;
        middle = previous->next;
    }

    if ( found )
    {
        previous->next = next;
        destroy_node(middle);
        linked_list->node_count--;
    }

    if ( !next ) { (*linked_list->tail) = previous; }

    return 0;
}

