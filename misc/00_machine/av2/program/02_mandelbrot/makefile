DEVICE = attiny85
CLOCK  = 16000000
COMPILE = avr-gcc -I../../../include -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all: main.o
	$(COMPILE) main.o -o main.elf
	avr-objcopy -j .text -j .data -O binary main.elf main.bin
	avr-objdump -S main.elf > main.lst
	cd ../.. && ./avr program/02_mandelbrot/main.bin

clean:
	rm *.o

main.o: main.c
	$(COMPILE) -c main.c -o main.o
