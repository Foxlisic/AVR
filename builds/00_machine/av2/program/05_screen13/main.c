#include <avrio.c>

void pset(int x, int y, char cl) {

    unsigned int pt = (y<<8) + (y<<6) + x;
    char* vm = (char*)0xc000;

    outp(0, pt >> 14);
    vm[ pt & 0x3FFF ] = cl;
}

int main() {

    outp(0x01, 1);

    for (int y = 0; y < 200; y++) 
    for (int x = 0; x < 320; x++) 
        pset(x, y, x + y);
    
}
