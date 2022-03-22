//
// Created by jordan on 2022-03-08.
//

#ifndef CPT_LINKED_LIST_H
#define CPT_LINKED_LIST_H

#include "../include/common.h"
#include "../include/linked_list_types.h"


/**
 * Create a Node.
 *
 * Use a void pointer to a struct to
 * initialize and return a Node.
 *
 * @param data      Void pointer to a struct.
 * @param data_size Size of <data> in bytes.
 * @return The new Node.
 */
Node * create_node(void * data, size_t data_size);


/**
 * Destroy a Node.
 *
 * Destroys the Node, freeing any allocated
 * memory and setting pointers to NULL.
 *
 * @param node
 */
void destroy_node(Node * node);


/**
 * Initialize a LinkedList.
 *
 * Initializes a LinkedList object using
 * <data> as head. Function will allocate
 * necessary memory, initialize data members,
 * and set the HEAD/TAIL pointers for the list.
 *
 * @param data
 * @param data_size
 * @return
 */
LinkedList * init_list_data(void * data, size_t data_size);


/**
 * Initialize a LinkedList.
 *
 * Initializes a LinkedList object using
 * <data> as head. Function will allocate
 * necessary memory, initialize data members,
 * and set the HEAD/TAIL pointers for the list.
 *
 * @param data
 * @param data_size
 * @return
 */
LinkedList * init_list_node(Node * node);


/**
 * Destroy the LinkedList.
 *
 * Destroys the LinkedList, freeing any allocated
 * memory and setting pointers to NULL.
 *
 * @param list A LinkedList object.
 */
void destroy_list(LinkedList * list);


/**
 * Iterate through every node of LinkedList.
 *
 * @param list      A LinkedList object.
 * @param consumer  A Consumer function pointer.
 */
void for_each(LinkedList * list, Consumer consumer);


/**
 * Get the head node from the linked list.
 *
 * Retrieves the de-referenced head node from the
 * given linked list. If the linked list has no
 * head node, function returns NULL.
 *
 * @param list An initialized LinkedList object.
 * @return The head Node object, or NULL on failure.
 */
Node * get_head_node(LinkedList * list);


/**
 * Push a node to the front of hte linked list.
 *
 * @param list      A LinkedList object.
 * @param data      Data for the LinkedList.
 * @param data_size Size od the data in bytes.
 */
void push_data(LinkedList * list, void * data, size_t data_size);


/**
 * Push a node to the front of hte linked list.
 *
 * @param list      A LinkedList object.
 * @param data      Data for the LinkedList.
 * @param data_size Size od the data in bytes.
 */
void push_node(LinkedList * list, Node * node);


/**
 * Filter linked list based on input parameters.
 *
 * Filters a LinkedList object based on the <comparator>
 * and <params> given to the function. Returns a new
 * LinkedList list with matches on success. If no
 * matches are found, the function returns NULL.
 *
 * @param list       A LinkedList object.
 * @param comparator  A comparator function pointer.
 * @param params     The search params for comparator.
 * @param num_params The number of params in params.
 * @return A LinkedList or NULL.
 */
LinkedList * filter(LinkedList * list, Comparator comparator, void * params, size_t num_params);


/**
 * Delete a Node from a LinkedList object.
 *
 * Deletes a node from a linked list object,
 * returning 0 on success, and -1 on failure.
 *
 *  - If a <Supplier> is provided, the function will
 *    delete the first instance that matches the
 *    supplier's return value.
 *  - If a <Supplier> is not provided, the function will
 *    delete the node by comparing the addresses of each
 *    Node to the <target>
 *
 * @param list     A LinkedList object.
 * @param target   The target Node to delete.
 * @param supplier A supplier function pointer.
 * @return 0 on success, -1 on failure.
 */
int delete_node(LinkedList * list, Comparator comparator, void * test_param);


/**
 * Find a node in the LinkedList.
 *
 * @param list        A LinkedList object.
 * @param comparator   comparator function pointer.
 * @param test_param  Test parameter for comparator.
 * @return Pointer to Node or NULL on failure.
 */
Node * find_node(LinkedList * list, Comparator comparator, void * test_param);

#endif //CPT_LINKED_LIST_H
