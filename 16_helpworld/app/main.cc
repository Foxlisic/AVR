#include <avrio.h>

void cls() {

    heapvm;
    for (int i = 0; i < 4000; i += 2) {
        vm[i  ] = 0x00;
        vm[i+1] = 0x07;
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

int main() {

    cls();
    test(" Help, World! ");
    for(;;);
}
