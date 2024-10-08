
PROJECT = midi2cv

CC =avr-gcc
CFLAGS = -mmcu=atmega328p -Wall -Os --std=c11 -DF_CPU=16000000UL

INCLUDE = -Isrc\

SRC = $(wildcard src/*.c)

hex:
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE) $(SRC) -o bin/$(PROJECT).elf
	avr-objcopy -j .text -j .data -O ihex bin/$(PROJECT).elf bin/$(PROJECT).hex

flash: hex
	@avrdude -p m328p -c usbtiny -e -U flash:w:bin/$(PROJECT).hex

unittest:
	make -C test unittest

clean:
	@rm -fr bin
	@make -C test clean
