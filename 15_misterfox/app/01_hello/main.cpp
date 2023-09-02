#include <avrio.h>
#include <display.h>

Display D;

int main() {

    D.cls(0x07);

    for (int i = 0; i < 1000; i++) D.print("/\\");

    D.color(0x17);
    D.locate(34, 12);
    D.print("HELLO WORLD");
}
