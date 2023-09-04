#ifndef __DISPLAYZX_HFILE
#define __DISPLAYZX_HFILE

#include "avrio.h"
#include "tahoma.h"

class DisplayZX {
public:

    // Очистка экрана от скверны
    void cls(byte color = 0x00) {

        heapzx;
        outp(VMODE, VM_ZXSPECTRUM);

        for (int i = 0x0000; i < 0x1800; i++) vm[i] = 0;
        for (int i = 0x1800; i < 0x1B00; i++) vm[i] = color;
    }
};

#endif
