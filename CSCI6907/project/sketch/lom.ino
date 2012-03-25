#include <SPI.h>
#include <Ethernet.h>
#include <avr/pgmspace.h>
#include "Max3110.h"
#include "Cli.h"

static byte mac[] = {0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed};
static IPAddress ip(192, 168, 1, 99);
static IPAddress gateway(192, 168, 1, 1);
static IPAddress subnet(255, 255, 255, 0);

// rlogin defaults to port 513
static EthernetServer server(513);
static EthernetClient client;

static prog_char *console_help PROGMEM = "console\n - Access the serial console of the system";
static int _console(int argc, char *argv[]) {
    char c;
    bool hashPressed;

    hashPressed = false;
    client.println("Press #. to exit console.");
    
    ExternalSerial.begin(9600);

    while (client.connected()) {
        if (client.available()) {
            c = client.read();

            if (!hashPressed && c == '#') {
                hashPressed = true;
            } else if (hashPressed && c == '.') {
                client.println();
                break;
            } else {
                hashPressed = false;
            }

            ExternalSerial.write(c);
        }

        if (ExternalSerial.available()) {
            c = ExternalSerial.read();
            client.write(c);
        }
    }
    
    ExternalSerial.end();

    return 0;
}

static prog_char *help_help PROGMEM = "help\n - Prints a list of available commands";
static int _help(int argc, char *argv[]) {
    client.println("This is the help");

    return 0;
}

static prog_char *exit_help PROGMEM = "exit\n - Disconnect from the LOM";
static int _exit(int argc, char *argv[]) {
    client.stop();
    return 0;
}

void setup() {
    // open the serial port for debugging
    Serial.begin(9600);
    
    // initialize the ethernet device
    Ethernet.begin(mac, ip, gateway, subnet);

    // start listening for clients
    server.begin();

    Cli::addCommand("console", console_help, _console);
    Cli::addCommand("help", help_help, _help);
    Cli::addCommand("exit", exit_help, _exit);
}

void loop() {
    char c;
    int count;

    if (client = server.available()) {
        count = 0;
        
        while (client.connected()) {
            if (client.available()) {
                c = client.read();

                if (c == 0) {
                    Serial.println();
                    if (++count == 4) {
                        client.write((uint8_t) 0);
                        break;
                    }
                } else {
                    Serial.print(c);
                }
            }
        }

        Serial.println("Connection established.");

        Cli cl(&client);
        cl.go();

        Serial.println("Connection ended.");
    }    
}
