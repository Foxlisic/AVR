# https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
DEVICE  = atmega328
OPTIONS = -Os -DF_CPU=25000000 -mmcu=$(DEVICE) -std=c++11 -g
INCLUDE = -I..
WARNING = -Wall -Wno-unused-but-set-variable
LINKER  = -fdata-sections -Wl,--gc-sections,--defsym=__stack=0x80ffff,--defsym=__heap_end=0x80ffff
COMPILE = avr-gcc $(INCLUDE) $(WARNING) $(OPTIONS) $(LINKER)
MIF     = php ../avr_bin2mif.php

all:
	$(COMPILE) main.cc -o main.elf
	avr-objcopy -j .text -j .data -O binary main.elf main.bin
	avr-objdump -S main.elf > main.lst
	avr-size -C --mcu=$(DEVICE) main.elf > mem.map
	rm main.elf
	$(MIF) 32 main.bin ../de0/mem_flash.mif
	../tb main.bin
mp4:
	ffmpeg -framerate 70 -r 60 -i out/record.ppm -vf "scale=w=1280:h=800,pad=width=1920:height=1080:x=320:y=140:color=black" -sws_flags neighbor -sws_dither none -f mp4 -q:v 0 -vcodec mpeg4 -y record.mp4
clean:
	rm -f *.o rm *.elf *.lst *.bin *.map
