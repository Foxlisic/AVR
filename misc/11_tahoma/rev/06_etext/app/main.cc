#include <avr_io.h>
#include <avr_sdcard.h>

IFace  D;
SDCard SD;

static const char kit[] PROGMEM = "Cat on the kit";

ISR(INT0_vect) { D.keyfetch(); }

int main() {

    D.cls(0x17);
    D.color(0x30);
    D.print("Кошки на китикэте");

    sei();

    for(;;) {

        byte in = D.inkey();
        if (in) D.term(in);
    }
}
