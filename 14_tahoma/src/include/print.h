#define SCREEN_WIDTH 32

#include "display.h"
#include "zxfont8.h"

// Удалить текущий курсор
void clrcursor() {

    heapvm;
    vm[0x1800 + (cursor.y*32) + cursor.x] &= ~0x80;
}

// Установка положения курсора
void locate(byte x, byte y) {

    heapvm;
    clrcursor();

    cursor.x = x;
    cursor.y = y;

    vm[0x1800 + (cursor.y*32) + cursor.x] |= 0x80;
}

// Печать символа [0x20-0x7F] на экране
void pchar(byte x, byte y, byte ch) {

    heapvm;

    ch -= 32;
    x  &= 31;
    y  &= 31;

    if (ch >= 96) ch = 0;

    word M = 8*ch;
    word A = LPW(height[y << 3]) + x;

    for (byte i = 0; i < 8; i++) {

        vm[A] = LPM(_zxfont8[M]);
        A += 0x100; M++;
    }

    vm[0x1800 + x + y*32] = cursor.attr;
}


// Печать символа в режиме телетайпа
void prn(byte ch) {

    clrcursor();

    // Нажатие на Enter
    if (ch == 10) {
        pnewline();

    } else {

        pchar(cursor.x, cursor.y, ch);
        cursor.x++;
        if (cursor.x == 32) {
            pnewline();
        }
    }

    locate(cursor.x, cursor.y);
}

// Печать строки
void print(const char* s) {

    int i = 0;
    while (s[i]) prn(s[i++]);
}

void printhex(unsigned long a, int b) {

    while (b > 0) {

        int k = (a >> (b-4)) & 15;
        prn(k + (k < 10 ? '0' : '7'));
        b -= 4;
    }

}

