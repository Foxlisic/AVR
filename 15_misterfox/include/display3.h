#ifndef __DISPLAY_HFILE
#define __DISPLAY_HFILE

#include "avrio.h"

class Display3 {
public:

    byte locx, locy;
    byte attr;

    // Очистка экрана от скверны
    void cls(byte _attr = 0x07) {

        heapvm;

        attr = _attr;
        locate(0, 0);

        for (int i = 0; i < 4000; i+= 2) {
            vm[i]   = 0x00;
            vm[i+1] = _attr;
        }
    }

    // Установка курсора
    void locate(byte x, byte y) {

        locx = x;
        locy = y;

        outp(CURX, x);
        outp(CURY, y);
    }

    // Пропечать одного символа
    void prn(char m) {

        heapvm;

        int loc = 2*locx + locy*160;
        vm[loc]   = m;
        vm[loc+1] = attr;

        locx++;
        if (locx >= 80) {
            locx = 0;
            locy++;
            if (locy >= 25) {
                locy = 24;
            }
        }

        locate(locx, locy);
    }

    // Печать символов на экране
    int print(const char* m) {

        int i = 0;
        while (m[i]) prn(m[i++]);
        return i;
    }

    // Печать строки из Program memory
    int print_pgm(const char* m) {

        int i = 0;
        char ch;
        while ((ch = pgm_read_byte(& m[i++]))) prn(ch);
        return i;
    }

    // Печать числа от -32767 до 32767
    void prnint(int x) {

        int  id = 0;
        char t[6];

        if (x < 0) { prn('-'); x = -x; }
        do { int a = x % 10; x = x / 10; t[id++] = '0' + a; } while (x);
        while (--id >= 0) prn(t[id]);
    }
};

#endif
