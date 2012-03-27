#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

void deleteChar(char *str);
int hexStringToInteger(char *str);
byte * stringToMacAddress(char *str);
byte * stringToIpAddress(char *str);
char * macAddressToString(byte *mac);
char * ipAddressToString(byte *ip);

#endif
