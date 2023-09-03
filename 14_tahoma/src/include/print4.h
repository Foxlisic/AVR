#include "display.h"
#include "zxfont4.h"

void locate(byte x, byte y) {

    heapvm;
    int w = LPW(charmapos[cursor.y]) + (x >> 1);

    for (byte i = 0; i < 8; i++) {

        vm[w] ^= (x & 1 ? 0x0F : 0xF0);
        w += 0x100;
    }

    cursor.visible = 1;
}

void clrcursor() {

    if (cursor.visible) {
        locate(cursor.x, cursor.y);
    }
    cursor.visible = 0;
}

// Вывод символа 4x8
void pchar(byte x, byte y, byte ch) {

    heapvm;

    ch -= 32;
    x  &= 63;

    if (ch >= 96) ch = 0;

    byte M  = ch >> 1;
    byte x2 = x >> 1;
    word A  = LPW(charmapos[y]) + x2;

    for (byte i = 0; i < 8; i++) {

        byte mask = LPM(zxfont4[M][i]);

        // Выбор номер символа
        if ((ch & 1) == 0) mask >>= 4;

        // Рисование в левой или правой стороне
        if (x & 1) {
            vm[A] = (vm[A] & 0xF0) | (mask & 15);
        } else {
            vm[A] = (vm[A] & 0x0F) | (mask << 4);
        }

        A += 0x100;
    }

    vm[0x1800 + x2 + y*32] = cursor.attr;
}
