#include <stdlib.h>
#include <string.h>
#include "Cli.h"
#include "Utils.h"

#define CLI_MAX_CMD_LEN 77
#define CLI_MAX_CMD_TOKENS 33
static const char *CLI_PROMPT = "> ";


// initialize commands with zeros
Cmd Cli::commands[] = {0};

Cli::Cli(EthernetClient *client) : cmdqueue(4) {
    this->client = client;
}

bool Cli::addCommand(char *name, prog_char *help, int (*cmdfunc)(const Cli *cl, int argc, char *argv[])) {
    // check whether the command already exists
    for (int i = 0; i < CLI_MAX_NUM_CMDS; i++) {
        if (Cli::commands[i].isValid && !strcmp(Cli::commands[i].name, name)) {
            Serial.print("Command '");
            Serial.print(name);
            Serial.println("' already exists.");
            return false;
        }
    }

    // find an invalid slot and fill it
    for (int i = 0; i < CLI_MAX_NUM_CMDS; i++) {
        if (!Cli::commands[i].isValid) {
            Cli::commands[i].cmdfunc = cmdfunc;
            Cli::commands[i].name = name;
            Cli::commands[i].help = help;
            Cli::commands[i].isValid = true;
            return true;
        }
    }

    // failed to find a command slot
    return false;
}

void Cli::go() {
    this->input = NULL;

    while (this->client->connected()) {
        this->processInput();
        this->processCommands();
    }
}
        
EthernetClient * Cli::getClient() const {
    return this->client;
}

void Cli::showHelp() const {
    this->client->println();

    for (int i = 0; i < CLI_MAX_NUM_CMDS; i++) {
        if (Cli::commands[i].isValid) {
            this->showHelp(Cli::commands[i]);
        }
    }
            
    this->client->println();
}

void Cli::showHelp(const Cmd &command) const {
    this->client->println(command.help);
}

bool Cli::tokenizeCommand(char *cmd, int *argc, char *argv[]) {
    bool inquote;
    char *cmdtok;

    inquote = false;

    /* handle quotes and escapes */
    for (int i = 0; i < strlen(cmd); i++) {
        if (cmd[i] == '"' && i < strlen(cmd) && cmd[i + 1] == '"') {
            /* handle empty quotes ("") -> inject dummy char in string */
            cmd[i] = 127;
            inquote = true;
        } else if (cmd[i] == '"' && (i == 0 || cmd[i - 1] != '\\') && !inquote) {
            /* handle open quote as long as it's not escaped */
            deleteChar(&cmd[i--]);
            inquote = true;
        } else if (inquote && cmd[i] == ' ') {
            /* replace any strings inside of quotes with a dummy char */
            cmd[i] = 127;
        } else if (inquote && cmd[i] == '"' && cmd[i - 1] != '\\') {
            /* handle closing quote as long as it's not escaped */
            deleteChar(&cmd[i--]);
            inquote = false;
        } else if (cmd[i] == '"' && i > 0 && cmd[i - 1] == '\\') {
            /* remove slash from escaped quotes */
            deleteChar(&cmd[(i--) - 1]);
        }

        /* 
         * unfortunately, I don't handle escaped backslashes so it's not
         * possible to have an explicit '\"' inside of a command
         */
    }

    /* if we didn't detect a closing quote, then return an error */
    if (inquote) {
        return false;
    }

    /* 
     * split along spaces using strtok...this will work because I replaced
     * spaces inside of quotes with dummy chars
     */
    cmdtok = strtok(cmd, " ");
    while (cmdtok && *argc < CLI_MAX_CMD_TOKENS) {
        if (strlen(cmdtok) == 1 && cmdtok[0] == 127) {
            /* handle empty quotes ("") -> remove dummy char from string */
            deleteChar(cmdtok);
        } else {
            /* replace dummy chars with space in quoted string */
            for (int i = 0; i < strlen(cmdtok); i++) {
                if (cmdtok[i] == 127) {
                    cmdtok[i] = ' ';
                }
            }
        }

        /* provide the token back to the calling function */
        argv[(*argc)++] = cmdtok;

        cmdtok = strtok(NULL, " ");
    }

    return true;
}

void Cli::processInput() {
    char c;

    if (!this->input) {
        this->input = (char *) malloc((CLI_MAX_CMD_LEN + 1) * sizeof(char));
        this->read = 0;

        if (this->input) {
            this->client->print(CLI_PROMPT);
        }
    }

    if (this->input && this->client->available()) {
        c = this->client->read();

        if (c == 127 && this->read > 0) {
            // backspace
            this->client->print("\b \b");
            this->read--;
        } else if (c == 13) {
            // new line
            this->client->println();
            this->input[this->read] = '\0';
            this->cmdqueue.enqueue(this->input);
            this->input = NULL;
        } else if (c == 3) {
            // ctrl+c
            this->client->println();
            free(this->input);
            this->input = NULL;
        } else if (c == 12) {
            // ctrl+l
            this->client->print("\033[2J\033[H");

            // restore prompt and input so far
            this->client->print(CLI_PROMPT);
            for (int i = 0; i < this->read; i++) {
                this->client->write(this->input[i]);
            }
        } else if (c == 4 && this->read == 0) {
            // ctrl+d (exit)
            this->client->println();
            this->client->stop();
            return;
        } else if (c >= 32 && c <= 126 && this->read < CLI_MAX_CMD_LEN) {
            // printable ascii
            this->client->write(c);
            this->input[this->read++] = c;
        }
    }
}

void Cli::processCommands() {
    char *cmd, *argv[CLI_MAX_CMD_TOKENS];
    int argc;
    bool cmdFound;

    while (this->cmdqueue.dequeue(cmd)) {
        argc = 0;

        if (Cli::tokenizeCommand(cmd, &argc, argv)) {
            if (argc > 0) {
                cmdFound = false;

                for (int i = 0; i < CLI_MAX_NUM_CMDS; i++) {
                    if (Cli::commands[i].isValid && !strcmp(Cli::commands[i].name, argv[0])) {
                        cmdFound = true;

                        if (Cli::commands[i].cmdfunc(this, argc, argv) == CMD_SHOW_HELP) {
                            this->client->println();
                            this->showHelp(Cli::commands[i]);
                            this->client->println();
                        }

                        break;
                    }
                }

                if (!cmdFound) {
                    this->client->print("Command not found: ");
                    this->client->print(argv[0]);
                    this->client->println();
                }
            }
        } else {
            this->client->println("Colud not parse command.");
        }

        free(cmd);
    }
}
