// ws2812_toy - main implementation
//
// Author: Amber Sprenkels <amber@electricdusk.com>
// Description: A toy implementation for using WS2812 RBG leds

#define F_CPU 9600000UL

#include "ws2812b_attiny13.h"
#include <inttypes.h>
#include <stdbool.h>

// Define a list of colors to pulsate
const struct color {
    const uint8_t red;
    const uint8_t green;
    const uint8_t blue;
} COLORS[] = {
    {0xFF, 0xFF, 0xFF}, // white
    {0x00, 0x00, 0x00}, // off
    {0xFF, 0xFF, 0xFF}, // white
    {0x00, 0x00, 0x00}, // off
    {0xFF, 0xFF, 0xFF}, // white
    {0x00, 0x00, 0x00}, // off
    {0xFF, 0x00, 0x00}, // red
    {0xFF, 0x00, 0x00}, // red
    {0x00, 0x00, 0x00}, // off
    {0x00, 0xFF, 0x00}, // green
    {0x00, 0xFF, 0x00}, // green
    {0x00, 0x00, 0x00}, // off
    {0x00, 0x00, 0xFF}, // blue
    {0x00, 0x00, 0xFF}, // blue
    {0x00, 0x00, 0x00}, // off
    {0x00, 0x00, 0x00}, // off
};

int main() {
    while (true) {
        for (uint8_t i = 0; i < sizeof(COLORS) / sizeof(COLORS[0]); i++) {
            
            // Turn light on for some time
            const struct color color = COLORS[i];
            ws2812b_set_color(_BV(PB3), color.red/3, color.green/2, color.blue);
            _delay_ms(500);
            
        }
    }
}
