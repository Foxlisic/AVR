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
            return i & 7;
        }
    }

    return 0;
}

int main()
{
    for (int y = -200; y < 200; y++)
    for (int x = -320; x < 320; x++) {
        point(mandel((float)x/100, (float)y/100));
    }

    return 0;
}
