DEVICE = attiny85
CLOCK  = 25000000

# http://microsin.net/programming/avr/avr-gcc-options.html
COMPILE = avr-gcc -I../../../include -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -Wl,--defsym=__stack=0x80bfff

all: main.o
	$(COMPILE) main.o -o main.elf
	avr-objcopy -j .text -j .data -O binary main.elf main.bin

dis:
	avr-objdump -S main.elf

clean:
	rm *.o

main.o: main.c
	$(COMPILE) -c main.c -o main.o