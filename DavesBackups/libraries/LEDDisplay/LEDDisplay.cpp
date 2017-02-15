/*
Released under Apache V2 license by www.thecoderscorner.com (Dave Cherry).
This library will drive an LED 7 segment display by interrupt.
*/

#include "LEDDisplay.h"

LEDDisplay* LEDDisplay::theDisplay;

LEDDisplay::LEDDisplay(char pinStart, boolean commonHigh) {
    this->pinStart = pinStart;
    this->commonHigh = commonHigh;

    for(int i=0;i<(LCD_BITS + LED_NUM_DIGITS);++i) {
        pinMode(pinStart + i, OUTPUT);
    }

    currentDigit = 0;
}

LEDDisplay::~LEDDisplay() {
}

void LEDDisplay::startInterrupt() {
    this->theDisplay = this;

  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 64000;   // preload timer
  TCCR1B |= _BV(CS11);    // 256 prescaler
  TIMSK1 |= _BV(TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}

void LEDDisplay::setValueDec(unsigned int newValue, boolean zeroPad) {
    setNumeric(newValue, 10, 0, 3, zeroPad);
}

void LEDDisplay::setValueHex(unsigned int newValue) {
    setNumeric(newValue, 16, 0, 3, true);
}

void LEDDisplay::setValueFloat(float newValue, unsigned int decimalPlaces, boolean zeroPad) {
    int whole = (int) newValue;
    int frac = (int) ((newValue - (float)whole) * pow(10, decimalPlaces + 1) + 0.5);
    int wholeDp = LED_NUM_DIGITS - decimalPlaces;
    setNumeric(frac, 10, wholeDp, decimalPlaces, true);
    setNumeric(whole, 10, 0, wholeDp-1, zeroPad);
    if(commonHigh) {
        digits[wholeDp-1] &= 0x7f;
    }
    else {
        digits[wholeDp-1] |= 0x80;
    }
}

void LEDDisplay::setNumeric(unsigned int value, unsigned int base, char start, char places, boolean zeroPad) {
    for(int i =places+start;i>=start;--i) {
        int digit = value % base;
        if(digit == 0 && !zeroPad) {
            digit=0x1d;
        }
        setValueRaw(i, digit);
        value = value / base;
    }
}


void LEDDisplay::setValueRaw(char position, char newValue, boolean dpOn) {
    if(position < LED_NUM_DIGITS) {
        digits[position] = charMap[newValue];

        if(dpOn) {
            digits[position] |= 0x80;
        }

        if(commonHigh) {
            digits[position] = ~digits[position];
        }
    }
}

void LEDDisplay::isr_display() {
    if(currentDigit >= LED_NUM_DIGITS) {
        digitalWrite(pinStart + LCD_BITS + LED_NUM_DIGITS -1, 0);
        currentDigit = 0;
    }
    else {
        digitalWrite(pinStart + LCD_BITS + currentDigit -1, 0);
    }

    for(int i=0;i<7;i++) {
        digitalWrite(pinStart + i, digits[currentDigit] & (1<<6-i));
    }

    digitalWrite(pinStart + 7, digits[currentDigit] & 0x80);

    digitalWrite(pinStart + LCD_BITS + currentDigit,1);

    ++currentDigit;
}

ISR(TIMER1_OVF_vect) {
    TCNT1 = 64000;
    LEDDisplay::instance()->isr_display();
}

