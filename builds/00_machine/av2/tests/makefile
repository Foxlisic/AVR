DEVICE = attiny85
CLOCK  = 25000000
COMPILE = avr-gcc -I../../include -Wall  -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -Wl,--defsym=__stack=0x80bfff

all:
	$(COMPILE) main.c -o main.elf
	avr-objcopy -j .text -j .data -O binary main.elf main.bin
	avr-objdump -S main.elf > main.lst
	rm main.elf
	cd .. && ./avr tests/main.bin

clean:
	rm *.o rm *.elf *.lst
