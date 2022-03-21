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

    if ( !(user = malloc(sizeof(User))) )
    {
        printf("Failed to allocate memory of size %lud\n", sizeof(User));
        return NULL;
    }

    memset(user, 0, sizeof(User));
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


UserNode * user_node(User * user)
{
    UserNode * user_node;

    user_node = create_node(user, sizeof(struct user_struct));
    return (user_node) ? user_node : NULL;
}


void push_user(Users users, User * user)
{
    UserNode * new_user;
    UserNode * next_user;

    new_user = create_node(user, sizeof(struct user_struct));
    next_user = *(users->head);
    (*users->head) = new_user;
    (*users->head)->next = next_user;

    if ( users->length == 1 )
    {
        (*users->tail) = next_user;   // set the first tail
    }

    users->length++;
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


Users users_init(User * user)
{
    Users users;

    if (!user) { return NULL; }

    users = init_list(user, sizeof(struct user_struct));
    return (users) ? users : NULL;
}


// ===================
// P R E D I C A T E S
// ===================

bool find_user_id(User * user, const int * id)
{
    return user->id == *id;
}


bool find_user_name(User * user, char * name)
{
    return !strcmp(user->name, name);
}


bool filter_user_id(User * user, FilterQuery * filter_query)
{
    int i, * num_crawler;
    int * IDs, num_IDs;

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


