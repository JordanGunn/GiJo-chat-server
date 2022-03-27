//
// Created by jordan on 2022-03-08.
//

#include "linked_list.h"
#include "common.h"


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

    node->node_size = data_size;
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


LinkedList * init_list_data(void * data, size_t data_size)
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

    if ( !(list->head = (Node **)malloc(sizeof(Node **))) ) { return NULL; }
    if ( !(list->tail = (Node **)malloc(sizeof(Node **))) ) { return NULL; }

    (*list->head) = head;
    (*list->tail) = head;
    list->length++;

    return list;
}


LinkedList * init_list_node(Node * head)
{
    LinkedList * list;

    if ( !(list = malloc(sizeof(LinkedList))) )
    {
        printf("Failed to allocate memory of size %lud\n", sizeof(LinkedList));
        return NULL;
    }

    if ( !(list->head = (Node **)malloc(sizeof(Node **))) ) { return NULL; }
    if ( !(list->tail = (Node **)malloc(sizeof(Node **))) ) { return NULL; }

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


int push_data(LinkedList * list, void * data, size_t data_size)
{
    Node * new_node;
    Node * next_node;

    new_node = create_node(data, data_size);

    if ( new_node )
    {
        next_node = *(list->head);
        (*list->head) = new_node;
        (*list->head)->next = next_node;

        if ( list->length == 1 )
        {
            (*list->tail) = next_node;   // set the first tail
        }

        list->length++;
    }

    return ( new_node ) ? 1 : 0;
}


int push_node(LinkedList * list, Node * node)
{
    Node * next_node;

    next_node = get_head_node(list);

    if ( next_node )
    {
        (*list->head) = node;
        node->next = next_node;

        if ( list->length == 1 )
            { (*list->tail) = node->next; } // set the first tail if needed
        list->length++;
    }

    return ( next_node ) ? 1 : 0;
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
                filtered = init_list_data(node_iterator->data, node_iterator->node_size);
            }
            else /* other-wise push the node */
            {
                push_data(filtered, node_iterator->data, node_iterator->node_size);
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

    if ( filtered ) { filtered->length = (int) found;  }

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

    node_iterator = get_head_node(list);
    while ( node_iterator )
    {
        if ( comparator(node_iterator->data, test_param) )
        {
            return node_iterator;
        }
        node_iterator = node_iterator->next;
    }
    return NULL;
}


int delete_node(LinkedList * list, Comparator comparator, void * test_param)
{
    bool found;
    Node  * node_iter, * next_next_node;

    node_iter = get_head_node(list);
    found = comparator(node_iter->data, test_param);
    if ( found )
    {
        *(list->head) = node_iter->next;
        destroy_node(node_iter);
        node_iter = NULL;
    }
    else
    {
        while ( node_iter->next->next )
        {
            found = comparator(node_iter->next->data, test_param);
            if ( found ) { break; }
            node_iter = node_iter->next;
        }
        if ( found )
        {
            next_next_node = node_iter->next->next;
            destroy_node(node_iter->next); node_iter->next = NULL;
            node_iter->next = next_next_node;
        }
        else
        {
            found = comparator(node_iter->next->data, test_param);
            if ( found )
            {
                *(list->tail) = node_iter;
                destroy_node(node_iter->next);
                node_iter->next = NULL;
            }
        }
    }

    if ( found ) { list->length--; }
    return (found) ? 1 : 0;
}
