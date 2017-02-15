/*
Released under Apache V2 license by www.thecoderscorner.com (Dave Cherry).
This library will drive an LED 7 segment display by interrupt.
*/

/**
This library can control an LCD display with a variable number of segments
It drives the display by timer interrupt, so the loop() is kept clean.
Future expansion may provide for a shift register implementation for devices
with smaller pinouts. It defaults to 4 digits, for more or less digits define
LED_NUM_DIGITS differently. Simple examples in the libraries example folder.
*/

#ifndef TCC_LED_DISPLAY_H
#define TCC_LED_DISPLAY_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>

// we default to 4 digits if no other value is set
#ifndef LED_NUM_DIGITS
#define LED_NUM_DIGITS 4
#endif

#define LCD_BITS 8

/** utility method for doing power of 10 on integers */
unsigned int fastPow10(unsigned int dp);

class LEDDisplay {
public:
	/** Create an LCD Display instance to control a connected display */
	LEDDisplay(char pinStart, boolean commonHigh);
	~LEDDisplay();

	static LEDDisplay* instance() {return theDisplay;}

	/** Starts a timer interrupt that will keep the display updated */
	void startInterrupt();

	/** Set the display to a decimal value */
	void setValueDec(unsigned int newValue, boolean zeroPad=true);

	/** Set the display to hex value */
	void setValueHex(unsigned int newValue);

	/** Set the display to a floating point value using the specified
        decimalPlaces */
	void setValueFloat(float newValue, unsigned int decimalPlaces, boolean zeroPad=false);

	/** Set digit at position to newValue */
	void setValueRaw(char position, char newValue, boolean dpOn = false);

	/** Advanced numeric digit facility, writes a number in a given base on the
	    display between segments start and end. */
    void setNumeric(unsigned int value, unsigned int base, char start, char places, boolean zeroPad);

    void isr_display();
private:
    /** The instance variable as created for the ISR */
    static LEDDisplay* theDisplay;

    /** The start of the pinouts for the display */
    char pinStart;

    /** Sets if the display needs common high or low */
    boolean commonHigh;

	/** memory representation of each item for display */
	volatile char digits[LED_NUM_DIGITS];

	/** the current digit being rendered */
	volatile int currentDigit;

	/** The actual character bit patterns for display */
	const int charMap[30] = {
// bits       6543210
// digits     abcdefg
            0b1111110,//0 0x00
            0b0110000,//1 0x01
            0b1101101,//2 0x02
            0b1111001,//3 0x03
            0b0110011,//4 0x04
            0b1011011,//5 0x05
            0b1011111,//6 0x06
            0b1110000,//7 0x07
            0b1111111,//8 0x08
            0b1111011,//9 0x09
            0b1110111,//a 0x0A
            0b0011111,//b 0x0B
            0b1001110,//c 0x0C
            0b0111101,//d 0x0D
            0b1101111,//e 0x0E
            0b1000111,//f 0x0F
            0b1111011,//g 0x10
            0b0110111,//h 0x11
            0b0110000,//i 0x12
            0b0111000,//j 0x13
            0b0001110,//l 0x14
            0b0010101,//n 0x15
            0b0011101,//o 0x16
            0b1100111,//p 0x17
            0b0000101,//r 0x18
            0b1011011,//s 0x19
            0b0001111,//t 0x1A
            0b0011100,//u 0x1B
            0b0111011,//y 0x1C
            0b0000000 //  0x1D
	};
};

#endif

