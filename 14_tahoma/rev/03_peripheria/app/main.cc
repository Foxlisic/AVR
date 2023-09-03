#include <avr_io.h>
#include <avr_sdcard.h>

IFace D;
SDCard SD;

ISR(INT0_vect) { D.keyfetch(); }

int main() {

    D.screen(0);
    D.cls(0x07);

    sei();

    for(;;) {

        byte in = D.inkey();
        if (in) D.term(in);
    }
}
