all:
	avr-as main.asm -o main.elf
	avr-ld main.elf -o main.o
	avr-objcopy -j .text -j .data -O binary main.o main.bin
	avr-objdump -S main.elf > main.lst
	rm main.elf main.o

clean:
	rm -f *.o rm *.elf *.lst *.bin *.map
