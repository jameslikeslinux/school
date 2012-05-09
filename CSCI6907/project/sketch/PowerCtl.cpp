#include <Arduino.h>
#include "PowerCtl.h"

#define POWERCTL_TIMEOUT 10000  // 10 seconds

PowerCtlClass PowerCtl(6, 9);

PowerCtlClass::PowerCtlClass(uint8_t ledPin, uint8_t powerPin) {
    this->ledPin = ledPin;
    this->powerPin = powerPin;

    pinMode(ledPin, INPUT);
    digitalWrite(ledPin, HIGH);

    pinMode(powerPin, OUTPUT);
}

bool PowerCtlClass::isSystemOn() {
    return !digitalRead(this->ledPin);
}

bool PowerCtlClass::powerOn() {
    unsigned long start;

    if (isSystemOn()) {
        return true;
    }

    start = millis();
    digitalWrite(this->powerPin, HIGH);
    while (!isSystemOn()) {
        if (millis() - start > POWERCTL_TIMEOUT) {
            /* timeout */
            break;
        }
    }
    digitalWrite(this->powerPin, LOW);

    if (!isSystemOn()) {
        return false;
    }

    return true;
}

bool PowerCtlClass::powerOff() {
    unsigned long start;

    if (!isSystemOn()) {
        return true;
    }
   
    start = millis(); 
    digitalWrite(this->powerPin, HIGH);
    while (isSystemOn()) {
        if (millis() - start > POWERCTL_TIMEOUT) {
            /* timeout */
            break;
        }
    }
    digitalWrite(this->powerPin, LOW);

    if (isSystemOn()) {
        return false;
    }

    return true;
}

bool PowerCtlClass::reset() {
    if (powerOff()) {
        if (powerOn()) {
            return true;
        }
    }

    return false;
}
