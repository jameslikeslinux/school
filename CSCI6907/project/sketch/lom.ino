#include <SPI.h>
#include "test.h"

const int INTERRUPT = 3;
const int SLAVE_SELECT = 4;

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

#define BAUD_7200 0x5
#define BAUD_9600 0xb
#define BAUD_19200 0xa
#define BAUD_57600 0x2
#define BAUD_115200 0x1

#define SERIAL_BUFFER_SIZE 64

struct ring_buffer {
    volatile uint8_t buffer[SERIAL_BUFFER_SIZE];
    volatile unsigned int head;
    volatile unsigned int tail;
};

static volatile ring_buffer rxbuffer = { { 0 }, 0, 0 };
static volatile ring_buffer txbuffer = { { 0 }, 0, 0 };
static volatile uint16_t configureWord = BAUD_9600 | ENABLE_RX_AVAIL_INT;

bool enqueue(volatile ring_buffer *buffer, uint8_t b) {
    unsigned int i = (buffer->head + 1) % SERIAL_BUFFER_SIZE;

    // if we should be storing the received character into the location
    // just before the tail (meaning that the head would advance to the
    // current location of the tail), we're about to overflow the buffer
    // and so we don't write the character or advance the head.
    if (i != buffer->tail) {
        buffer->buffer[buffer->head] = b;
        buffer->head = i;
        return true;
    }

    return false;
}

bool isempty(volatile ring_buffer *buffer) {
    return buffer->head == buffer->tail;
}

bool dequeue(volatile ring_buffer *buffer, uint8_t *b) {
    if (isempty(buffer))
        return false;

    *b = buffer->buffer[buffer->tail];
    buffer->tail = (buffer->tail + 1) % SERIAL_BUFFER_SIZE;
    return true;
}

void setup() {
    Serial.begin(9600);

    pinMode(SLAVE_SELECT, OUTPUT);
    digitalWrite(SLAVE_SELECT, HIGH);

    pinMode(INTERRUPT, INPUT);
    digitalWrite(INTERRUPT, HIGH);
    attachInterrupt(1, maxisr, FALLING);

    SPI.begin();
}

uint16_t maxtransfer(uint16_t word) {
    uint16_t receivedWord;
    
    digitalWrite(SLAVE_SELECT, LOW);
    receivedWord = ((uint16_t) SPI.transfer(word >> 8)) << 8;
    receivedWord |= SPI.transfer(word);
    digitalWrite(SLAVE_SELECT, HIGH);
    
    return receivedWord;
}

uint16_t maxconfigure() {
    uint16_t receivedWord,foo;

    do {
        maxtransfer(WRITE_CONF | configureWord);
        foo = maxtransfer(READ_CONF);
        receivedWord = foo & STRIP_RT;
    } while (configureWord != receivedWord);

    return foo;
}

bool maxgetbyte(uint8_t *b) {
    return dequeue(&rxbuffer, b);
}

bool maxputbyte(uint8_t b) {
    return enqueue(&txbuffer, b);
}

void maxflushtx() {
    noInterrupts();

    uint8_t b;
    uint16_t readword = maxtransfer(READ_DATA);

    if ((readword >> 15) & 1) {
        enqueue(&rxbuffer, readword);
    }

    while (((readword >> 14) & 1) && dequeue(&txbuffer, &b)) {
        readword = maxtransfer(WRITE_DATA | b);
        if ((readword >> 15) & 1) {
            enqueue(&rxbuffer, readword);
        }
    }

    interrupts();
}

void maxisr() {
    uint16_t readword = maxtransfer(READ_DATA);

    if ((readword >> 15) & 1) {
        enqueue(&rxbuffer, readword);
    }
}

void loop() {
    uint8_t b;

    if (maxgetbyte(&b)) {
        Serial.write(b);
    }

    if (Serial.available() > 0) {
        b = Serial.read();
        while (!maxputbyte(b)) {
            maxflushtx();
        }
    }

    if (!isempty(&txbuffer)) {
        maxflushtx();
    }
}
