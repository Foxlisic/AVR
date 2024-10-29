# https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
DEVICE  = atmega128
OPTIONS = -Os -DF_CPU=25000000 -mmcu=$(DEVICE) -std=c++11 -g
INCLUDE = -I../../include
WARNING = -Wall -Wno-unused-but-set-variable
LINKER  = -fdata-sections -Wl,--gc-sections,--defsym=__stack=0x80dfff,--defsym=__heap_end=0x80dfff
COMPILE = avr-gcc $(INCLUDE) $(WARNING) $(OPTIONS) $(LINKER)

all:
	$(COMPILE) main.cc -o main.elf
	avr-objcopy -j .text -j .data -O binary main.elf main.bin
	avr-objdump -S main.elf > main.lst
	avr-size -C --mcu=$(DEVICE) main.elf > mem.map
	rm main.elf
	komposer -mif 64k:16/flash/main.bin -dir ../../de0
	mv main.bin program.bin && ../../tb
scr:
	php ../../../utils/bin2h.php src/tailwind.scr wallpaper.h _wallpaper
clean:
	rm -f *.o rm *.elf *.lst *.bin *.map
