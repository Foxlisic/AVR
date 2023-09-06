#include <avrio.h>
#include <display3.h>
#include <kb.h>

Display3 D;
Keyboard KB;

ISR(INT1_vect) { KB.recv(); eoi; }

int main() {

    D.cls();
    ei(2);

    D.print("WRITE\n");

    heapvm;

    // Отсюда начинается SDRAM
    for (int i = 17; i < 256; i++) {

        bank(i);
        for (int j = 0; j < 4096; j++)
            vm[i] = j & 255;

        D.locate(0, 2);
        D.print("FILL MEMORY %");
        D.prnint((100 * i) / 255);
    }

    for(;;);
}
