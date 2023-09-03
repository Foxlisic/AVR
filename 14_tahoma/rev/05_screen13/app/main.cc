#include <math.h>

#include <avr_io.h>
#include <avr_video.h>

Video D;

int main() {

    uint16_t d = 0;

    for(;;) {

        D.flip();

        for (int y = 0; y < 200; y++)
        for (int x = 0; x < 320; x++)
            D.pset(x, y, (x*y) + d);

        d++;
    }
}
