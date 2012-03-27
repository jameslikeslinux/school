#include <Arduino.h>
#include <SPI.h>
#include "Max3110.h"

// commands
#define WRITE_CONF ((1 << 15) | (1 << 14))
#define READ_CONF (1 << 14)
#define READ_DATA 0
#define WRITE_DATA (1 << 15)

// for WRITE_CONF command
#define DISABLE_FIFO (1 << 13)
#define SOFTWARE_SHUTDOWN (1 << 12)
#define ENABLE_TX_BUF_INT (1 << 11)
#define ENABLE_RX_AVAIL_INT (1 << 10)
#define ENABLE_PARITY_BIT_INT (1 << 9)
#define ENABLE_FRAME_ERR_INT (1 << 8)
#define ENABLE_IRDA_MODE (1 << 7)
#define TWO_STOPS (1 << 6)
#define ENABLE_PARITY (1 << 5)
#define SEVEN_BIT (1 << 4)

// for WRITE_DATA command
#define TRANSMIT_DISABLE (1 << 10)

// returned from WRITE_DATA and READ_DATA commands
#define READY_TO_RECEIVE (1 << 9)

// returned from every command
#define DATA_RECEIVED (1 << 15)
#define TRANSMIT_BUFFER_EMPTY (1 << 14)
#define STRIP_RT (~(DATA_RECEIVED | TRANSMIT_BUFFER_EMPTY))

static const int MAX3110_NUM_BAUDS = 16;
static const unsigned long MAX3110_BAUDS[] = {
    230400,
    115200,
    57600,
    28800,
    14400,
    7200,
    3600,
    1800,
    76800,
    38400,
    19200,
    9600,
    4800,
    2400,
    1200,
    600
};

Max3110 ExternalSerial(4, 5, 3, 1);

static const int MAX3110_NUM_INSTANCES = 1;
static Max3110 *MAX3110_INSTANCES[] = {&ExternalSerial};

void max3110ISR() {
    // can't process interrupt if something else is talking on SPI bus
    // XXX: SS pin shouldn't be hardcoded
    if (digitalRead(10) == LOW) {
        return;
    }

    for (int i = 0; i < MAX3110_NUM_INSTANCES; i++) {
        MAX3110_INSTANCES[i]->receiveData();
    }
}

Max3110::Max3110(uint8_t ssPin, uint8_t shutdownPin, uint8_t interruptPin, uint8_t interruptNum) : rxBuffer(64) {
    this->ssPin = ssPin;
    this->shutdownPin = shutdownPin;
    this->interruptPin = interruptPin;
    this->configurationWord = 0;
    this->rtrState = 0;

    // setup slave select pin
    pinMode(ssPin, OUTPUT);
    digitalWrite(ssPin, HIGH);

    // setup SPI pins
    SPI.begin();

    // setup interrupt
    pinMode(interruptPin, INPUT);
    digitalWrite(interruptPin, HIGH);
    attachInterrupt(interruptNum, max3110ISR, LOW);

    // enter shutdown state
    end();
}
        
void Max3110::begin(unsigned long baud) {
    int baudConfiguration = -1;

    for (int i = 0; i < MAX3110_NUM_BAUDS && baudConfiguration == -1; i++) {
        if (baud == MAX3110_BAUDS[i]) {
            baudConfiguration = i;
        }
    }

    if (baudConfiguration == -1) {
        return;
    }

    // start transceivers
    digitalWrite(this->shutdownPin, HIGH);

    // set baud rate, enable rx interrupts, and exit software shutdown
    this->configurationWord |= baudConfiguration;
    this->configurationWord |= ENABLE_RX_AVAIL_INT;
    this->configurationWord &= ~SOFTWARE_SHUTDOWN;
    configure();

    // for now, just assert ready to receive
    //this->rtrState |= READY_TO_RECEIVE;
    //transfer(WRITE_DATA | TRANSMIT_DISABLE | this->rtrState);
}

void Max3110::end() {
    // enter software shutdown
    this->configurationWord |= SOFTWARE_SHUTDOWN;
    configure();

    // shutdown transceivers
    digitalWrite(this->shutdownPin, LOW);

    // clear buffers
    this->rxBuffer.clear();
}

size_t Max3110::write(uint8_t b) {
    uint8_t sregsaved;
    uint16_t readword;

    // disable interrupts so my buffers don't get clobbered
    sregsaved = SREG;
    noInterrupts();

    if (this->rxBuffer.isFull()) {
        // restore interrupts and return
        SREG = sregsaved;
        return 0;
    }

    // wait until the other host is ready to receive and the transmit buffer is empty
    //do {
    //    readword = transfer(WRITE_DATA | TRANSMIT_DISABLE | this->rtrState);
    //} while (!((readword & READY_TO_RECEIVE) && (readword & TRANSMIT_BUFFER_EMPTY)));
    do {
        readword = transfer(WRITE_DATA | TRANSMIT_DISABLE);
    } while (!(readword & TRANSMIT_BUFFER_EMPTY));
 
    //readword = transfer(WRITE_DATA | b | this->rtrState);
    readword = transfer(WRITE_DATA | b);
    
    // store any received data that might have come back
    if (readword & DATA_RECEIVED) {
        this->rxBuffer.enqueue(readword);
    }

    // restore interrupts
    SREG = sregsaved;

    return 1;
}

int Max3110::available() {
    int ret;
    uint8_t sregsaved;

    // disable interrupts so my buffers don't get clobbered
    sregsaved = SREG;
    noInterrupts();

    // if the rxbuffer is empty, it's possible I could have missed an
    // interrupt, so why not go out and get some data
    if (this->rxBuffer.isEmpty()) {
        receiveData();
    }

    ret = this->rxBuffer.available();

    // restore interrupts
    SREG = sregsaved;

    return ret;
}

int Max3110::read() {
    int ret;
    uint8_t sregsaved, b;

    // disable interrupts so my buffers don't get clobbered
    sregsaved = SREG;
    noInterrupts();

    // if the rxbuffer is empty, it's possible I could have missed an
    // interrupt, so why not go out and get some data
    if (this->rxBuffer.isEmpty()) {
        receiveData();
    }

    // if rxbuffer was empty and the UART still didn't receive any data, this
    // dequeue operation will fail, and that's okay...just communicate it to
    // the user with the return code
    if (this->rxBuffer.dequeue(b)) {
        ret = b;
    } else {
        ret = -1;
    }

    // restore interrupts
    SREG = sregsaved;

    return ret;
}

int Max3110::peek() {
    int ret;
    uint8_t sregsaved, b;

    // disable interrupts so my buffers don't get clobbered
    sregsaved = SREG;
    noInterrupts();

    // if the rxbuffer is empty, it's possible I could have missed an
    // interrupt, so why not go out and get some data
    if (this->rxBuffer.isEmpty()) {
        receiveData();
    }

    // if rxbuffer was empty and the UART still didn't receive any data, this
    // dequeue operation will fail, and that's okay...just communicate it to
    // the user with the return code
    if (this->rxBuffer.peek(b)) {
        ret = b;
    } else {
        ret = -1;
    }

    // restore interrupts
    SREG = sregsaved;

    return ret;
}

void Max3110::flush() {
    // do nothing...there is no txbuffer to flush
}

uint16_t Max3110::transfer(uint16_t word) {
    uint16_t receivedWord;
  
    digitalWrite(this->ssPin, LOW);
    receivedWord = ((uint16_t) SPI.transfer(word >> 8)) << 8;
    receivedWord |= SPI.transfer(word);
    digitalWrite(this->ssPin, HIGH);
    
    return receivedWord;
}

/*
 * Stubs for hardware flow control...doesn't work perfectly...timing seems to be critical
 *

uint16_t Max3110::transfer(uint16_t word) {
    uint16_t receivedWord;
 
    receivedWord = transfer1(word);
    if (receivedWord & DATA_RECEIVED && this->rtrState & READY_TO_RECEIVE) {
        this->rtrState &= ~READY_TO_RECEIVE;
        transfer1(WRITE_DATA | TRANSMIT_DISABLE | this->rtrState);
    } else if ((!(receivedWord & DATA_RECEIVED)) && (!(this->rtrState & READY_TO_RECEIVE)) && this->rxBuffer.isEmpty()) {
        this->rtrState |= READY_TO_RECEIVE;
        transfer1(WRITE_DATA | TRANSMIT_DISABLE | this->rtrState);
    }
    
    return receivedWord;
}
*/

void Max3110::configure() {
    uint16_t receivedWord;

    do {
        transfer(WRITE_CONF | this->configurationWord);
        receivedWord = transfer(READ_CONF) & STRIP_RT;
    } while (this->configurationWord != receivedWord);
}

void Max3110::receiveData() {
    uint16_t readword;
    
    while (true) {
        if (this->rxBuffer.isFull()) {
            return;
        }
    
        readword = transfer(READ_DATA);

        if (readword & DATA_RECEIVED) {
            // we got data from the UART and I know I can enqueue it because
            // I checked above and nothing between then and now could have
            // added anything else to the rxbuffer
            this->rxBuffer.enqueue(readword);
        } else {
            break;
        }
    }
}
