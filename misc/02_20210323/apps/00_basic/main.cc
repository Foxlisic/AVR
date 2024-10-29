#include <avr/pgmspace.h>
#include <screen3.cc>

// Текстовый видеорежим
screen3 D;

// pgm_read_byte(&program[0])
const byte program[] PROGMEM = "Program Space\0";

int main() {

    D.init();
    D.cls(7);

    heapvm;

    for (int c = 0; c < 256; c++)
    for (int b = 0; b < 256; b++)
    for (int a = 0; a < 256; a++) {


        unsigned char r = (a & (~b)) | (b & c);
        vm[a+b+c] = r;

    }

    for (;;);
}
