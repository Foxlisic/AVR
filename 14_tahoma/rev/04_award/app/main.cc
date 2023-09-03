#include <avr_io.h>
#include <avr_sdcard.h>

#include "fontsrc.h"

IFace D;
SDCard SD;

ISR(INT0_vect) { D.keyfetch(); }

void splash() {

    D.screen(0);
    D.cls(0x07);

    // Обновление Font ROM
    D.address(1*16);   for (int i = 0; i < 16*6;  i++) out(PORT_WRDATA, LPM(IconModularo[i]));
    D.address(128*16); for (int i = 0; i < 16*74; i++) out(PORT_WRDATA, LPM(IconEPAData[i]));

    // Вывод "человечка"
    D.color(11); D.term(1); D.term(2); D.term(3); D.locate(0, 1); D.term(4); D.term(5); D.term(6);

    int n = 0;
    for (int i = 0; i < 6; i++) {

        D.locate(63, i);
        D.color(i < 5 ? 14 : 10);
        for (int j = 0; j < 17; j++) D.term(LPM(IconEPATiles[n++]));
    }

    // Вывести надписи на экран
    D.color(7);
    D.locate(3,0);  D.printpgm(Message1);
    D.locate(3,1);  D.printpgm(Message2);
    D.locate(0,3);  D.printpgm(Message3);
    D.locate(0,5);  D.printpgm(Message4);
    D.locate(0,23); D.printpgm(Message5);
    D.locate(0,24); D.printpgm(Message6);
}

int main() {

    splash();

    // sei();

    for(;;) {

        // byte in = D.inkey(); if (in) D.term(in);
    }
}
