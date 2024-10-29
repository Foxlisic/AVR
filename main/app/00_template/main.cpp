#include <avrio.h>

int main() {

    outp(2, 'x');
    for (int i = 0; i < 256; i++) outp(2, 0x37);
}
