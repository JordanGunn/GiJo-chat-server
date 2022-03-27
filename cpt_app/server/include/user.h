//
// Created by jordan on 2022-03-18.
//

#ifndef CPT_USER_H
#define CPT_USER_H

#include "linked_list.h"
#include "cpt_definitions.h"
#include "cpt_types.h"

typedef struct users Users;
typedef struct user_struct User;
typedef struct user_node UserNode;


/**
 * A user Object.
 *
 * Contains data members useful for
 * handling user in the CPT environment.
 */
struct user_struct
{
    uint8_t id;
    uint8_t fd;
    char *  name;
};


struct users
{
    // properties
    int length;
    UserNode ** user_head;
    UserNode ** user_tail;
};


struct user_node
{
    User * user;
    UserNode * next_user;
    size_t user_size;
};


/**
 * Initialize a User object.
 *
 * Creates a User object, intialiazing any
 * data members and allocating the necessary
 * memory.
 *
 * @param id    User id.
 * @param fd    File descriptor for the user socket.
 * @param name  User name.
 * @return Pointer to a User object.
 */
User * user_init(int id, int fd, char * name);


/**
 * Destroy User object.
 *
 * Destroys a User object, freeing any memory
 * and setting necessary values to NULL.
 *
 * @param user A pointer to a User object.
 */
void user_destroy(User * user);


/**
 * Find a UserNode in a Users list by ID.
 *
 * @param users     LinkedList of users.
 * @param id        User id.
 * @return UserNode if success, NULL if failure.
 */
User * find_user(Users * users, int id);


/**
 * Push UserNode onto Users lists.
 *
 * @param users     LinkedList of users
 * @param user_node Pointer to UserNode object.
 */
void push_user_node(Users * users, UserNode * user_node);


/**
 * Create a UserNode object.
 *
 * Creates a UserNode object which can be added
 * to a LinkedList. This function mostly provides
 * a wrapper for the Node object, with additional
 * semantic meaning.
 *
 * @param user A pointer to a User object.
 * @return     A pointer to a UserNode object.
 */
UserNode * create_user_node(User * user);


/**
 *
 * @param users
 * @param id
 * @return
 */
int user_delete(Users * users, int id);


UserNode * get_head_user(Users * users);


/**
 * Initialize Users object.
 *
 * Initialize a Users object, allocating any
 * necessary memory and initializing data members.
 *
 * @param user      A pointer to a User object.
 * @return Pointer to Users LinkedList object.
 */
Users * users_init(UserNode * user_node);


/**
 * Destroy users object.
 *
 * A wrapper for destroy_list.
 * Frees any dynamic memory and set pointers to NULL.
 *
 * @param users     Pointer to a  LinkedList of User objects.
 */
void users_destroy(Users * users);


/**
 * Push a user onto a Users list.
 *
 * @param users Pointer to a linked list of User objects.
 * @param user  New user to add.
 */
int push_user(Users * users, User * user);


/**
 * Create a string from CptUser object.
 *
 * @param user
 * @return
 */
char * user_to_string(User * user);


// ===================
// P R E D I C A T E S
// ===================

bool find_user_id(void * data, void * test);
bool find_user_name(User * user, char * name);
bool filter_user_id(void * data, void * params);
bool filter_user_name(User * user, FilterQuery * filter_query);


// =================
// C O N S U M E R S
// =================
/**
 * Print Details about the channel.
 *
 * @param channel The target channel.
 */
void print_user(User * user);



#endif //CPT_USER_H
