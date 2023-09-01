all:
	avr-as -m atmega328 main.asm -o main.o
	avr-ld main.o -o main.elf
	avr-objcopy -j .text -j .data -O binary main.elf main.bin
	php ../export.php test/main.bin tb.hex
	rm main.o main.elf
	../avsim/avrsim.exe main.bin