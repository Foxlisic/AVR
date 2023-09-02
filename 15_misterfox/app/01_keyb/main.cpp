#include <avrio.h>

int n = 0;

ISR(INT1_vect) {

    heapvm;
    vm[n]   = inp(KEYB);
    vm[n+1] = 0x17;
    n += 2;

    eoi;
}

int main() {

    ei(2);
    for(;;);
}
