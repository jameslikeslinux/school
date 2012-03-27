#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

class SettingsClass {
    public:
        SettingsClass();
        void save();
        int version;
        bool useDhcp;
        byte mac[6], ip[4], subnet[4], gateway[4];
};

extern SettingsClass Settings;

#endif
