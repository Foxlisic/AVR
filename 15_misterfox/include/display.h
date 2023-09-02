#ifndef __DISPLAY_HFILE
#define __DISPLAY_HFILE

#include "avrio.h"

class Display {
protected:

    byte cmode, attr;

public:

    Display() { cmode = 0; };

    // Установка видеорежима
    void mode(int m) {

        cmode = 0;
        if (m == 13) cmode = 1; // 320x200
        outp(VMODE, cmode);
    }

    // Очистка экрана от скверны
    void cls(byte _attr = 0x07) {

        heapvm;
        attr = _attr;
vm[0x1] = 0x17;
        switch (cmode) {

            case 0: // 80x25 Текстовый режим

                for (int i = 0; i < 4000; i+= 2) {
                    vm[i]   = 0x00;
                    vm[i+1] = _attr;
                }

                break;
        }
    }
};

#endif
