#include <avrio.h>
#include <qb.cc>

static const char b1[] PROGMEM = "Helpo\x1B\x4FRAR\x1B\x07y InterNet Files";

ISR(INT0_vect) { prn(inp(0)); }

int main() {

    cls(0x07);
    print(b1, 1);

    //sei();
    for(;;);
}
