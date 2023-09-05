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
    bank(17);   // Отсюда начинается SDRAM
    for (int i = 0; i < 4096; i++) vm[i] = i & 255; // i & 1 ? 0x55 : 0xAA;

    // Ожидание N времени
    while (KB.get() == 0);

    D.locate(0, 2);

    int error = 0;
    for (int i = 0; i < 512; i++) {

        bank(17);
        byte ch = vm[i];
        D.attr = i &1 ? 0x07 : 0x08;
        if (ch != (i & 255)) { error++; D.attr = 0x04; }
        D.prnhex(ch);
    }

    D.locate(40, 1);
    D.print("ERRORS: ");
    D.prnint(error);

    for(;;);
}
