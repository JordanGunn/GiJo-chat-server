//
// Created by jordan on 2022-03-16.
//

#ifndef CPT_SANDBOX_H
#define CPT_SANDBOX_H
#include "linked_list.h"
#include "../cpt_api/include/cpt_packet.h"

// ==================================
// T E S T I N G    F U N C T I O N S
// ==================================

typedef struct data test_data;
typedef struct query id_query;

// struct for test data
struct data {
    int id;
    const char * text;
};

struct query {
    int num_IDs;
    int * IDs;
};

// LINKED LIST
LinkedList * test_list(int len);
void test_data_to_string(void * data);
bool filter_IDs(void * data, void * params);
bool find_id_equal(void * data, void * param);
void test_linked_list_find(LinkedList *linked_list);
void test_linked_list_filter(LinkedList *linked_list);

// CPT packet
CptPacket * test_create_cpt_packet();



#endif //CPT_SANDBOX_H
