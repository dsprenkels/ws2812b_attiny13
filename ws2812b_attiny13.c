#include <avr/io.h>
#include <inttypes.h>

void ws2812b_set_color_no_reset(const uint8_t pin_value, const uint8_t red, const uint8_t green, const uint8_t blue) {
    DDRB |= pin_value;
    const uint8_t portb = PORTB; // PORTB is volatile, so preload value
    const uint8_t lo = portb & ~pin_value;
    const uint8_t hi = portb | pin_value;
    __asm__  volatile (   
                // Each 12 cycles go high on cycle 0 and go low on cycle:
                //   - 8 if a one bit is transmitted
                //   - 4 if a zero bit is transmitted
                //
                // In the spare time we have left, we read out the next bit from the current color
                // byte. We keep a mask bit in r18, do a bitwise and operation, and branch if (not)
                // zero. After 8 shifts the carry flag will be set and we will move on to the next
                // color bit.
                //
                // I have figured that it should be possible to decode the bits from the RGB values
                // on the fly. However, I think that we do not have enough cycles to read from the
                // different color in a non-unrolled loop, but to be completely honest I did not
                // take a lot of effort to find a good argument on why this should be impossible.
                //
                ".green:                        \n\t"
                "mov r17,%[green]               \n\t"
                "ldi r18,0x80                   \n\t"
                "and r17,r18                    \n\t"
                "brne .green_transmit_one_0     \n\t"
                "rjmp .green_transmit_zero_0    \n\t"
                
                // Green
                ".green_transmit_one_1:         \n\t"
                "nop                            \n\t" // cycle -1
                ".green_transmit_one_0:         \n\t"
                "out 0x18,%[hi]                 \n\t" // cycle 0
                "nop                            \n\t" // cycle 1
                "lsr r18                        \n\t" // cycle 2
                "brcs .green_transmit_one_done  \n\t" // cycle 3
                "rjmp .+0                       \n\t" // cycle 4
                "mov r17,%[green]               \n\t" // cycle 6
                "and r17,r18                    \n\t" // cycle 7
                "out 0x18,%[lo]                 \n\t" // cycle 8
                "brne .green_transmit_one_1     \n\t" // cycle 9
                "rjmp .green_transmit_zero_0    \n\t" // cycle 10
                
                ".green_transmit_one_done:      \n\t"
                "ldi r18,0x80                   \n\t" // cycle 5
                "mov r17,%[red]                 \n\t" // cycle 6
                "and r17,r18                    \n\t" // cycle 7
                "out 0x18,%[lo]                 \n\t" // cycle 8
                "brne .red_transmit_one_1       \n\t" // cycle 9
                "rjmp .red_transmit_zero_0      \n\t" // cycle 10
                
                ".green_transmit_zero_0:        \n\t"
                "out 0x18,%[hi]                 \n\t" // cycle 0
                "lsr r18                        \n\t" // cycle 1
                "brcs .green_transmit_zero_done \n\t" // cycle 2
                "nop                            \n\t" // cycle 3
                "out 0x18,%[lo]                 \n\t" // cycle 4
                "rjmp .+0                       \n\t" // cycle 5
                "mov r17,%[green]               \n\t" // cycle 7
                "and r17,r18                    \n\t" // cycle 8
                "brne .green_transmit_one_1     \n\t" // cycle 9
                "rjmp .green_transmit_zero_0    \n\t" // cycle 10
                
                ".green_transmit_zero_done:     \n\t"
                "out 0x18,%[lo]                 \n\t" // cycle 4
                "nop                            \n\t" // cycle 5
                "ldi r18,0x80                   \n\t" // cycle 6
                "mov r17,%[red]                 \n\t" // cycle 7
                "and r17,r18                    \n\t" // cycle 8
                "brne .red_transmit_one_1       \n\t" // cycle 9
                "rjmp .red_transmit_zero_0      \n\t" // cycle 10
                
                // Red
                ".red_transmit_one_1:           \n\t"
                "nop                            \n\t" // cycle -1
                ".red_transmit_one_0:           \n\t"
                "out 0x18,%[hi]                 \n\t" // cycle 0
                "nop                            \n\t" // cycle 1
                "lsr r18                        \n\t" // cycle 2
                "brcs .red_transmit_one_done    \n\t" // cycle 3
                "rjmp .+0                       \n\t" // cycle 4
                "mov r17,%[red]                 \n\t" // cycle 6
                "and r17,r18                    \n\t" // cycle 7
                "out 0x18,%[lo]                 \n\t" // cycle 8
                "brne .red_transmit_one_1       \n\t" // cycle 9
                "rjmp .red_transmit_zero_0      \n\t" // cycle 10
                
                ".red_transmit_one_done:        \n\t"
                "ldi r18,0x80                   \n\t" // cycle 5
                "mov r17,%[blue]                \n\t" // cycle 6
                "and r17,r18                    \n\t" // cycle 7
                "out 0x18,%[lo]                 \n\t" // cycle 8
                "brne .blue_transmit_one_1      \n\t" // cycle 9
                "rjmp .blue_transmit_zero_0     \n\t" // cycle 10
                
                ".red_transmit_zero_0:          \n\t"
                "out 0x18,%[hi]                 \n\t" // cycle 0
                "lsr r18                        \n\t" // cycle 1
                "brcs .red_transmit_zero_done   \n\t" // cycle 2
                "nop                            \n\t" // cycle 3
                "out 0x18,%[lo]                 \n\t" // cycle 4
                "rjmp .+0                       \n\t" // cycle 5
                "mov r17,%[red]                 \n\t" // cycle 7
                "and r17,r18                    \n\t" // cycle 8
                "brne .red_transmit_one_1       \n\t" // cycle 9
                "rjmp .red_transmit_zero_0      \n\t" // cycle 10
                
                ".red_transmit_zero_done:       \n\t"
                "out 0x18,%[lo]                 \n\t" // cycle 4
                "nop                            \n\t" // cycle 5
                "ldi r18,0x80                   \n\t" // cycle 6
                "mov r17,%[blue]                \n\t" // cycle 7
                "and r17,r18                    \n\t" // cycle 8
                "brne .blue_transmit_one_1      \n\t" // cycle 9
                "rjmp .blue_transmit_zero_0     \n\t" // cycle 10
                
                // Blue
                ".blue_transmit_one_1:          \n\t"
                "nop                            \n\t" // cycle -1
                ".blue_transmit_one_0:          \n\t"
                "out 0x18,%[hi]                 \n\t" // cycle 0
                "rjmp .+0                       \n\t" // cycle 1
                "lsr r18                        \n\t" // cycle 3
                "brcs .blue_transmit_one_done   \n\t" // cycle 4
                "nop                            \n\t" // cycle 5
                "mov r17,%[blue]                \n\t" // cycle 6
                "and r17,r18                    \n\t" // cycle 7
                "out 0x18,%[lo]                 \n\t" // cycle 8
                "brne .blue_transmit_one_1      \n\t" // cycle 9
                "rjmp .blue_transmit_zero_0     \n\t" // cycle 10
                
                ".blue_transmit_one_done:       \n\t"
                "rjmp .blue_transmit_zero_done  \n\t" // cycle 6
                
                ".blue_transmit_zero_0:         \n\t"
                "out 0x18,%[hi]                 \n\t" // cycle 0
                "lsr r18                        \n\t" // cycle 1
                "brcs .blue_transmit_zero_done  \n\t" // cycle 2
                "nop                            \n\t" // cycle 3
                "out 0x18,%[lo]                 \n\t" // cycle 4
                "rjmp .+0                       \n\t" // cycle 5
                "mov r17,%[blue]                \n\t" // cycle 7
                "and r17,r18                    \n\t" // cycle 8
                "brne .blue_transmit_one_1      \n\t" // cycle 9
                "rjmp .blue_transmit_zero_0     \n\t" // cycle 10
                
                ".blue_transmit_zero_done:      \n\t"
                "out 0x18,%[lo]                 \n\t" // cycle 4 / cycle 8
                ".end:                          \n\t"
            
            : // No outputs
            : [lo] "r" (lo), [hi] "r" (hi), [green] "r" (green), [red] "r" (red), [blue] "r" (blue)
            : "r16", "r17", "r18"
    );
}
