//
// Created by jordan on 2022-03-18.
//

#include "cpt_user.h"

User * user_init(int id, int fd, char * name)
{
    User * user;

    if ( !(name) ) { name = DEFAULT_USERNAME; }
    else if (strlen(name) > MAX_NAME_SIZE)
    {
        write(STDERR_FILENO, "Name exceeds max of %d characters...\n", MAX_NAME_SIZE);
        return NULL;
    }

    if ( !(user = malloc(sizeof(struct user_struct))) )
    {
        printf("Failed to allocate memory of size %lud\n", sizeof(struct user_struct));
        return NULL;
    }

    memset(user, 0, sizeof(struct user_struct));
    user->fd = fd;
    user->id = id;
    user->name = strdup(name);

    return user;
}


void user_destroy(User * user)
{
    if (user)
    {
        if (user->name)
        {
            free(user->name);
            user->name = NULL;
        }
        free(user); user = NULL;
    }
}


UserNode * create_user_node(User * user)
{
    UserNode * user_node;

    user_node = (UserNode *)create_node(user, sizeof(struct user_struct));
    return (user_node) ? user_node : NULL;
}


UserNode * get_head_user(Users * users)
{
    UserNode * user_node;
    user_node = (UserNode *)get_head_node((LinkedList *) users);
    return user_node;
}


void push_user(Users * users, User * user)
{
    LinkedList * list;
    list = (LinkedList *) users;
    push_data(list, user, sizeof(struct user_struct));
}


void push_user_node(Users * users, UserNode * user_node)
{
    push_node( (LinkedList *)
            users, (Node *) user_node);
}


void print_user(User * user)
{
    char buffer[MD_BUFF_SIZE] = {0};

    sprintf(buffer,
        "ID: %d\t"    \
        "NAME: %s\n", \
        user->id, user->name
    );

    printf("%s\n", buffer);
}


User * find_user(Users * users, int id)
{
    UserNode * user_node;

    user_node = (UserNode *)
            find_node((LinkedList *) users, find_user_id, &id);

    return user_node->user;
}


int delete_user(Users * users, int id)
{
    int result;
    Comparator find_id;

    result = SYS_CALL_FAIL;
    find_id = (Comparator) find_user_id;
    if ( users )
    {
        result = delete_node((LinkedList *) users, find_id, &id);
        users->length--;
    }

    return result;
}


char * user_to_string(User * user)
{
    char buffer[MD_BUFF_SIZE] = {0};

    sprintf(buffer,
            "ID: %d\t"    \
        "NAME: %s\n", \
        user->id, user->name
    );

    return strdup(buffer);
}


Users * users_init(UserNode * user_node)
{
    Users * users;

    if (!user_node) { return NULL; }
    users = (Users *) init_list_node(
        (Node *) user_node
    );

    return (users) ? users : NULL;
}


// ===================
// P R E D I C A T E S
// ===================

bool find_user_id(void * data, void * test)
{
    int * id;
    User * user;

    user = (User *) data;
    id = (int *) test;
    return (user->id == *id);
}


bool find_user_name(User * user, char * name)
{
    return !strcmp(user->name, name);
}


bool filter_user_id(void * data, void * params)
{
    User * user;
    int i, * num_crawler;
    int * IDs, num_IDs;
    FilterQuery * filter_query;

    user = (User *) data;
    filter_query = (FilterQuery *) params;

    IDs = (int *)filter_query->params;
    num_IDs = filter_query->num_params;

    num_crawler = IDs;
    for (i = 0; i < num_IDs; i++)
    {
        if ( user->id == *(num_crawler) ) { return true; }
        num_crawler++;
    }
    return false;
}


