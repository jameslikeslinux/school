/*
 Chat  Server
 
 A simple server that distributes any incoming messages to all
 connected clients.  To use telnet to  your device's IP address and type.
 You can see the client's input in the serial monitor as well.
 Using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 10 August 2010
 by Tom Igoe
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include "Max3110.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1, 99);
IPAddress gateway(192,168,1, 1);
IPAddress subnet(255, 255, 255, 0);

// telnet defaults to port 23
EthernetServer server(22);
boolean gotAMessage = false; // whether or not you got a message from the client yet

void setup() {
  // initialize the ethernet device
  Ethernet.begin(mac, ip, gateway, subnet);
  // start listening for clients
  server.begin();
  // open the serial port
  Serial.begin(38400);
  ExternalSerial.begin(38400);
}

static unsigned long i = 0;
#define PROG 4

void loop() {
#if PROG == 0
    int b;
  // wait for a new client:
  EthernetClient client = server.available();
  
  // when the client sends the first byte, say hello:
//  if (client) {
//    if (!gotAMessage) {
//      ExternalSerial.println("We have a new client");
//      client.println("Hello, client!"); 
//      gotAMessage = true;
//    }

    while (client.connected()) {
        if ((b = client.read()) != -1) {
            ExternalSerial.write(b);
        }
    }
//  }
#elif PROG == 1
    int b;
    if ((b = Serial.read()) != -1) {
        ExternalSerial.write(b);
    }

    if ((b = ExternalSerial.read()) != -1) {
        Serial.write(b);
    }
#elif PROG == 2
    int b;
    if ((b = ExternalSerial.read()) != -1) {
        ExternalSerial.write(b);
    }
#else
    ExternalSerial.print("Hello, World! ");
    ExternalSerial.println(i++);
//    delay(1000);
#endif
}
