CC=g++
SDL=`sdl-config --cflags --libs`
LIBS=-lSDL
WARN=-Wall -Wno-strict-overflow -Wno-unused-result -Wno-unused-variable -Wno-unused-but-set-variable
CFLAGS=-Icore -I. $(SDL) -O3
OBJ=app.o avrconstr.o move.o arith.o avrcpu.o avr.o

all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) $(WARN) -o avr.out

avr.o: avr.cc avr.h
	$(CC) $(CFLAGS) avr.cc -c -o avr.o

app.o: core/app.h core/app.cc
	$(CC) $(CFLAGS) core/app.cc $(LIBS) $(WARN) -c -o app.o

# Компоненты процессора
avrcpu.o: core/avrcpu.h core/avrcpu.cc
	$(CC) $(CFLAGS) core/avrcpu.cc $(LIBS) $(WARN) -c -o avrcpu.o

arith.o: core/arith.cc core/avrcpu.h 
	$(CC) $(CFLAGS) core/arith.cc $(LIBS) $(WARN) -c -o arith.o

move.o: core/move.cc core/avrcpu.h 
	$(CC) $(CFLAGS) core/move.cc $(LIBS) $(WARN) -c -o move.o

avrconstr.o: core/avrconstr.cc core/avrcpu.h 
	$(CC) $(CFLAGS) core/avrconstr.cc $(LIBS) $(WARN) -c -o avrconstr.o

clean:
	rm *.o