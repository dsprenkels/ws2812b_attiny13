MCU=attiny13a
AVRDUDEMCU=t13
CC=avr-gcc
CFLAGS += -std=c11 -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wformat-security \
          -g -Os -mcall-prologues -mmcu=$(MCU)
OBJ2HEX=/usr/bin/avr-objcopy
AVRDUDE=/usr/bin/avrdude
TARGET=example

$(TARGET): example.c ws2812b_attiny13.o

$(TARGET).hex: $(TARGET)
	$(OBJ2HEX) -j .text -j .data -O ihex $(TARGET) $(TARGET).hex

.PHONY: size
size: $(TARGET)
	avr-size $(TARGET)

flash: $(TARGET).hex
	sudo $(AVRDUDE) -p $(AVRDUDEMCU) -c usbasp -U flash:w:$(TARGET).hex:i -U lfuse:w:0x7a:m -U hfuse:w:0xff:m

.PHONY: clean
clean:
	rm -f $(TARGET) $(TARGET).hex *.obj *.o
