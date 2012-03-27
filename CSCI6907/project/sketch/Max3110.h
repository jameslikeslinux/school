#ifndef MAX3110_H
#define MAX3110_H

#include <Stream.h>
#include "RingBuffer.h"

class Max3110 : public Stream {
    public:
        Max3110(uint8_t ssPin, uint8_t shutdownPin, uint8_t interruptPin, uint8_t interruptNum);
        void begin(unsigned long baud);
        void end();
        virtual size_t write(uint8_t b);
        virtual int available();
        virtual int read();
        virtual int peek();
        virtual void flush();

    private:
        uint16_t transfer(uint16_t word);
        void configure();
        void receiveData();
        friend void max3110ISR();

        uint8_t ssPin, shutdownPin, interruptPin;
        volatile RingBuffer<uint8_t> rxBuffer;
        uint16_t configurationWord;
        volatile uint16_t rtrState;
};

extern Max3110 ExternalSerial;

#endif
