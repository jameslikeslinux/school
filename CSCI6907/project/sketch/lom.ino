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
#define BAUD_38400 0x9
#define BAUD_57600 0x2
#define BAUD_115200 0x1

#define SERIAL_BUFFER_SIZE 64

struct ring_buffer {
    uint8_t buffer[SERIAL_BUFFER_SIZE];
    unsigned int head;
    unsigned int tail;
};

static volatile ring_buffer rxbuffer = {{0}, 0, 0};
static volatile ring_buffer txbuffer = {{0}, 0, 0};
static volatile uint16_t configureWord;

void setup() {
    Serial.begin(115200);

    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);

    pinMode(SLAVE_SELECT, OUTPUT);
    digitalWrite(SLAVE_SELECT, HIGH);

    pinMode(INTERRUPT, INPUT);
    digitalWrite(INTERRUPT, HIGH);
    attachInterrupt(1, maxisr, LOW);

    SPI.begin();

    configureWord = BAUD_115200 | ENABLE_RX_AVAIL_INT;
    maxconfigure();
}

inline bool enqueue(volatile ring_buffer *buffer, uint8_t b) {
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

inline bool isempty(volatile ring_buffer *buffer) {
    return buffer->head == buffer->tail;
}

inline bool isfull(volatile ring_buffer *buffer) {
    unsigned int i = (buffer->head + 1) % SERIAL_BUFFER_SIZE;
    return i == buffer->tail;
}

inline bool dequeue(volatile ring_buffer *buffer, uint8_t *b) {
    if (isempty(buffer))
        return false;

    *b = buffer->buffer[buffer->tail];
    buffer->tail = (buffer->tail + 1) % SERIAL_BUFFER_SIZE;
    return true;
}

inline uint16_t maxtransfer(uint16_t word) {
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
    bool ret;
    uint8_t sregsaved;

    // disable interrupts so my buffers don't get clobbered
    sregsaved = SREG;
    noInterrupts();

    // if the rxbuffer is empty, it's possible I could have missed an
    // interrupt, so why not go out and get some data
    if (isempty(&rxbuffer)) {
        maxisr();
    }

    // if rxbuffer was empty and the UART still didn't receive any data, this
    // dequeue operation will fail, and that's okay...just communicate it to
    // the user with the return code
    ret = dequeue(&rxbuffer, b);

    // restore interrupts
    SREG = sregsaved;

    return ret;
}

bool maxputbyte(uint8_t b) {
    uint8_t sregsaved;

    // disable interrupts so my buffers don't get clobbered
    sregsaved = SREG;
    noInterrupts();

    // if my txbuffer is full I can't add the byte to it, and I have no chance
    // of it clearing out because the rxbuffer needs to have room to do so
    // so I'm just going to refuse this byte
    if (isfull(&rxbuffer) && isfull(&txbuffer)) {
        // restore interrupts and return
        SREG = sregsaved;
        return false;
    }

    // if just the txbuffer is full I might have missed an interrupt
    // so just try forcing some transfers until there room in the buffer
    while (!enqueue(&txbuffer, b)) {
        maxisr();
    }

    // now my txbuffer has data so I want to be interrupted when the UART
    // is able to accept more transfers
    configureWord |= ENABLE_TX_BUF_INT;
    maxconfigure();

    // that said, the interrupt seems to only trigger when the hw transmit
    // buffer *becomes* empty--if it's already empty when the interrupt
    // is enabled, it doesn't fire...so I have to manually call the interrupt
    // or bits can get stuck sometimes
    //
    // from the datasheet:
    //   IRQ is asserted low if TM = 1 and the transmit buffer becomes empty.
    // XXX: This may not be the most efficient thing
    //maxisr();
    
    // restore interrupts
    SREG = sregsaved;
   
    return true;
}

/* XXX: Evaluate the need for this function
bool maxavailable() {
    bool ret;
    uint8_t sregsaved;

    // disable interrupts so my buffers don't get clobbered
    sregsaved = SREG;
    noInterrupts();

    // if the rxbuffer is empty, it's possible I could have missed an
    // interrupt, so why not go out and get some data
    if (isempty(&rxbuffer)) {
        maxisr();
    }

    ret = !isempty(&rxbuffer);

    // restore interrupts
    SREG = sregsaved;

    return ret;
}
*/

void maxisr() {
    uint8_t b;
    uint16_t readword;

    do {
        // if the receive buffer is full, there's nothing else we can do
        // we can continue to let the hardware FIFO fill and hope the user
        // clears out the rxbuffer before bytes start needing to be dropped
        if (isfull(&rxbuffer)) {
            return;
        }

        readword = maxtransfer(READ_DATA);

        if ((readword >> 15) & 1) {
            // we got data from the UART and I know I can enqueue it because
            // I checked above and nothing between then and now could have
            // added anything else to the rxbuffer
            enqueue(&rxbuffer, readword);
        }

        if (((readword >> 14) & 1) && !isempty(&txbuffer)) {
            // if the UART is able to accept tx, but the receive buffer is now full
            // then, again we can't do anything because a receive can come back with
            // the transmit request and we wouldn't have any place to put it.
            if (isfull(&rxbuffer)) {
                return;
            }

            // this dequeue must return data because I checked above and nothing
            // else could have modified txbuffer between then and now
            dequeue(&txbuffer, &b);

            readword = maxtransfer(WRITE_DATA | b);
            if ((readword >> 15) & 1) {
                // we got data from the UART and I know I can enqueue it because
                // I checked above and nothing between then and now could have
                // added anything else to the rxbuffer
                enqueue(&rxbuffer, readword);
            }
            
            // if that was the last byte in queue to be transferred
            // then I don't need to be interrupted about the tx buffer anymore
            if (isempty(&txbuffer)) {
                configureWord &= ~ENABLE_TX_BUF_INT;
                maxconfigure();
            }
        }

    // and repeat the above as long as the interrupt condition remains and
    // I'm able to accept new data
    } while (digitalRead(INTERRUPT) == LOW);
}

void loop() {
    int avail;
    uint8_t b, c;

    // if there's data in the rxbuffer
    if (maxgetbyte(&b)) {
        // then write a byte of it out the debug serial port
        Serial.write(b);
    }

    // if there's data available in the debug serial port
    while ((avail = Serial.available()) > 0) {
        b = Serial.read();

        // then try to write a byte of it out the maxim serial port
        while (!maxputbyte(b)) {
            // if I can't (because all the buffers are full), then try a read
            if (maxgetbyte(&c)) {
                Serial.write(c);
            }
        }
    }
}
