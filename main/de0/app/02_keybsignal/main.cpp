#include <avrio.h>

int main()
{
    byte bb = millis();

    for (;;)
    for (int y = 0; y < 240; y += 4) {

        int x = 0, b = 0, c = 0;
        while (x < 256) {

            byte a = inp(15);

            while (millis() == bb);
            bb = millis();

            if (a != b) { c = 0; }

            if (c < 64) {

                pset(x, y,   a & 1 ? 1 : 15);
                pset(x, y+1, a & 2 ? 3 : 1);
                c++; x++;
            }

            b = a;
        }

    }

    return 0;
}
