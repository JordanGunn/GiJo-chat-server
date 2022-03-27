//
// Created by jordan on 2022-03-16.
//

#include "sandbox.h"

void test_linked_list();

int main()
{
//    test_linked_list();
    test_create_cpt_packet();


    return 0;
}

void test_linked_list() {// test linked list creation and push_node
    LinkedList * linked_list;
    linked_list = test_list(5);

    // test linked list find
    test_linked_list_find(linked_list);

    // test linked list filter
    test_linked_list_filter(linked_list);
}

// ================================================================
//  L I N K E D    L I S T      T E S T I N G    F U N C T I O N S
// ================================================================

void test_linked_list_filter(LinkedList *linked_list) {
    int IDs[] = {1, 3, 4};
    LinkedList * filtered;
    int num_elements = 3;
    id_query idq;
    idq.num_IDs = 3;
    idq.IDs = IDs;


    puts("");
    puts("");
    puts("Filter result: checking for {1, 3, 4}:");
    filtered = filter(linked_list, filter_IDs, &idq, num_elements);
    for_each(filtered, test_data_to_string);
}


void test_linked_list_find(LinkedList *linked_list) {
    int search_id, search_head, search_tail;
    Node * found_node, * found_head, * found_tail;
    test_data * found_data;

    // test generic print
    for_each(linked_list, test_data_to_string);

    // test find_node
    search_id = 3;
    search_head = 4;
    search_tail = 0;

    found_node = find_node(linked_list, find_id_equal, &search_id);
    puts("");
    puts("");
    puts("Located node @:");
    found_data = found_node->data;
    test_data_to_string(found_data);

    found_head = find_node(linked_list, find_id_equal, &search_head);
    puts("");
    puts("");
    puts("Located head node @:");
    found_data = found_head->data;
    test_data_to_string(found_data);

    found_tail = find_node(linked_list, find_id_equal, &search_tail);
    puts("");
    puts("");
    puts("Located tail node @:");
    found_data = found_tail->data;
    test_data_to_string(found_data);

    // test generic print
    puts("");
    puts("");
    for_each(linked_list, test_data_to_string);
}


LinkedList * test_list(int len)
{
    // test params
    int id, i;
    LinkedList * list;
    const char * default_text;

    // set default text
    default_text = "Hello, world";

    // initialize count for id
    id = 0;

    // create the seed node for list initialization
    test_data data, some_data;

    data.id = id;
    data.text = default_text;

    // initialize the linked list
    list = init_list_data((void *) &data, sizeof(data));

    if ( !list )
    {
        printf("Failed to create linked list!\n");
        printf("From test function:\ttest_list()\n");
        return NULL;
    }

    // start adding nodes to the linked list
    for (i = id; i < len; i++)
    {
        // create some more data and push the node
        int new_id = i;
        some_data = (test_data){ (new_id), default_text };
        push_data(list, &some_data, sizeof(some_data));
    }

    return list;
}


// CONSUMERS
void test_data_to_string(void * data)
{
    test_data * td;
    td = (test_data *)data;
    printf("[ {id: %d} {text: %s} ]", (td->id), td->text );
    printf(" --> ");
}


// comparatorS
bool find_id_equal(void * data, void * param)
{
    test_data * td;
    td = (test_data *)data;

    return td->id == *( (int *)param );
}

bool filter_IDs(void * data, void * params)
{
    int i, * num_crawler;
    id_query * idq;
    test_data * td;

    td  = (test_data *) data;
    idq = (id_query *) params;

    num_crawler = idq->IDs;
    for (i = 0; i < idq->num_IDs; i++)
    {
        if ( td->id == *(num_crawler) ) { return true; }
        num_crawler++;
    }
    return false;
}


// ============================================================
//  C P T   B U I L D E R   T E S T I N G    F U N C T I O N S
// ============================================================

CptRequest * test_create_cpt_packet()
{
    uint8_t serial_buffer[LG_BUFF_SIZE];
    uint8_t  version_major, version_minor;
    uint8_t  command;
    uint16_t channel_id;
    char * msg, * first_cpt, * second_cpt;
    CptRequest * cpt_parsed;

    command = SEND;
    channel_id = 3;
    version_major = 1;
    version_minor = 1;
    msg = "Hello, World!";

    // build the stupid thing
    CptRequest * cpt;
    cpt = cpt_request_init();
    cpt_request_chan(cpt, channel_id);
    cpt_request_cmd(cpt, command);
    cpt_request_version(cpt, version_major, version_minor);
    cpt_request_msg(cpt, msg);

    // print the stupid thing
    first_cpt = cpt_to_string(cpt);
    puts(first_cpt);
    free(first_cpt);

    // test serialization
    cpt_serialize_packet(cpt, serial_buffer);

    // convert back to unpacked serial
    cpt_parsed = cpt_request_parse(serial_buffer);
    second_cpt = cpt_to_string(cpt_parsed);
    puts(second_cpt);
    free(second_cpt);


    return cpt;
}

