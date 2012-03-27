#include <EEPROM.h>
#include <SPI.h>
#include <Ethernet.h>
#include <avr/pgmspace.h>
#include "Max3110.h"
#include "Cli.h"
#include "Settings.h"
#include "Utils.h"
#include "PowerCtl.h"

// rlogin defaults to port 513
static EthernetServer server(513);

static void configureNetwork() {
    if (!Settings.useDhcp || !Ethernet.begin(Settings.mac)) {
        Ethernet.begin(Settings.mac, Settings.ip, Settings.gateway, Settings.subnet);
    }
}

static prog_char *status_help PROGMEM = "status\r\n - Show the system state";
static int _status(const Cli *cl, int argc, char *argv[]) {
    EthernetClient *client;

    client = cl->getClient();
    client->print("The system is ");
    client->println(PowerCtl.isSystemOn() ? "on." : "off.");

    return CMD_SUCCESS;
}

static prog_char *poweron_help PROGMEM = "poweron\r\n - Power the system on";
static int _poweron(const Cli *cl, int argc, char *argv[]) {
    EthernetClient *client;

    client = cl->getClient();

    if (PowerCtl.isSystemOn()) {
        client->println(F("The system is already powered on."));
    } else {
        client->println(F("Powering on..."));
        if (!PowerCtl.powerOn()) {
            client->println(F("Timed out while powering the system on."));
        }
    }

    return CMD_SUCCESS;
}

static prog_char *poweroff_help PROGMEM = "poweroff\r\n - Forcefully turn the system off";
static int _poweroff(const Cli *cl, int argc, char *argv[]) {
    EthernetClient *client;

    client = cl->getClient();

    if (!PowerCtl.isSystemOn()) {
        client->println(F("The system is already turned off."));
    } else {
        client->println(F("Turning off..."));
        if (!PowerCtl.powerOff()) {
            client->println(F("Timed out while turning the system off."));
        }
    }

    return CMD_SUCCESS;
}

static prog_char *reset_help PROGMEM = "reset\r\n - Hard-reset the system";
static int _reset(const Cli *cl, int argc, char *argv[]) {
    EthernetClient *client;

    client = cl->getClient();
    client->println(F("Resetting..."));
    if (!PowerCtl.reset()) {
        client->println(F("Timed out while reseting the system."));
    }

    return CMD_SUCCESS;
}

static prog_char *console_help PROGMEM = "console\r\n - Access the serial console of the system";
static int _console(const Cli *cl, int argc, char *argv[]) {
    EthernetClient *client;
    char c;
    bool hashPressed;

    client = cl->getClient();

    hashPressed = false;
    client->println("Press #. to exit console.");
    
    ExternalSerial.begin(9600);

    while (client->connected()) {
        if (client->available()) {
            c = client->read();

            if (!hashPressed && c == '#') {
                hashPressed = true;
            } else if (hashPressed && c == '.') {
                client->println();
                break;
            } else {
                hashPressed = false;
            }

            ExternalSerial.write(c);
        }

        if (ExternalSerial.available()) {
            c = ExternalSerial.read();
            client->write(c);
        }
    }
    
    ExternalSerial.end();

    return CMD_SUCCESS;
}

static prog_char *set_help PROGMEM = "set <name> <value>\r\n - Sets the specified setting (but does not commit the change)";
static int _set(const Cli *cl, int argc, char *argv[]) {
    EthernetClient *client;
    byte *value;

    if (argc != 3) {
        return CMD_SHOW_HELP;
    }

    client = cl->getClient();

    if (!strcmp(argv[1], "dhcp")) {
        if (!strcmp(argv[2], "true")) {
            Settings.useDhcp = true;
        } else if (!strcmp(argv[2], "false")) {
            Settings.useDhcp = false;
        } else {
            client->println("dhcp must be \"true\" or \"false\".");
            return CMD_ERROR;
        }
    } else if (!strcmp(argv[1], "mac")) {
        if (value = stringToMacAddress(argv[2])) {
            for (int i = 0; i < 6; i++) {
                Settings.mac[i] = value[i];
            } 
        } else {
            client->println("Invalid MAC address.");
            return CMD_ERROR;
        }
    } else if (!strcmp(argv[1], "ip")) {
        if (value = stringToIpAddress(argv[2])) {
            for (int i = 0; i < 4; i++) {
                Settings.ip[i] = value[i];
            }
        } else {
            client->println("Invalid IP address.");
            return CMD_ERROR;
        }
    } else if (!strcmp(argv[1], "gateway")) {
        if (value = stringToIpAddress(argv[2])) {
            for (int i = 0; i < 4; i++) {
                Settings.gateway[i] = value[i];
            }
        } else {
            client->println("Invalid IP address.");
            return CMD_ERROR;
        }
    } else if (!strcmp(argv[1], "subnet")) {
        if (value = stringToIpAddress(argv[2])) {
            for (int i = 0; i < 4; i++) {
                Settings.subnet[i] = value[i];
            }
        } else {
            client->println("Invalid IP address.");
            return CMD_ERROR;
        }
    } else {
        client->print("Invalid setting: ");
        client->print(argv[1]);
        client->println();
        return CMD_ERROR;
    }

    return CMD_SUCCESS;
}

static prog_char *get_help PROGMEM = "get [name]\r\n - Lists the values of the settings";
static int _get(const Cli *cl, int argc, char *argv[]) {
    EthernetClient *client;
    byte *value;

    if (argc > 2) {
        return CMD_SHOW_HELP;
    }

    client = cl->getClient();

    if (argc == 1) {
        client->print("dhcp: ");
        client->println(Settings.useDhcp ? "true": "false");
        client->print("mac: ");
        client->println(macAddressToString(Settings.mac));
        client->print("ip: ");
        client->println(ipAddressToString(Settings.ip));
        client->print("gateway: ");
        client->println(ipAddressToString(Settings.gateway));
        client->print("subnet: ");
        client->println(ipAddressToString(Settings.subnet));
    } else if (argc == 2) {
        if (!strcmp(argv[1], "dhcp")) {
            client->print("dhcp: ");
            client->println(Settings.useDhcp ? "true": "false");
        } else if (!strcmp(argv[1], "mac")) {
            client->print("mac: ");
            client->println(macAddressToString(Settings.mac));
        } else if (!strcmp(argv[1], "ip")) {
            client->print("ip: ");
            client->println(ipAddressToString(Settings.ip));
        } else if (!strcmp(argv[1], "gateway")) {
            client->print("gateway: ");
            client->println(ipAddressToString(Settings.gateway));
        } else if (!strcmp(argv[1], "subnet")) {
            client->print("subnet: ");
            client->println(ipAddressToString(Settings.subnet));
        } else {
            client->print("Invalid setting: ");
            client->print(argv[1]);
            client->println();
            return CMD_ERROR;
        }
    }

    return CMD_SUCCESS;
}

static prog_char *commit_help PROGMEM = "commit\r\n - Applies and saves network settings";
static int _commit(const Cli *cl, int argc, char *argv[]) {
    EthernetClient *client;

    client = cl->getClient();
    client->println("Reconfiguring networking settings...");
    client->stop();

    Settings.save();
    configureNetwork();
}

static prog_char *help_help PROGMEM = "help\r\n - Prints a list of available commands";
static int _help(const Cli *cl, int argc, char *argv[]) {
    cl->showHelp();
    return CMD_SUCCESS;
}

static prog_char *exit_help PROGMEM = "exit\r\n - Disconnect from the LOM";
static int _exit(const Cli *cl, int argc, char *argv[]) {
    EthernetClient *client;

    client = cl->getClient();
    client->stop();

    return CMD_SUCCESS;
}

void setup() {
    // open the serial port for debugging
    Serial.begin(9600);
    
    // initialize the ethernet device
    configureNetwork();

    // start listening for clients
    server.begin();

    Cli::addCommand("status", status_help, _status);
    Cli::addCommand("poweron", poweron_help, _poweron);
    Cli::addCommand("poweroff", poweroff_help, _poweroff);
    Cli::addCommand("reset", reset_help, _reset);
    Cli::addCommand("console", console_help, _console);
    Cli::addCommand("set", set_help, _set);
    Cli::addCommand("get", get_help, _get);
    Cli::addCommand("commit", commit_help, _commit);
    Cli::addCommand("help", help_help, _help);
    Cli::addCommand("exit", exit_help, _exit);
}

void loop() {
    char c;
    int count;
    EthernetClient client;

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
