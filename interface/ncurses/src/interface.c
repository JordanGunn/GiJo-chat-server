#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "command.h"
#include "user_state.h"

/* Global threading elements */
pthread_mutex_t     mutex;
pthread_cond_t      receiving;
bool is_receiving = false;
/* ------------------------- */

#define SLASH '/'
#define EXIT_KEY '0'
#define BUFF_SIZE 1024

#define GAME_WINDOW "Game Window:"
#define CHAT_WINDOW "Chat Input Window:"
#define DIALOGUE_WINDOW "Chat Dialogue Window:"


void clear_window(WINDOW *window, char *label)
{
    wclear(window);
    box(window, 0, 0);
    mvwprintw(window, 1, 1, label);
    wrefresh(window);
}

void update_window(WINDOW *window, char *label)
{
    box(window, 0, 0);
    mvwprintw(window, 1, 1, label);
    wrefresh(window);
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
    int chat_dialogue_height = 40;
    int chat_dialogue_width = 40 - 1;
    int chat_dialogue_start_y = 0;
    int chat_dialogue_start_x = 110 + 1;

    WINDOW * chat_dialogue_window = newwin(chat_dialogue_height, chat_dialogue_width, chat_dialogue_start_y, chat_dialogue_start_x);
    refresh();

    box(chat_dialogue_window, 0, 0);
    mvwprintw(chat_dialogue_window, 1, 1, DIALOGUE_WINDOW);
    wrefresh(chat_dialogue_window);

    return chat_dialogue_window;
}

void display_input(WINDOW * chat_dialogue_input, char **msg_history, const int *msg_count)
{
    clear_window(chat_dialogue_input, DIALOGUE_WINDOW);

    char *buffer = malloc(BUFF_SIZE);

    for (int i = 0; i <= *msg_count  - 1; i++)
    {
        strncat(buffer, msg_history[i], strlen(msg_history[i]));
        strcat(buffer, "\n ");
    }
    mvwprintw(chat_dialogue_input, 2, 1, "%s", buffer);
    update_window(chat_dialogue_input, DIALOGUE_WINDOW);
    wrefresh(chat_dialogue_input);
}

void update_msg_history(char msg[], char **msg_history, int *msg_count)
{
    msg_history[*msg_count] = malloc(strlen(msg) * sizeof(char));
    strcpy(msg_history[*msg_count], msg);

    *msg_count = *msg_count + 1;
}

void process_input(WINDOW * chat_input_window, WINDOW * chat_dialogue_window, char msg[], char **msg_history, int *msg_count, Command *cmd, UserState * ustate)
{
    wmove(chat_input_window, 2, 1);
    wrefresh(chat_input_window);

    echo();
    while (1)
    {
        wgetstr(chat_input_window, msg);
        noecho();

        if(strcmp(msg, "exit") == 0)
        {
            clear_window(chat_input_window, CHAT_WINDOW);
            break;
        }

        cmd->input = strdup(msg);
        parse_cmd_input(cmd);
        ustate->cmd = cmd;

//        if (is_valid_cmd(ustate->cmd))
//        {
//            is_receiving = true;
//            cmd_handler(ustate);
//            while(is_receiving)
//            {
//                puts("Waiting for response...");
//                pthread_cond_wait(&receiving, &mutex);
//            }
//        }
//        else
//        {
//            if ( ustate->cmd->input )
//            {
//                if ( strlen(ustate->cmd->input) )
//                {
//                    send_handler(ustate);
//                }
//            }
//        }
//        mvwprintw(chat_dialogue_window, 2, 1, "%d", *msg_count);
//        wrefresh(chat_dialogue_window);

        display_input(chat_dialogue_window, msg_history, msg_count);

        clear_window(chat_input_window, CHAT_WINDOW);
        wmove(chat_input_window, 2, 1);
        wrefresh(chat_input_window);
        echo();
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

    scrollok(chat_dialogue_window, TRUE);

    // waits for user input, returns int of key pressed
    int input;
    int exit_flag = 0;
    char msg[BUFF_SIZE];

    char **msg_history;
    int msg_count;

    msg_history = malloc(10 * sizeof(msg_history));
    msg_count = 0;

    Command *cmd;
    cmd = cmd_init();

    UserState * user_state;
    user_state = user_state_init();

    while ((input = getch()) != ERR) {
        switch(input)
        {
            case SLASH:
                //send cursor to chat input window
                process_input(chat_input_window, chat_dialogue_window, msg, msg_history, &msg_count, cmd, user_state);
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
