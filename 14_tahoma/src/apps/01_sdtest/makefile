# https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
DEVICE  = atmega128
OPTIONS = -Os -DF_CPU=25000000 -mmcu=$(DEVICE) -std=c++11
INCLUDE = -I../../include
WARNING = -Wall -Wno-unused-but-set-variable
LINKER  = -fdata-sections -Wl,--gc-sections,--defsym=__stack=0x80dfff,--defsym=__heap_end=0x80dfff
COMPILE = avr-gcc $(INCLUDE) $(WARNING) $(OPTIONS) $(LINKER)
MIF	    = php ../../../utils/bin2mif.php

all:
	$(COMPILE) main.cc -o main.elf
	avr-objcopy -j .text -j .data -O binary main.elf main.bin
	avr-objdump -S main.elf > main.lst
	avr-size -C --mcu=$(DEVICE) main.elf > mem.map
	rm main.elf
	$(MIF) main.bin > ../../de0/flash.mif
	mv main.bin program.bin && ../../tb
record:
	ffmpeg -framerate 70 -r 60 -i out/record.ppm -vf "scale=w=1280:h=800,pad=width=1920:height=1080:x=320:y=140:color=black" -sws_flags neighbor -sws_dither none -f mp4 -q:v 0 -vcodec mpeg4 -y record.mp4
clean:
	rm -f *.o rm *.elf *.lst *.bin *.map sd.img
