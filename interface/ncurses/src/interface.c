#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "command.h"
#include "user_state.h"
#include "handlers.h"
#include "cli_dc.h"

#define SLASH '/'
#define EXIT_KEY '0'
#define BUFF_SIZE 1024
#define MSG_HISTORY_SIZE 100

#define GAME_WINDOW "Game Window:"
#define CHAT_WINDOW "Chat Input Window:"
#define DIALOGUE_WINDOW "Chat Dialogue Window:"


const int const_chat_dialogue_height = 40;
const int const_chat_dialogue_width = 40 - 1;
const int const_chat_dialogue_start_y = 0;
const int const_chat_dialogue_start_x = 110 + 1;

struct Message_Node {
    char *msg;
    int y_pos_start;
    struct Message_Node *next;
};

struct Message_Node_Deployer {
    uint8_t max_num_msg;
    uint8_t current_num_msg;
    struct Message_Node *head;
};

struct Message_Node_Deployer *init_msg_display(uint8_t max_num_msg) {
    struct Message_Node_Deployer *mnd = (struct Message_Node_Deployer*) malloc(sizeof(struct Message_Node_Deployer));
    mnd->max_num_msg = max_num_msg;
    mnd->current_num_msg = 0;
    mnd->head = NULL;
    return mnd;
}


void add_msg_node(struct Message_Node_Deployer* mnd, char* msg) {
    //if current is 0, then the head node is empty.
    if (mnd->current_num_msg == 0) {
        mnd->head = (struct Message_Node*) malloc(sizeof (struct Message_Node));
        mnd->head->msg = malloc(strlen(msg) + 1);
        strcpy(mnd->head->msg, msg);
        mnd->head->next = NULL;
    } else {
        struct Message_Node *current_node = mnd->head;
        while (current_node != NULL) {
            //if currentnode is the tail node
            if (current_node->next == NULL) {
                current_node->next = (struct Message_Node*) malloc(sizeof (struct Message_Node));
                current_node->next->msg = malloc(strlen(msg) + 1);
                strcpy(current_node->next->msg, msg);
                current_node->next->next = NULL;
                break;
            } else {
                current_node = current_node->next;
            }
        }
    }
    mnd->current_num_msg++;
    if (mnd->current_num_msg > mnd->max_num_msg) {
        //Cut the head;
        struct Message_Node *new_head = mnd->head->next;
        free(mnd->head->msg);
        free(mnd->head);
        mnd->head = new_head;
    }
}


void print_all_messages(WINDOW * win, struct Message_Node_Deployer* mnd, int x_pos, int y_pos) {
    struct Message_Node *current_node = mnd->head;
    int starting_y_pos = y_pos;
    while (current_node != NULL) {
        mvwprintw(win, starting_y_pos, x_pos, current_node->msg);
        current_node->y_pos_start = starting_y_pos;
        current_node = current_node->next;
        ++starting_y_pos;
    }

}

void clear_all_message_nodes(struct Message_Node_Deployer* mnd) {
    mnd->current_num_msg = 0;

    while (mnd->head != NULL) {
        struct Message_Node *new_head = mnd->head->next;
        free(mnd->head->msg);
        free(mnd->head);
        mnd->head = new_head;
    }
}

void parse_message(struct Message_Node_Deployer* mnd, char* msg, int max_line_length) {
    if ((int) strlen(msg) < max_line_length) {
        add_msg_node(mnd, msg);
    } else {
        int current_pos = 0;
        int msg_length = (int) strlen(msg);
        while (current_pos < msg_length) {
            char current_msg[max_line_length + 1];
            memset(current_msg, '\0', max_line_length + 1);
            if (msg_length - current_pos < max_line_length) {
                strncpy(current_msg, msg + current_pos, msg_length - current_pos);
                current_pos += msg_length - current_pos;

            } else {
                strncpy(current_msg, msg + current_pos, max_line_length);
                current_pos += max_line_length;
            }
            add_msg_node(mnd, current_msg);
        }
    }
}

WINDOW * build_game_component()
{
    // Chat input window size
    int game_height = 30;
    int game_width = 110;
    int game_start_y = 0;
    int game_start_x = 0;

    WINDOW * game_window = newwin(game_height, game_width, game_start_y, game_start_x);
    refresh();

    box(game_window, 0, 0);
    mvwprintw(game_window, 1, 1, GAME_WINDOW);
    wrefresh(game_window);

    return game_window;
}

WINDOW * build_chat_component()
{
    // Chat input window size
    int chat_input_height = 10;
    int chat_input_width = 110;
    int chat_input_start_y = 30;
    int chat_input_start_x = 0;

    WINDOW * chat_input_window = newwin(chat_input_height, chat_input_width, chat_input_start_y, chat_input_start_x);
    refresh();

    box(chat_input_window, 0, 0);
    mvwprintw(chat_input_window, 1, 1, CHAT_WINDOW);
    wrefresh(chat_input_window);

    return chat_input_window;
}

WINDOW * build_dialogue_component()
{
    // Chat input window size
    int chat_dialogue_height = const_chat_dialogue_height;
    int chat_dialogue_width = const_chat_dialogue_width;
    int chat_dialogue_start_y = const_chat_dialogue_start_y;
    int chat_dialogue_start_x = const_chat_dialogue_start_x;

    WINDOW * chat_dialogue_window = newwin(chat_dialogue_height, chat_dialogue_width, chat_dialogue_start_y, chat_dialogue_start_x);
    refresh();

    box(chat_dialogue_window, 0, 0);
    mvwprintw(chat_dialogue_window, 1, 1, DIALOGUE_WINDOW);
    wrefresh(chat_dialogue_window);

    return chat_dialogue_window;
}

int is_user_logged_in(UserState *ustate)
{
    if(*ustate->ncurses_state->is_logged_in == 0)
    {
        clear_window(ustate->ncurses_state->chat_dialogue_window, DIALOGUE_WINDOW);

        char *buffer = malloc(BUFF_SIZE);
        strcat(buffer, "Please Log-in (eg. @login Charlie)\n ");

        update_msg_history(buffer, ustate->ncurses_state->msg_history, &ustate->ncurses_state->msg_count);
        display_input(ustate->ncurses_state->chat_dialogue_window, ustate->ncurses_state->msg_history,
                                      &ustate->ncurses_state->msg_count);

        clear_window(ustate->ncurses_state->chat_input_window, CHAT_WINDOW);
        wmove(ustate->ncurses_state->chat_input_window, 2, 1);
        wrefresh(ustate->ncurses_state->chat_input_window);
        echo();

        return 0;
    }
    else
    {
        clear_window(ustate->ncurses_state->chat_dialogue_window, DIALOGUE_WINDOW);
        wrefresh(ustate->ncurses_state->chat_dialogue_window);
        return 1;
    }
}

void process_input(char msg[], UserState * ustate, struct Message_Node_Deployer *mnd)
{
    pthread_t th[2];

    wmove(ustate->ncurses_state->chat_input_window, 2, 1);
    wrefresh(ustate->ncurses_state->chat_input_window);

    echo();

    while (1)
    {
//        wgetstr(ustate->ncurses_state->chat_input_window, msg);
//        noecho();
//
//        if(strcmp(msg, "exit") == 0)
//        {
//            clear_window(ustate->ncurses_state->chat_input_window, CHAT_WINDOW);
//            break;
//        }

//        if (is_user_logged_in(ustate) == 0)
        if (*ustate->ncurses_state->is_logged_in == 0)
        {
            while (1) {
                wgetstr(ustate->ncurses_state->chat_input_window, msg);
                noecho();

                if(strcmp(msg, "exit") == 0)
                {
                    clear_window(ustate->ncurses_state->chat_input_window, CHAT_WINDOW);
                    break;
                }

                if (strncmp(msg, "@login", strlen("@login")) == 0) {
                    char name[BUFF_SIZE];

                    strncpy(name, msg + strlen("@login") + 1, strlen(msg + strlen("@login")));

                    user_login(ustate, "127.0.0.1", "8080", name);

                    if (ustate->LOGGED_IN) {
                        char log_in_msg[BUFF_SIZE];
                        sprintf(log_in_msg, "Logged-In: %s", name);

//                        update_msg_history(log_in_msg, ustate->ncurses_state->msg_history, &ustate->ncurses_state->msg_count);
//                        display_input(ustate->ncurses_state->chat_dialogue_window, ustate->ncurses_state->msg_history,
//                                      &ustate->ncurses_state->msg_count);
                        clear_window(ustate->ncurses_state->chat_dialogue_window, DIALOGUE_WINDOW);
                        mvwprintw(ustate->ncurses_state->chat_dialogue_window, 2, 1, "Logged-In: %s", name);
                        wrefresh(ustate->ncurses_state->chat_dialogue_window);

                        clear_window(ustate->ncurses_state->chat_input_window, CHAT_WINDOW);
                        wmove(ustate->ncurses_state->chat_input_window, 2, 1);
                        wrefresh(ustate->ncurses_state->chat_input_window);
                        echo();

                        *ustate->ncurses_state->is_logged_in = 1;
                        break;
                    } else {
                        char log_in_msg[BUFF_SIZE];
                        sprintf(log_in_msg, "%s Failed to Logged-In.", name);

//                        update_msg_history(log_in_msg, ustate->ncurses_state->msg_history, &ustate->ncurses_state->msg_count);
//                        display_input(ustate->ncurses_state->chat_dialogue_window, ustate->ncurses_state->msg_history,
//                                      &ustate->ncurses_state->msg_count);
                        clear_window(ustate->ncurses_state->chat_dialogue_window, DIALOGUE_WINDOW);
                        mvwprintw(ustate->ncurses_state->chat_dialogue_window, 2, 1, "%s Failed to Logged-In.", name);
                        wrefresh(ustate->ncurses_state->chat_dialogue_window);

                        clear_window(ustate->ncurses_state->chat_input_window, CHAT_WINDOW);
                        wmove(ustate->ncurses_state->chat_input_window, 2, 1);
                        wrefresh(ustate->ncurses_state->chat_input_window);
                        echo();
                    }
                }
                else
                {
                    clear_window(ustate->ncurses_state->chat_dialogue_window, DIALOGUE_WINDOW);

                    char *buffer = malloc(BUFF_SIZE);
                    strcat(buffer, "Please Log-in (eg. @login Charlie)\n ");

                    clear_window(ustate->ncurses_state->chat_dialogue_window, DIALOGUE_WINDOW);
                    mvwprintw(ustate->ncurses_state->chat_dialogue_window, 2, 1, "%s", buffer);
                    wrefresh(ustate->ncurses_state->chat_dialogue_window);

//                    update_msg_history(buffer, ustate->ncurses_state->msg_history, &ustate->ncurses_state->msg_count);
//                    display_input(ustate->ncurses_state->chat_dialogue_window, ustate->ncurses_state->msg_history,
//                                  &ustate->ncurses_state->msg_count);

                    clear_window(ustate->ncurses_state->chat_input_window, CHAT_WINDOW);
                    wmove(ustate->ncurses_state->chat_input_window, 2, 1);
                    wrefresh(ustate->ncurses_state->chat_input_window);
                    echo();
                }
            }
        }
        else
        {
            thread_chat_io(th, ustate);
//            clear_window(ustate->ncurses_state->chat_input_window, CHAT_WINDOW);
//            wmove(ustate->ncurses_state->chat_input_window, 2, 1);
//            wrefresh(ustate->ncurses_state->chat_input_window);
//            echo();
        }

//        ustate->cmd->input = strdup(msg);
//        parse_cmd_input(ustate->cmd);

//        update_msg_history(msg, ustate->ncurses_state->msg_history, &ustate->ncurses_state->msg_count);
//        display_input(ustate->ncurses_state->chat_dialogue_window, ustate->ncurses_state->msg_history, &ustate->ncurses_state->msg_count);

//        mvwprintw(ustate->ncurses_state->chat_dialogue_window, 2, 1, msg);
//        wrefresh(ustate->ncurses_state->chat_dialogue_window);



// Uncomment
//        parse_message(mnd, msg, const_chat_dialogue_width);
//        print_all_messages(ustate->ncurses_state->chat_dialogue_window, mnd, 1, 2);
//        wrefresh(ustate->ncurses_state->chat_dialogue_window);

//        clear_window(ustate->ncurses_state->chat_input_window, CHAT_WINDOW);
//        wmove(ustate->ncurses_state->chat_input_window, 2, 1);
//        wrefresh(ustate->ncurses_state->chat_input_window);
//        echo();
    }

}


void return_to_game_window(WINDOW * game_window)
{
    wmove(game_window, 2, 1);
    wrefresh(game_window);
}

void run_chat_ncurses()
{
    // initializes screen and allocates memory for ncurses window
    initscr();

    // enables/disables echoing of user input on screen
    noecho();

    // Create windows for interface
    WINDOW * game_window = build_game_component();
    WINDOW * chat_input_window = build_chat_component();
    WINDOW * chat_dialogue_window = build_dialogue_component();
    return_to_game_window(game_window);

    // Auto scroll-down in chat dialogue window
    scrollok(chat_dialogue_window, TRUE);

    // Initializes Command object
    Command *cmd;
    cmd = cmd_init();

    // Initializes NcursesState object and assigns WINDOWs to NcursesState
    NcursesState * ncurses_state;
    ncurses_state = ncurses_state_init();
    ncurses_state->msg_history = malloc(MSG_HISTORY_SIZE * sizeof(ncurses_state->msg_history));
    ncurses_state->game_window = game_window;
    ncurses_state->chat_input_window = chat_input_window;
    ncurses_state->chat_dialogue_window = chat_dialogue_window;

    // Initializes UserState object and assigns Command and NcursesState object to UserState
    UserState * user_state;
    user_state = user_state_init();
    user_state->cmd = cmd;
    user_state->ncurses_state = ncurses_state;

    // Variables for main ncurses loop
    int input;
    int exit_flag = 0;
    char msg[BUFF_SIZE];

    // Message Displayer
    struct Message_Node_Deployer *mnd = init_msg_display(const_chat_dialogue_height - 2);

    // Main ncurses loop
    while ((input = getch()) != ERR) {
        switch(input)
        {
            case SLASH:
                //send cursor to chat input window
                process_input(msg, user_state, mnd);
                return_to_game_window(game_window);
                break;
            case EXIT_KEY:
                exit_flag = 1;
                break;
            default:
                continue;
        }
        if (exit_flag)
        {
            break;
        }
    }
    //ends ncurses window and deallocates memory for the ncurses window
    endwin();

}
