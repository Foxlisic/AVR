#include <avrio.h>

ISR(INT0_vect) { eoi; } // 1 VSYNC
ISR(INT1_vect) { eoi; } // 2 KEYBOARD

int main() {

    ei(3); // Vsync + Keyboard
    for(;;);
}
