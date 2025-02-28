DEVICE  = atmega103
CLOCK   = 25000000
COMPILE = avr-gcc -I../../include -Wall -Wno-unused-but-set-variable -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -fdata-sections -Wl,--gc-sections,--defsym=__stack=0x80ffff,--defsym=__heap_end=0x80ffff
VM		= ../../vm/avr
BM		= php ../../bin2mif.php

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
