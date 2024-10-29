all: app
	./avr

app: avr_cpu.o avr_assign.o avr_disasm.o
	g++ -Ofast -o avr avr.cc -lSDL2

# Сборка o-файлов
avr_cpu.o: avr_cpu.cc
	g++ -c avr_cpu.cc -o avr_cpu.o
avr_assign.o: avr_assign.cc
	g++ -c avr_assign.cc -o avr_assign.o
avr_disasm.o: avr_disasm.cc
	g++ -c avr_disasm.cc -o avr_disasm.o
clean:
	rm avr *.o
