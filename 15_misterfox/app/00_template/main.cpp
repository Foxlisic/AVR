#include <avrio.h>

ISR(INT0_vect) { } // TIMER
ISR(INT1_vect) { } // KEYBOARD

int main() {

    sei();
    for(;;);
}
