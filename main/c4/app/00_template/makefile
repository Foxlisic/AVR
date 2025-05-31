DEVICE  = atmega328
CLOCK   = 25000000
COMPILE = avr-gcc -I../../../include -Wall -Wno-unused-but-set-variable -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -fdata-sections -Wl,--gc-sections,--defsym=__stack=0x8017ff,--defsym=__heap_end=0x8017ff
VM		= ../../../vm/avr -4 -d
BM		= php ../../../util/bin2mif.php

all:
	$(COMPILE) main.cpp -o main.elf
	avr-objcopy -j .text -j .data -O binary main.elf main.bin
	avr-objdump -S main.elf > main.lst
	avr-size -C --mcu=$(DEVICE) main.elf > mem.map
	rm main.elf
	$(BM) main.bin
	$(VM) main.bin > avr.log

clean:
	rm -f *.o rm *.elf *.lst *.bin *.map avr.log
