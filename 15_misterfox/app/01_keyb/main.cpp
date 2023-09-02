#include <avrio.h>
#include <display.h>

Display D;

ISR(INT1_vect) {

    D.prn(inp(KEYB));
    eoi;
}

int main() {

    D.cls();
    ei(2);
    for(;;);
}
