#include <avrio.h>

// WARN: Если не используется, заглушку все равно оставить
ISR(INT0_vect) { eoi; } // 1 VSYNC
ISR(INT1_vect) { eoi; } // 2 KEYBOARD

int main() {

    ei(3);
    for(;;);
}
