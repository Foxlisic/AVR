#ifndef __DISPLAY_HFILE
#define __DISPLAY_HFILE

#include "avrio.h"

class Display {
protected:

    byte cmode, attr;
    byte locx, locy;

public:

    Display() { cmode = 0; };

    // Установка видеорежима
    void mode(int m) {

        cmode = 0;
        if (m == 13) cmode = 1; // 320x200
        outp(VMODE, cmode);
    }

    void color(byte ch) { attr = ch; }

    // Очистка экрана от скверны
    void cls(byte _attr = 0x07) {

        heapvm;

        attr = _attr;
        locate(0, 0);

        switch (cmode) {

            case 0: // 80x25 Текстовый режим

                for (int i = 0; i < 4000; i+= 2) {
                    vm[i]   = 0x00;
                    vm[i+1] = _attr;
                }

                break;
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

        if (cmode == 0) {

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
    }

    void print(const char* m) {

        int i = 0;
        while (m[i]) prn(m[i++]);
    }

    // Печать числа от -32767 до 32767
    void printi(int x) {

        int  id = 0;
        char t[6];

        if (x < 0) { prn('-'); x = -x; }
        do { int a = x % 10; x = x / 10; t[id++] = '0' + a; } while (x);
        while (--id >= 0) prn(t[id]);
    }
};

#endif
