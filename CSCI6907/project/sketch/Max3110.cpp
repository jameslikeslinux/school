#include <Arduino.h>
#include <SPI.h>
#include "Max3110.h"

#define STRIP_RT (~((1 << 15) | (1 << 14)))
#define WRITE_CONF ((1 << 15) | (1 << 14))
#define READ_CONF (1 << 14)
#define READ_DATA 0
#define WRITE_DATA (1 << 15)
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

Max3110 ExternalSerial(4, 3, 1);

static const int MAX3110_NUM_INSTANCES = 1;
static Max3110 *MAX3110_INSTANCES[] = {&ExternalSerial};

void max3110ISR() {
    if (digitalRead(10) == LOW) {
        return;
    }

    for (int i = 0; i < MAX3110_NUM_INSTANCES; i++) {
//        if (digitalRead(MAX3110_INSTANCES[i]->interruptPin) == LOW) {
            MAX3110_INSTANCES[i]->sendAndReceiveData();
//        }
    }
}

Max3110::Max3110(uint8_t ssPin, uint8_t interruptPin, uint8_t interruptNum) : rxBuffer(64), txBuffer(64) {
    this->ssPin = ssPin;
    this->interruptPin = interruptPin;
    this->configurationWord = 0;

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

    // XXX: start transceivers

    // set baud rate, enable rx interrupts, and exit software shutdown
    this->configurationWord |= baudConfiguration;
    this->configurationWord |= ENABLE_RX_AVAIL_INT;
    this->configurationWord &= ~SOFTWARE_SHUTDOWN;
    configure();

    // XXX: How much do i need to delay?
//    delay(1000);
}

void Max3110::end() {
    // enter software shutdown
    this->configurationWord |= SOFTWARE_SHUTDOWN;
    configure();

    // XXX: shutdown transceivers

    // clear buffers
    this->rxBuffer.clear();
    this->txBuffer.clear();
}

size_t Max3110::write(uint8_t b) {
    uint8_t sregsaved;
    uint16_t readword;
    int i = 0;

    // disable interrupts so my buffers don't get clobbered
    sregsaved = SREG;
    noInterrupts();

    while (!((transfer(READ_CONF) >> 14) & 1));
 
    readword = transfer(WRITE_DATA | b);
    
    // store any received data that might have come back
    if ((readword >> 15) & 1) {
        this->rxBuffer.enqueue(readword);
    }

    // XXX: big hack
    //delay(1000);
/*
    // if my txbuffer is full I can't add the byte to it, and I have no chance
    // of it clearing out because the rxbuffer needs to have room to do so
    // so I'm just going to refuse this byte
    if (this->rxBuffer.isFull() && this->txBuffer.isFull()) {
        // restore interrupts and return
        SREG = sregsaved;
        return 0;
    }

    // if just the txbuffer is full I might have missed an interrupt
    // so just try forcing some transfers until there room in the buffer
    while (!this->txBuffer.enqueue(b)) {
        sendAndReceiveData();
    }

    // now my txbuffer has data so I want to be interrupted when the UART
    // is able to accept more transfers
    this->configurationWord &= ~ENABLE_RX_AVAIL_INT;
    this->configurationWord |= ENABLE_TX_BUF_INT;
    configure();
   
    // that said, the interrupt seems to only trigger when the hw transmit
    // buffer *becomes* empty--if it's already empty when the interrupt
    // is enabled, it doesn't fire...so I have to manually call the interrupt
    // or bits can get stuck sometimes
    //
    // from the datasheet:
    //   IRQ is asserted low if TM = 1 and the transmit buffer becomes empty.
    // XXX: This may not be the most efficient thing
    //sendAndReceiveData();
*/
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
        sendAndReceiveData();
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
        sendAndReceiveData();
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
        sendAndReceiveData();
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
    uint8_t sregsaved;

    // disable interrupts so my buffers don't get clobbered
    sregsaved = SREG;
    noInterrupts();

    // force sending data until txBuffer is empty or rxBuffer is full
    while (!this->rxBuffer.isFull() && !this->txBuffer.isEmpty()) {
        sendAndReceiveData();
    }
    
    // restore interrupts
    SREG = sregsaved;
}
        
uint16_t Max3110::transfer(uint16_t word) {
    uint16_t receivedWord;
  
    digitalWrite(this->ssPin, LOW);
    receivedWord = ((uint16_t) SPI.transfer(word >> 8)) << 8;
    receivedWord |= SPI.transfer(word);
    digitalWrite(this->ssPin, HIGH);
    
    return receivedWord;
}

void Max3110::configure() {
    uint16_t receivedWord;

    do {
        transfer(WRITE_CONF | this->configurationWord);
        receivedWord = transfer(READ_CONF) & STRIP_RT;
    } while (this->configurationWord != receivedWord);
}

void Max3110::sendAndReceiveData() {
    uint8_t b;
    uint16_t readword;

    if (this->rxBuffer.isFull()) {
        return;
    }

    readword = transfer(READ_DATA);

    if ((readword >> 15) & 1) {
        // we got data from the UART and I know I can enqueue it because
        // I checked above and nothing between then and now could have
        // added anything else to the rxbuffer
        this->rxBuffer.enqueue(readword);
    }
/*
    //do {
        // if the receive buffer is full, there's nothing else we can do
        // we can continue to let the hardware FIFO fill and hope the user
        // clears out the rxbuffer before bytes start needing to be dropped
        if (this->rxBuffer.isFull()) {
            return;
        }

        readword = transfer(READ_DATA);

        if ((readword >> 15) & 1) {
            // we got data from the UART and I know I can enqueue it because
            // I checked above and nothing between then and now could have
            // added anything else to the rxbuffer
            this->rxBuffer.enqueue(readword);
        }

        if (((readword >> 14) & 1) && !this->txBuffer.isEmpty()) {
            // if the UART is able to accept tx, but the receive buffer is now full
            // then, again we can't do anything because a receive can come back with
            // the transmit request and we wouldn't have any place to put it.
            if (this->rxBuffer.isFull()) {
                return;
            }

            // this dequeue must return data because I checked above and nothing
            // else could have modified txbuffer between then and now
            this->txBuffer.dequeue(b);

            readword = transfer(WRITE_DATA | b);
            if ((readword >> 15) & 1) {
                // we got data from the UART and I know I can enqueue it because
                // I checked above and nothing between then and now could have
                // added anything else to the rxbuffer
                this->rxBuffer.enqueue(readword);
            }
            
            // if that was the last byte in queue to be transferred
            // then I don't need to be interrupted about the tx buffer anymore
            if (this->txBuffer.isEmpty()) {
                this->configurationWord &= ~ENABLE_TX_BUF_INT;
                this->configurationWord |= ENABLE_RX_AVAIL_INT;
                configure();
            }
        }

    // and repeat the above as long as the interrupt condition remains and
    // I'm able to accept new data
    //} while (digitalRead(this->interruptPin) == LOW);
*/
}
