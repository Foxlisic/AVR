DEVICE = atmega103
CLOCK  = 25000000
COMPILE = avr-gcc -I../../classlib -I../../include -Wall -Wno-unused-but-set-variable -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -fdata-sections -Wl,--gc-sections,--defsym=__stack=0x80efff,--defsym=__heap_end=0x80efff

all:
	$(COMPILE) main.cpp -o main.elf
	avr-objcopy -j .text -j .data -O binary main.elf main.bin
	avr-objdump -S main.elf > main.lst
	avr-size -C --mcu=$(DEVICE) main.elf > mem.map
	rm main.elf

	# FPGA
	# php ../../de0/hexer.php main.bin > ../../de0/avr.hex
	# php ../../de0/mifer.php main.bin > ../../de0/flash.mif
	# cd ../../de0 && make

	avr main.bin

clean:
	rm -f *.o rm *.elf *.lst *.bin *.map
