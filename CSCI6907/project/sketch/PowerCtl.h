#ifndef POWERCTL_H
#define POWERCTL_H

class PowerCtlClass {
    public:
        PowerCtlClass(uint8_t ledPin, uint8_t powerPin);
        bool isSystemOn();
        bool powerOn();
        bool powerOff();
        bool reset();

    private:
        uint8_t ledPin, powerPin;
};

extern PowerCtlClass PowerCtl;

#endif
