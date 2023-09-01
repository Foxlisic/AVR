#include <avrio.cc>
#include <math.h>

// Генератор шума на синусе на плоскости
float rnd(float x, float y) {

    float m = 43758.5453123*sin(x*12.9898 + y*78.233);
    return m - floor(m);
}

// Генерация плавного шума
float noise(float x, float y) {

    float ix = floor(x), iy = floor(y);
    float fx = x - ix;
    float fy = y - iy;

    float a = rnd(ix,     iy);
    float b = rnd(ix + 1, iy);
    float c = rnd(ix,     iy + 1);
    float d = rnd(ix + 1, iy + 1);

    float ux = fx*fx*(3 - 2*fx);
    float uy = fy*fy*(3 - 2*fy);

    return a * (1 - ux) + b * ux + (c - a) * uy * (1 - ux) + (d - b) * ux * uy;
}

// Основной генератор шума Перлина (octaves=5)
float fbm(float x, float y, int octaves) {

    float value = 0;
    float amp   = 1;

    for (int i = 0; i < octaves; i++) {

        value += amp * noise(x, y);
        x *= 2;
        y *= 2;
        amp = amp * .5;
    }

    return value;
}

int main() {

    heapvm;

    bank(2);

    float f = 8;
    float xorg = 0.5, yorg = 0.0;

    while (1) {
    for (int y = 0; y < 25; y++)
    for (int x = 0; x < 80; x++) {

        int k = 2*x + y*160;

        vm[k  ] = 0xb1 + noise(xorg + (float)x/f/2, yorg + (float)y/f)*1.9 + 0.5;
        vm[k+1] = 0x17;
    }
    xorg += 0.1;
    }

    for (;;);
}
