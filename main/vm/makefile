all: app
	./avr > avr.log

app:
	g++ -Ofast avr.cc -o avr -lSDL2 -Wno-unused-result

clean:
	rm avr *.o
