#include <avrio.h>
#include <sd.h>

SD sd;
byte sector[512];

ISR(INT0_vect) { } // TIMER
ISR(INT1_vect) { } // KEYBOARD

int main() {

    heapvm;
    outp(CURX, 2);

    sd.read(0, vm);

    for(;;);
}
