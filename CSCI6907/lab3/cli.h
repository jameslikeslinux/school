/*
 * cli.h
 *
 * Author: James Lee
 * Email:  jameslee@gwu.edu
 * Class:  CSCI 6907
 * Date:   20120319
 * Lab:    3
 */

#ifndef CLI_H
#define CLI_H

/* suggested return values for commands */
#define CMD_SUCCESS 0
#define CMD_SHOW_HELP 1
#define CMD_ERROR 2

void cli_init();
void cli_start();
void cli_stop();

/*
 * Adds a command to be available in the CLI
 *
 * name - the name of the command
 * help - will be displayed if 'cmd_func' returns 'CMD_SHOW_HELP'
 * cmd_func - the function to be called when 'name' is entered in the CLI
 *            the semantics of this function are like 'main'
 *
 * returns - 1 if succesfully added, 0 otherwise
 */
int cli_add_command(char *name, char *help, int (*cmd_func)(int argc, char *argv[]));

/*
 * Remove a command from the CLI
 *
 * returns - 1 if succesfully added, 0 otherwise
 */
int cli_remove_command(char *name);

/*
 * Sets the CLI prompt
 *
 * prompt - to be copied into a 15 char buffer
 */
void cli_set_prompt(char *prompt);

/*
 * Way for commands to be executed outside of the serial input
 *
 * text - copied into dynamic memory internally
 */
void cli_inject_text(char *text);

/*
 * Displays all available commands and their help
 */
void cli_show_help();

#endif