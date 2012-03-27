#ifndef CLI_H
#define CLI_H

#include <avr/pgmspace.h>
#include <stdlib.h>
#include <Ethernet.h>
#include "RingBuffer.h"

#define CLI_MAX_NUM_CMDS 10

#define CMD_SUCCESS 0
#define CMD_SHOW_HELP 1
#define CMD_ERROR 2

class Cli;

struct Cmd {
    int (*cmdfunc)(const Cli *cl, int argc, char *argv[]);
    char *name;
    prog_char *help;
    bool isValid;
};

class Cli {
    public:
        Cli(EthernetClient *client);
        static bool addCommand(char *name, prog_char *help, int (*cmdfunc)(const Cli *cl, int argc, char *argv[]));
        void go();
        EthernetClient * getClient() const;
        void showHelp() const;

    private:
        static bool tokenizeCommand(char *cmd, int *argc, char *argv[]);
        void processInput();
        void processCommands();
        void showHelp(const Cmd &command) const;

        EthernetClient *client;
        RingBuffer<char *> cmdqueue;
        static Cmd commands[CLI_MAX_NUM_CMDS];
        char *input;
        int read;
};

#endif
