#include <graphics.cc>

byte zxram[32767];
Graphics g;

#include "48k.h"
#include "z80.cc"
#include "zxspectrum.cc"

int main() {

    g.start();
    zxspectrum z80;

    for (;;) {
        z80.frame();
    }
}
