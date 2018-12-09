#ifndef WS2812B_ATTINY13_H_
#define WS2812B_ATTINY13_H_

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>

#if F_CPU != 9600000UL
# error "The WS2812b code can only be used with an F_CPU of exactly 9600000UL"
# pragma message("The default clock frequency of an attiny13 is 1.2 MHz. \
To set the clock frequency to 9.6 MHz, unprogram the CKDIV8 bit (bit no. 4) from the lower fuse register. \
For more information, look at the datasheet (pages 25, 103).")
#endif

void ws2812b_set_color_no_reset(const uint8_t pin_value, const uint8_t red, const uint8_t green, const uint8_t blue);

static void ws2812b_set_color(const uint8_t pin_value, const uint8_t red, const uint8_t green, const uint8_t blue) {
    DDRB |= pin_value;
    PORTB &= ~pin_value;
    _delay_us(50);
    ws2812b_set_color_no_reset(pin_value, red, green, blue);
}

#endif // WS2812B_ATTINY13_H_