#include <zneo.h>

/* December 22, 2008
 * Dan Eisenreich
 * 
 * A very simple program to blink ONE LED on the ZNEOCTK0100KIT
 * (thats the ZNEO contest kit).
 * The LED of choice is the upper left of the D1 array. 
 */


void delay(void);
void all_on(void); 
void all_off(void); 
void init_led(void); 
void one_on(void); 


void main() { 

    init_led();

    while(1) { 
        all_off();
        delay(); 
		delay();
        one_on(); 
        delay();
		delay();
    }
}


/* 
 * Hang out for a little while. 
 * This is NOT a good way to delay but its
 * simple enough so its ok for the time being.
 */

void delay(void)
{
    int i,j;
    for(i=0;i<0xff;i++) {
        for(j=0;j<0xff;j++) {
            // do nothing
        }
    }
}


/*
 * Configure the ports we will be using
 */

void init_led(void) { 
    PEDD = 0x00;        // Data Direction = OUTPUT
    PGDD = 0x00;
}


/*
 * Turn on just the upper left LED on the D1 array
 */

void one_on(void) {   
    PEOUT &= ~0x10;     // Clear bit 4 
                        // ~0x10 makes more sense to me than 0xEF does
    PEOUT |=  0x0F;     // Set bits 3..0
    PGOUT |=  0x01;     // Set bit 0
    PGOUT &= ~0x7E;     // Clear bit 6..1
                        // ~0x7E make more sense to me than 0x81

    // cycle PE7 (create a rising edge for the latch)
    PEOUT &= ~0x80;     // Clear bit 7
    PEOUT |= 0x80;      // Set bit 7
}



/*
 * Turn off ALL LEDs on all 4 arrays
 */

void all_off(void) { 
    PEOUT &= ~0x1F;     // set lower 5 bits low
    PGOUT &= ~0x7F;     // set lower 7 bits low

     // cycle PE7
    PEOUT &= ~0x80;     // clear bit 7
    PEOUT |= 0x80;      // set bit 7

    PGOUT &= ~0x80;     // cycle PG7
    PGOUT |= 0x80; 

    PEOUT &= ~0x20;     // cycle PE5
    PEOUT |= 0x20; 

    PEOUT &= ~0x40;     // cycle PE6
    PEOUT |= 0x40; 
}
