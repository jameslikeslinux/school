#include "Settings.h"
#include "EEPROMAnything.h"

#define SETTINGS_VERSION 1

SettingsClass Settings;
        
SettingsClass::SettingsClass() {
    EEPROM_readAnything(0, *this);

    if (this->version != SETTINGS_VERSION) {
        this->version = SETTINGS_VERSION;
        this->mac = {0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed};
        this->ip = {192, 168, 1, 99};
        this->subnet = {255, 255, 255, 0};
        this->gateway = {192, 168, 1, 1};
        save();
    }
}

void SettingsClass::save() {
    EEPROM_writeAnything(0, *this);
}
