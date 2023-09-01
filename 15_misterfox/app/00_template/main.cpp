#include <avrio.h>

// WARN: Если не используется, заглушку все равно оставить
ISR(INT0_vect) { } // TIMER
ISR(INT1_vect) { } // KEYBOARD

int main() {

    sei();
    for(;;);
}
