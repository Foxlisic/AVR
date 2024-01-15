#include <avrio.h>

int x = 0;

void cls(byte color) {

    heapvm;
    for (int i = 0; i < 4000; i += 2) {
        vm[i  ] = 0x00;
        vm[i+1] = color;
    }
}

void test(const char* s) {

    heapvm;
    int i = 0;
    while (s[i]) {
        vm[2*(33+11*80)+2*i] = s[i];
        vm[2*(33+11*80)+2*i+1] = 0x70;
        i++;
    }
}

ISR(INT0_vect) {

    heapvm;

    vm[x] = inp(0);
    x += 2;
}

int main() {

    cls(0x07);
    test(" Help, World! ");

    sei();
    for(;;);
}
