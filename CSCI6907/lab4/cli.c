/*
 * cli.c
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120319
 * Lab:    3
 */

#include <stdlib.h>
#include <stdio.h>
#include <sio.h>
#include <string.h>
#include <limits.h>
#include "cli.h"
#include "queue.h"
#include "utils.h"
#include "uart.h"

#define CLI_MAX_CMD_LEN 256
#define CLI_MAX_CMD_TOKENS (CLI_MAX_CMD_LEN / 2)
#define CLI_MAX_NUM_CMDS 16
#define CLI_MAX_PROMPT_LEN 16

typedef struct {
    int (*cmd_func)(int argc, char **argv);
    char *name;
    char *help;
    int isvalid;
} cmd_t;

static queue_t cmdqueue;
static volatile int stop;
static char prompt[CLI_MAX_PROMPT_LEN] = "> ";
static cmd_t commands[CLI_MAX_NUM_CMDS];    /* this would be better as a hashtable */

static int _cli_tokenize_command(char *cmd, int *argc, char *argv[]) {
    int i, inquote;
    char *cmdtok;

    inquote = 0;

    /* handle quotes and escapes */
    for (i = 0; i < strlen(cmd); i++) {
        if (cmd[i] == '"' && i < strlen(cmd) && cmd[i + 1] == '"') {
            /* handle empty quotes ("") -> inject dummy char in string */
            cmd[i] = 127;
            inquote = 1;
        } else if (cmd[i] == '"' && (i == 0 || cmd[i - 1] != '\\') && !inquote) {
            /* handle open quote as long as it's not escaped */
            delete_char(&cmd[i--]);
            inquote = 1;
        } else if (inquote && cmd[i] == ' ') {
            /* replace any strings inside of quotes with a dummy char */
            cmd[i] = 127;
        } else if (inquote && cmd[i] == '"' && cmd[i - 1] != '\\') {
            /* handle closing quote as long as it's not escaped */
            delete_char(&cmd[i--]);
            inquote = 0;
        } else if (cmd[i] == '"' && i > 0 && cmd[i - 1] == '\\') {
            /* remove slash from escaped quotes */
            delete_char(&cmd[(i--) - 1]);
        }

        /* 
         * unfortunately, I don't handle escaped backslashes so it's not
         * possible to have an explicit '\"' inside of a command
         */
    }

    /* if we didn't detect a closing quote, then return an error */
    if (inquote) {
        return 0;
    }

    /* 
     * split along spaces using strtok...this will work because I replaced
     * spaces inside of quotes with dummy chars
     */
    cmdtok = strtok(cmd, " ");
    while (cmdtok && *argc < CLI_MAX_CMD_TOKENS) {
        if (strlen(cmdtok) == 1 && cmdtok[0] == 127) {
            /* handle empty quotes ("") -> remove dummy char from string */
            delete_char(cmdtok);
        } else {
            /* replace dummy chars with space in quoted string */
            for (i = 0; i < strlen(cmdtok); i++) {
                if (cmdtok[i] == 127) {
                    cmdtok[i] = ' ';
                }
            }
        }

        /* provide the token back to the calling function */
        argv[(*argc)++] = cmdtok;

        cmdtok = strtok(NULL, " ");
    }

    return 1;
}

void cli_init() {
    int i;

    for (i = 0; i < CLI_MAX_NUM_CMDS; i++) {
        commands[i].isvalid = 0;
    }

    /* a small buffer for incomming commands */
    queue_init(&cmdqueue, 8);
}

void cli_start() {
    char c, *input, *cmd, *argv[CLI_MAX_CMD_TOKENS];
    int read, argc, i, cmdfound;
    
    input = NULL;
    read = 0;
    stop = 0;

    while (!stop) {
        /*
         * INITIALIZE INPUT (if necessary)
         */
        if (!input) {
            input = (char *) malloc(CLI_MAX_CMD_LEN * sizeof(char));
            read = 0;

            if (input) {
                myprintf("%s", prompt);
            }
        }

        /*
         * READ AND PROCESS INPUT (if available)
         */
        if (input && uart_kbhit(&uart0)) {
            c = uart_getch(&uart0);

            if (c == 127 && read > 0) {
                /* backspace */
                uart_putch(&uart0, c);
                read--;
            } else if (c == 13) {
                /* carriage return */
                uart_putch(&uart0, '\n');
                input[read] = '\0';
                queue_enqueue(&cmdqueue, input);    /* queue command for execution */
                input = NULL;
            } else if (c == 3) {
                /* ctrl+c */
                uart_putch(&uart0, 10);
                free(input);
                input = NULL;
            } else if (c == 12) {
                /* ctrl+l -> clear screen */
                uart_putch(&uart0, c);

                /* restore prompt and input so far */
                myprintf("%s", prompt);
                for (i = 0; i < read; i++) {
                    uart_putch(&uart0, input[i]);
                }
            } else if (c >= 32 && c <= 126 && read < CLI_MAX_CMD_LEN - 1) {
                /* printable ascii */
                uart_putch(&uart0, c);
                input[read++] = c;
            }
        }

        /*
         * EXECUTE COMMANDS IN QUEUE
         */
        while (queue_dequeue(&cmdqueue, &cmd)) {
            if (input) {
                /*
                 * if input is not null, then the commands in queue must have
                 * been injected (by button presses, for example), so print
                 * out the command
                 */
                for (i = 0; i < read; i++) {
                    uart_putch(&uart0, 127);
                }

                myprintf("%s\n", cmd);
            }

            argc = 0;
            if (_cli_tokenize_command(cmd, &argc, argv)) {
                if (argc > 0) {
                    cmdfound = 0;

                    for (i = 0; i < CLI_MAX_NUM_CMDS; i++) {
                        /* like I said, 'commands' should be a hashtable */
                        if (commands[i].isvalid && !strcmp(commands[i].name, argv[0])) {
                            cmdfound = 1;

                            /* execute command function and check return value */
                            if (commands[i].cmd_func(argc, argv) == CMD_SHOW_HELP) {
                                myprintf("\n%s\n\n", commands[i].help);
                            }

                            break;
                        }
                    }

                    if (!cmdfound) {
                        myprintf("Command not found: %s\n", argv[0]);
                    }
                }
            } else {
                myprintf("Could not parse command.\n");
            }

            if (input) {
                /* restore any text the user had been inputting */
                myprintf("%s", prompt);
                for (i = 0; i < read; i++) {
                    uart_putch(&uart0, input[i]);
                }
            }

            free(cmd);
        }
    }
}

void cli_stop() {
    stop = 1;
}

int cli_add_command(char *name, char *help, int (*cmd_func)(int argc, char *argv[])) {
    int i;

    /* check whether the command already exists */
    for (i = 0; i < CLI_MAX_NUM_CMDS; i++) {
        if (commands[i].isvalid && !strcmp(commands[i].name, name)) {
            myprintf("Command '%s' already exists.\n", name);
            return 0;
        }
    }

    /* find an invalid slot and fill it */
    for (i = 0; i < CLI_MAX_NUM_CMDS; i++) {
        if (!commands[i].isvalid) {
            commands[i].cmd_func = cmd_func;
            commands[i].name = name;
            commands[i].help = help;
            commands[i].isvalid = 1;
            return 1;
        }
    }

    /* failed to find a command slot */
    return 0;
}

int cli_remove_command(char *name) {
    int i = 0;

    for (i = 0; i < CLI_MAX_NUM_CMDS; i++) {
        if (commands[i].isvalid && !strcmp(commands[i].name, name)) {
            commands[i].isvalid = 0;
            return 1;
        }
    }

    return 0;
}

void cli_set_prompt(char *p) {
    strncpy(prompt, p, CLI_MAX_PROMPT_LEN);
    prompt[CLI_MAX_PROMPT_LEN - 1] = '\0';
}

void cli_inject_text(char *text) {
    char *copy_text = (char *) malloc((strlen(text) + 1) * sizeof(char));

    if (copy_text) {
        strcpy(copy_text, text);
        copy_text[strlen(text)] = '\0';
        queue_enqueue(&cmdqueue, copy_text);
    } else {
        myprintf("Failed to allocate memory in cli_inject_text\n");
    }
}

void cli_show_help() {
    int i;

    myprintf("\n");

    for (i = 0; i < CLI_MAX_NUM_CMDS; i++) {
        if (commands[i].isvalid) {
            myprintf("%s\n\n", commands[i].help);
        }
    }
}