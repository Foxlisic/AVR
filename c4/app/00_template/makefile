DEVICE  = atmega328p
CLOCK   = 25000000
COMPILE = avr-gcc -I../../../include -Wall -Wno-unused-but-set-variable -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -fdata-sections -Wl,--gc-sections,--defsym=__stack=0x8017ff,--defsym=__heap_end=0x8017ff
BM      = php ../../../utils/avr2mif.php

all:
	$(COMPILE) main.cpp -o main.elf
	avr-objcopy -j .text -j .data -O binary main.elf main.bin
	avr-objdump -S main.elf > main.lst
	avr-size -C --mcu=$(DEVICE) main.elf > mem.map
	$(BM) main.bin ../../m32.mif
	rm main.elf main.bin

clean:
	rm -f *.o rm *.elf *.lst *.bin *.map avr.log
