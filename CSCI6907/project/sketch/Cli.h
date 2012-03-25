#ifndef CLI_H
#define CLI_H

#include <avr/pgmspace.h>
#include <stdlib.h>
#include <Ethernet.h>
#include "RingBuffer.h"

#define CLI_MAX_NUM_CMDS 8

struct Cmd {
    int (*cmdfunc)(int argc, char *argv[]);
    char *name;
    prog_char *help;
    bool isValid;
};

class Cli {
    public:
        Cli(EthernetClient *client);
        void go();
        static bool addCommand(char *name, prog_char *help, int (*cmdfunc)(int argc, char *argv[]));

    private:
        void processInput();
        void processCommands();

        EthernetClient *client;
        RingBuffer<char *> cmdqueue;
        static Cmd commands[CLI_MAX_NUM_CMDS];
        char *input;
        int read;
};

#endif
