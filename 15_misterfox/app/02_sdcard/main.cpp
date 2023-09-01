#include <avrio.h>
#include <sd.h>

SD sd;
byte sector[512];

ISR(INT0_vect) { } // TIMER
ISR(INT1_vect) { } // KEYBOARD

int main() {

    sd.read (0, sector);
    sd.write(0, sector);

    for(;;);
}
