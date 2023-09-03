#include <avrio.h>
#include <display3.h>

Display3 D;

ISR(INT1_vect) {

    D.prn(inp(KEYB));
    eoi;
}

int main() {

    D.cls();
    ei(2);
    for(;;);
}
