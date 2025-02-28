#include <avrio.h>

int mandel(float cx, float cy)
{
    float x = 0, y = 0;
    for (int i = 0; i < 16; i++) {

        double tx = x*x - y*y + cx,
               ty = 2*x*y     + cy;

        x = tx;
        y = ty;

        if (x*x + y*y >= 4) {
            return i;
        }
    }

    return 0;
}

int main()
{
    loc(0, 0);
    border(0);

    for (int y = -120; y < 120; y++)
    for (int x = -128; x < 128; x++) {
        padd(mandel((float)x/64, (float)y/64));
    }

    return 0;
}
